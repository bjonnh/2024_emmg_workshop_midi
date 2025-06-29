The controller works fine using default parameters. But the synthesizer is much happier overclocking the pico around 200MHz (I tested at 240MHz and it was fine)


The synthesizer is coming from the excellent:
https://github.com/risgk/digital-synth-pra32-u

I am also using the hysteris filter from:
https://github.com/tttapa/Control-Surface

## Synthesizer MIDI CC Parameters

The synthesizer can be controlled via MIDI CC messages on the configured MIDI channel. Here are all available CC parameters:

### Page 1: Oscillators
- CC 102: Osc 1 Wave (O1W)
- CC 19: Osc 1 Shape (O1S)
- CC 20: Osc 1 Morph (O1M)
- CC 21: Mixer Osc Mix (MXO)
- CC 104: Osc 2 Wave (O2W)
- CC 85: Osc 2 Coarse (O2C)
- CC 76: Osc 2 Pitch (O2P)

### Page 2: Envelope
- CC 24: Filter EG amount (FEG)
- CC 91: EG Osc Amt (EOA)
- CC 89: EG Osc Dst (EOD)
- CC 73: EG Attack (EGA)
- CC 75: EG Decay (EGD)
- CC 30: EG Sustain (EGS)
- CC 72: EG Release (EGR)

### Page 3: LFO
- CC 13: LFO Osc Amt (LOA)
- CC 103: LFO Osc Dst (LOD)
- CC 25: LFO Filter Amt (LFA)
- CC 12: LFO Wave (LFW)
- CC 3: LFO Rate (LFR)
- CC 17: LFO Depth (LFD)
- CC 56: LFO Fade Time (LFF)

### Page 4: Amp
- CC 15: Amp Gain (AG)
- CC 28: EG Amp Mod (EAM)
- CC 61: Breath Amp Mod (BAM)
- CC 52: Amp Attack (AMA)
- CC 53: Amp Decay (AMD)
- CC 54: Amp Sustain (AMS)
- CC 55: Amp Release (AMR)

### Page 5: Chorus/Delay
- CC 27: Chorus Mix (CMX)
- CC 58: Chorus Rate (CRT)
- CC 59: Chorus Depth (CDP)
- CC 92: Delay Feedback (DFB)
- CC 90: Delay Time (DTM)
- CC 35: Delay Mode (DMD)

### Page 6: Filters/breath
- CC 26: Filter Key track (FKT)
- CC 74: Filter cutoff (FCO)
- CC 71: Filter resonance (FRE)
- CC 78: Filter Mode (FMO)
- CC 60: Breath Filter Amt (BFA)
- CC 2: Breath (BRH)

### Page 7: Others
- CC 23: Mixer noise/sub (MXN)
- CC 29: Release = Decay (RDC)
- CC 57: Pitch Bend Range (PBR)
- CC 62: EG Velocity Sensitivity (EVS)
- CC 63: Amp Velocity Sensitivity (AVS)

### Additional Parameters
- CC 1: Modulation (MOD)
- CC 14: Voice Mode (VMO)
- CC 5: Portamento (POR)
- CC 87: Program Number to Write to (PNW)
- CC 106: Write Parameters to Program (WPP)


