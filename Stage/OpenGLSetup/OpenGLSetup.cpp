#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <FreeImage/FreeImage.h>
#include <stdio.h>
#include <math.h>
#include <array>
#include <iostream>
#include <IrrKlang/irrKlang.h>
using namespace irrklang;
using namespace std;

#define MAIN_PANEL_W 600 // Width of the main panel
#define SIDE_PANEL_W 200 // Width of each side panel
#define BOTTOM_PANEL_H 200 // Height of the bottom panel
#define TOP_PANEL_H 50 // Height of top panel
#define WIN_W 1000 // Adjusted window width
#define WIN_H 850 // Adjusted window height

int frame = 0, groundNum = 2;

float speed = 0.15, gravity = 0.2;

float jumpTimer = 10, resetJumpTimer;
float jumpAcceleration = 1.0f; // Adjust this value for jump height
float jumpVelocity = 0.5f; // Adjust this value for initial jump velocity

bool showCollision = true;

bool lt, rt, jump, contact, onGround;

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

	void DrawGameObject(bool);
	void DrawPlayer(bool);
};

// Gameobjects on the screen
GameObject player, bottomCheck, leftCheck, rightCheck, topCheck, ground[3], collectible;

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

// Draws a square and you can determine the size, position, and color.
void drawSquare(float sizeX, float sizeY, float sizeZ, float x, float y, float z, float r, float g, float b) {
	glPushMatrix();

	glScalef(sizeX, sizeY, sizeZ);
	glTranslatef(x, y, z);
	glColor3f(r, g, b); // Light gray background
	glBegin(GL_POLYGON);
	glVertex3f(-0.5, -0.5, 0);
	glVertex3f(0.5, -0.5, 0);
	glVertex3f(0.5, 0.5, 0);
	glVertex3f(-0.5, 0.5, 0);
	glEnd();

	glPopMatrix();
}

void init(void) {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the window screen and change the background color
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-7.0, 7.0, -7.0, 7.0, -10.0, 10.0);

	resetJumpTimer = jumpTimer;

	bottomCheck.colorR = 0;
	leftCheck.colorR = 0;
	rightCheck.colorR = 0;
	topCheck.colorR = 0;

	ground[0].x = -3.5;
	ground[0].y = -3;
	ground[0].sizeX = 7;
	ground[0].colorG = 0;

	ground[1].x = 3.5;
	ground[1].y = -3;
	ground[1].sizeX = 3;
	ground[1].colorG = 0;

	ground[2].x = -5.5;
	ground[2].y = -1;
	ground[2].sizeX = 3;
	ground[2].colorG = 0;

	collectible.x = -4;
	collectible.y = 2;
	collectible.sizeX = 0.5;
	collectible.sizeY = 0.5;
	collectible.colorB = 0;

	SoundEngine->play2D("audio/The Return of Caped Crusader Cat.mp3", true);
}

void CreatePlayer(bool show) {
	// Draw player sprite and ground check
	glPushMatrix();
	player.DrawPlayer(true);
	bottomCheck.x = player.x + 0.2;
	bottomCheck.y = player.y;
	bottomCheck.sizeX = 0.6;
	bottomCheck.sizeY = 0.2;

	bottomCheck.canSee = show;

	leftCheck.x = player.x;
	leftCheck.y = player.y + 0.2;
	leftCheck.sizeX = 0.2;
	leftCheck.sizeY = 0.6;

	leftCheck.canSee = show;

	rightCheck.x = player.x + 0.8;
	rightCheck.y = player.y + 0.2;
	rightCheck.sizeX = 0.2;
	rightCheck.sizeY = 0.6;

	rightCheck.canSee = show;

	topCheck.x = player.x + 0.2;
	topCheck.y = player.y + 0.8;
	topCheck.sizeX = 0.6;
	topCheck.sizeY = 0.2;

	topCheck.canSee = show;

	bottomCheck.DrawGameObject(false);
	leftCheck.DrawGameObject(false);
	rightCheck.DrawGameObject(false);
	topCheck.DrawGameObject(false);
	glPopMatrix();
}

