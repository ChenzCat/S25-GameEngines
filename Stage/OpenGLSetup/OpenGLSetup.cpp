// Game Engines		  | Project 3: 2D Platformer
// Vincenzo Cavallaro | Professor Meldin Bektic
// Mar 20, 2025

#include <GL/glut.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <array>
#include <list>
#include <iostream>
#include <sstream>
#include <functional>
#include <string>
#include <IrrKlang/irrKlang.h>

using namespace irrklang;
using namespace std;

#define MAIN_PANEL_W 600 // Width of the main panel
#define SIDE_PANEL_W 200 // Width of each side panel
#define MIDDLE_PANEL_H 600 // Height of middle panels
#define BOTTOM_PANEL_H 200 // Height of the bottom panel
#define TOP_PANEL_H 50 // Height of top panel

#define WIN_W 1000 // Adjusted window width
#define WIN_H 850 // Adjusted window height

// Frame & Object Counter
int displayFrame = 0, groundNum = 0, coffeeNum = 0;

// Camera Positions
float cameraX = 0.0f, cameraY = 0.0f;
float offsetX = 0.0f, offsetY = 3.0f;

// Movement Values
float speed = 0.15, gravity = 0.05;

float jumpTimer = 10, resetJumpTimer;
float jumpAcceleration = 0.02f;
float jumpVelocity = 0.65f;

// Collision Flags
bool showCollision = false;
bool lt, rt, jump, contact, onGround;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Toggles
bool axis;
bool buttonDebug = false;
bool gravityOn = false; // Toggle the gravity. I'm just doing it like this for the example but you should do it per object (Hint: maybe in the class)

// Mechanic Values
int totalcoffee = 10;					// Collectibles (Total)
int coffeeCollected = 0;				// Collectibles (Gathered)
static int timeLeft = 60;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool gridMode = true;
bool dragging = false;
bool displaySceneStateDisplay = true;

float editorCameraX = 0.0f;
float editorCameraY = 0.0f;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Game States
enum Scenes
{
	MenuScene,
	GameScene,
	LoseScene,
	WinScene,
	PauseScene,
	EditScene
};
Scenes currentScene = EditScene;

