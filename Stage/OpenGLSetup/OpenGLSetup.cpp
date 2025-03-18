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
int displayFrame = 0, groundNum = 0, coffeeNum = 0;
float cameraX = 0.0f, cameraY = 0.0f;
float cameraOffsetX = 0.0f, cameraOffsetY = 3.0f; // Offsets make cameras easier to understand. Might get reused for the game engine

float speed = 0.15f;				// Horizontal speed of the player
float gravity = 0.2f;

float jumpTimer = 5, resetJumpTimer;
float jumpAcceleration = 0.15f;		// Jump height
float jumpVelocity = 0.8f;			// Initial upward jump velocity

// Collision
bool showCollision = true;
bool lt, rt, jump, contact, onGround;

// Mechanics
int totalcoffee = 10;
int coffeeCollected = 0;
static int timeLeft = 60;

// Game States
enum Scenes
{
	MenuScene,
	GameScene,
	LoseScene,
	WinScene,
	PauseScene
};

Scenes currentScene = MenuScene;

// Player States
enum PlayerState
{
	IdleLeft,
	RunLeft,
	IdleRight,
	RunRight,
	JumpLeft,
	JumpRight
};


PlayerState currentState = IdleRight;  // Start facing right, idle
int animFrameIndex = 0;                 // For cycling run frames
bool facingLeft = false;                // Track last facing direction


// coffee Animation
int coffeeAnimFrame = 0;      // Animation frame index
int coffeeAnimFrames = 4;     // Total coffee animation frames
int coffeeAnimDelay = 200;    // Milliseconds per frame

// Creates sound engine
ISoundEngine* SoundEngine = createIrrKlangDevice();

GLuint texID[70]; // Texture ID's for the four textures.

char* textureFileNames[70] = {	// File names for the files from which texture images are loaded
	// Knight
	(char*)"sprite/KnightLeft1.png",
	(char*)"sprite/KnightLeft2.png",
	(char*)"sprite/KnightLeft3.png",
	(char*)"sprite/KnightLeft4.png",
	(char*)"sprite/KnightLeft5.png",
	(char*)"sprite/KnightLeft6.png",
	(char*)"sprite/KnightLeft7.png",
	(char*)"sprite/KnightLeft8.png",
	(char*)"sprite/KnightLeftStanding9.png",
	(char*)"sprite/KnightRight10.png",
	(char*)"sprite/KnightRight11.png",
	(char*)"sprite/KnightRight12.png",
	(char*)"sprite/KnightRight13.png",
	(char*)"sprite/KnightRight14.png",
	(char*)"sprite/KnightRight15.png",
	(char*)"sprite/KnightRight16.png",
	(char*)"sprite/KnightRight17.png",
	(char*)"sprite/KnightRightStanding18.png",

	// Coffee
	(char*)"sprite/Coffee19.png",
	(char*)"sprite/Coffee20.png",
	(char*)"sprite/Coffee21.png",
	(char*)"sprite/Coffee22.png",

	// Environment
		// Stone Top
	(char*)"sprite/TopCornerLeft23.png",
	(char*)"sprite/TopInnerLeft24.png",
	(char*)"sprite/TopInnerRight25.png",
	(char*)"sprite/TopCornerRight26.png",
	// Stone Middle
	(char*)"sprite/MiddleSideLeft27.png",
	(char*)"sprite/MiddleSideRight28.png",
	// Stone Bottom
	(char*)"sprite/BottomCornerLeft29.png",
	(char*)"sprite/BottomInnerLeft30.png",
	(char*)"sprite/BottomInnerRight31.png",
	(char*)"sprite/BottomCornerRight32.png",
	// Stone Segment Bottom
	(char*)"sprite/BottomSegment(1)33.png",
	(char*)"sprite/BottomSegment(2)34.png",
	(char*)"sprite/BottomSegment(3)35.png",
	// Stone Segment Top
	(char*)"sprite/TopSegment(1)36.png",
	(char*)"sprite/TopSegment(2)37.png",
	(char*)"sprite/TopSegment(3)38.png",
	// Wood
	(char*)"sprite/MiddleWoodLeft(1)39.png",
	(char*)"sprite/MiddleWood(2)40.png",
	(char*)"sprite/MiddleWoodRight(3)41.png",
	(char*)"sprite/TopWood42.png",
	(char*)"sprite/BottomWood43.png",
	// Wood Beam
	(char*)"sprite/Beam(1)44.png",
	(char*)"sprite/Beam(2)45.png",
	(char*)"sprite/Beam(3)46.png",
	(char*)"sprite/Beam(4)47.png",
	(char*)"sprite/Beam(5)48.png",
	// Void
	(char*)"sprite/Void49.png",
	// Props
	(char*)"sprite/Door(1)50.png",
	(char*)"sprite/Door(2)51.png",

	(char*)"sprite/StonePlatform(1)52.png",
	(char*)"sprite/StonePlatform(2)53.png",
	(char*)"sprite/StonePlatform(3)54.png",

	(char*)"sprite/Crate55.png",

	(char*)"sprite/Medo(1)56.png",
	(char*)"sprite/Medo(2)57.png",
	(char*)"sprite/Medo(3)58.png",
	(char*)"sprite/Medo(4)59.png",
	(char*)"sprite/Medo(5)60.png",
	(char*)"sprite/Medo(6)61.png",
	(char*)"sprite/Medo(7)62.png",
	(char*)"sprite/Medo(8)63.png",
	(char*)"sprite/Medo(9)64.png",
	(char*)"sprite/Medo(10)65.png",
	(char*)"sprite/Medo(11)66.png",
	(char*)"sprite/Medo(12)67.png",
	(char*)"sprite/Medo(13)68.png",
	(char*)"sprite/Medo(14)69.png",
	(char*)"sprite/Medo(15)70.png",
};

