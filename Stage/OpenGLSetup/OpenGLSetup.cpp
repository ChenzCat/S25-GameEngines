#include <GL/glut.h>
#include <IrrKlang/irrKlang.h>
using namespace irrklang;

#define WIN_X 100
#define WIN_Y 100
#define WIN_H 600 // in pixels
#define WIN_W 600

// Creates sound engine
ISoundEngine* SoundEngine = createIrrKlangDevice();

void init(void) {

	glClearColor(0.0, 0.0, 0.0, 1.0); // clear the window screen
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

void procKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'p': // Pause all sound
		SoundEngine->setAllSoundsPaused(true);
		break;
	case 'm': // Unpause all sound
		SoundEngine->setAllSoundsPaused(false);
		break;
	case 'd': // Drop our sound engine
		SoundEngine->drop();
		break;
	case 'a': // Add a sound engine
		SoundEngine = createIrrKlangDevice();
		break;
	case 27: // escape
		exit(0);
	}
}

void mouse(int btn, int state, int x, int y)
{
	// Add music
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		SoundEngine->play2D("audio/breakout.mp3", true);

	// Stops all sounds
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		SoundEngine->stopAllSounds();
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Code here

	glFlush();
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB); // RGB mode
	glutInitWindowSize(WIN_W, WIN_H); // window size
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutCreateWindow("Sound/Music Example");

	init();

	glutDisplayFunc(MyDisplay); // call the drawing function

	// Gets mouse inputs
	glutMouseFunc(mouse);

	glutKeyboardFunc(procKeys);

	glutMainLoop();
	return 0;
}