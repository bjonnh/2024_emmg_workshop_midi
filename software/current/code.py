import time, random
import board, busio,  audiopwmio, audiomixer, synthio, audiobusio, analogio, digitalio
from knobs import Knobs
import ulab.numpy as np
import supervisor

from display import Display
from midi import Midi
from touch import Touch
from wavetable import Wavetable
from wave_generators import gen_sine, gen_tri

# If you want to disable auto-reloading
#supervisor.runtime.autoreload = False

screen_scl_pin   = board.GP19
screen_sda_pin   = board.GP18
display = Display(screen_scl_pin, screen_sda_pin)

knobs_analog_pin = board.GP28
knobs_address_gpios = (board.GP2, board.GP3, board.GP4)
knobs_addresses = [(1,1,1), (1,0,1), (1,1,0), (0,0,0), (0,0,1), (0,1,1), (0,1,0), (1,0,0)]
knobs = Knobs(knobs_analog_pin, knobs_address_gpios, knobs_addresses)

touch_pins = (
    board.GP14, board.GP11, board.GP8, board.GP16, board.GP15, board.GP12,
    board.GP9, board.GP6 ,board.GP13, board.GP10, board.GP7, board.GP17)
touch = Touch(touch_pins)

midi_tx_pin = board.GP0
midi_rx_pin = board.GP1
midi = Midi(midi_tx_pin, midi_rx_pin)


audio = audiobusio.I2SOut(bit_clock=board.GP21, word_select=board.GP20, data=board.GP22)

lpf_basef = 100     # filter lowest frequency

SAMPLE_RATE=44100
mixer = audiomixer.Mixer(channel_count=1, sample_rate=SAMPLE_RATE, buffer_size=2048)
synth = synthio.Synthesizer(channel_count=1, sample_rate=SAMPLE_RATE)
audio.play(mixer)
mixer.voice[0].play(synth)
mixer.voice[0].level = 0.25

SAMPLE_VOLUME = 4000  # 0-32767
wavetable_fname = "wav/PLAITS02.WAV"  # from http://waveeditonline.com/index-17.html
wavetable_sample_size = 256 # number of samples per wave in wavetable (256 is standard)
SAMPLE_SIZE = wavetable_sample_size

half_period = SAMPLE_SIZE // 2
wave_sine = gen_sine(SAMPLE_SIZE, SAMPLE_VOLUME)
wave_tri =  gen_tri(SAMPLE_SIZE, SAMPLE_VOLUME)

wavetable1 = Wavetable(wavetable_fname, wave_len=wavetable_sample_size)
amp_env = synthio.Envelope(attack_time=0.1, release_time=0.2, sustain_level=0.95, attack_level=0.95)

voices = []

bend_mod = synthio.LFO(rate=0.1, scale=0.01, offset=0)
for i in range(len(touch_pins)):
   voices.append(synthio.Note(frequency=0, envelope=amp_env, waveform=wavetable1.waveform, bend=bend_mod ))

def set_voice_note(voice, n):
    # Changing that spread of the random is pretty cool
    voice.frequency = synthio.midi_to_hz( n + bend_mod.value) # random.uniform(0,100.0)

# the LFO that modulates the filter cutoff
lfo_filtermod = synthio.LFO(rate=1, scale=100, offset=100)
# we can't attach this directly to a filter input, so stash it in the blocks runner
synth.blocks.append(lfo_filtermod)

note = 0
last_note_time = time.monotonic()
last_filtermod_time = time.monotonic()

base_note = 0
old_notes = [-1] * touch.size()
last_wave_pos = -1

def play_note(touch_pad):
    print("touch", touch_pad)
    global base_note, voices
    new_note = base_note + i
    if old_notes[touch_pad] != new_note:
        set_voice_note(voices[i], new_note)
        synth.press([voices[i]])
        old_notes[i] = new_note

def release_note(touch_pad):
    synth.release([voices[i]])
    old_notes[i] = -1

touch.on_touch = play_note
touch.on_release = release_note

while True:
    midi.tick()
    touch.tick()
    base_note = knobs.values[3] // 1024

    # We don't need to run controllers updates every turn
    if time.monotonic() - last_filtermod_time > 0.1:
        knobs.tick()
        filter_freq = (knobs.values[1] + lfo_filtermod.value + 200) / 256 * lpf_basef
        if filter_freq > SAMPLE_RATE / 2.1:
            filter_freq = SAMPLE_RATE / 2.1

        lpf_resonance = max(0, (knobs.values[6] - 256) / 8192)
        for v in voices:
            v.filter = synth.low_pass_filter(filter_freq, lpf_resonance)

        wave_pos = knobs.values[2]//4096+15
        if wave_pos != last_wave_pos:
            wavetable1.set_wave_pos(wave_pos)
            last_wave_pos = wave_pos
        last_filtermod_time = time.monotonic()

        bend_mod.rate = max(0, (knobs.values[4] - 256)  / 8192)
        bend_mod.scale = max(0, (knobs.values[5] - 256) / 8192)

        # randomly modulate the filter frequency ('rate' in synthio) to make more dynamic
        #lfo_filtermod.rate = lfo_filtermod.rate + (random.random()-0.5) / 8
