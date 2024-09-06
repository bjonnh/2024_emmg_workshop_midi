import ulab.numpy as np

def gen_sine(size: 256, volume: 1.0):
    return np.array(np.sin(np.linspace(0, 2 * np.pi, size, endpoint=False)) * volume, dtype=np.int16)

def gen_tri(size: 256, volume: 1.0):
    return np.concatenate((np.linspace(-volume, volume, num=size//2, dtype=np.int16),
                           np.linspace(volume, -volume, num=size//2, dtype=np.int16)))
