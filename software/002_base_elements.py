import time, random
import board, busio,  audiopwmio, audiomixer, synthio, audiobusio, analogio
import ulab.numpy as np
import neopixel, rainbowio   # circup install neopixel
import displayio, terminalio
import adafruit_displayio_ssd1306
from adafruit_display_text import bitmap_label as label
import adafruit_midi
import adafruit_wave
import supervisor
from adafruit_midi.note_on import NoteOn
from adafruit_midi.note_off import NoteOff
#supervisor.runtime.autoreload = False

i2c_scl_pin   = board.GP3
i2c_sda_pin   = board.GP2
knobA_pin = board.GP28
displayio.release_displays()
i2c = busio.I2C(scl=i2c_scl_pin, sda=i2c_sda_pin, frequency=1_000_000)

dw,dh = 128, 64
display_bus = displayio.I2CDisplay(i2c, device_address=0x3c)
display = adafruit_displayio_ssd1306.SSD1306(display_bus, width=dw, height=dh, rotation=180)

knobA = analogio.AnalogIn(knobA_pin)
maingroup = displayio.Group()
display.root_group = maingroup
print("Ready for display")
text1 = label.Label(terminalio.FONT, text="EMMG 2024", x=0, y=10)
text2 = label.Label(terminalio.FONT, text="---------", x=0, y=25)
text3 = label.Label(terminalio.FONT, text="Midisynth workshop", x=0, y=54)
for t in (text1, text2, text3):
    maingroup.append(t)
time.sleep(1)

audio = audiobusio.I2SOut(bit_clock=board.GP7, word_select=board.GP8, data=board.GP6)

midi_in_channel = 1
midi_out_channel = 1
uart = busio.UART(board.GP12, board.GP13, baudrate=31250, timeout=0.001)  
midi = adafruit_midi.MIDI(
    midi_in=uart,
    midi_out=uart,
    in_channel=(midi_in_channel - 1),
    out_channel=(midi_out_channel - 1),
    debug=False,
)

num_voices = 1   # how many voices for each note
lpf_basef = 100     # filter lowest frequency
lpf_resonance = 2.0  # filter q

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
wave_sine = np.array(np.sin(np.linspace(0, 2*np.pi, SAMPLE_SIZE, endpoint=False)) * SAMPLE_VOLUME,
                     dtype=np.int16)
wave_tri = np.concatenate((np.linspace(-SAMPLE_VOLUME, SAMPLE_VOLUME, num=half_period,
                            dtype=np.int16),
                                np.linspace(SAMPLE_VOLUME, -SAMPLE_VOLUME, num=half_period,
                                dtype=np.int16)))
def lerp(a, b, t):  return (1-t)*a + t*b

class Wavetable:
    """ A 'waveform' for synthio.Note that uses a wavetable w/ a scannable wave position."""
    def __init__(self, filepath, wave_len=256):
        self.w = adafruit_wave.open(filepath)
        self.wave_len = wave_len  # how many samples in each wave
        if self.w.getsampwidth() != 2 or self.w.getnchannels() != 1:
            raise ValueError("unsupported WAV format")
        self.waveform = np.zeros(wave_len, dtype=np.int16)  # empty buffer we'll copy into
        self.num_waves = self.w.getnframes() // self.wave_len
        self.set_wave_pos(0)

    def set_wave_pos(self, pos):
        """Pick where in wavetable to be, morphing between waves"""
        pos = min(max(pos, 0), self.num_waves-1)  # constrain
        samp_pos = int(pos) * self.wave_len  # get sample position
        self.w.setpos(samp_pos)
        waveA = np.frombuffer(self.w.readframes(self.wave_len), dtype=np.int16)
        self.w.setpos(samp_pos + self.wave_len)  # one wave up
        waveB = np.frombuffer(self.w.readframes(self.wave_len), dtype=np.int16)
        pos_frac = pos - int(pos)  # fractional position between wave A & B
        self.waveform[:] = lerp(waveA, waveB, pos_frac)  # mix waveforms A & B

wave_lfo = synthio.LFO(rate=0.1, waveform=np.array((0,32767), dtype=np.int16) )
synth.blocks.append(wave_lfo)
wavetable1 = Wavetable(wavetable_fname, wave_len=wavetable_sample_size)
amp_env = synthio.Envelope(attack_time=0.1, release_time=0.2, sustain_level=0.95, attack_level=0.95)

# set up the voices (aka "Notes" in synthio-speak) w/ initial values
voices = []
def set_wave_lfo_minmax(wmin, wmax):
    scale = (wmax - wmin)
    wave_lfo.scale = scale
    wave_lfo.offset = wmin
wave_lfo_min = 10  # which wavetable number to start from
wave_lfo_max = 25  # which wavetable number to go up to

set_wave_lfo_minmax(wave_lfo_min, wave_lfo_max)

bend_mod = synthio.LFO(rate=0.1, scale=0.01, offset=0)
for i in range(num_voices):
    wave_tri = np.concatenate((np.linspace(-SAMPLE_VOLUME, SAMPLE_VOLUME, num=SAMPLE_SIZE//(i+2),
                            dtype=np.int16),
                                np.linspace(SAMPLE_VOLUME, -SAMPLE_VOLUME, num=SAMPLE_SIZE-SAMPLE_SIZE//(i+2),
                                dtype=np.int16)))

    voices.append( synthio.Note( frequency=0, envelope=amp_env, waveform=wavetable1.waveform , bend=bend_mod )) # wavetable1.waveform+wave_tri ) )

# set all the voices to the "same" frequency (with random detuning)
# zeroth voice is sub-oscillator, one-octave down
def set_notes(n):
    for voice in voices:
        #f = synthio.midi_to_hz( n ) + random.uniform(0,1.0)  # what orig sketch does
        f = synthio.midi_to_hz( n + random.uniform(0,1.0) ) # more valid if we move up the scale
        voice.frequency = f
    voices[0].frequency = voices[0].frequency/2  # bass note one octave down

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


def get_note(old_note):
    msg_in = midi.receive()
    if isinstance(msg_in, NoteOn) and msg_in.velocity != 0:
        note = msg_in.note
        return note
    if isinstance(msg_in, NoteOff) or (isinstance(msg_in, NoteOn) and msg_in.velocity == 0):
        return 0
    return -1
old_note = 0

while True:
    note = get_note(old_note)
    print("note: ", note)
    filter_freq = (knobA.value + lfo_filtermod.value+200) / 256 * lpf_basef 
    if (filter_freq > SAMPLE_RATE/2.1):
        filter_freq = SAMPLE_RATE/2.1

    for v in voices:
        if note != -1:
            set_voice_note(v, note)
        else:
            pass
            #set_voice_note(v, old_note)
        v.filter = synth.low_pass_filter( filter_freq , lpf_resonance )


    if note != -1:
        if note == 0 and old_note != 0:
            print("Release")
            synth.release(voices)
        else:
            if old_note == 0:
                print("Press")
                synth.press(voices)
        old_note = note
    if time.monotonic() - last_filtermod_time > 1:
        wavetable1.set_wave_pos( wave_lfo.value )
        last_filtermod_time = time.monotonic()
        # randomly modulate the filter frequency ('rate' in synthio) to make more dynamic
        #lfo_filtermod.rate = lfo_filtermod.rate + (random.random()-0.5) / 8

