import adafruit_displayio_ssd1306
import busio
import displayio
import terminalio
from adafruit_display_text import bitmap_label as label

class Display:
    def __init__(self, screen_scl_pin, screen_sda_pin):
        displayio.release_displays()
        screen_i2c = busio.I2C(scl=screen_scl_pin, sda=screen_sda_pin, frequency=1_000_000)

        dw,dh = 128, 64
        display_bus = displayio.I2CDisplay(screen_i2c, device_address=0x3c)
        self.display = adafruit_displayio_ssd1306.SSD1306(display_bus, width=dw, height=dh, rotation=180)

        self.maingroup = displayio.Group()
        self.display.root_group = self.maingroup
        text1 = label.Label(terminalio.FONT, text="EMMG 2024", x=0, y=10)
        text2 = label.Label(terminalio.FONT, text="---------", x=0, y=25)
        text3 = label.Label(terminalio.FONT, text="Midisynth workshop", x=0, y=54)
        for t in (text1, text2, text3):
            self.maingroup.append(t)
        self.current_y = 10

    def clear(self):
        self.current_y = 10
        for item in range(len(self.maingroup)):
            self.maingroup.pop()

    def print(self, text):
        text = label.Label(terminalio.FONT, text=text, x=0, y=self.current_y)
        self.maingroup.append(text)
        self.current_y += 15

