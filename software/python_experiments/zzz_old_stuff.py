wave_lfo = synthio.LFO(rate=0.1, waveform=np.array((0,32767), dtype=np.int16) )
synth.blocks.append(wave_lfo)

def set_wave_lfo_minmax(wmin, wmax):
    scale = (wmax - wmin)
    wave_lfo.scale = scale
    wave_lfo.offset = wmin
wave_lfo_min = 10  # which wavetable number to start from
wave_lfo_max = 25  # which wavetable number to go up to

set_wave_lfo_minmax(wave_lfo_min, wave_lfo_max)

wave_tri = np.concatenate((np.linspace(-SAMPLE_VOLUME, SAMPLE_VOLUME, num=SAMPLE_SIZE // (i + 2),
                                       dtype=np.int16),
                           np.linspace(SAMPLE_VOLUME, -SAMPLE_VOLUME, num=SAMPLE_SIZE - SAMPLE_SIZE // (i + 2),
                                       dtype=np.int16)))


