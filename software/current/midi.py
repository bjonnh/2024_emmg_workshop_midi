import adafruit_midi
import busio
from adafruit_midi.note_on import NoteOn
from adafruit_midi.note_off import NoteOff


class Midi:
    def __init__(self, tx_pin, rx_pin, midi_in_channel=1, midi_out_channel=1):
        uart = busio.UART(tx_pin, rx_pin, baudrate=31250, timeout=0.001)
        self.midi = adafruit_midi.MIDI(
            midi_in=uart,
            midi_out=uart,
            in_channel=(midi_in_channel - 1),
            out_channel=(midi_out_channel - 1),
            debug=False,
        )
        self.on_note_on = None
        self.on_note_off = None
        self.on_cc = None

    def tick(self):
        msg_in = self.midi.receive()
        if isinstance(msg_in, NoteOn) and msg_in.velocity != 0:
            if self.on_note_on is not None:
                self.on_note_on(msg_in.note, msg_in.velocity)
        if isinstance(msg_in, NoteOff) or (isinstance(msg_in, NoteOn) and msg_in.velocity == 0):
            if self.on_note_off is not None:
                self.on_note_off(msg_in.note, msg_in.velocity)

