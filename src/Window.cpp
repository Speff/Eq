#include "window.h"

int screen_width = 500;
int screen_height = 200;

int main(){
	GLFWwindow* window;

    GLuint VAO, VBO;
    GLuint program;
    GLfloat *lineCoords;
    GLuint lineCoords_nVerts = NLINES * 2; // 1 Line = 2 Verts = 4 floats (x, y)
    size_t lineCoords_size = sizeof(GLfloat) * 2 * lineCoords_nVerts;

	PaStream *audioStream = NULL;
	paTestData data;

	printf("Init glfw\n");
	if(!glfwInit()) return -1;

    // Set glfw window hints
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    // Enable multisampling
    glEnable(GL_MULTISAMPLE); 

	printf("Create glfw window\n");
	window = glfwCreateWindow(
			screen_width,  // Window Width
			screen_height, // Window Height
			"Eq",          // Window Title
			NULL,          // GLFW Monitor (for fullscreen)
			NULL);         // For context sharing
	printf("Make sure window is created\n");
	if(!window){
		glfwTerminate();
		return -1;
	}

    glViewport(0, 0, screen_width, screen_height);
	
	printf("Make glfw context current\n");
	glfwMakeContextCurrent(window);

    printf("Setting blending function\n");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    printf("Enable vsync\n");
    glfwSwapInterval(1);

    printf("Setting GLFW callbacks\n");
    glfwSetKeyCallback(window, key_pressed);
    glfwSetWindowSizeCallback(window, window_resized);

    printf("Initializing OpenGL boilerplate code\n");
    initGLBoilerplate(screen_width, screen_height, &program);

    printf("Initializing PaAudio lib\n");
	initAudio(audioStream, &data);

    
    lineCoords = initLineVerts(
            lineCoords_nVerts,
            lineCoords_size,
            screen_width/10,
            screen_height/10,
            screen_width * 8/10,
            screen_height * 8/10);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
            GL_ARRAY_BUFFER,
            lineCoords_size,
            lineCoords,
            GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glPrintError(glGetError());
    printf("%f, %f\n%f, %f\n", lineCoords[4], lineCoords[5], lineCoords[6],
            lineCoords[7]);

	printf("Hello World\n");
	while(!glfwWindowShouldClose(window)){
        // Render here
        glClearColor(0.05f, 0.05f, 0.05f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwGetFramebufferSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2*lineCoords_nVerts);

		// Swap frame buffers
		glfwSwapBuffers(window);

		// Poll for events
		glfwPollEvents();
		
		for(int i = 0; i < (int)floor(100*data.left_phase) + 30; i++)
			printf(" ");
		printf("\rLeft: ");
		for(int i = 0; i < (int)floor(100*data.left_phase); i++){
			printf("*");
		}
		printf("\r");
		Sleep(16);
	}

    free(lineCoords);
	destroyAudio(audioStream);
	glfwTerminate();
	return 1;
}

static int paTestCallback(
		const void *inputBuffer,
		void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData){

	paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	float *in = (float*)inputBuffer;
	(void) out;
	float avgLeft = 0;
	float avgRight = 0;

	for(unsigned int i = 0; i < framesPerBuffer; i++){
		avgLeft += fabs(*in);
		in++;
		avgRight += fabs(*in);
		in++;
	}

	data->left_phase = avgLeft/framesPerBuffer;
	data->right_phase = avgRight/framesPerBuffer;

	return 0;
}

static void initAudio(PaStream *audioStream, paTestData *data){
	PaError paErr;

	paErr = Pa_Initialize();
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));

	// Open an audio stream
	paErr = Pa_OpenDefaultStream(
			&audioStream,
			2,              // Number of input channels
			0,              // Number of output channels
			paFloat32,      // 32-bit floating point output
			SAMPLE_RATE,
			256,            // frames per buffer, ie. the number
			                // of sample frames PA will request
			                // from callback function
			paTestCallback, // cb function
			data);         // Pointer passed to cb
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));

	paErr = Pa_StartStream(audioStream);
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));
}

static void destroyAudio(PaStream *audioStream){
	PaError paErr;
	
	paErr = Pa_StopStream(audioStream);
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));

	paErr = Pa_Terminate();
	if(paErr != paNoError)
		printf("PortAudio error: %s\n", Pa_GetErrorText(paErr));
}

GLfloat* initLineVerts(GLuint nVerts, size_t lSize, GLfloat startX,
        GLfloat startY, GLfloat drawSizeX, GLfloat drawSizeY){

    GLfloat *lineCoords = (GLfloat*)malloc(sizeof(GLfloat) * nVerts * 2);

    for(unsigned int i = 0; i < nVerts/2; i++){
        lineCoords[4*i] = startX + i*(drawSizeX/nVerts*2.0);
        lineCoords[4*i + 1] = startY;
        lineCoords[4*i + 2] = startX + i*(drawSizeX/nVerts*2.0);
        lineCoords[4*i + 3] = startY + drawSizeY/2.0;
    }

    return lineCoords;
}



