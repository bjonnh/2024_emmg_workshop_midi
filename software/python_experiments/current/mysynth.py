import time, random
import synthio
import audiomixer
import ulab.numpy as np

from wavetable import Wavetable
from wave_generators import gen_sine, gen_tri

class MySynth:
    def __init__(self, touch, knobs, midi, audio):
        self.touch = touch
        self.knobs = knobs
        self.midi = midi
        self.audio = audio
        self.lpf_basef = 100     # filter lowest frequency

        self.SAMPLE_RATE=44100
        self.mixer = audiomixer.Mixer(channel_count=1, sample_rate=self.SAMPLE_RATE, buffer_size=2048)
        self.synth = synthio.Synthesizer(channel_count=1, sample_rate=self.SAMPLE_RATE)
        audio.play(self.mixer)
        self.mixer.voice[0].play(self.synth)
        self.mixer.voice[0].level = 0.25

        SAMPLE_VOLUME = 4000  # 0-32767
        wavetable_fname = "wav/PLAITS02.WAV"  # from http://waveeditonline.com/index-17.html
        wavetable_sample_size = 256 # number of samples per wave in wavetable (256 is standard)
        SAMPLE_SIZE = wavetable_sample_size

        half_period = SAMPLE_SIZE // 2
        wave_sine = gen_sine(SAMPLE_SIZE, SAMPLE_VOLUME)
        wave_tri =  gen_tri(SAMPLE_SIZE, SAMPLE_VOLUME)

        self.wavetable1 = Wavetable(wavetable_fname, wave_len=wavetable_sample_size)
        amp_env = synthio.Envelope(attack_time=0.1, release_time=0.2, sustain_level=0.95, attack_level=0.95)

        self.voices = []

        self.bend_mod = synthio.LFO(rate=0.1, scale=0.01, offset=0)
        for i in range(len(self.touch.touchs)):
           self.voices.append(synthio.Note(frequency=0, envelope=amp_env, waveform=self.wavetable1.waveform, bend=self.bend_mod))
        # the LFO that modulates the filter cutoff
        self.lfo_filtermod = synthio.LFO(rate=1, scale=100, offset=100)
        # we can't attach this directly to a filter input, so stash it in the blocks runner
        self.synth.blocks.append(self.lfo_filtermod)

        self.last_note_time = time.monotonic()
        self.last_filtermod_time = time.monotonic()

        self.base_note = 0
        self.old_notes = [-1] * self.touch.size()
        self.last_wave_pos = -1
        self.touch.on_touch = self.play_note
        self.touch.on_release = self.release_note

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
            self.midi.tick()
            self.touch.tick()
            self.base_note = self.knobs.values[3] // 1024

            # We don't need to run controllers updates every turn
            if time.monotonic() - self.last_filtermod_time > 0.1:
                self.knobs.tick()
                filter_freq = (self.knobs.values[1] + self.lfo_filtermod.value + 200) / 256 * self.lpf_basef
                if filter_freq > self.SAMPLE_RATE / 2.1:
                    filter_freq = self.SAMPLE_RATE / 2.1

                lpf_resonance = max(0, (self.knobs.values[6] - 256) / 8192)
                for v in self.voices:
                    v.filter = self.synth.low_pass_filter(filter_freq, lpf_resonance)

                wave_pos = self.knobs.values[2]//4096+15
                if wave_pos != self.last_wave_pos:
                    self.wavetable1.set_wave_pos(wave_pos)
                    self.last_wave_pos = wave_pos
                self.last_filtermod_time = time.monotonic()

                self.bend_mod.rate = max(0, (self.knobs.values[4] - 256)  / 8192)
                self.bend_mod.scale = max(0, (self.knobs.values[5] - 256) / 8192)

                # randomly modulate the filter frequency ('rate' in synthio) to make more dynamic
                #lfo_filtermod.rate = lfo_filtermod.rate + (random.random()-0.5) / 8

