import board
import touchio
from adafruit_debouncer import Debouncer 



class Touch:
    def __init__(self, touch_pins):
        self.touchins = []
        self.touchs = []
        for pin in touch_pins:
            touchin = touchio.TouchIn(pin)
            touchin.threshold = int(touchin.threshold * 1.05)
            self.touchins.append(touchin)
            self.touchs.append(Debouncer(touchin))
        self.old_touches = [False] * self.size()
        self.on_touch = None
        self.on_release = None

    def size(self):
        return len(self.touchs)

    def tick(self):
        [touch.update() for touch in self.touchs]
        new_touches = [touch.value for touch in self.touchs]
        for i, (new, old) in enumerate(zip(new_touches, self.old_touches)):
            if new != old:
                if new and self.on_touch is not None:
                    self.on_touch(i)
                if not new and self.on_release is not None:
                    self.on_release(i)
                self.old_touches[i] = new

    def single_touch(self):
        """Returns the first touch in the order of the pads"""
        self.tick()
        for i, touch in enumerate(self.old_touches):
            if touch:
                return i
        return None