void gravityCheck() {
	onGround = false;

	for (int i = 0; i <= groundNum; ++i) {
		if (CheckCollision(bottomCheck, ground[i])) {
			onGround = true;
			break;
		}
	}

	for (int i = 0; i <= groundNum; ++i) {
		if (CheckCollision(leftCheck, ground[i])) {
			player.x += speed;
			break;
		}
	}

	for (int i = 0; i <= groundNum; ++i) {
		if (CheckCollision(rightCheck, ground[i])) {
			player.x -= speed;
			break;
		}
	}

	for (int i = 0; i <= groundNum; ++i) {
		if (CheckCollision(topCheck, ground[i]) && jump) {
			jump = false;
			jumpTimer = resetJumpTimer;
			break;
		}
	}

	if (!onGround) {
		// Apply gravity
		player.y -= gravity;
	}
}

void drawLeftPanel() {

	glPushMatrix();
	glViewport(0, TOP_PANEL_H, SIDE_PANEL_W, WIN_H + TOP_PANEL_H);

	glScalef(4, 1, 1);

	// Blue background
	drawSquare(4, 9.5, 1, 0, 0, 0, 0, 0, 1);

	// Makes the player
	//CreatePlayer(showCollision);
	glPopMatrix();
}

void drawMainPanel() {
	glPushMatrix();
	glViewport(SIDE_PANEL_W, TOP_PANEL_H, MAIN_PANEL_W, WIN_H + TOP_PANEL_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glScalef(1.3, 1, 1);

	// Makes the player
	CreatePlayer(showCollision);

	// Makes the ground
	ground[0].DrawGameObject(false);
	ground[1].DrawGameObject(false);
	ground[2].DrawGameObject(false);

	// Makes the collectible
	collectible.DrawGameObject(false);

	// Removes the collectible
	if (CheckCollision(player, collectible))
		collectible.destroyed = true;

	gravityCheck();

	//Movement
	if (lt)
		player.x -= speed;
	if (rt)
		player.x += speed;

	glPopMatrix();
}

void drawRightPanel() {
	glPushMatrix();
	glViewport(SIDE_PANEL_W + MAIN_PANEL_W, TOP_PANEL_H, SIDE_PANEL_W, WIN_H + TOP_PANEL_H);
	glScalef(4, 1, 1);

	// Green background
	drawSquare(4, 9.5, 1, 0, 0, 0, 0, 1, 0);

	// Makes the player
	//CreatePlayer(showCollision);
	glPopMatrix();
}

void drawTopPanel() {
	glPushMatrix();
	glViewport(0, WIN_H - TOP_PANEL_H, WIN_W, TOP_PANEL_H);

	glScalef(1, 20, 1);

	// Grey background 
	drawSquare(14, .7, 1, 0, 0, 0, 0.7, 0.7, 0.7);


	glPopMatrix();
}

void drawBottomPanel() {
	glPushMatrix();
	glViewport(0, 0, WIN_W, BOTTOM_PANEL_H);

	glScalef(1, 5, 1);

	// Blue background
	drawSquare(14, 3, 1, 0, 0, 0, 1, 0, 0);

	// Makes the player
	//CreatePlayer(showCollision);
	glPopMatrix();
}

void MyDisplay() {
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// These do follow the rendering order too. Keep that in mind.
	drawLeftPanel();
	drawMainPanel();
	drawRightPanel();
	drawTopPanel();
	drawBottomPanel();

	glutSwapBuffers();
}

void specialKeyboard(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		lt = true;
		rt = false;
		break;
	case GLUT_KEY_RIGHT:
		lt = false;
		rt = true;
		break;
	}

	glutPostRedisplay();
}