enum PlayerState
{
	IdleLeft,
	RunLeft,
	IdleRight,
	RunRight,
	JumpLeft,
	JumpRight
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Player Animation
PlayerState currentState = IdleRight;	// Start Looking Right
int PlayerAnimFrame = 0;                // Cycle Frames
bool facingLeft = false;                // Check Left Idle from Right Idle

// Coffee Animation
int coffeeAnimFrame = 0;      // Similar Index to Cycle Frames
int coffeeAnimDelay = 200;    // Frame Cycle Speed

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Textures
GLuint texID[70];
char* textureFileNames[70] =
{
	// Knight Textures
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

	// Coffee Textures
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
		// Door
		(char*)"sprite/Door(1)50.png",
		(char*)"sprite/Door(2)51.png",

		// Stone Platforms
		(char*)"sprite/StonePlatform(1)52.png",
		(char*)"sprite/StonePlatform(2)53.png",
		(char*)"sprite/StonePlatform(3)54.png",

		// Create (Ended up using this the most)
		(char*)"sprite/Crate55.png",

		// Bonus
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

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Audio
ISoundEngine* SoundEngine = createIrrKlangDevice();

char* backgroundMusic[4] =
{
	(char*)"audio/Menu.mp3",
	(char*)"audio/Game.mp3",
	(char*)"audio/Lose.mp3",
	(char*)"audio/Win.mp3"
};

char* sfxMusic[1] =
{
	(char*)"audio/Jump.ogg"
};
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void playMenuMusic()
{
	if (buttonDebug == false)
	{
	SoundEngine->stopAllSounds();
	SoundEngine->play2D(backgroundMusic[0], true);
	}
}

void playGameMusic()
{
	if (buttonDebug == false)
	{
		SoundEngine->stopAllSounds();
		SoundEngine->play2D(backgroundMusic[1], true);
	}
}

void playLoseMusic()
{
	if (buttonDebug == false)
	{
		SoundEngine->stopAllSounds();
		SoundEngine->play2D(backgroundMusic[2], false);
	}
}

void playWinMusic()
{
	if (buttonDebug == false)
	{
		SoundEngine->stopAllSounds();
		SoundEngine->play2D(backgroundMusic[3], false);
	}
}

void playJumpSFX()
{
	if (buttonDebug == false)
	{
		SoundEngine->play2D(sfxMusic[0], false);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Gameobject Class
class GameObject
{
public:
	// object state
	GLfloat   x, y, z, sizeX, sizeY;
	GLfloat   colorR, colorG, colorB;
	GLfloat   mass;
	bool	  canSee;							// Visability
	bool      isSolid;							// Collider
	bool      destroyed;						// Remove Object
	bool      gravity;							// Gravity
	int		  textureIndex;						// Texture Use

	// Constructor (Bottom)
	GameObject();	

	void DrawGameObject(bool sprite);
	void DrawPlayer(bool sprite);
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Game Objects On Screen
GameObject Player, BottomCheck, LeftCheck, RightCheck, TopCheck, Collectible, Exit, Hazard, Ground;

// Total Tiles, Collectibles, Hazards, Exits
std::list<GameObject> platforms;
std::list<GameObject> Collectibles;
std::list<GameObject> hazards;
std::list<GameObject> exits;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Text Input Class
class TextInput {
public:

	enum Field {
		X,
		Y,
		Z,
		X_SCALE,
		Y_SCALE
	} field;

	float x, y; // Position of the text input box in the viewport
	float width, height; // Dimensions of the text input box in the viewport
	float windowX, windowY; // Position of the button in the window
	float windowWidth, windowHeight; // Dimensions of the button in the window
	string text; // Text to be displayed in the text input box
	GameObject* gameObject; // Used to get and set the variables of the game object
	bool active; // Indicates whether the text input box is active (selected for input)

	TextInput(float posX, float posY, float w, float h,
		float winX, float winY, float winW, float winH,
		const string& txt, Field f)
		: x(posX), y(posY), width(w), height(h),
		windowX(winX), windowY(winY), windowWidth(winW), windowHeight(winH),
		text(txt), active(false), gameObject(nullptr), field(f)
	{}

	void setActive(bool isActive) {
		active = isActive;
	}

	// Convert text to float
	float textToFloat() {
		float result;
		stringstream ss(text);
		ss >> result;
		return result;
	}

	void handleKeyPress(unsigned char key) {
		if (active) {
			if (key == '\b' && !text.empty()) { // Backspace key
				text.pop_back();
			}
			else if ((key >= '0' && key <= '9') || key == '.' || key == '-') {
				text += key;
			}
			if (gameObject != nullptr)
				switch (field) {
				case X:       gameObject->x = textToFloat(); break;
				case Y:       gameObject->y = textToFloat(); break;
				case Z:       gameObject->z = textToFloat(); break;
				case X_SCALE: gameObject->sizeX = textToFloat(); break;
				case Y_SCALE: gameObject->sizeY = textToFloat(); break;
				}
		}
	}

	bool isInside(int mouseX, int mouseY) {
		// Calculate the boundaries of the button
		float leftBoundary = windowX - windowWidth / 2;
		float rightBoundary = windowX + windowWidth / 2;
		float bottomBoundary = windowY - windowHeight / 2;
		float topBoundary = windowY + windowHeight / 2;

		// Check if mouseX falls within the button's x-boundaries
		bool insideX = (mouseX >= leftBoundary && mouseX <= rightBoundary);

		// Check if mouseY falls within the button's y-boundaries
		bool insideY = (mouseY >= bottomBoundary && mouseY <= topBoundary);

		// Return true only if both mouseX and mouseY fall within the button's boundaries
		return insideX && insideY;
	}

	void draw() {
		// Draw the text input box background
		glColor3f(/* Red */ 43.0f / 255.0f, /* Green */ 31.0f / 255.0f, /* Blue */ 41.0f / 255.0f);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
		glEnd();

		// Draw the text inside the text input box
		glColor3f(1.0f, 1.0f, 1.0f); // Input Text Color
		glRasterPos2f(x + 0.05f, y + (height / 2) - 0.05f); // Adjust position for centering
		for (char c : text) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
		}
	}
};

// Button class
class Button {
public:
	float x, y; // Position of the button in the viewport
	float width, height; // Dimensions of the button in the viewport
	float windowX, windowY; // Position of the button in the window
	float windowWidth, windowHeight; // Dimensions of the button in the window
	string text; // Text to be displayed on the button
	bool clicked; // Indicates whether the button has been clicked
	function<void(Button&)> buttonAction; // Action handler for the button
	// Had to update here to allow for button passing
	GameObject* gameObject; // Used to get and set the variables of the game object
	int textureNum; // Texture number
	bool sprite; // If there is a sprite or not

	// x, y, w, h, windowX, windowY, windowWidth, windowHeight, text, buttonAction, gameObject pointer
	// Updated to allow for the passing of a gameObject reference but is not necessary
	Button(float posX, float posY, float w, float h,
		float winX, float winY, float winW, float winH,
		function<void(Button&)> act, const string& txt, GameObject* obj = nullptr)
		: x(posX), y(posY), width(w), height(h),
		windowX(winX), windowY(winY), windowWidth(winW), windowHeight(winH),
		text(txt), buttonAction(act), clicked(false), gameObject(obj), textureNum(0), sprite(false) {
	}

	bool isInside(int mouseX, int mouseY) {
		// Calculate the boundaries of the button
		float leftBoundary = windowX - windowWidth / 2;
		float rightBoundary = windowX + windowWidth / 2;
		float bottomBoundary = windowY - windowHeight / 2;
		float topBoundary = windowY + windowHeight / 2;

		// Check if mouseX falls within the button's x-boundaries
		bool insideX = (mouseX >= leftBoundary && mouseX <= rightBoundary);

		// Check if mouseY falls within the button's y-boundaries
		bool insideY = (mouseY >= bottomBoundary && mouseY <= topBoundary);

		// Return true only if both mouseX and mouseY fall within the button's boundaries
		return insideX && insideY;
	}

	// Method to handle button click
	void handleClick() {
		if (buttonAction) {
			buttonAction(*this); // Invoke the action handler and passes the button now
		}
	}

	// Method to draw the button
	void draw() {
		// Calculate the position to center the button
		float centerX = x - width / 2;
		float centerY = y - height / 2;

		// Draw the button background using the calculated center position
		glColor3f(/* Red */ 42.0f / 255.0f, /* Green */ 30.0f / 255.0f, /* Blue */ 40.0f / 255.0f);
		glBegin(GL_QUADS);
		glVertex3f(centerX, centerY, 0);
		glVertex3f(centerX + width, centerY, 0);
		glVertex3f(centerX + width, centerY + height, 0);
		glVertex3f(centerX, centerY + height, 0);
		glEnd();

		// Adjust horizontal position to center the text horizontally within the button
		float textX = centerX + 0.1;

		// Adjust vertical position to center the text vertically within the button
		float textY = centerY + (height - 0.2) / 2;

		// Draw the button text
		glColor3f(1.0f, 1.0f, 1.0f); 
		glRasterPos3f(textX, textY, 1);
		for (int i = 0; i < text.length(); i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
		}
	}

	void drawAssetButton() {
		// Calculate the position to center the button
		float centerX = x - width / 2;
		float centerY = y - height / 2;

		// Draw the button background using the calculated center position
		glColor3f(/* Red */ 42.0f / 255.0f, /* Green */ 30.0f / 255.0f, /* Blue */ 40.0f / 255.0f);
		glBegin(GL_QUADS);
		glVertex3f(centerX, centerY, 0);
		glVertex3f(centerX + width, centerY, 0);
		glVertex3f(centerX + width, centerY + height, 0);
		glVertex3f(centerX, centerY + height, 0);
		glEnd();

		float widthAdjust = centerX + width / 10;
		float heightAdjust = centerY + height / 3;

		if (sprite) {
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glEnable(GL_TEXTURE_2D); // Enable texturing

			glBindTexture(GL_TEXTURE_2D, texID[textureNum]); // Which texture

			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(widthAdjust, heightAdjust, 0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(widthAdjust + (width - (width / 10) * 2), heightAdjust, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(widthAdjust + (width - (width / 10) * 2), heightAdjust + (height / 2), 0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(widthAdjust, heightAdjust + (height / 2), 0);
			glEnd();

			glDisable(GL_TEXTURE_2D); // Turn texturing off
		}

		else {
			glColor3f(1, 0, 1); // Purple color for the sprite in the button
			glBegin(GL_QUADS);
			glVertex3f(widthAdjust, heightAdjust, 0);
			glVertex3f(widthAdjust + (width - (width / 10) * 2), heightAdjust, 0);
			glVertex3f(widthAdjust + (width - (width / 10) * 2), heightAdjust + (height / 2), 0);
			glVertex3f(widthAdjust, heightAdjust + (height / 2), 0);
			glEnd();
		}

		// Adjust horizontal position to center the text horizontally within the button
		float textX = centerX + 0.1;

		// Adjust vertical position to center the text vertically within the button
		float textY = centerY + (height / 10);

		// Draw the button text
		glColor3f(1.0f, 1.0f, 1.0f); // Button Text Color
		glRasterPos3f(textX, textY, 1);
		for (int i = 0; i < text.length(); i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
		}
	}

	// Static method to create and add a new button to a specified list
	static void addToList(list<Button>& buttonList, float x, float y, float w, float h,
		float winX, float winY, float winW, float winH,
		function<void(Button&)> action, const string& text, GameObject* obj) {

		// Create a new button instance and add it to the list
		buttonList.emplace_back(x, y, w, h, winX, winY, winW, winH, action, text, obj);
	}
};

// List of buttons (Hint: You could make multiple lists for the different panels buttons)
list<Button> leftPanelButtons, bottomPanelButtons;


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Create a list of buttons for the Right Panel

// Text input box button X
TextInput textInputX(-5.8, 5.4, 4.5, 0.4, 850, 109, 66, 14, "", TextInput::X);

// Text input box button Y
TextInput textInputY(-5.8, 4.6, 4.5, 0.4, 850, 145, 66, 14, "", TextInput::Y);

// Text input box button Z
TextInput textInputZ(-5.8f, 3.8f, 4.5f, 0.4f, 850, 179, 66, 14, "", TextInput::Z);


// Give buttons functions by making functions and 
// assiging them when the button is made.
void hierarchyButton(Button& button) {
	GameObject* o = button.gameObject;
	if (o != nullptr) {
		// Populate X field
		textInputX.text = to_string(o->x);
		textInputX.gameObject = o;
		cout << "hierarchyButton: loaded X = " << o->x << endl;

		// Populate Y field
		textInputY.text = to_string(o->y);
		textInputY.gameObject = o;
		cout << "hierarchyButton: loaded Y = " << o->y << endl;

		// Populate Z field
		textInputZ.text = to_string(o->z);
		textInputZ.gameObject = o;
		cout << "hierarchyButton: loaded Y = " << o->z << endl;
	}
	else {
		cout << "hierarchyButton: Button's gameObject is null" << endl;
	}

	cout << "hierarchyButton: Completed button action" << endl;
}






// Adds a ground asset to the game
void buttonAddGround(Button& button) {
	// Define actions for buttons
	GameObject ground;
	ground.colorG = 0;
	platforms.emplace_back(ground);

	// Top left corner is (0, 0), so if you want to make a list of 
	// buttons that you can just add, you'll need to add to the 
	// windowY rather than subtract to go down the window.
	button.addToList(leftPanelButtons, 0, leftPanelButtons.back().y - 0.7, 14, 0.5,
		100, leftPanelButtons.back().windowY + 30, 200, 20,
		hierarchyButton, "Ground " + to_string(leftPanelButtons.size()), &platforms.back());

	printf("Completed action add ground \n");
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void init(void) {

	glClearColor((52.0f / 255.0f), (32.0f / 255.0f), (43.0f / 255.0f), 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-7.0, 7.0, -7.0, 7.0, -10.0, 10.0);

	resetJumpTimer = jumpTimer;

	Player.sizeX = 1.0f;
	Player.sizeY = 1.0f;
	Player.x = 0.0f;
	Player.y = 2.0f; // Make sure this is initially above ground

	BottomCheck.sizeX = 0.6f;
	BottomCheck.sizeY = 0.2f;
	LeftCheck.sizeX = 0.2f;
	LeftCheck.sizeY = 0.6f;
	RightCheck.sizeX = 0.2f;
	RightCheck.sizeY = 0.6f;
	TopCheck.sizeX = 0.6f;
	TopCheck.sizeY = 0.2f;

	// Ensure initial ground exists
	GameObject ground;
	ground.x = -5.0f;
	ground.y = -1.0f;
	ground.sizeX = 10.0f;
	ground.sizeY = 1.0f;
	ground.isSolid = true;
	platforms.push_back(ground);

	BottomCheck.colorR = 0;
	LeftCheck.colorR = 0;
	RightCheck.colorR = 0;
	TopCheck.colorR = 0;

	Collectible.x = -4;
	Collectible.y = 2;
	Collectible.sizeX = 0.5;
	Collectible.sizeY = 0.5;
	Collectible.colorB = 0;

	SoundEngine->play2D("audio/The Return of Caped Crusader Cat.mp3", true);

	// Create buttons and add them to the list
	Button button1(0, 6, 14, 0.5, 100, 92, 200, 20, hierarchyButton, "Player", &Player);
	// Add buttons to list
	leftPanelButtons.push_back(button1);


	// Bottom buttons
	Button bottomButton(-6, -1, 1, 8, 70, 763, 70, 114, buttonAddGround, "  Ground");
	bottomPanelButtons.push_back(bottomButton);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void CreateMechanics()
{
	glPushMatrix();
	// Simply display the timer and coin count without modifying the timer value
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(Player.x - 6.3f, Player.y + 9.5f);
	string timerText = "Time: " + std::to_string(timeLeft);
	for (char c : timerText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

	glRasterPos2f(Player.x + 4.3f, Player.y + 8.0f);
	string coinText = "Espresso: " + to_string(coffeeCollected) + "/" + to_string(totalcoffee);
	for (char c : coinText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glPopMatrix();
}

void coffeeAnimationTimer(int value)
{
	coffeeAnimFrame = (coffeeAnimFrame + 1) % 4;
	glutPostRedisplay();  // Redraw the scene with the new coffee frame
	glutTimerFunc(coffeeAnimDelay, coffeeAnimationTimer, 0);  // Schedule next frame update
}

void editorCameraLogic()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(editorCameraX, editorCameraY, 5, editorCameraX, editorCameraY, 0, 0, 1, 0);
}

// Game Camera Logic
void gameCameraLogic()
{
	// Viewport Game Camera: Essential
	cameraX = Player.x + Player.sizeX / 2 + offsetX;
	cameraY = Player.y + Player.sizeY / 2 + offsetY;

	// Camera Rules
		// Out of Bounds Camera: Extra
	if (Player.y == -3.0f) cameraY = -3.0f;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void drawAxis()
{
	glPushMatrix();

	if (GameScene)
	{
		glTranslatef(cameraX, cameraY - 2, 0.0f);
	}
	else if (EditScene)
	{
		glTranslatef(editorCameraX, editorCameraY - 2, 0.0f);
	}


	// X-axis (Red)
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();

	// Y-axis (Green)
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	glPopMatrix();
}

void drawGrid()
{
	glPushMatrix();

	if (currentScene == EditScene)
		glTranslatef(editorCameraX, editorCameraY, 0.0f);
	else
		glTranslatef(cameraX, cameraY, 0.0f);

	glColor3f(0.3f, 0.3f, 0.3f); // Grid line color

	float gridSize = 1.0f;  // Size of each square
	int gridExtent = 50;    // Number of lines in each direction
	glBegin(GL_LINES);
	for (int i = -gridExtent; i <= gridExtent; ++i)
	{
		// Vertical lines
		glVertex3f(i * gridSize, -gridExtent * gridSize, 0.0f);
		glVertex3f(i * gridSize, gridExtent * gridSize, 0.0f);

		// Horizontal lines
		glVertex3f(-gridExtent * gridSize, i * gridSize, 0.0f);
		glVertex3f(gridExtent * gridSize, i * gridSize, 0.0f);
	}
	glEnd();
	glPopMatrix();

}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CreatePlayer(bool show) {
	// Draw Player sprite and ground check
	glPushMatrix();
	Player.DrawPlayer(true);
	BottomCheck.x = Player.x + 0.2;
	BottomCheck.y = Player.y;
	BottomCheck.sizeX = 0.6;
	BottomCheck.sizeY = 0.2;

	BottomCheck.canSee = show;

	LeftCheck.x = Player.x;
	LeftCheck.y = Player.y + 0.2;
	LeftCheck.sizeX = 0.2;
	LeftCheck.sizeY = 0.6;

	LeftCheck.canSee = show;

	RightCheck.x = Player.x + 0.8;
	RightCheck.y = Player.y + 0.2;
	RightCheck.sizeX = 0.2;
	RightCheck.sizeY = 0.6;

	RightCheck.canSee = show;

	TopCheck.x = Player.x + 0.2;
	TopCheck.y = Player.y + 0.8;
	TopCheck.sizeX = 0.6;
	TopCheck.sizeY = 0.2;

	TopCheck.canSee = show;

	BottomCheck.DrawGameObject(false);
	LeftCheck.DrawGameObject(false);
	RightCheck.DrawGameObject(false);
	TopCheck.DrawGameObject(false);
	glPopMatrix();
}

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

// Updated to match the new list for all the ground platforms
void gravityCheck()
{
	if (currentScene != GameScene) return;
	onGround = false;

	// Bottom Check – detect standing on ground
	for (auto& ground : platforms)
	{
		if (ground.isSolid && CheckCollision(BottomCheck, ground))
		{
			onGround = true;
			break;
		}
	}

	// Left Check – push player right
	for (auto& ground : platforms)
	{
		if (ground.isSolid && CheckCollision(LeftCheck, ground))
		{
			Player.x += speed;
			break;
		}
	}

	// Right Check – push player left
	for (auto& ground : platforms)
	{
		if (ground.isSolid && CheckCollision(RightCheck, ground))
		{
			Player.x -= speed;
			break;
		}
	}

	// Top Check – stop jump if hitting ceiling
	for (auto& ground : platforms)
	{
		if (ground.isSolid && CheckCollision(TopCheck, ground) && jump)
		{
			jump = false;
			jumpTimer = resetJumpTimer;
			break;
		}
	}

	// If not on ground: apply gravity
	if (!onGround)
	{
		Player.y -= gravity;
	}

	// Fall out of bounds
	if (Player.y < -10.0)
	{
		currentScene = LoseScene;
		playLoseMusic();
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// UI Shapes

// Draws a square and you can determine the size, position, and color.
void drawSquare(float sizeX, float sizeY, float sizeZ, float x, float y, float z, float r, float g, float b) 
{
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

void drawTextWithBG(string text, float x, float y, float width, float height, bool showBG) 
{
	// Calculate the position to center the text with background
	float centerX = x - width / 2;
	float centerY = y - height / 2;

	// Draw the text background using the calculated center position
	if (showBG) { // Turn background on or off
		glColor3f(/* Red */  44.5f / 255.0f, /* Green */ 30.5f / 255.0f, /* Blue */ 41.5f / 255.0f);
		glBegin(GL_QUADS);
		glVertex3f(centerX, centerY, 0);
		glVertex3f(centerX + width, centerY, 0);
		glVertex3f(centerX + width, centerY + height, 0);
		glVertex3f(centerX, centerY + height, 0);
		glEnd();
	}

	// Adjust horizontal position to center the text horizontally within the backgound
	float textX = centerX + 0.1;

	// Adjust vertical position to center the text vertically within the background
	float textY = centerY + (height - 0.2) / 2;

	// Draw the button text
	glColor3f(/* Red */  246.0f / 255.0f, /* Green */ 231.0f / 255.0f, /* Blue */ 254.0f / 255.0f); // Text Color
	glRasterPos3f(textX, textY, 1);
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




// UI Functions

void drawTopPanelMessages()
{
	glPushMatrix();
	if (displaySceneStateDisplay)
	{
		switch (currentScene)
		{
		case EditScene:
			drawTextWithBG("Scene: Edit", 12.5f, 3.9, 14.0f, 0.4f, false);
			break;

		case MenuScene:
			drawTextWithBG("Scene: Menu", 12.5f, 3.9, 14.0f, 0.4f, false);
			break;

		case GameScene:
			drawTextWithBG("Scene: Game", 12.5f, 3.9, 14.0f, 0.4f, false);
			break;

		case WinScene:
			drawTextWithBG("Scene: Win", 12.5f, 3.9, 14.0f, 0.4f, false);
			break;

		case LoseScene:
			drawTextWithBG("Scene: Lose", 12.5f, 3.9, 14.0f, 0.4f, false);
			break;
		}
	}
	glPopMatrix();
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Main Panel Scenes

void ActiveGame()
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Behaivor While Following Player
	gameCameraLogic();

	gluLookAt(cameraX, cameraY, 5, cameraX, cameraY, 0, 0, 1, 0);

	// Makes the Player
	CreatePlayer(showCollision);

	// Make the ground
	for (auto& ground : platforms) {
		ground.DrawGameObject(false);
	}

	// Makes the Collectible
	Collectible.DrawGameObject(false);

	// Removes the Collectible
	if (CheckCollision(Player, Collectible))
		Collectible.destroyed = true;

		gravityCheck();
		CreateMechanics();
	//Movement
	if (lt)
		Player.x -= speed;
	if (rt)
		Player.x += speed;
	glPopMatrix();

	

}

void MainMenu()
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);

	// Title
	glRasterPos2f(-1.3f, -1.0f);
	const char* title = "Caffeine Crusader";
	for (int i = 0; title[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, title[i]);
	}

	// Instructions
	glRasterPos2f(-0.75f, -1.6f);
	const char* menuMsg = "Press G to Start";
	for (int i = 0; menuMsg[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menuMsg[i]);
	}
	glRasterPos2f(-0.75f, -1.4f);
	const char* menuDescripiton = "Collect all 10 Espresso Shots and return before the clock hits 0";
	for (int i = 0; menuDescripiton[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menuDescripiton[i]);
	}
	glRasterPos2f(-0.75f, -1.2f);
	const char* menuComment = "Feed your addiction before you crash!";
	for (int i = 0; menuComment[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menuComment[i]);
	}
	glPopMatrix();
}

void PauseGame()
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glRasterPos2f(0.8f, 1.5f);
	const char* pauseTitle = "Game Paused";
	for (int i = 0; pauseTitle[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, pauseTitle[i]);
	}

	glRasterPos2f(0.4f, 1.0f);
	const char* pauseMsg = "Press P to Resume";
	for (int i = 0; pauseMsg[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, pauseMsg[i]);
	}
	glPopMatrix();
}

void LoseGame()
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glRasterPos2f(0.8f, 1.5f);
	const char* message = "Game Over";
	for (int i = 0; message[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
	}
	glRasterPos2f(Player.x - 0.6f, Player.y + 1.0f);
	const char* gameOverMsg = "You were too eepy :(";
	for (int i = 0; gameOverMsg[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, gameOverMsg[i]);
	}
	glPopMatrix();
}

void WinGame()
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glRasterPos2f(0.5f, 1);
	const char* message = "YOU WIN!";
	for (int i = 0; message[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
	}
	glRasterPos2f(0.3f, 0.5f);
	const char* winMsg = "Wow! Awesome! B)";
	for (int i = 0; winMsg[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, winMsg[i]);
	}
	glPopMatrix();
}

void EditGame()
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Debug
	if (axis) drawAxis();
	if (gridMode) drawGrid();


	// Makes the Player
	CreatePlayer(showCollision);

	// Make the ground
	for (auto& ground : platforms) {
		ground.DrawGameObject(false);
	}

	// Makes the Collectible
	Collectible.DrawGameObject(false);

	// Removes the Collectible
	if (CheckCollision(Player, Collectible))
		Collectible.destroyed = true;

	if (gravityOn)
		gravityCheck();

	//Movement
	if (lt)
		Player.x -= speed;
	if (rt)
		Player.x += speed;

	glPopMatrix();
}

// UI Parts

void drawLeftPanel() {

	glPushMatrix();
	glViewport(0, BOTTOM_PANEL_H, SIDE_PANEL_W, MIDDLE_PANEL_H);

	// Dark Grey background
	drawSquare(15, 14, 1, 0, 0, 0,  /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);

	// Draw text box to label panel
	drawTextWithBG("                  Hierarchy",
		0, 6.6, 14, 0.8, true);

	// Draw the buttons
	for (auto& button : leftPanelButtons) {
		button.draw();
	}

	glPopMatrix();
}

void drawMainPanel() {
	if (currentScene == MenuScene) MainMenu();
	else if (currentScene == PauseScene) PauseGame();
	else if (currentScene == LoseScene) LoseGame();
	else if (currentScene == WinScene) WinGame();
	else if (currentScene == GameScene) ActiveGame();
	else if (currentScene == EditScene) EditGame();
}

void drawRightPanel() {
	glPushMatrix();
	glViewport(SIDE_PANEL_W + MAIN_PANEL_W, BOTTOM_PANEL_H, SIDE_PANEL_W, MIDDLE_PANEL_H);

	// Background
	drawSquare(15, 14, 1, 0, 0, 0, /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);

	// Draw text box to label panel
	drawTextWithBG("                 Inscpector",
		0, 6.6, 14, 0.8, true);

	// Draw text box for x coordinate label
	drawTextWithBG("X:", -4, 5.6, 6, 0.5, true);

	// Draw the text box
	textInputX.draw();


	drawTextWithBG("Y:", -4, 4.8f, 6, 0.5f, true);

	// Draw the text box
	textInputY.draw();

	drawTextWithBG("Z:", -4, 4.0f, 6, 0.5f, true);
	textInputZ.draw();

	glPopMatrix();
}

void drawTopPanel() {
	glPushMatrix();
	glViewport(0, WIN_H - TOP_PANEL_H, WIN_W, TOP_PANEL_H);

	//glScalef(1, 20, 1);

	// Blue background 
	drawSquare(20, 20, 1, 0, 0, 0, /* Red */ 42.0f / 255.0f, /* Green */ 30.0f / 255.0f, /* Blue */ 40.0f / 255.0f);
	glPopMatrix();

	// Functions
	drawTopPanelMessages();
}

void drawBottomPanel() {
	glPushMatrix();
	glViewport(0, 0, WIN_W, BOTTOM_PANEL_H);

	//glScalef(1, 5, 1);

	// Dark Grey background
	drawSquare(14, 15, 1, 0, 0, 0, /* Red */ 42.0f / 255.0f, /* Green */ 30.0f / 255.0f, /* Blue */ 40.0f / 255.0f);

	drawTextWithBG("                  Assets",
		0, 5.7, 14, 2.6, true);

	// Draw the buttons
	for (auto& button : bottomPanelButtons) {
		button.drawAssetButton();
	}

	glPopMatrix();
}

void MyDisplay() {
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	drawLeftPanel();
	drawMainPanel();
	drawRightPanel();
	drawTopPanel();
	drawBottomPanel();

	glutSwapBuffers();
}

// 

// Controls

void specialKeyboard(int key, int x, int y)
{
	if (currentScene == GameScene) {
		switch (key) {
		case GLUT_KEY_LEFT:
			lt = true;
			rt = false;
			facingLeft = true;
			currentState = RunLeft;
			break;

		case GLUT_KEY_RIGHT:
			rt = true;
			lt = false;
			facingLeft = false;
			currentState = RunRight;
			break;
		}
	}
	glutPostRedisplay();
}

void specialKeyboardRelease(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		lt = false;
		currentState = IdleLeft;
		PlayerAnimFrame = 0;
		break;
	case GLUT_KEY_RIGHT:
		rt = false;
		currentState = IdleRight;
		PlayerAnimFrame = 0;
		break;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	// Text Box Input | Source: 
	textInputX.handleKeyPress(key);
	textInputY.handleKeyPress(key);
	textInputZ.handleKeyPress(key);

	switch (key)
	{
	case 'a': // Axis Toggle 
		axis = !axis;
		break;

	case 's':
		showCollision = !showCollision;
		break;
	case 32: // Spacebar: Jump while in game
		if (currentScene == GameScene && onGround && !jump)
		{
			jump = true;
			currentState = facingLeft ? JumpLeft : JumpRight;
			playJumpSFX();
		}
		break;
	case 'g':
		if (currentScene == MenuScene)
		{
			currentScene = GameScene;
			playGameMusic();
		}
		break;
	case 'p': // Toggle Pause while in game 
		if (currentScene == GameScene)
		{
			currentScene = PauseScene;
			SoundEngine->setAllSoundsPaused(true);
		}
		else if (currentScene == PauseScene)
		{
			currentScene = GameScene;
			SoundEngine->setAllSoundsPaused(false);
		}
		break;
	case 27: // Escape
		exit(0);

	case 'e': // Toggle Edit Mode
		if (currentScene == MenuScene)
			currentScene = EditScene;
		else if (currentScene == EditScene)
			currentScene = MenuScene;
		break;

	case 'z': // Toggle grid in edit mode
		if (currentScene == EditScene)
			gridMode = !gridMode;

		break;
	}

	glutPostRedisplay();
}


void MouseControl(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Initialize world coordinates and clicked panel
		float worldX, worldY;
		string clickedPanel;

		// Determine which viewport was clicked
		if (x >= 0 && x < SIDE_PANEL_W) {
			// Left panel clicked
			clickedPanel = "Left Panel";
		}
		else if (x >= SIDE_PANEL_W && x < SIDE_PANEL_W + MAIN_PANEL_W) {
			// Main panel clicked
			clickedPanel = "Main Panel";
		}
		else if (x >= SIDE_PANEL_W + MAIN_PANEL_W && x < SIDE_PANEL_W + MAIN_PANEL_W * 2) {
			// Right panel clicked
			clickedPanel = "Right Panel";
		}
		else {
			// Main panel clicked
			clickedPanel = "Main Panel";
		}

		if (y >= WIN_H - BOTTOM_PANEL_H) {
			// Bottom panel clicked
			clickedPanel = "Bottom Panel";
		}
		else if (y < TOP_PANEL_H) {
			// Top panel clicked
			clickedPanel = "Top Panel";
		}

		cout << "Clicked in " << clickedPanel << " at window coordinates (" << x << ", " << y << ")" << endl;

		// Check if any button was clicked
		for (auto& button : leftPanelButtons) {
			if (button.isInside(x, y)) {
				button.handleClick(); // Trigger the action associated with the button
				break; // Exit the loop after handling the click for one button
			}
		}

		for (auto& button : bottomPanelButtons) {
			if (button.isInside(x, y)) {
				button.handleClick(); // Trigger the action associated with the button
				break; // Exit the loop after handling the click for one button
			}
		}
	}
	int invertedY = WIN_H - y;

	// Check if clicked in or out of text box to set as active or not.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
		if (textInputX.isInside(x, y)) {
			textInputX.setActive(true);
			textInputY.setActive(false);
		}
		else if (textInputY.isInside(x, y)) {
			textInputY.setActive(true);
			textInputX.setActive(false);
		}
		else if (textInputZ.isInside(x, y)) {
			textInputX.setActive(false);
			textInputY.setActive(false);
			textInputZ.setActive(true);
		}
		else {
			textInputX.setActive(false);
			textInputY.setActive(false);
			textInputZ.setActive(false);
		}
	}

	glutPostRedisplay();
}

// 

// Texture Logic

void loadTextures()
{
	int i;
	glGenTextures(70, texID); // Get the texture object IDs (Reserve IDs)
	for (i = 0; i < 70; i++)
	{
		// Load image with FreeImage
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
		if (format == FIF_UNKNOWN)
		{
			printf("Unknown file type for texture image file %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], PNG_DEFAULT); // Load PNG files
		if (!bitmap)
		{
			printf("Failed to load image %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap); // Convert to 32-bit (with alpha channel)
		FreeImage_Unload(bitmap);
		if (bitmap32)
		{
			BYTE* bits = FreeImage_GetBits(bitmap32);
			int width = FreeImage_GetWidth(bitmap32);
			int height = FreeImage_GetHeight(bitmap32);
			for (int j = 0; j < width * height; j++)
			{
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

// Animation Logic


void updateAnimationFrame()
{
	// Jumping Frame
	if (jump)
	{
		// Left Facing Jump
		if (facingLeft)
		{
			// example: pick frame 14 for jump-left
			displayFrame = 2;
		}
		else
		{
			// Otherwise Face Right
			displayFrame = 15;
		}
		return;
	}

	// Otherwise, pick frames based on currentState:
	switch (currentState)
	{
	case RunLeft:
		// frames [0..7] for running left
		// playerAnimFrame cycles 0..7
		displayFrame = PlayerAnimFrame;
		break;

	case IdleLeft:
		// single frame = 8
		displayFrame = 8;
		break;

	case RunRight:
		// frames [9..16] for running right
		displayFrame = 9 + PlayerAnimFrame;
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

void cycleRunFrames()
{
	// Running Left
	if (currentState == RunLeft)
	{
		PlayerAnimFrame++;
		if (PlayerAnimFrame > 7) PlayerAnimFrame = 0;
	}

	// Running Right
	else if (currentState == RunRight)
	{
		PlayerAnimFrame++;
		if (PlayerAnimFrame > 7) PlayerAnimFrame = 0;
	}
}	


//

// Global Timer Logic
void timer(int v) {
	if (currentScene == GameScene) {
		static int gameTimerAccumulator = 0;
		gameTimerAccumulator += 16; // ~60 FPS timing
		if (gameTimerAccumulator >= 1000) {
			timeLeft--;
			gameTimerAccumulator = 0;
			if (timeLeft <= 0) {
				currentScene = LoseScene;
				playLoseMusic();
			}
		}

		if (jump)
		{
			if (jumpTimer > 0)
			{
				if (lt)
				{
					currentState = JumpLeft;
					facingLeft = true;
				}
				else if (rt)
				{
					currentState = JumpRight;
					facingLeft = false;
				}
				Player.y += jumpVelocity;
				jumpVelocity -= jumpAcceleration;
				jumpTimer--;
			}
			else
			{
				jump = false;
				jumpTimer = resetJumpTimer;
				jumpVelocity = 0.65f;
				currentState = (lt ? RunLeft : (rt ? RunRight : (facingLeft ? IdleLeft : IdleRight)));
			}
		}

		// Check gravity and collisions
		gravityCheck();

		// Update player animation frames
		cycleRunFrames();
		updateAnimationFrame();
	}

	glutPostRedisplay();
	glutTimerFunc(16, timer, v); // Consistent frame rate
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // RGB mode with double buffering

	// Initialize window and create it
	glutInitWindowSize(WIN_W, WIN_H);
	glutCreateWindow("V Cavallaro | 811097945");

	glutTimerFunc(0, timer, 0);
	glutTimerFunc(coffeeAnimDelay, coffeeAnimationTimer, 0);

	init();

	loadTextures();

	glutDisplayFunc(MyDisplay); // call the drawing function

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardRelease);
	glutMouseFunc(MouseControl);

	// Terminal Msgs
	std::cout << std::endl;
	std::cout << "Controls:\n";
	std::cout << "Arrow Keys: Move the Player\n";
	std::cout << "Space Bar: Jump\n";
	std::cout << "'a': Toggle axis\n";
	std::cout << "'g': Start Game\n";
	std::cout << "'p': Pause Game\n";
	std::cout << "'s': Debug Collider Boxs\n";
	std::cout << "ESC: Exit\n\n";

	std::cout << "Bonus Features:\n";
	std::cout << "Movement State Conditions\n\n";

	std::cout << "Credits:\n";
	std::cout << "The Legend of Zelda: Majora's Mask\n";
	std::cout << "Bakudas: Generic Dungeon Pack\n";
	std::cout << "Wulax:  Medieval fantasy character sprites\n";

	glutMainLoop();
	return 0;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor Abyss: Game Object Implementation

GameObject::GameObject()
{
	x = y = z = 0;
	sizeX = sizeY = 1;
	colorR = colorG = colorB = 1;
	mass = 0;
	canSee = true;
	isSolid = true;
	destroyed = false;
	gravity = false;
	textureIndex = 0;
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

			if (textureIndex == 18)
			{ // Coffee Sprite Condiiton
				glBindTexture(GL_TEXTURE_2D, texID[18 + coffeeAnimFrame]);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, texID[textureIndex]);
			}

			glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, -0.5f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.5f + sizeX - 1.0f, -0.5f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.5f + sizeX - 1.0f, 0.5f + sizeY - 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.5f, 0.5f + sizeY - 1.0f, 0.0f);
			glEnd();

			glDisable(GL_TEXTURE_2D); // Turn texturing off
		}
		else // Fallback
		{
			glColor3f(colorR, colorG, colorB);

			glBegin(GL_POLYGON);
			glVertex3f(-0.5f, -0.5f, 0.0f);
			glVertex3f(0.5f + sizeX - 1.0f, -0.5f, 0.0f);
			glVertex3f(0.5f + sizeX - 1.0f, 0.5f + sizeY - 1.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f + sizeY - 1.0f, 0.0f);
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
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5f, -0.5f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.5f + sizeX - 1.0f, -0.5f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.5f + sizeX - 1.0f, 0.5f + sizeY - 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.5f, 0.5f + sizeY - 1.0f, 0.0f);
			glEnd();

			glDisable(GL_TEXTURE_2D); // Turn texturing off
		}
		else
		{
			glColor3f(colorR, colorG, colorB);

			glBegin(GL_POLYGON);
			glVertex3f(-0.5f, -0.5f, 0.0f);
			glVertex3f(0.5f + sizeX - 1.0f, -0.5f, 0.0f);
			glVertex3f(0.5f + sizeX - 1.0f, 0.5f + sizeY - 1.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f + sizeY - 1.0f, 0.0f);
			glEnd();
		}
	}

	glPopMatrix();
}