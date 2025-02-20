#include <GL/glut.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <IrrKlang/irrKlang.h>
using namespace irrklang;
using namespace std;

#define WIN_X 100
#define WIN_Y 100
#define WIN_H 600 // in pixels
#define WIN_W 600

int frame = 0;

float zoomFactor = 1.0; // Global, if you want. Modified by user input. Initially 1.0

float camX = 0, camY = 0, speed = 1; // Global for camera movement.

// Creates sound engine
ISoundEngine* SoundEngine = createIrrKlangDevice();

GLuint texID[56]; // Texture ID's for the four textures.

const char* textureFileNames[56];	// File names for the files from which texture images are loaded

string name = "sprite/frame";

void DrawSprite() {
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_2D); // Enable texturing

	glBindTexture(GL_TEXTURE_2D, texID[frame]); // Which texture
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.5, -0.5, 0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.5, -0.5, 0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.5, 0.5, 0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.5, 0.5, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D); // Turn texturing off
}


void init(void) {

	glClearColor(0.0, 0.0, 0.0, 1.0); // clear the window screen
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);

	SoundEngine->play2D("audio/GuardianBattle.mp3", true);
}

void MyDisplay() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);


	DrawSprite();

	glFlush();
	glutSwapBuffers();
}

void loadTextures() {
	int i;
	glGenTextures(56, texID); // Get the texture object IDs.
	for (i = 0; i < 56; i++) {
		void* imgData; // Pointer to image color data read from the file.
		int imgWidth; // The width of the image that was read.
		int imgHeight; // The height.

		string fileName;
		const char* tmpName;

		fileName = name + to_string(i) + ".png";
		tmpName = fileName.c_str();
		textureFileNames[i] = tmpName;

		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
		if (format == FIF_UNKNOWN) {
			printf("Unknown file type for texture image file %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], 0); // Read image from file.
		if (!bitmap) {
			printf("Failed to load image %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap); // Convert to RGB or BGR format
		FreeImage_Unload(bitmap);
		imgData = FreeImage_GetBits(bitmap2); // Grab the data we need from the bitmap.
		imgWidth = FreeImage_GetWidth(bitmap2);
		imgHeight = FreeImage_GetHeight(bitmap2);
		if (imgData) {
			printf("Texture image loaded from file %s, size %dx%d\n",
				textureFileNames[i], imgWidth, imgHeight);
			glBindTexture(GL_TEXTURE_2D, texID[i]); // Will load image data into texture object #i
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_BGR_EXT,
				GL_UNSIGNED_BYTE, imgData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Required since there are no mipmaps.
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		}
		else {
			printf("Failed to get texture data from %s\n", textureFileNames[i]);
		} // end of else
	} // end of for loop
} // end of LoadTextures()

void timer(int v)
{
	frame++;

	if (frame >= 56) {
		frame = 0;
	}

	glutPostRedisplay();
	glutTimerFunc(100, timer, v); // Creates a frame delay that is counted in miliseconds
}

void procSpecialKeys(int key, int x, int y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (key == GLUT_KEY_LEFT)
	{
		camX -= 0.1 * speed;
	}

	if (key == GLUT_KEY_RIGHT)
	{
		camX += 0.1 * speed;
	}

	if (key == GLUT_KEY_UP)
	{
		camY += 0.1 * speed;
	}

	if (key == GLUT_KEY_DOWN)
	{
		camY -= 0.1 * speed;
	}

	glOrtho(-1 / zoomFactor, 1 / zoomFactor, -1 / zoomFactor, 1 / zoomFactor, -1, 1);
	glTranslatef(camX, camY, 0);
}

void procMouse(int button, int state, int x, int y) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Middle scroll wheel : zoom in or out with next two functions
	if (button == 3)
	{
		if (state == GLUT_UP)
		{
			zoomFactor -= 0.1;
			glOrtho(-1 / zoomFactor, 1 / zoomFactor, -1 / zoomFactor, 1 / zoomFactor, -1, 1);
		}
	}

	if (button == 4)
	{
		if (state == GLUT_UP)
		{
			zoomFactor += 0.1;
			glOrtho(-1 / zoomFactor, 1 / zoomFactor, -1 / zoomFactor, 1 / zoomFactor, -1, 1);
		}
	}

	glTranslatef(camX, camY, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB); // RGB mode
	glutInitWindowSize(WIN_W, WIN_H); // window size
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutCreateWindow("Camera Zoom Example");

	glutTimerFunc(0, timer, 0);

	init();

	loadTextures();

	glutDisplayFunc(MyDisplay); // call the drawing function

	glutSpecialFunc(procSpecialKeys);
	glutMouseFunc(procMouse);

	glutMainLoop();
	return 0;
}