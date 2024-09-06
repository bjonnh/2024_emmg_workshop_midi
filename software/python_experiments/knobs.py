import analogio, digitalio

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
        self.values = [0] * len(knobs_addresses)
        for pin in self.address_pins:
            pin.direction = digitalio.Direction.OUTPUT
        self.tick()

    def tick(self):
        for i, address in enumerate(self.knobs_addresses):
            for j, pin in enumerate(self.address_pins):
                pin.value = address[j]
            self.values[i] = 65535 - self.analog_in.value
        return self.values

    def read(self, knob):
        if knob >= len(self.knobs_addresses):
            raise ValueError("Knob index out of range, they start at 0")
        for j, pin in enumerate(self.address_pins):
            pin.value = self.knobs_addresses[knob][j]
            
        self.values[knob] = 65535 - self.analog_in.value
        return self.values[knob]

