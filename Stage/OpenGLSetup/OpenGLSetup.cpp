#include <GL/glut.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <stdio.h>
#include <math.h>
#include <array>
#include <iostream>
#include <IrrKlang/irrKlang.h>
#include <string>

using namespace irrklang;
using namespace std;

#define WIN_X 0
#define WIN_Y 0
#define WIN_H 1080
#define WIN_W 1920
//-------------------------------------------------------------------------------------------------------------

// Global Variables
int frame = 0, groundNum = 0;
float cameraX = 0.0f, cameraY = 0.0f;
float cameraOffsetX = 0.0f, cameraOffsetY = 3.0f; // Offsets make cameras easier to understand. Might get reused for the game engine

float speed = 0.15f;				// Horizontal speed of the player
float gravity = 0.2f;

float jumpTimer = 10, resetJumpTimer;
float jumpAcceleration = 1.0f;		// Jump height
float jumpVelocity = 0.5f;			// Initial upward jump velocity

// Collision
bool showCollision = true;
bool lt, rt, jump, contact, onGround;

// Mechanics
int totalCoins = 10;
int coinsCollected = 0;

// Game States
bool pausedState = false;
bool menuState;
bool gameOverState = false;
bool winState;
int timeLeft = 60; // Start with 60 seconds

// Creates sound engine
ISoundEngine* SoundEngine = createIrrKlangDevice();

GLuint texID[22]; // Texture ID's for the four textures.

char* textureFileNames[22] = {	// File names for the files from which texture images are loaded
	(char*)"sprite/knightRightMoving (1).png",
	(char*)"sprite/knightRightMoving (2).png",
	(char*)"sprite/knightRightMoving (3).png",
	(char*)"sprite/knightRightMoving (4).png",
	(char*)"sprite/knightRightMoving (5).png",
	(char*)"sprite/knightRightMoving (6).png",
	(char*)"sprite/knightRightMoving (7).png",
	(char*)"sprite/knightRightMoving (8).png",
	// Environment
	(char*)"sprite/Tiles493.png",
	(char*)"sprite/Tiles494.png",
	(char*)"sprite/Tiles495.png",
	(char*)"sprite/Tiles116.png",
	(char*)"sprite/Tiles117.png",
	(char*)"sprite/Tiles118.png",
	(char*)"sprite/Tiles119.png",
	(char*)"sprite/Tiles164.png",
	(char*)"sprite/Tiles165.png",
	(char*)"sprite/Tiles166.png",
	(char*)"sprite/Tiles167.png",
	(char*)"sprite/Tiles415.png",
	(char*)"sprite/Tiles416.png",
	(char*)"sprite/Tiles417.png",
};

char* catMeows[3] = {
	(char*)"audio/nes-sfx24.wav",
	(char*)"audio/nes-sfx26.wav",
	(char*)"audio/nes-sfx25.wav",
};

// Gameobject class
class GameObject
{
public:
	// Position and size
	GLfloat   x, y, z, sizeX, sizeY;

	// RGB color
	GLfloat   colorR, colorG, colorB;

	// Additional properties
	GLfloat   mass;
	bool	  canSee;		// Visable Object
	bool      isSolid;		// Block Objects
	bool      destroyed;	// Remove Object
	bool      gravity;		// Gravity affects the object
	int		  textureIndex; // Texture Use

	GameObject();	// Constructor

	void DrawGameObject(bool sprite);
	void DrawPlayer(bool sprite);
};

// ========================================================================================================================================================================

// Global Objects

// Player and Collision Boxes
GameObject player, bottomCheck, leftCheck, rightCheck, topCheck;

// Solid Blocks
GameObject CreateGround(float x, float y, float width, float height, bool collider = true, int texIndex = -1)
{
	GameObject ground;
	ground.x = x;						// X Axis Origin
	ground.y = y;						// Y Axis Origin
	ground.sizeX = width;				// Width
	ground.sizeY = height;				// Height
	ground.isSolid = collider;			// Collision
	ground.canSee = true;				// Visibility
	ground.textureIndex = texIndex;		// Store Texture Index
	return ground;
}