void initGLBoilerplate(int screenWidth, int screenHeight, GLuint *program){
    // Compile the shaders
    GLuint vertex, fragment;
    GLint compilationSuccess;
    GLchar infoLog[512];
    GLchar *vertShdr, *fragShdr;
    FILE *sourceFile = NULL;
    size_t readSize;

    // Read vert shader from file
    sourceFile = fopen("vert.glsl", "r");
    if(sourceFile == NULL) printf("Can't open vert file\n");
    // Get file size
    fseek(sourceFile, 0, SEEK_END);
    readSize = ftell(sourceFile);
    // Allocate space for shader source code var
    vertShdr = (char*)malloc(sizeof(char) * readSize + 1);
    if(vertShdr == NULL) printf("Memory error - vertShdr\n");
    // Read file into source variable
    rewind(sourceFile);
    fread(vertShdr, 1, readSize, sourceFile);

    // Read frag shader from file
    sourceFile = fopen("frag.glsl", "r");
    if(sourceFile == NULL) printf("Can't open frag file\n");
    // Get file size
    fseek(sourceFile, 0, SEEK_END);
    readSize = ftell(sourceFile);
    // Allocate space for shader source code var
    fragShdr = (char*)malloc(sizeof(char) * readSize + 2);
    if(fragShdr == NULL) printf("Memory error - fragShdr\n");
    // Read file into source variable
    rewind(sourceFile);
    fread(fragShdr, 1, readSize, sourceFile);

    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertShdr, NULL);
    glCompileShader(vertex);
    //Print compilation errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &compilationSuccess);
    if(!compilationSuccess){
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("Vert shader compilation failed: %s\n", infoLog);
        printf("---\n%s\n---\n", vertShdr);
    }

    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragShdr, NULL);
    glCompileShader(fragment);
    //Print compilation errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &compilationSuccess);
    if(!compilationSuccess){
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("Frag shader compilation failed: %s\n", infoLog);
        printf("---\n%s\n---\n", fragShdr);
    }

    free(vertShdr);
    free(fragShdr);

    // Create shader program
    *program = glCreateProgram();
    glAttachShader(*program, vertex);
    glAttachShader(*program, fragment);
    glLinkProgram(*program);
    // Print linking errors
    glGetProgramiv(*program, GL_LINK_STATUS, &compilationSuccess);
    if(!compilationSuccess){
        glGetProgramInfoLog(*program, 512, NULL, infoLog);
        printf("Program linking failed: %s\n", infoLog);
    }
    glUseProgram(*program);

    // Delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    setProjectionMatrix(program, screenWidth, screenHeight);
}

static void key_pressed(GLFWwindow* window, int key, int scancode,
        int action,int mods){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void window_resized(GLFWwindow* window, int width, int height){
    //setProjectionMatrix(&program, width, height);
    screen_width = width;
    screen_height = height;
}

void setProjectionMatrix(GLuint *prog, int screen_width, int screen_height){
    // Set projection uniform
    GLfloat projMat[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat left    = 0.0f;
    GLfloat right   = (GLfloat)screen_width;
    GLfloat bottom  = 0.0f;
    GLfloat top     = (GLfloat)screen_height;
    GLfloat nearVal = 0.0f;
    GLfloat farVal  = 1.0f;
    GLfloat tx      = -(right + left)/(right-left);
    GLfloat ty      = -(top + bottom)/(top-bottom);
    GLfloat tz      = -(farVal + nearVal)/(farVal - nearVal);

    projMat[0]  *= 2.0f / (right - left);
    projMat[5]  *= 2.0f / (top - bottom);
    projMat[10] *= -2.0f / (farVal - nearVal);
    projMat[12]  = tx;
    projMat[13]  = ty;
    projMat[14]  = tz;

    //printf("Ortho matrix:\n\
    //        %+1.3f %+1.3f %+1.3f %+1.3f\n\
    //        %+1.3f %+1.3f %+1.3f %+1.3f\n\
    //        %+1.3f %+1.3f %+1.3f %+1.3f\n\
    //        %+1.3f %+1.3f %+1.3f %+1.3f\n",
    //        projMat[0],   projMat[1],   projMat[2],   projMat[3],
    //        projMat[4],   projMat[5],   projMat[6],   projMat[7],
    //        projMat[8],   projMat[9],   projMat[10],  projMat[11],
    //        projMat[12],  projMat[13],  projMat[14],  projMat[15]);

    GLint loc = glGetUniformLocation(*prog, "projection");
    if(loc != -1){
        glUniformMatrix4fv(loc, 1, GL_FALSE, projMat);
    }
    else printf("Can't get projection uniform location\n");

}

void glPrintError(GLenum error){
    if(error == GL_NO_ERROR)
		printf("OpenGL Error: GL_NO_ERROR\n");
    else if(error == GL_INVALID_ENUM)
		printf("OpenGL Error: GL_INVALID_ENUM\n");
    else if(error == GL_INVALID_VALUE)
		printf("OpenGL Error: GL_INVALID_VALUE\n");
    else if(error == GL_INVALID_OPERATION)
		printf("OpenGL Error: GL_INVALID_OPERATION\n");
    else if(error == GL_INVALID_FRAMEBUFFER_OPERATION)
		printf("OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION\n");
    else if(error == GL_OUT_OF_MEMORY)
		printf("OpenGL Error: GL_OUT_OF_MEMORY\n");
}
