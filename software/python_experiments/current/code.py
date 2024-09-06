import time, random
import board, audiobusio, analogio, digitalio
from knobs import Knobs
import ulab.numpy as np
import supervisor

from display import Display
from midi import Midi
from myseq import MySeq
from touch import Touch

from mysynth import MySynth
from mysampler import MySampler
from mycontroller import MyController

# If you want to disable auto-reloading
# If there is a file "reload.txt" ignore thatl ine
try:
    with open("reload.txt") as f:
        pass
except:
    supervisor.runtime.autoreload = False

screen_scl_pin   = board.GP19
screen_sda_pin   = board.GP18
display = Display(screen_scl_pin, screen_sda_pin)

knobs_analog_pin = board.GP28
knobs_address_gpios = (board.GP2, board.GP3, board.GP4)
knobs_addresses = [(1,1,1), (1,0,1), (1,1,0), (0,0,0), (0,0,1), (0,1,1), (0,1,0), (1,0,0)]
knobs = Knobs(knobs_analog_pin, knobs_address_gpios, knobs_addresses)

touch_pins = (
    board.GP14, board.GP11, board.GP8, board.GP16, 
    board.GP15, board.GP12, board.GP9, board.GP6,
    board.GP13, board.GP10, board.GP7, board.GP17)
touch = Touch(touch_pins)

midi_tx_pin = board.GP0
midi_rx_pin = board.GP1
midi = Midi(midi_tx_pin, midi_rx_pin)

audio = audiobusio.I2SOut(bit_clock=board.GP21, word_select=board.GP20, data=board.GP22)

time.sleep(1)
display.clear()
display.print("Touch a pad to start")
display.print("1: Syn 2: Smp 3: Cnt")
display.print("4: Seq")
touched = None
while touched is None:
    touched = touch.single_touch()
    if touched == 0:
        display.clear()
        display.print("Synthesizing")
        mySynth = MySynth(touch, knobs, midi, audio)
        mySynth.loop()
    elif touched == 1:
        display.clear()
        display.print("Sampler")
        mySampler = MySampler(touch, knobs, midi, audio)
        mySampler.loop()
    elif touched == 2:
        display.clear()
        display.print("Controller")
        myController = MyController(touch, knobs, midi, audio)
        myController.loop()
    elif touched == 3:
        display.clear()
        mySeq = MySeq(display, touch, knobs, midi, audio)
        mySeq.loop()
    else:
        touched = None