GameObject ground[200];

// Collectable object
GameObject collectible;

GameObject createInvisibleCollider(float x, float y, float width, float height) {
	GameObject collider;
	collider.x = x;
	collider.y = y;
	collider.sizeX = width;
	collider.sizeY = height;

	collider.isSolid = true;
	collider.canSee = false;
	collider.colorR = 1.0f;
	collider.colorG = 0.0f;
	collider.colorB = 0.0f;
	collider.textureIndex = -1; // No texture

	return collider;
}

// GameObject Helper Functions

void createColumn(char direction, float startX, float startY,
	int length, int rows, float tileSize,
	bool collider = true,
	int startTexIndex = 8,
	int endTexIndex = -1,
	bool invisible = false  // <-- NEW PARAMETER
)
{
	// If invisible is false, we create visible tiles.
	// If invisible is true, we skip making visible tiles, but still make the collider if requested.

	if (!invisible)
	{
		int totalTiles = length * rows;
		int textureRange = (endTexIndex - startTexIndex) + 1;
		if (textureRange < 1) textureRange = 1; // Safety fallback

		int tileCounter = 0; // Will increment as we place each tile

		// Loop over rows (r) and columns (c)
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < length; c++)
			{
				float xPos, yPos;

				// 'H' => horizontal
				if (direction == 'H' || direction == 'h')
				{
					xPos = startX + c * tileSize;
					yPos = startY + r * tileSize;
				}
				// 'V' => vertical
				else
				{
					xPos = startX + r * tileSize;
					yPos = startY + c * tileSize;
				}

				// Calculate which texture to use for this tile
				int currentTexOffset = tileCounter % textureRange;
				int currentTexIndex = startTexIndex + currentTexOffset;

				// Create each tile with the current texture
				ground[groundNum] = CreateGround(
					xPos,
					yPos,
					tileSize,
					tileSize,
					false,    // Non-collidable by default
					currentTexIndex
				);
				groundNum++;
				tileCounter++;
			}
		}
	}
	// If invisible == true, we skip creating visible tiles entirely

	// Optionally create a single invisible collider for the entire block
	if (collider)
	{
		float colliderWidth, colliderHeight;

		if (direction == 'H' || direction == 'h')
		{
			colliderWidth = length * tileSize;
			colliderHeight = rows * tileSize;
		}
		else // 'V' or 'v'
		{
			colliderWidth = rows * tileSize;
			colliderHeight = length * tileSize;
		}

		ground[groundNum] = createInvisibleCollider(
			startX,
			startY,
			colliderWidth,
			colliderHeight
		);
		groundNum++;
	}
}





// Implementing Collision Detection (Used from the platform example)
bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
	// If either end up getting destroied, skip collisionGameObject
	if (one.destroyed || two.destroyed)
		return false;

	// collision Y:
	bool collisionX = (one.x + one.sizeX + 0.01f >= two.x) &&
		(two.x + two.sizeX + 0.01f >= one.x);
	// collision X:
	bool collisionY = (one.y + one.sizeY + 0.01f >= two.y) &&
		(two.y + two.sizeY + 0.01f >= one.y);


	// collision only if on both axes
	return collisionX && collisionY;
}

// ========================================================================================================================================================================
// Environment Initialization

void init(void) {
	// Clear The Window + Set Color
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);


	// 2D Projection Setup
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-10.0, 10.0, -10.0, 5.0, -10.0, 10.0);

	// Prepare jump timer
	resetJumpTimer = jumpTimer;

	// Give collision check boxes a color
	bottomCheck.colorR = 0;
	leftCheck.colorR = 0;
	rightCheck.colorR = 0;
	topCheck.colorR = 0;

	// Sprite Sheet
	createColumn('H', -3.5f, 3.0f, 22, 2, 1.0f, false, 1, 22);

	// Floor
	createColumn('H', -5.5f, -3.0f, 30, 2, 1.0f, true, 12);
	createColumn('H', -5.5f, 1.0f, 30, 2, 1.0f, true, 11);


	// Background
	createColumn('H', -4.5f, -1.0f, 6, 2, 1.0f, false , 10, 10);



	// Collectable Setup
	collectible.x = -2.0f;
	collectible.y = 0.0f;
	collectible.sizeX = 0.2f;
	collectible.sizeY = 0.4f;
	collectible.colorB = 0.0f;

	// Sound System BGM
	SoundEngine->play2D("audio/track_25.OGG", true);
}

