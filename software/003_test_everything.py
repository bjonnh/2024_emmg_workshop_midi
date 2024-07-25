import time, random
import board, busio,  audiopwmio, audiomixer, synthio, audiobusio, analogio, digitalio
from knobs import Knobs
import touchio
import ulab.numpy as np
import neopixel, rainbowio   # circup install neopixel
import displayio, terminalio
import adafruit_displayio_ssd1306
from adafruit_display_text import bitmap_label as label
import adafruit_midi
from adafruit_debouncer import Debouncer
import adafruit_wave
import supervisor
from adafruit_midi.note_on import NoteOn
from adafruit_midi.note_off import NoteOff
#supervisor.runtime.autoreload = False

i2c_scl_pin   = board.GP19
i2c_sda_pin   = board.GP18

knobs_analog_pin = board.GP28
knobs_address_gpios = (board.GP2, board.GP3, board.GP4)
knobs_addresses = [(1,1,1), (1,0,1), (1,1,0), (0,0,0),
                   (0,0,1), (0,1,1), (0,1,0), (1,0,0)]
knobs = Knobs(knobs_analog_pin, knobs_address_gpios, knobs_addresses)

touch_pins = touch_pins = (
    board.GP14, board.GP11, board.GP8, board.GP16, board.GP15, board.GP12,
    board.GP9, board.GP6 ,board.GP13, board.GP10, board.GP7, board.GP17)

touchins = []
touchs = []
for pin in touch_pins:
    touchin = touchio.TouchIn(pin)
    touchin.threshold = int(touchin.threshold * 1.05)
    touchins.append(touchin)
    touchs.append(Debouncer(touchin))

displayio.release_displays()
i2c = busio.I2C(scl=i2c_scl_pin, sda=i2c_sda_pin, frequency=1_000_000)

dw,dh = 128, 64
display_bus = displayio.I2CDisplay(i2c, device_address=0x3c)
display = adafruit_displayio_ssd1306.SSD1306(display_bus, width=dw, height=dh, rotation=180)

maingroup = displayio.Group()
display.root_group = maingroup
text1 = label.Label(terminalio.FONT, text="EMMG 2024", x=0, y=10)
text2 = label.Label(terminalio.FONT, text="---------", x=0, y=25)
text3 = label.Label(terminalio.FONT, text="Midisynth workshop", x=0, y=54)
for t in (text1, text2, text3):
    maingroup.append(t)
time.sleep(1)

audio = audiobusio.I2SOut(bit_clock=board.GP21, word_select=board.GP20, data=board.GP22)

midi_in_channel = 1
midi_out_channel = 1
uart = busio.UART(board.GP0, board.GP1, baudrate=31250, timeout=0.001)  
midi = adafruit_midi.MIDI(
    midi_in=uart,
    midi_out=uart,
    in_channel=(midi_in_channel - 1),
    out_channel=(midi_out_channel - 1),
    debug=False,
)

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
for i in range(len(touch_pins)):
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
        #f = synthio.midi_to_hz( n + random.uniform(0,1.0) ) # more valid if we move up the scale
        f = 600.0
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

old_notes = [-1] * len(touchs)
def play_touches(base_note):
    global old_notes, last_base_note
    [touch.update() for touch in touchs]
    new_touches = [touch.value for touch in touchs]
    for i, (new, old) in enumerate(zip(new_touches, old_notes)):
        new_note = base_note + i
        if new and (old != new_note):
            print("Playing note", new_note)
            set_voice_note(voices[i], new_note)
            synth.press([voices[i]])
            old_notes[i] = new_note
        elif old != -1 and not new:
            synth.release([voices[i]])
            old_notes[i] = -1
    last_touch = new_touches


def get_note(old_note):
    msg_in = midi.receive()
    if isinstance(msg_in, NoteOn) and msg_in.velocity != 0:
        note = msg_in.note
        return note
    if isinstance(msg_in, NoteOff) or (isinstance(msg_in, NoteOn) and msg_in.velocity == 0):
        return 0
    return -1
old_note = 0

last_wave_pos = -1

while True:
    knobs.read_all()
    play_touches(base_note = knobs.values[3]//1024)
    #print("T:" + ''.join(["%3d " % (t.raw_value//16) for t in touchins]))
    #print("K:" + ''.join(["%4d " % (k//16) for k in knobs.read_all()]))
    
    filter_freq = (knobs.values[1] + lfo_filtermod.value+200) / 256 * lpf_basef 
    if (filter_freq > SAMPLE_RATE/2.1):
        filter_freq = SAMPLE_RATE/2.1

    lpf_resonance = max(0, (knobs.values[6] - 256) / 8192)
    for v in voices:
        v.filter = synth.low_pass_filter( filter_freq , lpf_resonance )


    if time.monotonic() - last_filtermod_time > 0.1:
        wave_pos = knobs.values[2]//4096+15
        if wave_pos != last_wave_pos:
            print("wave_pos", wave_pos)
            wavetable1.set_wave_pos(wave_pos)
            last_wave_pos = wave_pos
        last_filtermod_time = time.monotonic()

        bend_mod.rate = max(0, (knobs.values[4] - 256)  / 8192)
        bend_mod.scale = max(0, (knobs.values[5] - 256) / 8192)
        print("bend_mod", bend_mod.rate, bend_mod.scale)

        # randomly modulate the filter frequency ('rate' in synthio) to make more dynamic
        #lfo_filtermod.rate = lfo_filtermod.rate + (random.random()-0.5) / 8

