#ifndef EMMG_CONFIG_H
#define EMMG_CONFIG_H

#define NUMBER_OF_KNOBS 8
#define NUMBER_OF_PADS 12

#define KNOBS_PINS { 2, 3, 4 }
#define KNOBS_ANALOG_PIN A2
#define KNOBS_ANALOG_PIN_INDEX 2 // make sure it matches the pin above, ADC gpios are all together in a block
#define KNOBS_ADDRESS_SIZE 3
#define KNOBS_ADDRESSES { { 1, 1, 1 }, { 1, 0, 1 }, { 1, 1, 0 }, { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 }, { 1, 0, 0 } }

#define PADS_PINS {14, 11, 8, 16, 15, 12, 9, 6, 13, 10, 7, 17}
#endif // EMMG_CONFIG_H