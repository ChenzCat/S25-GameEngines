#include <GL/glut.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <stdio.h>
#include <math.h>
#include <IrrKlang/irrKlang.h>
using namespace irrklang;

#define WIN_X 100
#define WIN_Y 100
#define WIN_H 600 // in pixels
#define WIN_W 600

int frame = 0;

float speed = 0.15, gravity = 0.2;

float jumpHeight = 0.5, jumpForce = 0.3, jumpTimer = 5, resetJumpTimer;

bool showGroundCheck = true;

bool left, right, jump;

// Creates sound engine
ISoundEngine* SoundEngine = createIrrKlangDevice();

GLuint texID[5]; // Texture ID's for the four textures.

char* textureFileNames[5] = {	// File names for the files from which texture images are loaded
	(char*)"sprite/catWag0.png",
	(char*)"sprite/catWag1.png",
	(char*)"sprite/catWag2.png",
	(char*)"sprite/catWag3.png",
	(char*)"sprite/catWag4.png",
};

char* catMeows[8] = {
	(char*)"audio/Cat 1.wav",
	(char*)"audio/Cat 2.wav",
	(char*)"audio/Cat 3.wav",
	(char*)"audio/Cat 4.wav",
	(char*)"audio/Cat 5.wav",
	(char*)"audio/Cat 6.wav",
	(char*)"audio/Cat 7.wav",
	(char*)"audio/Cat 8.wav",
};

// Gameobject class
class GameObject
{
public:
	// object state
	GLfloat   x, y, z, sizeX, sizeY;
	GLfloat   colorR, colorG, colorB;
	GLfloat   mass;
	bool	  canSee;
	bool      isSolid;
	bool      destroyed;
	bool      gravity;
	
	GameObject();

	void DrawGameObject(bool, GameObject);
	void DrawPlayer(GameObject, bool);
};

// Gameobjects on the screen
GameObject player, groundCheck, ground, collectible;

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
	if (one.destroyed || two.destroyed)
		return false;

	// collision x-axis?
	bool collisionX = one.x + one.sizeX >= two.x && two.x + two.sizeX >= one.x;

	// collision y-axis?
	bool collisionY = one.y + one.sizeY >= two.y && two.y + two.sizeY >= one.y;


	// collision only if on both axes
	return collisionX && collisionY;
}

void init(void) {

	glClearColor(0.0, 0.0, 0.0, 1.0); // clear the window screen
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5.0, 5.0, -5.0, 5.0, -10.0, 10.0);

	resetJumpTimer = jumpTimer;

	groundCheck.colorR = 0;

	ground.x = -3.5;
	ground.y = -3;
	ground.sizeX = 7;
	ground.colorG = 0;

	collectible.x = -2;
	collectible.y = -1;
	collectible.sizeX = 0.5;
	collectible.sizeY = 0.5;
	collectible.colorB = 0;

	SoundEngine->play2D("audio/The Return of Caped Crusader Cat.mp3", true);
}

void CreatePlayer(bool show) {
	// Draw player sprite and ground check
	glPushMatrix();
	player.DrawPlayer(player, true);
	groundCheck.x = player.x + 0.25;
	groundCheck.y = player.y;
	groundCheck.sizeX = 0.6;
	groundCheck.sizeY = 0.2;

	groundCheck.canSee = show;

	groundCheck.DrawGameObject(false, groundCheck);
	glPopMatrix();
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Makes the player
	CreatePlayer(showGroundCheck);

	// Makes the ground
	ground.DrawGameObject(false, ground);

	// Makes the collectible
	collectible.DrawGameObject(false, collectible);

	// Removes the collectible
	if (CheckCollision(player, collectible))
		collectible.destroyed = true;

	// Check if player is on the ground
	if (!CheckCollision(groundCheck, ground) && !jump)
		player.y -= gravity;

	// Check if player is hitting the ground but ground checker is not
	if (CheckCollision(player, ground) && !CheckCollision(groundCheck, ground) && !jump)
		left = right = false;

	//Movement
	if(left)
		player.x -= speed;
	if(right)
		player.x += speed;

	glFlush();
	glutSwapBuffers();
}

void specialKeyboard(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT:
			left = true;
			right = false;
			break;
		case GLUT_KEY_RIGHT:
			left = false;
			right = true;
			break;
	}

	glutPostRedisplay();
}