// Draw Player Character + Colliders
void CreatePlayer(bool show)
{
	glPushMatrix(); // Player Character

	// Draw Player and Size
	player.sizeX = 1.0f;
	player.sizeY = 1.0f;
	player.DrawPlayer(true);

	// Bottom Collision Check
	// Offesets
	bottomCheck.x = player.x + player.sizeX * 0.35f;
	bottomCheck.y = player.y;

	// Size
	bottomCheck.sizeX = player.sizeX * 0.30f;
	bottomCheck.sizeY = player.sizeY * 0.05f;

	bottomCheck.canSee = show;
	bottomCheck.DrawGameObject(false);


	// Top Collision Check
	// Offesets
	topCheck.x = player.x + player.sizeX * 0.35f;
	topCheck.y = player.y + player.sizeY * 0.80f;

	// Size
	topCheck.sizeX = player.sizeX * 0.30f;
	topCheck.sizeY = player.sizeY * 0.05f;

	topCheck.canSee = show;
	topCheck.DrawGameObject(false);

	// Left Collision Check
	// Offesets
	leftCheck.x = player.x + player.sizeX * 0.20f;
	leftCheck.y = player.y + player.sizeY * 0.80f;

	// Size
	leftCheck.sizeX = player.sizeX * 0.05f;
	leftCheck.sizeY = -player.sizeY * 0.75f;

	leftCheck.canSee = show;
	leftCheck.DrawGameObject(false);

	// Right Collision Check
	// Offesets
	rightCheck.x = player.x + player.sizeX * 0.75f;
	rightCheck.y = player.y + player.sizeY * 0.80f;

	// Size
	rightCheck.sizeX = player.sizeX * 0.05f;
	rightCheck.sizeY = -player.sizeY * 0.75f;

	rightCheck.canSee = show;
	rightCheck.DrawGameObject(false);

	glPopMatrix();
}
// ========================================================================================================================================================================
// Environment Logic

// Physics
void gravityCheck() {
	onGround = false;

	// If bottomCheck collides with any ground object: onGround=true
	for (int i = 0; i < groundNum; ++i) {
		if (ground[i].isSolid && CheckCollision(bottomCheck, ground[i])) {
			onGround = true;
			break;
		}
	}

	// If leftCheck collides: push the player right
	for (int i = 0; i < groundNum; ++i) {
		if (ground[i].isSolid && CheckCollision(leftCheck, ground[i])) {
			player.x += speed;
			break;
		}
	}

	// If rightCheck collides: push the player left
	for (int i = 0; i < groundNum; ++i) {
		if (ground[i].isSolid && CheckCollision(rightCheck, ground[i])) {
			player.x -= speed;
			break;
		}
	}

	// If topCheck collides (while jumping): Reset Jump Timer
	for (int i = 0; i < groundNum; ++i) {
		if (ground[i].isSolid && CheckCollision(topCheck, ground[i]) && jump) {
			jump = false;
			jumpTimer = resetJumpTimer;
			break;
		}
	}

	// If not on ground: Apply Gravity
	if (!onGround) {
		// Apply gravity
		player.y -= gravity;
	}

	if (player.y < -10.0) { // Out of bounds check
		gameOverState = true;
	}


}