char* backgroundMusic[4] =
{
	// Sound System BGM
	//SoundEngine->play2D("audio/Game.mp3", true);
	(char*)"audio/Menu.mp3",
	(char*)"audio/Game.mp3",
	(char*)"audio/Lose.mp3",
	(char*)"audio/Win.mp3"
};

char* sfxQueue[2] = 
{
	(char*)"audio/nes-sfx24.wav",
	(char*)"audio/nes-sfx24.wav"
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

// Platforms and Background Tiles
GameObject ground[1000];

// Collectable object Tiles
GameObject collectible[10];

GameObject hazard;

GameObject mapExit;

GameObject createInvisibleCollider(float x, float y, float width, float height) 
{
	GameObject collider;
	collider.x = x;
	collider.y = y;
	collider.sizeX = width;
	collider.sizeY = height;

	collider.isSolid = true;
	collider.canSee = true;
	collider.colorR = 1.0f;
	collider.colorG = 0.0f;
	collider.colorB = 0.0f;
	collider.textureIndex = -1; // No texture

	return collider;
}

// GameObject Helper Functions

void createcoffee(float x, float y)
{
	collectible[coffeeNum].x = x;
	collectible[coffeeNum].y = y;
	collectible[coffeeNum].sizeX = 0.3f;
	collectible[coffeeNum].sizeY = 0.4f;

	collectible[coffeeNum].isSolid = false;
	collectible[coffeeNum].canSee = true;
	collectible[coffeeNum].destroyed = false;

	collectible[coffeeNum].textureIndex = 18; // Example coffee texture index

	coffeeNum++;
}

void createColumn(char direction, float startX, float startY, int length, int rows, float tileSize, bool collider = true, int startTexIndex = 8, int endTexIndex = -1, bool invisible = false)
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

void createHazard(float x, float y, float tileWidth = 1.0f, float tileLength = 1.0f) 
{
	hazard.x = x;
	hazard.y = y;
	hazard.sizeX = tileWidth;
	hazard.sizeY = tileLength;
	hazard.isSolid = false;					
	hazard.canSee = false;					
	hazard.destroyed = false;
	hazard.textureIndex = -1;			

	// Red Debug
	hazard.colorR = 1.0f;
	hazard.colorG = 0.0f;
	hazard.colorB = 0.0f;
}

void createMapExit(float x, float y, float tileSize = 1.0f, bool visible = false) {
	mapExit.x = x;
	mapExit.y = y;
	mapExit.sizeX = tileSize;
	mapExit.sizeY = tileSize;
	mapExit.isSolid = false;
	mapExit.canSee = visible;
	mapExit.destroyed = false;
	mapExit.textureIndex = -1;
	// Set debug color (green) if you choose to render it.
	mapExit.colorR = 0.0f;
	mapExit.colorG = 1.0f;
	mapExit.colorB = 0.0f;
}

// Sound Library
void playMenuMusic() {
	SoundEngine->stopAllSounds();
	SoundEngine->play2D(backgroundMusic[0], true);
}

void playGameMusic() {
	SoundEngine->stopAllSounds();
	SoundEngine->play2D(backgroundMusic[1], true);
}

void playLoseMusic() {
	SoundEngine->stopAllSounds();
	SoundEngine->play2D(backgroundMusic[2], false);
}

void playWinMusic() {
	SoundEngine->stopAllSounds();
	SoundEngine->play2D(backgroundMusic[3], false);
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
	glClearColor((52.0f / 255.0f), (32.0f / 255.0f), (43.0f / 255.0f), 1.0f);


	// 2D Projection Setup
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-10.0, 10.0, -10.0, 5.0, -10.0, 10.0);

	// Prepare jump timer
	resetJumpTimer = jumpTimer;

	player.x = -4.5f;

	playMenuMusic();

	// Give collision check boxes a color
	bottomCheck.colorR = 0;
	leftCheck.colorR = 0;
	rightCheck.colorR = 0;
	topCheck.colorR = 0;

	// Floor

	createColumn('H', -5.5f, -2.0f, 8, 1, 1.0f, true, 36);
	createColumn('H', 4.0f, -2.0f, 20, 1, 1.0f, true, 36);

	createColumn('H', -5.5f, 6.0f, 30, 1, 1.0f, true, 33);

	// Walls
	createColumn('V', -5.4f, -1.0f, 7, 1, 1.0f, true, 34);
		createColumn('V', -5.5f, 6.0f, 1, 1, 1.0f, true, 30);
			createColumn('V', -5.5f, -2.0f, 1, 1, 1.0f, true, 24);


	// Background
	createColumn('V', -4.5f, -1.0f, 7, 30, 1.0f, false, 39);
	createColumn('H', 34, 0.0f, 5, 5, 1.0f, false, 39);	// Secret Room
		// Door
		createColumn('V', -4.5f, -1.0f, 2, 1, 1.0f, false, 49, 50);

	// Crates
		createColumn('H', -1.5f, -1.0f, 2, 1, 1.0f, true, 54);
		createColumn('V', 0.0f, 0.0f, 1, 2, 1.0f, true, 54);
		createColumn('V', 0.5f, -1.0f, 1,2, 1.0f, true, 54);

		// Stairs
			createColumn('V', 3.8f, -1.0f, 1, 1, 1.0f, true, 54);
			createColumn('V', 4.8f, -1.0f, 2, 1, 1.0f, true, 54);
			createColumn('V', 5.8f, -1.0f, 3, 1, 1.0f, true, 54);
			createColumn('V', 6.8f, -1.0f, 4, 1, 1.0f, true, 54);
		
		// Stacks
			createColumn('V', 7.8f, 0.0f, 4, 1, 1.0f, true, 54);
			createColumn('V', 9.8f, 0.0f, 3, 1, 1.0f, true, 54);
			createColumn('V', 11.8f, 0.0f, 2, 1, 1.0f, true, 54);
			createColumn('V', 13.8f, 0.0f, 1, 1, 1.0f, true, 54);
			createColumn('V', 15.8f, -1.0f, 1, 1, 1.0f, true, 54);
		
		// Path Out
			createColumn('H', 7.8f, -1.0f, 7, 1, 1.0f, false, 54);
		
		
		// Cluttered Boxes
			createColumn('H', 18.0f, -1.0f, 20, 1, 1.0f, true, 54); // Floor
			createColumn('H', 18.8f, 1.0f, 16, 3, 1.0f, true, 54);  // Upper Regions
			createColumn('H', 20.0f, 4.0f, 15, 1, 1.0f, true, 54);
			createColumn('H', 21.0f, 5.0f, 17, 1, 1.0f, true, 54);
			createColumn('H', 24.0f, 6.0f, 15, 1, 1.0f, true, 54);
			createColumn('H', 25.0f, 7.0f, 13, 1, 1.0f, true, 54);
			createColumn('H', 26.0f, 8.0f, 14, 1, 1.0f, true, 54);

		// Wall Block
			createColumn('V', 38.0f, -5.0f, 16, 10, 1.0f, true, 54);

		// Lower Layering
			createColumn('H', 24.0f, -2.0f, 14, 1, 1.0f, true, 54);
			createColumn('H', 25.0f, -3.0f, 13, 1, 1.0f, true, 54);
			createColumn('H', 26.0f, -4.0f, 12, 1, 1.0f, true, 54);
			createColumn('H', 27.0f, -5.0f, 11, 1, 1.0f, true, 54);

		// Secret Path
			createColumn('H', 18.8f, 0.0f, 16, 1, 1.0f, false , 54); 

		// Medo :3
			createColumn('H', 35, 2.0f, 4, 1, 1.0f, false, 55, 58);
			createColumn('H', 35, 1.0f, 4, 1, 1.0f, false, 59, 62);
			createColumn('H', 35, 0.0f, 4, 1, 1.0f, false, 63, 66);
			createColumn('H', 35, -1.0f, 3, 1, 1.0f, false, 67, 69);


	// Bounds
	createColumn('H', -5.5f, -2.95f, 30, 1, 1.0f, false, 48);
	createColumn('H', -5.5f, 6.95f, 30, 1, 1.0f, false, 48);
	createColumn('V', -6.2f, -2.0f, 9, 1, 1.0f, false, 48);



	// Collectable Setup
	createcoffee(1.2f, 1.2f);  // 1
	createcoffee(3.3f, 0.6f);  // 2
	createcoffee(6.2f, 4.0f);  // 3
	createcoffee(8.2f, 5.0f);  // 4
	createcoffee(10.2f, 4.0f); // 5
	createcoffee(12.2f, 4.0f); // 6
	createcoffee(14.2f, 2.0f); // 7 
	createcoffee(15.2f, 3.0f); // 8
	createcoffee(16.2f, 1.5f); // 9
	createcoffee(17.4f, 1.0f); // 10





	// Win/Lose Objects
	createHazard(-5.0f, -10.0f, 60.0f, 0.05f);    // Clipping Out and Falling in the 1 gap I made ends the game
	createMapExit(-4.5f, -1.0f, 1.0f);



}

