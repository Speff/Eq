#define SAMPLE_RATE (44100)

#include <stdio.h>

#include "portaudio.h"

typedef struct{
    float left_phase;
    float right_phase;
}   
paTestData;

static int paTestCallback(
		const void *,
		void *,
		unsigned long,
		const PaStreamCallbackTimeInfo*,
		PaStreamCallbackFlags,
		void *);
