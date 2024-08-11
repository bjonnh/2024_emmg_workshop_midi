import time, random
import ulab.numpy as np
from adafruit_midi.control_change import ControlChange
from adafruit_midi.note_off import NoteOff
from adafruit_midi.note_on import NoteOn

class MyController:
    def __init__(self, touch, knobs, midi, audio):
        self.touch = touch
        self.knobs = knobs
        self.midi = midi
        self.audio = audio
        self.old_knobs = [int(i)//512 for i in self.knobs.values]
        self.touch.on_touch = self.play_note
        self.touch.on_release = self.release_note

    def play_note(self, touch_pad):
        self.midi.send(NoteOn(40+touch_pad, 127))

    def release_note(self,touch_pad):
        self.midi.send(NoteOff(40+touch_pad, 0))

    def loop(self):
        while True:
            self.midi.tick()
            self.touch.tick()
            self.knobs.tick()

            for i,v in enumerate(zip(self.knobs.values, self.old_knobs)):
                if v[0]//512 != v[1]:
                    self.midi.send(ControlChange(50+i, v[0]//512))
                    self.old_knobs[i] = v[0]//512

