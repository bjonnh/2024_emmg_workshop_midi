import time, random
import synthio
import audiomixer
import ulab.numpy as np
import audiocore
from audiomp3 import MP3Decoder


from wavetable import Wavetable
from wave_generators import gen_sine, gen_tri

class MySampler:
    def __init__(self, touch, knobs, midi, audio):
        self.touch = touch
        self.knobs = knobs
        self.midi = midi
        self.audio = audio

    def set_voice_note(self, voice, n):
        # Changing that spread of the random is pretty cool
        voice.frequency = synthio.midi_to_hz( n + self.bend_mod.value) # random.uniform(0,100.0)

    def play_note(self, touch_pad):
        new_note = self.base_note + touch_pad
        if self.old_notes[touch_pad] != new_note:
            self.set_voice_note(self.voices[touch_pad], new_note)
            self.synth.press([self.voices[touch_pad]])
            self.old_notes[touch_pad] = new_note

    def release_note(self,touch_pad):
        self.synth.release([self.voices[touch_pad]])
        self.old_notes[touch_pad] = -1

    def loop(self):
        while True:
            mp3 = open("wav/demo0.mp3", "rb")
            decoder = MP3Decoder(mp3)
            self.audio.play(decoder)
            while self.audio.playing:
                pass