void CreateMechanics()
{
	// Simply display the timer and coin count without modifying the timer value
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(player.x - 9.3f, player.y + 8.0f);
	string timerText = "Time: " + std::to_string(timeLeft);
	for (char c : timerText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

	glRasterPos2f(player.x + 8.3f, player.y + 8.0f);
	string coinText = "Espresso: " + to_string(coffeeCollected) + "/" + to_string(totalcoffee);
	for (char c : coinText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
}

void coffeeAnimationTimer(int value)
{
	coffeeAnimFrame = (coffeeAnimFrame + 1) % coffeeAnimFrames;
	glutPostRedisplay();  // Redraw the scene with the new coffee frame
	glutTimerFunc(coffeeAnimDelay, coffeeAnimationTimer, 0);  // Schedule next frame update
}

// Draw Player Character + Colliders
void CreatePlayer(bool show) {
	glPushMatrix();
	player.DrawPlayer(true);
	bottomCheck.x = player.x + 0.3;
	bottomCheck.y = player.y;
	bottomCheck.sizeX = 0.4;
	bottomCheck.sizeY = 0.2;

	bottomCheck.canSee = show;

	leftCheck.x = player.x + 0.25;
	leftCheck.y = player.y + 0.3;
	leftCheck.sizeX = 0.15;
	leftCheck.sizeY = 0.4;

	leftCheck.canSee = show;

	rightCheck.x = player.x + 0.60;
	rightCheck.y = player.y + 0.3;
	rightCheck.sizeX = 0.15;
	rightCheck.sizeY = 0.4;

	rightCheck.canSee = show;

	topCheck.x = player.x + 0.3;
	topCheck.y = player.y + 0.75;
	topCheck.sizeX = 0.4;
	topCheck.sizeY = 0.15;

	topCheck.canSee = show;

	bottomCheck.DrawGameObject(false);
	leftCheck.DrawGameObject(false);
	rightCheck.DrawGameObject(false);
	topCheck.DrawGameObject(false);
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
		currentScene = LoseScene;
	}


}


// ========================================================================================================================================================================
// Main Display

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	// Clear the screen


	if (currentScene == MenuScene) {
		glColor3f(1.0, 1.0, 1.0);
		// Draw the large title text
		glRasterPos2f(-1.3f, -1.0f); // Adjust as needed for centering
		const char* title = "Caffeine Crusader";
		for (int i = 0; title[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, title[i]);
		}
		// Draw the smaller instruction text below the title
		glRasterPos2f(-0.75f, -1.6f); // Adjust position as needed
		const char* menuMsg = "Press G to Start";
		for (int i = 0; menuMsg[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menuMsg[i]);
		}
		// Draw the smaller instruction text below the title
		glRasterPos2f(-0.75f, -1.4f); // Adjust position as needed
		const char* menuDescripiton = "Collect all 10 Espresso Shots and return before the clock hits 0";
		for (int i = 0; menuDescripiton[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menuDescripiton[i]);
		}
		// Draw the smaller instruction text below the title
		glRasterPos2f(-0.75f, -1.2f); // Adjust position as needed
		const char* menuComment = "Feed your addiction before you crash!";
		for (int i = 0; menuComment[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menuComment[i]);
		}
		glutSwapBuffers();
		return;
	}

	// Pause Scene: Display pause message and instruction.
	else if (currentScene == PauseScene) {
		glColor3f(1.0, 1.0, 1.0);
		// Display large pause title
		glRasterPos2f(player.x - 2.0f, player.y + 1.5f);
		const char* pauseTitle = "GAME PAUSED";
		for (int i = 0; pauseTitle[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, pauseTitle[i]);
		}
		// Display smaller resume instruction below
		glRasterPos2f(player.x - 1.8f, player.y + 1.0f);
		const char* pauseMsg = "- Press P to Resume";
		for (int i = 0; pauseMsg[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, pauseMsg[i]);
		}
		glutSwapBuffers();
		return;
	}

	else if (currentScene == LoseScene) {
		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(player.x - 0.5f, player.y + 1);
		const char* message = "GAME OVER!";
		for (int i = 0; message[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
		}
		glRasterPos2f(player.x - 0.3f, player.y + 0.5); // Adjust position as needed
		const char* gameOverMsg = "You were too eepy :(";
		for (int i = 0; gameOverMsg[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, gameOverMsg[i]);
		}
		glutSwapBuffers();
		return;
	}

	else if (currentScene == WinScene) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glRasterPos2f(player.x - 0.5f, player.y + 1);
		const char* message = "YOU WIN!";
		for (int i = 0; message[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
		}
		glRasterPos2f(player.x - 0.3f, player.y + 0.5);
		const char* winMsg = "Wow! Awesome! B)";
		for (int i = 0; winMsg[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, winMsg[i]);
		}
		glutSwapBuffers();
		return;
	}

	// Object Spawn

	for (int i = 0; i < groundNum; i++) {
		if (!ground[i].isSolid) {
			ground[i].DrawGameObject(true);
		}
	}

	// Update camera position
	cameraX = player.x + player.sizeX / 2 + cameraOffsetX;
	cameraY = player.y + player.sizeY / 2 + cameraOffsetY;
	if (player.y == -3.0f)
	{
		cameraY = -3.0f;
	}

	// Set view matrix to follow the player
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraX, cameraY, 5,		// Camera position (x, y, z)
		cameraX, cameraY, 0,			// Looking at (player)
		0, 1, 0);						// Up vector (y-axis)

	// Draws the player (with or without collision visualized)
	CreatePlayer(showCollision);

	for (int i = 0; i < coffeeNum; i++)
	{
		if (!collectible[i].destroyed)
		{
			collectible[i].DrawGameObject(true);  // Draw coffee with texture
			if (CheckCollision(player, collectible[i]))
			{
				collectible[i].destroyed = true;
				coffeeCollected++;
			}
		}
	}


	hazard.DrawGameObject(false);
	mapExit.DrawGameObject(false);

	if (CheckCollision(player, hazard)) {
		currentScene = LoseScene;
		playLoseMusic();
	}

	if (CheckCollision(player, mapExit) && coffeeCollected >= totalcoffee) {
		currentScene = WinScene;
		playWinMusic();
	}

	// Enable Rules:
	CreateMechanics();

	// Apply Gravity + Collision
	gravityCheck();

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
		currentState = RunLeft;
		facingLeft = true;
		break;

	case GLUT_KEY_RIGHT:
		lt = false;
		rt = true;
		currentState = RunRight;
		facingLeft = false;
		break;
	}

	glutPostRedisplay();
}

