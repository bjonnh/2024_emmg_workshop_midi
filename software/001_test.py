import time, random
import board, audiopwmio, audiomixer, synthio, audiobusio
import ulab.numpy as np
import neopixel, rainbowio   # circup install neopixel

import supervisor
#supervisor.runtime.autoreload = False

audio = audiobusio.I2SOut(bit_clock=board.GP7, word_select=board.GP8, data=board.GP6)


notes = (35, 34, 33) # possible notes to play MIDI A1, A1#, G1
note_duration = 1   # how long each note plays for
num_voices = 20    # how many voices for each note
lpf_basef = 50      # filter lowest frequency
lpf_resonance = 2.0  # filter q

led = neopixel.NeoPixel(board.GP23, 1, brightness=0.1)
SAMPLE_RATE=44100
mixer = audiomixer.Mixer(channel_count=1, sample_rate=SAMPLE_RATE, buffer_size=8192)
synth = synthio.Synthesizer(channel_count=1, sample_rate=SAMPLE_RATE)
audio.play(mixer)
mixer.voice[0].play(synth)
mixer.voice[0].level = 0.25

SAMPLE_SIZE = 32
SAMPLE_VOLUME = 2000  # 0-32767
half_period = SAMPLE_SIZE // 2
wave_sine = np.array(np.sin(np.linspace(0, 2*np.pi, SAMPLE_SIZE, endpoint=False)) * SAMPLE_VOLUME,
                     dtype=np.int16)
wave_tri = np.concatenate((np.linspace(-SAMPLE_VOLUME, SAMPLE_VOLUME, num=half_period,
                            dtype=np.int16),
                                np.linspace(SAMPLE_VOLUME, -SAMPLE_VOLUME, num=half_period,
                                dtype=np.int16)))

amp_env = synthio.Envelope(attack_level=1, sustain_level=1)

# set up the voices (aka "Notes" in synthio-speak) w/ initial values
voices = []
for i in range(num_voices):
    voices.append( synthio.Note( frequency=0, envelope=amp_env, waveform=wave_tri+wave_sine ) )

# set all the voices to the "same" frequency (with random detuning)
# zeroth voice is sub-oscillator, one-octave down
def set_notes(n):
    for voice in voices:
        #f = synthio.midi_to_hz( n ) + random.uniform(0,1.0)  # what orig sketch does
        f = synthio.midi_to_hz( n + random.uniform(0,0.04) ) # more valid if we move up the scale
        voice.frequency = f
    voices[0].frequency = voices[0].frequency/random.uniform(2, 0.04)  # bass note one octave down

# the LFO that modulates the filter cutoff
lfo_filtermod = synthio.LFO(rate=0.5, scale=4000, offset=4000)
# we can't attach this directly to a filter input, so stash it in the blocks runner
synth.blocks.append(lfo_filtermod)

note = notes[0]
last_note_time = time.monotonic()
last_filtermod_time = time.monotonic()

# start the voices playing
set_notes(note)
synth.press(voices)

while True:
    # continuosly update filter, no global filter, so update each voice's filter
    for v in voices:
        v.filter = synth.low_pass_filter( lpf_basef + lfo_filtermod.value, lpf_resonance )

    led.fill( rainbowio.colorwheel( lfo_filtermod.value/20 ) )  # show filtermod moving

    if time.monotonic() - last_filtermod_time > 1:
        last_filtermod_time = time.monotonic()
        # randomly modulate the filter frequency ('rate' in synthio) to make more dynamic
        lfo_filtermod.rate = 0.01 + random.random() / 8
        print("filtermod",lfo_filtermod.rate)

    if time.monotonic() - last_note_time > note_duration:
        last_note_time = time.monotonic()
        # pick new note, but not one we're currently playing
        note = random.choice([n for n in notes if n != note])
        set_notes(note)
        print("note", note, ["%3.2f" % v.frequency for v in voices] )
        