// ========================================================================================================================================================================
// Main Display

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	// Clear the screen

	if (pausedState) {
		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(player.x - 1, player.y);
		const char* message = "GAME PAUSED" "- Press P to Resume";
		for (int i = 0; message[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, message[i]);
		}
		glutSwapBuffers();
		return;
	}

	if (gameOverState) {
		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(player.x - 1, player.y + 1);
		const char* message = "GAME OVER!";
		for (int i = 0; message[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, message[i]);
		}
		glutSwapBuffers();
		return;
	}

	if (winState) {
		glColor3f(0.0, 1.0, 0.0);
		glRasterPos2f(player.x - 1, player.y);
		const char* message = "YOU WIN!";
		for (int i = 0; message[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, message[i]);
		}
		glutSwapBuffers();
		return;
	}


	// Update camera position (smooth follow effect)
	cameraX = player.x + player.sizeX / 2 + cameraOffsetX;
	cameraY = player.y + player.sizeY / 2 + cameraOffsetY;

	// Set view matrix to follow the player
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraX, cameraY, 5,		// Camera position (x, y, z)
		cameraX, cameraY, 0,			// Looking at (player)
		0, 1, 0);						// Up vector (y-axis)

	for (int i = 0; i < groundNum; i++) {
		if (!ground[i].isSolid) {
			ground[i].DrawGameObject(true);
		}
	}

	// Draws the player (with or without collision visualized)
	CreatePlayer(showCollision);

	// 2) Draw collidable objects next
	for (int i = 0; i < groundNum; i++) {
		if (ground[i].isSolid) {
			ground[i].DrawGameObject(true);
		}
	}








	// Draw the collectible
	collectible.DrawGameObject(false);


	// Removes the collectible when Touched by Player
	if (CheckCollision(player, collectible))
	{
		collectible.destroyed = true; // Check collision Skipped
		winState = true;
	}



	// Enable Rules:

	// Apply Gravity + Collision
	gravityCheck();

	// Timer
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(player.x - 0.2f, player.y + 0.8f);
	string timerText = "Time: " + std::to_string(timeLeft);
	for (char c : timerText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, c);

	// Then the coin line (lower on the screen, or different offset)
	glRasterPos2f(player.x - 0.2f, player.y + 0.6f);
	string coinText = "Coins: " + to_string(coinsCollected) + "/" + to_string(totalCoins);
	for (char c : coinText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, c);

	// Apply Player Movement Logic
	if (lt)
		player.x -= speed;
	if (rt)
		player.x += speed;





	glFlush();
	glutSwapBuffers();
}

// ========================================================================================================================================================================
// Controls

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
		if (onGround) {
			SoundEngine->play2D(catMeows[1], false);
			jump = true;
		}
		break;

	case 'p': // Toggle Pause
		pausedState = !pausedState;
		break;

	case 27: // escape
		exit(0);
	}


	glutPostRedisplay();
}


// ========================================================================================================================================================================
// Loading Textures

void loadTextures() {
	int i;
	glGenTextures(22, texID); // Get the texture object IDs (Reserve IDs)
	for (i = 0; i < 22; i++) {
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

// ========================================================================================================================================================================
// Global Timer

void timer(int v)
{
	frame++;

	if (frame >= 6) {
		frame = 0;
	}

	if (jump) {
		if (jumpTimer > 0) {
			// Temporary copies to show how velocity changes 
			float jumpAccelerationTemp = jumpAcceleration;
			float jumpVelocityTemp = jumpVelocity;

			// Move player up
			player.y += jumpVelocityTemp;
			// Reduce velocity over time
			jumpVelocityTemp -= jumpAccelerationTemp;

			jumpTimer--;
		}
		else {
			// Reset jump
			jump = false;
			jumpTimer = resetJumpTimer;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(100, timer, v); // Adjust frame delay based on FPS
}

void collectionTimer(int v) {
	if (!pausedState && !gameOverState && !winState) {
		timeLeft--;
		if (timeLeft <= 0) {
			gameOverState = true;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(1000, timer, 0); // Call every second
}

// ========================================================================================================================================================================
// Main

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB); // RGB mode
	glutInitWindowSize(WIN_W, WIN_H); // window size
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutCreateWindow("Platform Example");

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

// ========================================================================================================================================================================
// Game Object Implementation

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

	// Only draw if not destroyed and is visible
	if (canSee && !destroyed)
	{
		if (sprite && textureIndex >= 0)
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glEnable(GL_TEXTURE_2D); // Enable texturing

			// Use the current frame for animation or [x] set number
			glBindTexture(GL_TEXTURE_2D, texID[textureIndex]); // Which texture

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
		else // Fallback
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

