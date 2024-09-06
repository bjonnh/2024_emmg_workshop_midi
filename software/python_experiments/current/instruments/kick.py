# Inspired from https://gist.github.com/gamblor21/15a430929abf0e10eeaba8a45b01f5a8
import ulab.numpy as np
import random
import synthio

SAMPLE_SIZE = 4096
sinwave1 = None
sinwave2 = None
downwave = None

def get_sinwave1():
    global sinwave1

    if sinwave1 is None:
        sinwave1 = np.array(np.sin(np.linspace(0, 2*np.pi, SAMPLE_SIZE, endpoint=False)) * 4096, dtype=np.int16)

    return sinwave1
def get_sinwave2():
    global sinwave2

    if sinwave2 is None:
        sinwave2 = np.array(np.sin(np.linspace(0.5*np.pi, 2.5*np.pi, SAMPLE_SIZE, endpoint=False)) * 4096, dtype=np.int16)
    return sinwave2

def get_downwave():
    global downwave

    if downwave is None:
        downwave = np.linspace(32767, -32767, num=256, dtype=np.int16)

    return downwave


class KickDrum:
    def __init__(self, synth):
        self.synth = synth

        self.lfo = synthio.LFO(waveform=get_downwave())
        self.lfo.once = True
        self.lfo.offset = 0.33
        self.lfo.scale = 0.3
        self.lfo.rate = 20

        self.filter_fr = 2000
        self.lpf = self.synth.low_pass_filter(frequency=self.filter_fr)

        self.amp_env1 = synthio.Envelope(attack_time=0.4, decay_time=0.475, release_time=0, attack_level=1,
                                         sustain_level=0)
        self.note1 = synthio.Note(frequency=53, envelope=self.amp_env1, waveform=get_sinwave2(),
                                 filter=self.lpf)
                                 #bend=self.lfo)

        self.amp_env2 = synthio.Envelope(attack_time=0.4, decay_time=0.455, release_time=0, attack_level=1,
                                         sustain_level=0)
        self.note2 = synthio.Note(frequency=72, envelope=self.amp_env2, waveform=get_sinwave1(), filter=self.lpf,
                                  bend=self.lfo)

        self.amp_env3 = synthio.Envelope(attack_time=0.4, decay_time=0.495, release_time=0, attack_level=1,
                                         sustain_level=0)
        self.note3 = synthio.Note(frequency=41, envelope=self.amp_env3, waveform=get_sinwave2(), filter=self.lpf,
                                  bend=self.lfo)

    def setLPF(self, fr):
        if self.filter_fr != fr:
            self.filter_fr = fr
            self.lpf = self.synth.low_pass_filter(frequency=self.filter_fr)
            self.note1.filter = self.lpf
            self.note2.filter = self.lpf
            self.note3.filter = self.lpf

    def play(self, synth=None):
        if synth is None:
            synth = self.synth
        self.lfo.retrigger()
        synth.press((self.note1, self.note2, self.note3))

    def release(self):
        self.synth.release([self.note1, self.note2, self.note3])