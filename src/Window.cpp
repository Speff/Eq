#include "window.h"


int main(){
	PaStream *audioStream;
	PaError paErr;

	static paTestData data;

	paErr = Pa_Initialize();
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));

	// Open an audio stream
	paErr = Pa_OpenDefaultStream(
			&audioStream,
			0,              // Number of input channels
			2,              // Number of output channels
			paFloat32,      // 32-bit floating point output
			SAMPLE_RATE,
			256,            // frames per buffer, ie. the number
			                // of sample frames PA will request
			                // from callback function
			paTestCallback, // cb function
			&data);         // Pointer passed to cb
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));

	paErr = Pa_StartStream(audioStream);
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));

	printf("Hello World\n");
	while(1);

	paErr = Pa_StopStream(audioStream);
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));

	paErr = Pa_Terminate();
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));
	return 1;
}

static int paTestCallback(
		const void *inputBuffer,
		void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData){

	//paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	float *in = (float*)inputBuffer;
	(void) in;
	float avgLeft = 0;
	float avgRight = 0;

	for(unsigned int i = 0; i < framesPerBuffer; i++){
		avgLeft += *out * *out;
		out++;
		avgRight += *out * *out;
		out++;
	}

	printf("%f\t%f\n", avgLeft, avgRight);

	return 0;
}
