#include <GL/glut.h>
#include <IrrKlang/irrKlang.h>
using namespace irrklang;

#define WIN_X 100
#define WIN_Y 100
#define WIN_H 600 // in pixels
#define WIN_W 600

ISoundEngine* SoundEngine = createIrrKlangDevice();

float red = 1, green = 1, blue = 1;

float posX, posY, posZ = 0, speed = 0.1;

// Message/text to be displayed
const char* testText = "Insert text here! \n"
					   "Isn't that cool!";

// Function to draw text
void drawText(const char* text, float x, float y, float z, float lineSpace) { //im doing this becasue "\n" doesnt work for some reason?

	glRasterPos3f(x, y, z);

	// loop through all chars
	for (int i = 0; text[i] != '\0'; i++) {
		if (text[i] == '\n') {
			// move down to the next line when "\n" is detected
			y -= lineSpace; //line spacing
			glRasterPos3f(x, y, z);
		}
		else {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
			/*
			There is only a set number of options for text types with this approach.
			This is the simplest approach to adding text to OpenGL.

			These are all the font options you have:
			GLUT_BITMAP_8_BY_13
			GLUT_BITMAP_9_BY_15
			GLUT_BITMAP_TIMES_ROMAN_10
			GLUT_BITMAP_TIMES_ROMAN_24
			GLUT_BITMAP_HELVETICA_10
			GLUT_BITMAP_HELVETICA_12
			GLUT_BITMAP_HELVETICA_18

			I've included and linked the files for the freetype library 
			and added fonts as well. You should only need to add the 
			necessary includes for it to use it but it is on the more 
			adanced side, we will only focus on the basics in class.
			*/
		}
	}
}

void drawSquare() {
	glBegin(GL_POLYGON);
	glVertex3f(-0.25, -0.25, 0);
	glVertex3f(0.25, -0.25, 0);
	glVertex3f(0.25, 0.25, 0);
	glVertex3f(-0.25, 0.25, 0);
	glEnd();
}

void init(void) {

	glClearColor(0.0, 0.0, 0.0, 1.0); // clear the window screen
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	SoundEngine->play2D("audio/SanctuaryGuardians.mp3", true);
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Red square
	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(-0.5, -0.5, -1);
	drawSquare();
	glPopMatrix();

	// Green square
	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslatef(0, -0.5, 0);
	drawSquare();
	glPopMatrix();

	// Blue square
	glPushMatrix();
	glColor3f(0, 0, 1);
	glTranslatef(0.5, -0.5, 1);
	drawSquare();
	glPopMatrix();

	// Text color
	glColor3f(red, green, blue);

	// Draw Text to the display
	drawText(testText, posX, posY, posZ, 0.05);

	glFlush();
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {

	// Change text color
    case '1': // red text
		red = 1;
		green = 0;
		blue = 0;
        break;
    case '2': // green text
		red = 0;
		green = 1;
		blue = 0;
        break;
    case '3': // blue text
		red = 0;
		green = 0;
		blue = 1;
        break;
    case '4': // grey text
		red = 0.5;
		green = 0.5;
		blue = 0.5;
        break;
    case '5': // mustard yellow text
		red = 0.5;
		green = 0.5;
		blue = 0;
        break;
    case '6': // turquoise text
		red = 0;
		green = 0.5;
		blue = 0.5;
        break;
    case '7': // purple text
		red = 0.5;
		green = 0;
		blue = 0.5;
        break;
	case '8': // white text
		red = 1;
		green = 1;
		blue = 1;
		break;

	// Moving text through foreground and background
	case 'w': // Move text towards camera
		posZ += 1;
		break;
	case 's': // Move text away from camera
		posZ -= 1;
		break;

	// Changing what text says
	case 'i': // Move text towards camera
		testText = "Yeah! That's super cool!";
		break;
	case 'o': // Move text away from camera
		testText = "Can you go back to the old text?";
		break;
	case 'p': // Move text away from camera
		testText = "            No...\n"
				   "Sorry about that...\n"
				   "Maybe next time...";
		break;
    case 27:  //ESC 
        exit(0);
        break;
    }

	glutPostRedisplay();
}

void SpecialKeyboard(int key, int, int)
{;
    switch (key)
    {
    case GLUT_KEY_LEFT: // Move text left
		posX -= speed;
        break;
    case GLUT_KEY_RIGHT: // Move text right
		posX += speed;
        break;
    case GLUT_KEY_UP: // Move text up
		posY += speed;
        break;
    case GLUT_KEY_DOWN: // Move text down
		posY -= speed;
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB); // RGB mode
	glutInitWindowSize(WIN_W, WIN_H); // Window size
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutCreateWindow("Text Example");

	init();

	glutDisplayFunc(MyDisplay); // Call the drawing function

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);

	glutMainLoop();
	return 0;
}