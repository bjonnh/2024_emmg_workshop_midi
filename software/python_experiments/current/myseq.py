import time, random
import synthio
import audiomixer
from instruments.kick import KickDrum
import ulab.numpy as np

from wavetable import Wavetable
from wave_generators import gen_sine, gen_tri

class MySeq:
    def __init__(self, display, touch, knobs, midi, audio):
        self.display = display
        self.touch = touch
        self.knobs = knobs
        self.midi = midi
        self.audio = audio

        self.SAMPLE_RATE=28000
        self.mixer = audiomixer.Mixer(channel_count=2, sample_rate=self.SAMPLE_RATE, buffer_size=2048, bits_per_sample=16,
                                      samples_signed=True)
        self.synth = synthio.Synthesizer(channel_count=2, sample_rate=self.SAMPLE_RATE)
        audio.play(self.mixer)
        self.mixer.voice[0].play(self.synth)
        self.mixer.voice[0].level = 1.0

        self.kickdrum = KickDrum(self.synth)

        self.last_wave_pos = -1
        self.touch.on_touch = self.play_note
        self.touch.on_release = self.release_note
        self.last_knob_time = time.monotonic()
        self.last_kick_time = time.monotonic()

    def play_note(self, touch_pad):
        if touch_pad == 8:
            self.kickdrum.play()


    def release_note(self,touch_pad):
        if touch_pad == 8:
            self.kickdrum.release()

    def loop(self):
        while True:
            #self.midi.tick()
            #self.touch.tick()
            if time.monotonic() - self.last_kick_time > 0.5:
                self.kickdrum.play()
                self.last_kick_time = time.monotonic()

            if time.monotonic() - self.last_knob_time > 0.1:
                self.knobs.tick()
                self.last_knob_time = time.monotonic()
                new_filter_value = (self.knobs.values[1] * self.SAMPLE_RATE  / 2) // 65536
                self.kickdrum.setLPF(new_filter_value)
                new_lfo_rate = (self.knobs.values[2] * 100) // 65536
                self.kickdrum.lfo.rate = new_lfo_rate
                new_lfo_offset = (self.knobs.values[3]) / 65536
                self.kickdrum.lfo.offset = new_lfo_offset
                new_lfo_scale = (self.knobs.values[4]) / 65536
                self.kickdrum.lfo.scale = new_lfo_scale
                # self.display.clear()
                # self.display.print("F: {}".format(new_filter_value))
                # self.display.print("LFO: {} Hz".format(new_lfo_rate))
