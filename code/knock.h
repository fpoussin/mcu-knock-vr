#include "ch.h"

#define FFT_SIZE 512
#define FFT_SAMPLES (FFT_SIZE*2)
#define FFT_FREQ (117263/2)
#define SPECTRUM_SIZE (FFT_SIZE/2)
#define KNOCK_RATIO 4.66f
#define KNOCK_MAX (3.3*KNOCK_RATIO)