void specialKeyboardRelease(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		lt = false;
		break;
	case GLUT_KEY_RIGHT:
		rt = false;
		break;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's': // Show ground check
		showCollision = !showCollision;
		break;
	case 32: // Spacebar
		if (onGround && !jump) {
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
		// Load image with FreeImage
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
		if (format == FIF_UNKNOWN) {
			printf("Unknown file type for texture image file %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], PNG_DEFAULT); // Load PNG files
		if (!bitmap) {
			printf("Failed to load image %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap); // Convert to 32-bit (with alpha channel)
		FreeImage_Unload(bitmap);
		if (bitmap32) {
			BYTE* bits = FreeImage_GetBits(bitmap32);
			int width = FreeImage_GetWidth(bitmap32);
			int height = FreeImage_GetHeight(bitmap32);
			for (int j = 0; j < width * height; j++) {
				BYTE temp = bits[j * 4];
				bits[j * 4] = bits[j * 4 + 2];
				bits[j * 4 + 2] = temp;
			}

			glBindTexture(GL_TEXTURE_2D, texID[i]); // Bind texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			FreeImage_Unload(bitmap32);
		}
	}
	glEnable(GL_BLEND); // Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending function
}

void timer(int v)
{
	frame++;

	if (frame >= 5) {
		frame = 0;
	}

	// jump timer
	if (jump) {
		if (jumpTimer > 0) {
			float jumpAccelerationTemp = jumpAcceleration;
			float jumpVelocityTemp = jumpVelocity;
			player.y += jumpVelocityTemp; // Increment player's y-coordinate based on velocity
			jumpVelocityTemp -= jumpAccelerationTemp; // Apply gravity to decrease velocity
			jumpTimer--;
		}
		else {
			jump = false;
			jumpTimer = resetJumpTimer;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(100, timer, v); // Adjust frame delay based on FPS
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // RGB mode with double buffering

	// Initialize window and create it
	glutInitWindowSize(WIN_W, WIN_H);
	glutCreateWindow("Panel Example");

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

void GameObject::DrawGameObject(bool sprite)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	if (canSee && !destroyed)
	{
		if (sprite)
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glEnable(GL_TEXTURE_2D); // Enable texturing

			glBindTexture(GL_TEXTURE_2D, texID[frame]); // Which texture

			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(-0.5, -0.5, 0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(0.5 + sizeX - 1, -0.5, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(0.5 + sizeX - 1, 0.5 + sizeY - 1, 0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-0.5, 0.5 + sizeY - 1, 0);
			glEnd();

			glDisable(GL_TEXTURE_2D); // Turn texturing off
		}
		else
		{
			glColor3f(colorR, colorG, colorB);

			glBegin(GL_POLYGON);
			glVertex3f(-0.5, -0.5, 0);
			glVertex3f(0.5 + sizeX - 1, -0.5, 0);
			glVertex3f(0.5 + sizeX - 1, 0.5 + sizeY - 1, 0);
			glVertex3f(-0.5, 0.5 + sizeY - 1, 0);
			glEnd();
		}
	}
	glPopMatrix();
}

void GameObject::DrawPlayer(bool sprite)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	if (canSee && !destroyed)
	{
		//Draw Player
		if (sprite)
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glEnable(GL_TEXTURE_2D); // Enable texturing

			glBindTexture(GL_TEXTURE_2D, texID[frame]); // Which texture

			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(-0.5, -0.5, 0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(0.5 + sizeX - 1, -0.5, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(0.5 + sizeX - 1, 0.5 + sizeY - 1, 0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-0.5, 0.5 + sizeY - 1, 0);
			glEnd();

			glDisable(GL_TEXTURE_2D); // Turn texturing off
		}
		else
		{
			glColor3f(colorR, colorG, colorB);

			glBegin(GL_POLYGON);
			glVertex3f(-0.5, -0.5, 0);
			glVertex3f(0.5 + sizeX - 1, -0.5, 0);
			glVertex3f(0.5 + sizeX - 1, 0.5 + sizeY - 1, 0);
			glVertex3f(-0.5, 0.5 + sizeY - 1, 0);
			glEnd();
		}
	}

	glPopMatrix();
}