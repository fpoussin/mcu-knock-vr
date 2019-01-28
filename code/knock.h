#include "ch.h"

#define KNOCK_ADC ADC2
#define KNOCK_ADCD ADCD2
#define KNOCK_OPAMP OPAMP2
#define KNOCK_OPAMPD OPAMPD2

#define KNOCK_DAC DAC2
#define KNOCK_DACD DACD2

#define FFT_SIZE 512
#define FFT_SAMPLES (FFT_SIZE*2) // We need double the FFT size
#define FFT_FREQ (117263/2)
#define SPECTRUM_SIZE (FFT_SIZE/2) // We don't care about the imaginary half
#define KNOCK_RATIO 4.66f // Knock voltage ratio after the divider
#define KNOCK_MAX (3.3*KNOCK_RATIO)
