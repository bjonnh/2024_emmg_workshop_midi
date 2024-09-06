import analogio, digitalio


class LowPassFilter:
    def __init__(self, alpha=0.05, delta=1024):  
        # alpha is a smoothing factor, 0 < alpha < 1, the lowest the more it filters but the slower it responds
        # delta is the minimum difference between values to consider an immediate change skipping the filter
        self.alpha = alpha
        self.delta = delta
        self.filtered_value = None

    def apply(self, value):
        if self.filtered_value is None:
            self.filtered_value = value
        else:
            if abs(value - self.filtered_value) > self.delta:
                self.filtered_value = value
            else:
                self.filtered_value = self.alpha * value + (1 - self.alpha) * self.filtered_value
        return self.filtered_value

class Knobs:
    def __init__(self, analog_gpio, address_gpios, knobs_addresses):
        """
        analog_pin is the board.GPxx of the analog input pin
        address_gpios is a list of board.GPxx for the addresses of the multiplexer
        knobs_addresses list of tuples with the addresses of the knobs
        """
        # Check length of knobs_addresses
        for address in knobs_addresses:
            if len(address) != len(address_gpios):
                raise ValueError("Knobs addresses must have the same length as address gpios")
        self.analog_in = analogio.AnalogIn(analog_gpio)
        self.address_pins = [digitalio.DigitalInOut(pin) for pin in address_gpios]
        self.knobs_addresses = knobs_addresses
        self.filters = [LowPassFilter() for _ in knobs_addresses]
        self.values = [0] * len(knobs_addresses)
        for pin in self.address_pins:
            pin.direction = digitalio.Direction.OUTPUT
        self.tick()

    def tick(self):
        for i in range(len(self.knobs_addresses)):
            self.read(i)
        return self.values

    def read(self, knob):
        if knob >= len(self.knobs_addresses):
            raise ValueError("Knob index out of range, they start at 0")
        for j, pin in enumerate(self.address_pins):
            pin.value = self.knobs_addresses[knob][j]
        raw_value = 65535 - self.analog_in.value  
        self.values[knob] = int(self.filters[knob].apply(raw_value))
        return self.values[knob]

