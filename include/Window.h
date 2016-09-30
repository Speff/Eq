#define SAMPLE_RATE (44100)
#define AUDIO_FPB   (256)
#define NLINES      (128)

#include <stdio.h>
#include <math.h>
#include <Windows.h>

#include "portaudio.h"
#include <gl_core_3_3.h>
#include <GLFW/glfw3.h>

typedef struct{
    float left_phase;
    float right_phase;
} paTestData;

typedef struct{
    float *x;
    float *y;
} coord;

typedef struct{
    GLfloat startX;
    GLfloat startY;
    GLfloat sizeX;
    GLfloat sizeY;
    GLuint nVerts;
    GLuint nElements;
    GLuint nLines;
    size_t size;
    coord *vertStart;
    coord *vertEnd;
    GLfloat *lineMag;
} eqFFTProps;

static int paTestCallback(
		const void *,
		void *,
		unsigned long,
		const PaStreamCallbackTimeInfo*,
		PaStreamCallbackFlags,
		void *);

static void initGLBoilerplate(int, int, GLuint*);
static void initAudio(PaStream*, paTestData*);
//GLfloat* initLineVerts(GLuint, size_t, GLfloat, GLfloat, GLfloat, GLfloat);
GLfloat* initLineVerts(eqFFTProps*);
void updateVerts(eqFFTProps*);
static void destroyAudio(PaStream*);

static void key_pressed(GLFWwindow*, int, int, int, int);
static void window_resized(GLFWwindow*, int, int);
static void setProjectionMatrix(GLuint*, int, int);
static void glPrintError(GLenum);