void specialKeyboardRelease(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		lt = false;
		if (currentState == RunLeft) 
		{
			currentState = IdleLeft;
			animFrameIndex = 0; // reset run frames
		}
		break;
	case GLUT_KEY_RIGHT:
		rt = false;
		if (currentState == RunRight) {
			currentState = IdleRight;
			animFrameIndex = 0;
		}
		break;
	}
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 's':
		showCollision = !showCollision;
		break;
	case 32: // Spacebar for jump (only in GameScene)
		if (currentScene == GameScene && onGround && !jump) {
			jump = true;
			currentState = facingLeft ? JumpLeft : JumpRight;
		}
		break;
	case 'g':
		if (currentScene == MenuScene) {
			currentScene = GameScene;
			playGameMusic();
		}
		break;
	case 'p': // Toggle Pause
		if (currentScene == GameScene) {
			currentScene = PauseScene;
			SoundEngine->setAllSoundsPaused(true);
		}
		else if (currentScene == PauseScene) {
			currentScene = GameScene;
			SoundEngine->setAllSoundsPaused(false);
		}
		break;
	case 27: // Escape key
		exit(0);
	}
	glutPostRedisplay();
}


// ========================================================================================================================================================================
// Loading Textures

void loadTextures() {
	int i;
	glGenTextures(70, texID); // Get the texture object IDs (Reserve IDs)
	for (i = 0; i < 70; i++) {
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


void updateAnimationFrame() {
	// If the player is jumping, use a special frame or frames
	if (jump) {
		// If you want separate left/right jump frames:
		if (facingLeft) {
			// example: pick frame 14 for jump-left
			displayFrame = 14;
		}
		else {
			// example: pick frame 15 for jump-right
			displayFrame = 15;
		}
		return;
	}

	// Otherwise, pick frames based on currentState:
	switch (currentState) {
	case RunLeft:
		// frames [0..7] for running left
		// animFrameIndex cycles 0..7
		displayFrame = animFrameIndex;
		break;

	case IdleLeft:
		// single frame = 8
		displayFrame = 8;
		break;

	case RunRight:
		// frames [9..16] for running right
		displayFrame = 9 + animFrameIndex;
		break;

	case IdleRight:
		// single frame = 17
		displayFrame = 17;
		break;

	default:
		// fallback
		displayFrame = 8;
		break;
	}
}

void cycleRunFrames() {
	// If RunLeft, cycle 0..7
	if (currentState == RunLeft) {
		animFrameIndex++;
		if (animFrameIndex > 7) animFrameIndex = 0;
	}
	// If RunRight, cycle 0..7 (added offset in updateAnimationFrame)
	else if (currentState == RunRight) {
		animFrameIndex++;
		if (animFrameIndex > 7) animFrameIndex = 0;
	}
	// Idle states don't cycle
	// Jump states we override frame anyway
}

// Global Timer
void timer(int v) {
	// Only update the game timer when in GameScene.
	if (currentScene == MenuScene || currentScene == PauseScene || currentScene == LoseScene || currentScene == WinScene) {
		glutTimerFunc(100, timer, 0);
		return;
	}

	static int gameTimerAccumulator = 0;
	gameTimerAccumulator += 100;
	if (gameTimerAccumulator >= 1000) {
		timeLeft--;
		gameTimerAccumulator = 0;
		if (timeLeft <= 0) {
			currentScene = LoseScene;
			playLoseMusic();
		}
	}
	if (jump) {
		if (jumpTimer > 0) {
			if (lt) {
				currentState = JumpLeft;
				facingLeft = true;
			}
			else if (rt) {
				currentState = JumpRight;
				facingLeft = false;
			}
			player.y += jumpVelocity;
			jumpVelocity -= jumpAcceleration;
			jumpTimer--;
		}
		else {
			jump = false;
			jumpTimer = resetJumpTimer;
			jumpVelocity = 0.8f;
			currentState = (lt ? RunLeft : (rt ? RunRight : (facingLeft ? IdleLeft : IdleRight)));
		}
	}
	cycleRunFrames();
	updateAnimationFrame();
	glutPostRedisplay();
	glutTimerFunc(100, timer, 0);
}

// ========================================================================================================================================================================
// Main

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB); // RGB mode
    glutInitWindowSize(WIN_W, WIN_H); // window size
    glutInitWindowPosition(WIN_X, WIN_Y);
    glutCreateWindow("Platform Example");

    // Start the coffee animation timer
    glutTimerFunc(coffeeAnimDelay, coffeeAnimationTimer, 0);
    
    glutTimerFunc(0, timer, 0);
    init();
    loadTextures();
    
    glutDisplayFunc(MyDisplay); // Drawing function
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

			if (textureIndex == 18) { // If it's a coffee
				glBindTexture(GL_TEXTURE_2D, texID[18 + coffeeAnimFrame]);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, texID[textureIndex]);
			}

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

			glBindTexture(GL_TEXTURE_2D, texID[displayFrame]); // Which texture

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