void specialKeyboardRelease(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT:
			left = false;
			break;
		case GLUT_KEY_RIGHT:
			right = false;
			break;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's': // Show ground check
		showGroundCheck = !showGroundCheck;
		break;
	case 32: // Spacebar
		if (CheckCollision(groundCheck, ground) && !jump) {
			SoundEngine->play2D(catMeows[rand() % 7], false);
			jump = true;
		}
		break;
	case 27: // escape
		exit(0);
	}

	glutPostRedisplay();
}

void loadTextures() {
	int i;
	glGenTextures(5, texID); // Get the texture object IDs.
	for (i = 0; i < 5; i++) {
		void* imgData; // Pointer to image color data read from the file.
		int imgWidth; // The width of the image that was read.
		int imgHeight; // The height.
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

	if (frame >= 1) { // set to 1 to stop it. Normally 4
		frame = 0;
	}

	// jump timer
	if (jump) {
		jumpTimer -= 1;
		player.y += jumpForce;

		if (jumpTimer <= 0) {
			jumpTimer = resetJumpTimer;
			jump = false;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(100, timer, v); // Creates a frame delay that is counted in miliseconds
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB); // RGB mode
	glutInitWindowSize(WIN_W, WIN_H); // window size
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutCreateWindow("Gravity Example");

	glutTimerFunc(0, timer, 0);

	init();

	loadTextures();

	glutDisplayFunc(MyDisplay); // call the drawing function

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardRelease);

	glutMainLoop();
	return 0;
}

GameObject::GameObject() {
	x = y = z = 0;
	sizeX = sizeY = 1;
	colorR = colorG = colorB = 1;
	mass = 0;
	canSee = true;
	isSolid = false;
	destroyed = false;
	gravity = false;
}

void GameObject::DrawGameObject(bool sprite, GameObject parameters)
{
	glPushMatrix();
	glTranslatef(parameters.x, parameters.y, parameters.z);

	if (parameters.canSee && !parameters.destroyed)
	{
		if (sprite)
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glEnable(GL_TEXTURE_2D); // Enable texturing

			glBindTexture(GL_TEXTURE_2D, texID[frame]); // Which texture

			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(-0.5+ parameters.sizeX-1, -0.5, 0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(0.5 + parameters.sizeX - 1, -0.5, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(0.5 + parameters.sizeX - 1, 0.5 + parameters.sizeY - 1, 0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-0.5, 0.5 + parameters.sizeY - 1, 0);
			glEnd();

			glDisable(GL_TEXTURE_2D); // Turn texturing off
		}
		else
		{
			glColor3f(parameters.colorR, parameters.colorG, parameters.colorB);

			glBegin(GL_POLYGON);
			glVertex3f(-0.5, -0.5, 0);
			glVertex3f(0.5 + parameters.sizeX-1, -0.5, 0);
			glVertex3f(0.5 + parameters.sizeX-1, 0.5 + parameters.sizeY-1, 0);
			glVertex3f(-0.5, 0.5 + parameters.sizeY-1, 0);
			glEnd();
		}
	}
	glPopMatrix();
}

void GameObject::DrawPlayer(GameObject player, bool sprite)
{
	glPushMatrix();
	glTranslatef(player.x, player.y, player.z);

	if (player.canSee && !player.destroyed)
	{
		//Draw Player
		if (sprite)
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glEnable(GL_TEXTURE_2D); // Enable texturing

			glBindTexture(GL_TEXTURE_2D, texID[frame]); // Which texture

			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(-0.5 + player.sizeX - 1, -0.5, 0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(0.5 + player.sizeX - 1, -0.5, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(0.5 + player.sizeX - 1, 0.5 + player.sizeY - 1, 0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-0.5, 0.5 + player.sizeY - 1, 0);
			glEnd();

			glDisable(GL_TEXTURE_2D); // Turn texturing off
		}
		else
		{
			glColor3f(player.colorR, player.colorG, player.colorB);

			glBegin(GL_POLYGON);
			glVertex3f(-0.5, -0.5, 0);
			glVertex3f(0.5 + player.sizeX - 1, -0.5, 0);
			glVertex3f(0.5 + player.sizeX - 1, 0.5 + player.sizeY - 1, 0);
			glVertex3f(-0.5, 0.5 + player.sizeY - 1, 0);
			glEnd();
		}
	}

	glPopMatrix();
}

