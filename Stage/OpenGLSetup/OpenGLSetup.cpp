// Game Engines		  | Project 3: 2D Platformer
// Vincenzo Cavallaro | Professor Meldin Bektic
// April 22, 2025

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
#include <fstream>
#include <functional>
#include <string>
#include <vector>
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
bool gravityOn = false;
bool playerActive = false;

// Mechanic Values
int totalcoffee = 10;					// Collectibles (Total)
int coffeeCollected = 0;				// Collectibles (Gathered)
static int timeLeft = 60;
int loop = 0;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool gridMode = true;
bool displaySceneStateDisplay = true;
bool showFileMenu = false;
bool showSaveSubmenu = false;
bool showLoadSubmenu = false;
bool showHelpMenu = false;
bool scrolling = false;

float hierarchyScrollY = 0;

float editorCameraX = 0.0f;
float editorCameraY = 0.0f;

int groundCount = 0;
int collectableCount = 0;
int hazardCount = 0;
int exitCount = 0;
int playerCount = 0;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Game States
enum Scenes
{
	MenuScene,
	GameScene,
	LoseScene,
	WinScene,
	PauseScene,
	EditScene,
};
Scenes currentScene = EditScene;

const int MAX_SCENES = 10;
int currentSceneIndex = 0;
vector<string> sceneFiles;
vector<pair<float, float>> sceneStartPos;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

void playTrack1()
{
	if (buttonDebug == false)
	{
		SoundEngine->stopAllSounds();
		SoundEngine->play2D("audio/01 Green And Amber Herb.mp3", true);
	}
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
std::list<GameObject> collectibles;
std::list<GameObject> hazards;
std::list<GameObject> exits;
std::list<GameObject> players;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Text Input Class
class TextInput 
{
public:

	enum Field 
	{
		X,
		Y,
		Z,
		X_SCALE,
		Y_SCALE,
		TEXTURE
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

	void setActive(bool isActive) 
	{
		active = isActive;
	}

	// Convert text to float
	float textToFloat()
	{
		float result;
		stringstream ss(text);
		ss >> result;
		return result;
	}

	void handleKeyPress(unsigned char key) 
	{
		if (active) 
		{
			if (key == '\b' && !text.empty()) 
			{ // Backspace key
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
				case TEXTURE: gameObject->textureIndex = static_cast<int>(textToFloat()); break;
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

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<typename C1, typename C2>
void collideLists(C1& aList, C2& bList,
	std::function<void(GameObject&, GameObject&)> onHit)
{
	for (auto& A : aList) {
		if (A.destroyed || !A.isSolid) continue;
		for (auto& B : bList) {
			if (&A == &B)               continue; // skip self when same container
			if (B.destroyed || !B.isSolid) continue;
			if (CheckCollision(A, B))
				onHit(A, B);
		}
	}
}

// Check for collision between two game objects
void handleCollision(GameObject* A, GameObject* B)
{
	// Collectibles 
	if ((A == &Player && std::find_if(collectibles.begin(), collectibles.end(),
		[&](auto& c) { return &c == B; }) != collectibles.end())
		|| (B == &Player && std::find_if(collectibles.begin(), collectibles.end(),
			[&](auto& c) { return &c == A; }) != collectibles.end()))
	{
		GameObject* col = (A == &Player ? B : A);
		col->destroyed = true;
		++coffeeCollected;
		//LoadSlot(4);
		return;
	}
	// Hazards
	if ((A == &Player && std::find_if(hazards.begin(), hazards.end(),
		[&](auto& c) { return &c == B; }) != hazards.end())
		|| (B == &Player && std::find_if(hazards.begin(), hazards.end(),
			[&](auto& c) { return &c == A; }) != hazards.end()))
	{
		GameObject* col = (A == &Player ? B : A);
		col->destroyed = true;
		currentScene = LoseScene;
		playLoseMusic();
		return;
	}

	if ((A == &Player && std::find_if(hazards.begin(), hazards.end(),
		[&](auto& h) { return &h == B; }) != hazards.end())
		|| (B == &Player && std::find_if(hazards.begin(), hazards.end(),
			[&](auto& h) { return &h == A; }) != hazards.end()))
	{
		currentScene = LoseScene;
		playLoseMusic();
		return;
	}

	// Exits
	if ((A == &Player && std::find_if(exits.begin(), exits.end(),
		[&](auto& e) { return &e == B; }) != exits.end())
		|| (B == &Player && std::find_if(exits.begin(), exits.end(),
			[&](auto& e) { return &e == A; }) != exits.end()))
	{
		// only trigger win if you've collected them all
		if (coffeeCollected == totalcoffee && loop == 10) {
			currentScene = WinScene;
			playWinMusic();
		}
		return;
	}
}

// Button class
class Button // Obvious Source: Dynamic Button List
{
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

	bool isInside(int mouseX, int mouseY) 
	{
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
	void handleClick() 
	{
		if (buttonAction) 
		{
			buttonAction(*this); // Invoke the action handler and passes the button now
		}
	}

	// Method to draw the button
	void draw() 
	{
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

	void drawAssetButton() 
	{
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

		if (sprite) 
		{
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

		else 
		{
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
list<Button> leftPanelButtons, bottomPanelButtons, rightPanelButtons;


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Create a list of buttons for the Right Panel

// Text input box button X
TextInput textInputX(-5.8, 5.4, 4.5, 0.4, 850, 109, 66, 14, "", TextInput::X);

// Text input box button Y
TextInput textInputY(-5.8, 4.6, 4.5, 0.4, 850, 145, 66, 14, "", TextInput::Y);

// Text input box button Z
TextInput textInputZ(-5.8f, 3.8f, 4.5f, 0.4f, 850, 179, 66, 14, "", TextInput::Z);

// Text input box button X Scale
TextInput textInputXScale(-5.8f, 3.0f, 4.5f, 0.4f, 850, 213, 66, 14, "", TextInput::X_SCALE);

// Text input box button Y Scale
TextInput textInputYScale(-5.8f, 2.2f, 4.5f, 0.4f, 850, 247, 66, 14, "", TextInput::Y_SCALE);

// Text input box button Texture
TextInput textInputTexture(-3.2f, 1.4f, 1.9f, 0.4f, 850, 281, 66, 14, "", TextInput::TEXTURE);

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Give buttons functions by making functions and 
// assiging them when the button is made.
void hierarchyButton(Button& button) 
{
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

		// Populate X Scale field
		textInputXScale.text = to_string(o->sizeX);
		textInputXScale.gameObject = o;
		cout << "hierarchyButton: loaded X Scale = " << o->sizeX << endl;

		// Populate Y Scale field
		textInputYScale.text = to_string(o->sizeY);
		textInputYScale.gameObject = o;
		cout << "hierarchyButton: loaded Y Scale = " << o->sizeY << endl;

		// Populate Texture field
		textInputTexture.text = to_string(o->textureIndex);
		textInputTexture.gameObject = o;
		cout << "hierarchyButton: loaded Texture = " << o->textureIndex << endl;

		for (auto& tb : rightPanelButtons) {
			tb.gameObject = o;
		}
	}
	else {
		cout << "hierarchyButton: Button's gameObject is null" << endl;
	}

	cout << "hierarchyButton: Completed button action" << endl;
}

void toggleCollision(Button& b) 
{
	if (!b.gameObject) return;
	b.gameObject->isSolid = !b.gameObject->isSolid;
	std::cout << "Collision is now "
		<< (b.gameObject->isSolid ? "On\n" : "Off\n");
}
void toggleVisible(Button& b) 
{
	if (!b.gameObject) return;
	b.gameObject->canSee = !b.gameObject->canSee;
	std::cout << "Visibility is now "
		<< (b.gameObject->canSee ? "On\n" : "Off\n");
}
void toggleGravity(Button& b) 
{
	if (!b.gameObject) return;
	b.gameObject->gravity = !b.gameObject->gravity;
	std::cout << "Gravity is now "
		<< (b.gameObject->gravity ? "On\n" : "Off\n");
}

// Adds a ground asset to the game
void buttonAddGround(Button& button) 
{
	// Define actions for buttons
	groundCount++;
	GameObject ground;
	ground.isSolid = true;
	ground.canSee = true;
	ground.gravity = false;
	ground.textureIndex = 48;
	platforms.emplace_back(ground);

	button.addToList(leftPanelButtons, 0, leftPanelButtons.back().y - 0.7, 14, 0.5,
		100, leftPanelButtons.back().windowY + 30, 200, 20,
		hierarchyButton, "Ground " + to_string(groundCount), &platforms.back());

	printf("Completed action add ground \n");
}

void buttonAddCollectable(Button& button) 
{
	collectableCount++;
	GameObject collectible;
	collectible.colorG = 0;
	collectible.textureIndex = 18;   // Coffee Sprite
	collectible.canSee = true;
	collectible.isSolid = true;
	collectible.gravity = false;
	collectibles.emplace_back(collectible);


	button.addToList(leftPanelButtons, 0, leftPanelButtons.back().y - 0.7, 14, 0.5,
		100, leftPanelButtons.back().windowY + 30, 200, 20,
		hierarchyButton, "Collectable " + to_string(collectableCount), &collectibles.back());

	printf("Added Collectable\n");
}

void buttonAddHazard(Button& button) 
{
	hazardCount++;
	GameObject hazard;
	hazard.colorR = 1;
	hazard.colorG = 0;
	hazard.colorB = 0;
	hazard.textureIndex = 39;
	hazard.canSee = true;
	hazard.isSolid = true;
	hazard.gravity = false;
	hazards.emplace_back(hazard);


	button.addToList(leftPanelButtons, 0, leftPanelButtons.back().y - 0.7, 14, 0.5,
		100, leftPanelButtons.back().windowY + 30, 200, 20,
		hierarchyButton, "Hazard " + to_string(hazardCount), &hazards.back());

	printf("Added Hazard\n");
}

void buttonAddExit(Button& button) 
{
	exitCount++;
	GameObject exit;
	exit.colorR = 0;
	exit.colorG = 0;
	exit.colorB = 1;
	exit.textureIndex = 50;
	exit.canSee = true;
	exit.isSolid = true;
	exit.gravity = false;
	exits.emplace_back(exit);

	button.addToList(leftPanelButtons, 0, leftPanelButtons.back().y - 0.7, 14, 0.5,
		100, leftPanelButtons.back().windowY + 30, 200, 20,
		hierarchyButton, "Exit " + to_string(exitCount), &exits.back());

	printf("Completed action add Exit\n");
}

void buttonAddPlayer(Button& button)
{
	if (playerActive) {
		std::cout << "Player already present\n";
		return;
	}
	playerCount++;
	Player = GameObject();         
	Player.sizeX = 1.0f;
	Player.sizeY = 1.0f;
	Player.colorR = 1.0f;
	Player.colorG = 1.0f;
	Player.colorB = 1.0f;
	Player.canSee = true;
	Player.isSolid = true;
	Player.gravity = true;;
	Player.textureIndex = displayFrame;       

	if (playerCount > 1)
	{
		printf("Maxmimum Player Limit Reached\n");
		return; // Prevent adding multiple players
	}

	button.addToList(leftPanelButtons, 0, leftPanelButtons.back().y - 0.7, 14, 0.5,
		100, leftPanelButtons.back().windowY + 30, 200, 20,
		hierarchyButton, "Player ", &Player);

	printf("Completed action add Player\n");

	playerActive = true;
}


void worldModifier(int loop) {
	static int lastLoop = -1;
	if (loop != lastLoop) 
	{
		// loop just changed
		switch (loop) 
		{
		case 1:  std::cout << "Hi\n";                 break;
		case 2:  playTrack1();                        break;
		case 3:  displaySceneStateDisplay = false;    break;
		case 4:  std::cout << "Please\n";                 break;
		case 5:  playTrack1();                        break;
		case 6:  displaySceneStateDisplay = false;    break;
		case 7:  std::cout << "Stop\n";                 break;
		case 8:  playTrack1();                        break;
		case 9:  displaySceneStateDisplay = false;    break;
		case 10:  std::cout << "Hi\n";                break;
		}
		lastLoop = loop;
	}
}



// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void init(void) 
{
	glClearColor((52.0f / 255.0f), (32.0f / 255.0f), (43.0f / 255.0f), 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-7.0, 7.0, -7.0, 7.0, -10.0, 10.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	resetJumpTimer = jumpTimer;
 

	// Left Panel Buttons (Since I removed the player button) 
	Button sceneButton(0.0f, 6.0f, 14.0f, 0.5f, 100, 92, 200, 20, nullptr, "My World", nullptr ); // Plan to make multiple scenes in next iteration
	leftPanelButtons.push_back(sceneButton);

	// Bottom buttons
	Button groundButton(-6, -1, 1, 8, 70, 763, 70, 114, buttonAddGround, "  Ground");
	groundButton.sprite = true;
	groundButton.textureNum = 48;	// Set Preview Sprite
	bottomPanelButtons.push_back(groundButton);

	Button collectableButton(-3, -1, 1, 8, 290, 763, 70, 114, buttonAddCollectable, "  Collectable");
	collectableButton.sprite = true;
	collectableButton.textureNum = 19;	// Set Preview Sprite
	bottomPanelButtons.push_back(collectableButton);

	Button hazardButton(0, -1, 1, 8, 490, 763, 70, 114, buttonAddHazard, "  Hazard");
	hazardButton.sprite = true;
	hazardButton.textureNum = 39;	// Set Preview Sprite
	bottomPanelButtons.push_back(hazardButton);

	// Exit button
	Button exitButton(3, -1, 1, 8, 710, 763, 70, 114, buttonAddExit, "  Exit");
	exitButton.sprite = true;
	exitButton.textureNum = 50;		// Set Preview Sprite
	bottomPanelButtons.push_back(exitButton);


	// Create buttons and add them to the list
	Button playerCreatedButton(6, -1, 1, 8, 920, 763, 70, 114, buttonAddPlayer, "  Player");
	playerCreatedButton.sprite = true;
	playerCreatedButton.textureNum = 9;		// Set Preview Sprite
	bottomPanelButtons.push_back(playerCreatedButton);


	// Right Panel Buttons
	rightPanelButtons.emplace_back(3.5f, 5.6f, 1.0f, 0.5f, 955, 105, 80, 30, toggleCollision, "", nullptr);
	rightPanelButtons.emplace_back(3.5f, 4.8f, 1.0f, 0.5f, 955, 140, 80, 30, toggleVisible, "", nullptr);
	rightPanelButtons.emplace_back(3.5f, 4.0f, 1.0f, 0.5f, 955, 175, 80, 30, toggleGravity, "", nullptr);
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

	glRasterPos2f(Player.x + 3.82f, Player.y + 9.5f);
	string coinText = "Espresso: " + to_string(coffeeCollected) + "/" + to_string(totalcoffee);
	for (char c : coinText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	glPopMatrix();
}

void coffeeAnimationTimer(int value)
{
	coffeeAnimFrame = (coffeeAnimFrame + 1) % 4;
	glutPostRedisplay();									  // Redraw the scene with the new coffee frame
	glutTimerFunc(coffeeAnimDelay, coffeeAnimationTimer, 0);  // Schedule next frame update
}

void editorCameraLogic()
{
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
	GameObject* selected = nullptr;  // Future Note: Make a list of objects, allow transformation and multi selection 
	if (!rightPanelButtons.empty())
		selected = rightPanelButtons.front().gameObject;	// Default Origin

	float originX, originY;

	if (selected) 
	{
		// Center of Current Object
		originX = selected->x - 0.5f + selected->sizeX * 0.5f;
		originY = selected->y - 0.5f + selected->sizeY * 0.5f;
	}
	else if (currentScene == GameScene) 
	{
		originX = cameraX;
		originY = cameraY - 2.0f;
	}
	else 
	{
		originX = editorCameraX;
		originY = editorCameraY - 2.0f;
	}

	glPushMatrix();
	glTranslatef(originX, originY, 0.0f);

	// X-axis (red)
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(1, 0);
	glEnd();

	// Y-axis (green)
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(0, 1);
	glEnd();

	glPopMatrix();
}

void drawGrid()
{
	glPushMatrix();

	float camX = (currentScene == EditScene) ? editorCameraX : cameraX;
	float camY = (currentScene == EditScene) ? editorCameraY : cameraY;

	// cell size
	const float G = 1.0f;               
	float offX = fmod(camX, G);
	float offY = fmod(camY, G);


	glTranslatef(camX - offX + 0.5f,
		camY - offY + 0.5f, 0.0f);

	glColor3f(0.3f, 0.3f, 0.3f);
	const int ext = 50;
	glBegin(GL_LINES);
	for (int i = -ext; i <= ext; ++i) 
	{
		glVertex3f(i * G, -ext * G, 0);
		glVertex3f(i * G, ext * G, 0);
		glVertex3f(-ext * G, i * G, 0);
		glVertex3f(ext * G, i * G, 0);
	}
	glEnd();

	glPopMatrix();
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CreatePlayer(bool show) 
{
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

	// collision x-axis
	bool collisionX = one.x + one.sizeX >= two.x && two.x + two.sizeX >= one.x;

	// collision y-axis
	bool collisionY = one.y + one.sizeY >= two.y && two.y + two.sizeY >= one.y;

	// collision only if on both axes
	return collisionX && collisionY;
}

// Saving and Loading Scenes

void saveGameObjects(const std::string& slot, const std::string& baseName, const std::list<GameObject>& objects)
{
	// Create Slot
	std::string path = "save/" + slot + "/" + baseName + ".txt";
	std::ofstream out(path, std::ios::out | std::ios::trunc);
	if (!out) {
		std::cerr << "Failed opening " << path << "\n";
		return;
	}
	out << objects.size() << "\n";
	for (auto& o : objects) {
		out
			<< o.x << ' ' << o.y << ' ' << o.z << ' '
			<< o.sizeX << ' ' << o.sizeY << ' '
			<< o.colorR << ' ' << o.colorG << ' ' << o.colorB << ' '
			<< o.mass << ' '
			<< o.canSee << ' ' << o.isSolid << ' '
			<< o.destroyed << ' ' << o.gravity << ' '
			<< o.textureIndex
			<< "\n";
	}
}

void loadGameObjects(const std::string& slot,
	const std::string& baseName,
	std::list<GameObject>& objects)
{
	std::string path = "save/" + slot + "/" + baseName + ".txt";
	std::ifstream in(path);
	if (!in) {
		std::cerr << "Failed opening " << path << "\n";
		return;
	}
	objects.clear();
	size_t count;
	in >> count;
	for (size_t i = 0; i < count; ++i) {
		GameObject o;
		in
			>> o.x >> o.y >> o.z
			>> o.sizeX >> o.sizeY
			>> o.colorR >> o.colorG >> o.colorB
			>> o.mass
			>> o.canSee >> o.isSolid
			>> o.destroyed >> o.gravity
			>> o.textureIndex;
		objects.push_back(o);
	}
}

void SaveSlot(int n) {
	std::string slot = "Slot" + std::to_string(n);
	saveGameObjects(slot, "platforms", platforms);
	saveGameObjects(slot, "collectibles", collectibles);
	saveGameObjects(slot, "hazards", hazards);
	saveGameObjects(slot, "exits", exits);
	if (playerActive) {
		std::list<GameObject> tmp{ Player };
		saveGameObjects(slot, "player", tmp);
	}
	std::cout << "Saved to " << slot << "\n";
}

void LoadSlot(int n) 
{
	std::string slot = "Slot" + std::to_string(n);
	loadGameObjects(slot, "platforms", platforms);
	loadGameObjects(slot, "collectibles", collectibles);
	loadGameObjects(slot, "hazards", hazards);
	loadGameObjects(slot, "exits", exits);

	// 2) Clear out the old hierarchy buttons
	//    (but maybe keep your “Scene 1” button at index 0)
	Button sceneBtn = leftPanelButtons.front();
	leftPanelButtons.clear();
	leftPanelButtons.push_back(sceneBtn);

	// 3) Re-create one button per platform
	int idx = 1;
	for (auto& plat : platforms) {
		Button::addToList(
			leftPanelButtons,
			/*x*/0, /*y*/ leftPanelButtons.back().y - 0.7f,
			/*w*/14, /*h*/0.5f,
			/*winX*/100, /*winY*/ leftPanelButtons.back().windowY + 30,
			/*winW*/200, /*winH*/20,
			hierarchyButton,
			"Ground " + to_string(idx++),
			&plat
		);
	}

	// 4) Re-create one button per collectible
	idx = 1;
	for (auto& c : collectibles) {
		Button::addToList(
			leftPanelButtons,
			0, leftPanelButtons.back().y - 0.7f, 14, 0.5f,
			100, leftPanelButtons.back().windowY + 30, 200, 20,
			hierarchyButton,
			"Collectable " + to_string(idx++),
			&c
		);
	}

	// 5) Hazards
	idx = 1;
	for (auto& h : hazards) {
		Button::addToList(
			leftPanelButtons,
			0, leftPanelButtons.back().y - 0.7f, 14, 0.5f,
			100, leftPanelButtons.back().windowY + 30, 200, 20,
			hierarchyButton,
			"Hazard " + to_string(idx++),
			&h
		);
	}

	// 6) Exits
	idx = 1;
	for (auto& e : exits) {
		Button::addToList(
			leftPanelButtons,
			0, leftPanelButtons.back().y - 0.7f, 14, 0.5f,
			100, leftPanelButtons.back().windowY + 30, 200, 20,
			hierarchyButton,
			"Exit " + to_string(idx++),
			&e
		);
	}

	// 7) Finally, restore the player
	std::list<GameObject> tmp;
	loadGameObjects(slot, "player", tmp);
	if (!tmp.empty()) {
		Player = tmp.front();
		playerActive = true;

		// Add the one Player button (only if not already there)
		bool hasPlayer =
			std::find_if(
				leftPanelButtons.begin(),
				leftPanelButtons.end(),
				[](const Button& b) { return b.text == "Player"; }
			) != leftPanelButtons.end();
	
		if (!hasPlayer) {
			Button::addToList(
				leftPanelButtons,
				0, leftPanelButtons.back().y - 0.7f, 14, 0.5f,
				100, leftPanelButtons.back().windowY + 30, 200, 20,
				hierarchyButton,
				"Player",
				&Player
			);
		}
	}

	groundCount = platforms.size();
	collectableCount = collectibles.size();
	hazardCount = hazards.size();
	exitCount = exits.size();
	playerCount = playerActive ? 1 : 0;

	std::cout << "Scene loaded and hierarchy rebuilt.\n";
}


// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



// Updated to match the new list for all the ground platforms
void gravityCheck()
{
	if (currentScene != GameScene) return;
	onGround = false;

	// Bottom Check – detect standing on ground
	if (Player.isSolid)
	{
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

	}
	// If not on ground: apply gravity
	if (!onGround && Player.gravity)
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

void detectAllCollisions() {
// World Interactions Future Note: Make list unresponsive after any additional inputs on other panels or keyboard. 
	// Platforms on Platforms
	collideLists(platforms, platforms, [](GameObject& A, GameObject& B) 
		{
			if (!A.gravity) return;
			float Abot = A.y - 0.5f;
			float Btop = B.y + B.sizeY - 0.5f;
			if (Abot <= Btop) 
			{
				A.y = B.y + B.sizeY;
			}
		});

	// Hazards on Platforms
	collideLists(hazards, platforms, [](GameObject& A, GameObject& B) 
		{
			if (!A.gravity) return;
			float Abot = A.y - 0.5f;
			float Btop = B.y + B.sizeY - 0.5f;
			if (Abot <= Btop) 
			{
				A.y = B.y + B.sizeY;
			}
		});

	// Collectibles on Platforms
	collideLists(collectibles, platforms, [](GameObject& A, GameObject& B) 
		{
			if (!CheckCollision(A, B)) return;
			A.y = B.y + B.sizeY;
		});

	// Exits on Platforms
	collideLists(exits, platforms, [](GameObject& A, GameObject& B) 
		{
			if (!A.gravity) return;
			float Abot = A.y - 0.5f;
			float Btop = B.y + B.sizeY - 0.5f;
			if (Abot <= Btop) 
			{
				A.y = B.y + B.sizeY;
				A.gravity = false;
			}
		});


	// Player Interactions
	// Currently, the player exists at all times. I just toggled the interactions. 
		if (playerActive == true)
		{
			// Collectibles
			for (auto& c : collectibles) 
			{
				if (!c.destroyed && CheckCollision(Player, c)) 
				{
					c.destroyed = true;
					coffeeCollected++;
					
					if (coffeeCollected == 11)
					{
						coffeeCollected = 0;
						loop++;
						LoadSlot(loop);
					}
					break;
				}
			}

			// Hazards
			for (auto& h : hazards) {
				if (CheckCollision(Player, h)) 
				{
					currentScene = LoseScene;
					playLoseMusic();
				}
			}
			// Exit
			if (coffeeCollected == totalcoffee) 
			{
				for (auto& e : exits) {
					if (CheckCollision(Player, e)) 
					{
						if (loop == totalcoffee)
						{
							currentScene = WinScene;
							playWinMusic();
						}
					}
				}
			}
		}


	std::list<GameObject*> all;
	if (playerActive) all.push_back(&Player);

	for (auto& p : platforms)    all.push_back(&p);
	for (auto& c : collectibles) all.push_back(&c);
	for (auto& h : hazards)      all.push_back(&h);
	for (auto& e : exits)        all.push_back(&e);

	for (auto it = all.begin(); it != all.end(); ++it) 
	{
		GameObject* A = *it;
		if (A->destroyed || !A->isSolid) continue;
		auto jt = it; ++jt;
		for (; jt != all.end(); ++jt) {
			GameObject* B = *jt;
			if (B->destroyed || !B->isSolid) continue;
			if (CheckCollision(*A, *B))
				handleCollision(A, B);
		}
	}
}

// Object Settings
void applyGravityToObjects() 
{
	// any object with gravity==true will fall
	for (auto& platform : platforms) {
		if (platform.gravity) platform.y -= gravity;
	}
	for (auto& collectible : collectibles) {
		if (collectible.gravity && !collectible.destroyed) collectible.y -= gravity;
	}
	for (auto& hazard : hazards) {
		if (hazard.gravity) hazard.y -= gravity;
	}
	for (auto& exit : exits) {
		if (exit.gravity) exit.y -= gravity;
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

	if (showBG) 
	{
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

void drawCheckbox(float cx, float cy, bool checked) 
{
	glPushMatrix();
	drawSquare(1.2f, 0.4f, 0, cx, cy, 0,
		42.0f / 255.0f, 30.0f / 255.0f, 40.0f / 255.0f);

	if (checked) 
	{
		drawSquare(0.8f, 0.2f, 0, cx * 1.5f, cy * 2.0f, 1.0f,
			246.0f / 255.0f, 231.0f / 255.0f, 254.0f / 255.0f);
	}
	glPopMatrix();
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

void drawTopPanelButtons() 
{
	glPushMatrix();
	 
	glPopMatrix();
}

void topbarLogic()
{ 
	glPushMatrix();
	drawTextWithBG("File", 0.0f, 3.9f, 14.0f, 0.4f, false);
	if (showFileMenu)
	{
		drawSquare(1.85f, 10.0f, 1, -3.30f, -0.2f, 0.0f,  /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);
		// Menu Entries
		drawTextWithBG("Save", 1.2f, -2.0f, 14.0f, 0.4f, false);
		drawTextWithBG("Load", 0.6f, -2.0f, 14.0f, 0.4f, false);
		drawTextWithBG("Quit", 0.0f, -2.0f, 14.0f, 0.4f, false);


		if (showSaveSubmenu)
		{
			drawSquare(10.0f, 10.0f, 1.0f, -0.3f, -0.2f, 0.0f,  /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);
			drawTextWithBG("Save File:", 0.0f, -2.0f, 14.0f, 0.4f, false);

			for (int i = 1; i <= 10; ++i)
			{
				float x = 1.0f + (i - 1) * 0.8f;
				drawTextWithBG("Slot: " + to_string(i), x, -2.0f, 14.0f, 0.4f, false);
			}
		}

		if (showLoadSubmenu)
		{
			drawSquare(10.0f, 10.0f, 1.0f, -0.3f, -0.2f, 0.0f,  /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);
			drawTextWithBG("Load File:", 0.0f, -2.0f, 14.0f, 0.4f, false);

			for (int i = 1; i <= 10; ++i)
			{
				float x = 1.0f + (i - 1) * 0.8f;
				drawTextWithBG("Slot: " + to_string(i), x, -2.0f, 14.0f, 0.4f, false);
			}
		}
	}
	glPopMatrix();


	glPushMatrix();
	drawTextWithBG("Help", 0.5f, 3.9f, 14.0f, 0.4f, false);
		if (showHelpMenu) 
		{
			drawSquare(2.85f, 10.0f, 1, -1.775f, -0.2f, 0.0f,  /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);
			drawTextWithBG("Controls", 0.5f, -2.0f, 14.0f, 0.4f, false);
			drawTextWithBG("About", 1.5f, -2.0f, 14.0f, 0.4f, false);
			drawTextWithBG("Contact", 2.5f, -2.0f, 14.0f, 0.4f, false);
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

	if (axis) drawAxis();

	// Make the ground
	for (auto& ground : platforms) 
	{
		ground.DrawGameObject(true);
	}

	for (auto& c : collectibles) 
	{
		c.DrawGameObject(true);
	}

	// Draw all hazards:
	for (auto& h : hazards) 
	{
		h.DrawGameObject(true);
	}

	// Draw all exits:
	for (auto& e : exits) 
	{
		e.DrawGameObject(true);
	}

	// Draw all players:
	if (playerActive == true) 	CreatePlayer(showCollision);

		applyGravityToObjects();
		detectAllCollisions();
		gravityCheck();
		CreateMechanics();
	//Movement
	if (lt)
		Player.x -= speed;
	if (rt)
		Player.x += speed;

	worldModifier(loop);

	glPopMatrix();
}

void MainMenu()
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);

	// Title
	glRasterPos2f(-2.0f, 1.0f);
	const char* title = "Caffeine Crusader";
	for (int i = 0; title[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, title[i]);
	}

	// Instructions
	glRasterPos2f(-0.6f, -1.0f);
	const char* menuMsg = "Press G to Start";
	for (int i = 0; menuMsg[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menuMsg[i]);
	}
	glRasterPos2f(-1.8f, -1.4f);
	const char* menuDescripiton = "Collect all 10 Espresso Shots and return before the clock hits 0";
	for (int i = 0; menuDescripiton[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menuDescripiton[i]);
	}
	glRasterPos2f(-1.8f, -1.2f);
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

	glRasterPos2f(-1.6f, 1.0f);
	const char* pauseTitle = "Game Paused";
	for (int i = 0; pauseTitle[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, pauseTitle[i]);
	}

	glRasterPos2f(-0.8f, -0.0f);
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

	glRasterPos2f(-1.3f, 1.0f);
	const char* message = "Game Over";
	for (int i = 0; message[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
	}
	glRasterPos2f(-1.0f, -0.0f);
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

	glRasterPos2f(-1.2f, 1);
	const char* message = "YOU WIN!";
	for (int i = 0; message[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
	}
	glRasterPos2f(-0.9f, 0.5f);
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

	editorCameraLogic();

	// Debug Assets
	if (axis) drawAxis();
	if (gridMode) drawGrid();


	// Draw all platforms:
	for (auto& g : platforms)    g.DrawGameObject(true);

	// Draw all collectibles:
	for (auto& c : collectibles) c.DrawGameObject(true);

	// Draw all hazards:
	for (auto& h : hazards)      h.DrawGameObject(true);

	// Draw all exits:
	for (auto& e : exits)        e.DrawGameObject(true);

	// Draw all players:
	if (playerActive == true) 	CreatePlayer(showCollision);

	glPopMatrix();
}

// UI Parts

void drawLeftPanel() 
{
	glPushMatrix();
	// Bonus Scrolling
	glTranslatef(0.0f, hierarchyScrollY, 0.0f);
	glViewport(0, BOTTOM_PANEL_H, SIDE_PANEL_W, MIDDLE_PANEL_H);

	float totalButtons = float(leftPanelButtons.size());

	// Draw Background
	drawSquare(15, 14.0f + (totalButtons * 2), 1, 0, 0, 0,  /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);

	// Draw the buttons
	for (auto& button : leftPanelButtons) 
	{
		button.draw();
	}

	// Draw text box to label panel
	drawTextWithBG("                  Hierarchy", 0, 6.6f - hierarchyScrollY, 14, 0.8, true);

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

void drawRightPanel() 
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W + MAIN_PANEL_W, BOTTOM_PANEL_H, SIDE_PANEL_W, MIDDLE_PANEL_H);

	// Background
	drawSquare(15, 14, 1, 0, 0, 0, /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);

	// Draw text box to label panel
	drawTextWithBG("                 Inscpector", 0, 6.6, 14, 0.8, true);

	// X coordinate label
	drawTextWithBG("X:", -4, 5.6, 6, 0.5, true);
	textInputX.draw();

	// Y coordinate label
	drawTextWithBG("Y:", -4, 4.8f, 6, 0.5f, true);
	textInputY.draw();

	// Z coordinate label
	drawTextWithBG("Z:", -4, 4.0f, 6, 0.5f, true);
	textInputZ.draw();

	// X Scale label
	drawTextWithBG("W:", -4, 3.2f, 6, 0.5f, true);
	textInputXScale.draw();

	// Y Scale label
	drawTextWithBG("H:", -4, 2.4f, 6, 0.5f, true);
	textInputYScale.draw();

	// Texture label
	drawTextWithBG("Sprite ID:", -4, 1.6f, 6, 0.5f, true);
	textInputTexture.draw();
	

	// Draw Right Panel Buttons
	auto it = rightPanelButtons.begin();

	// Collision label
	drawTextWithBG("Collision:", 3.0f, 5.6, 6, 0.5, true);
	drawCheckbox(4.0f, 14.0f, it->gameObject && it->gameObject->isSolid);


	// Visibility label
	drawTextWithBG("Visible:", 3.0f, 4.8, 6, 0.5, true);
	drawCheckbox(4.0f, 12.0f, it->gameObject && it->gameObject->canSee);

	// Gravity label
	drawTextWithBG("Gravity:", 3.0f, 4.0, 6, 0.5, true);
	drawCheckbox(4.0f, 10.0f, it->gameObject && it->gameObject->gravity);

	// Draw the buttons
	glPopMatrix();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void drawTopPanel() 
{
	glPushMatrix();
	glViewport(0, WIN_H - TOP_PANEL_H, WIN_W, TOP_PANEL_H);

	// Background 
	drawSquare(20, 20, 1, 0, 0, 0, /* Red */ 42.0f / 255.0f, /* Green */ 30.0f / 255.0f, /* Blue */ 40.0f / 255.0f);
	glPopMatrix();

	// Functions
	drawTopPanelButtons();
	topbarLogic();

	drawTopPanelMessages();
}

void drawBottomPanel() 
{
	glPushMatrix();
	glViewport(0, 0, WIN_W, BOTTOM_PANEL_H);
	drawSquare(14, 15, 1, 0, 0, 0, /* Red */ 42.0f / 255.0f, /* Green */ 30.0f / 255.0f, /* Blue */ 40.0f / 255.0f);

	drawTextWithBG("                  Assets",0, 5.7, 14, 2.6, true);

	// Draw the buttons
	for (auto& button : bottomPanelButtons) {
		button.drawAssetButton();
	}

	glPopMatrix();
}

void MyDisplay() 
{
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// Render in 3D and then 2D for the panels
	drawMainPanel();
	glDisable(GL_DEPTH_TEST);
	drawLeftPanel();
	drawRightPanel();
	drawTopPanel();
	drawBottomPanel();
	glutSwapBuffers();
}

// -----------------------------------------------------------------------------------------


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
	
	else if (currentScene == EditScene) {
		// pan the editor camera instead of moving the player
		switch (key) {
		case GLUT_KEY_LEFT:
			editorCameraX -= 0.2f;
			break;
		case GLUT_KEY_RIGHT:
			editorCameraX += 0.2f;
			break;
		case GLUT_KEY_UP:
			editorCameraY += 0.2f;
			break;
		case GLUT_KEY_DOWN:
			editorCameraY -= 0.2f;
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
	textInputXScale.handleKeyPress(key);
	textInputYScale.handleKeyPress(key);
	textInputTexture.handleKeyPress(key);

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
			LoadSlot(1);
			currentScene = GameScene;
			//playGameMusic();
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
		{
			currentScene = EditScene;
			SoundEngine->stopAllSounds();
		}
			
		else if (currentScene == EditScene)
		{
			currentScene = MenuScene;
			//playMenuMusic();
		}
		break;

	case 'z': // Toggle grid in edit mode
		if (currentScene == EditScene)
			gridMode = !gridMode;

		break;
	}

	glutPostRedisplay();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

		cout << "\nClicked in " << clickedPanel << " at window coordinates (" << x << ", " << y << ")" << endl;

		// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if (clickedPanel == "Left Panel") {

			const float WORLD_HEIGHT = 14.0f;
			const int   VIEWPORT_PIX_H = MIDDLE_PANEL_H;      // 600
			float       pixelsPerUnit = VIEWPORT_PIX_H / WORLD_HEIGHT;
			int         scrollPixels = int(hierarchyScrollY * pixelsPerUnit);


			int adjustedY = y + scrollPixels;

			for (auto& button : leftPanelButtons) {
				if (button.isInside(x, adjustedY)) {
					button.handleClick();
					break;
				}
			}
		}

		if (clickedPanel == "Left Panel") 	scrolling = true;
		else scrolling = false;

		// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if (clickedPanel == "Bottom Panel")
		{
			for (auto& button : bottomPanelButtons) {
				if (button.isInside(x, y)) {
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}
		}

		// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if (clickedPanel == "Top Panel") {
			// File Menu Open
			if (showFileMenu) 
			{

				// Save area
				if (x >= 95 && x < 150 && y >= 25 && y <= 70 && !showSaveSubmenu && !showLoadSubmenu)
				{
					std::cout << "Displaying Submenu Save Slots.\n";
					showSaveSubmenu = true;
					showLoadSubmenu = false;
					return;
				}

				// If I have the time to fix this to be accurate and automated, I will change it.
				// Save Submenu
				if (showSaveSubmenu && !showLoadSubmenu)
				{
					for (int i = 1; i <= 10; ++i)
					{
						int startX = 75 + (i - 1) * 60;
						int endX = startX + 35;
						if (x >= startX && x < endX && y >= 25 && y <= 70) {
							SaveSlot(i);
							std::cout << "Clicked: Slot " << i << "\n";
							break;
						}
					}
					showSaveSubmenu = false;
					return;
				}


				// Load area
				if (x >= 50 && x < 80 && y >= 25 && y <= 70 && !showSaveSubmenu && !showLoadSubmenu)
				{
					std::cout << "Displaying Submenu Load Slots.\n";
					showLoadSubmenu = true;
					showSaveSubmenu = false;
					return;
				}

				// Load Submenu
				if (showLoadSubmenu && !showSaveSubmenu)
				{
					for (int i = 1; i <= 10; ++i)
					{
						int startX = 75 + (i - 1) * 60;
						int endX = startX + 35;
						if (x >= startX && x < endX && y >= 25 && y <= 70) {
							LoadSlot(i);
							std::cout << "Clicked: Slot " << i << "\n";
							break;
						}
					}
					showLoadSubmenu = false;
					return;
				}

				// Quit area
				if (x >= 5.0f && x <= 30.0f && y >= 25.0f && y <= 70.0f && !showSaveSubmenu && !showLoadSubmenu)
				{
					std::cout << "Quit selected, exiting.\n";
					exit(0);
				}

				showFileMenu = false;
				return;
			}

			// Help Menu Open
			if (showHelpMenu) {
				// Controls area
				if (x >= 40 && x <= 85 && y >= 25 && y <= 70)
				{
					std::cout << "\nControls:\n Arrow Keys: Move the Player & Editor Camera\n Space Bar: Jump\n 'a': Toggle axis\n 'z': Toggle Grid\n 'g': Start Game\n 'p': Pause Game\n 's': Debug Collision Boxs\n ESC: Exit\n Left Mouse Button: Interact With Panel Elements\n Scroll Wheel: Scroll Through Hierarchy\n";
					return;
				}
				// About area
				if(x >= 108 && x <= 140 && y >= 25 && y <= 70)
				{
					std::cout << "\nAbout:\n This is an editor which allows the user to place: The Player, Platforms, Hazards, Collectibles, and an Exit\n Collect all Espresso and reach the exit, before the timer reaches 0\n ";
					return;
				}
				// Contact area
				if(x >= 185 && x <= 230 && y >= 25 && y <= 70)
				{
					std::cout << "\nSpawning virtual consultant :)\nAvatar: Giant Spider\nHeading to #### #### Kent, OH\n\nSee you soon, User ID: mbektic!\n";
					return;
				}
				showHelpMenu = false;
				return;
			}

			// Toggle menus
			if (x >= 5 && x <= 30) {
				showFileMenu = true;
				showHelpMenu = false;
			}
			else if (x >= 40 && x <= 70) {
				showHelpMenu = true;
				showFileMenu = false;
			}
			else {
				showFileMenu = false;
				showHelpMenu = false;
			}
			return;
		}
		// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if (clickedPanel == "Right Panel")
		{
			for (auto& btn : rightPanelButtons) {
				if (btn.isInside(x, y)) {
					btn.handleClick();
					return;
				}
			}
			if (textInputX.isInside(x, y))
			{
				textInputX.setActive(true);
				textInputY.setActive(false);
				textInputZ.setActive(false);
				textInputXScale.setActive(false);
				textInputYScale.setActive(false);
				textInputTexture.setActive(false);
			}
			else if (textInputY.isInside(x, y))
			{
				textInputY.setActive(true);
				textInputX.setActive(false);
				textInputZ.setActive(false);
				textInputXScale.setActive(false);
				textInputYScale.setActive(false);
				textInputTexture.setActive(false);
			}
			else if (textInputZ.isInside(x, y))
			{
				textInputX.setActive(false);
				textInputY.setActive(false);
				textInputZ.setActive(true);
				textInputXScale.setActive(false);
				textInputYScale.setActive(false);
				textInputTexture.setActive(false);
			}
			else if (textInputXScale.isInside(x, y))
			{
				textInputX.setActive(false);
				textInputY.setActive(false);
				textInputZ.setActive(false);
				textInputXScale.setActive(true);
				textInputYScale.setActive(false);
				textInputTexture.setActive(false);
			}
			else if (textInputYScale.isInside(x, y))
			{
				textInputX.setActive(false);
				textInputY.setActive(false);
				textInputZ.setActive(false);
				textInputXScale.setActive(false);
				textInputYScale.setActive(true);
				textInputTexture.setActive(false);
			}
			else if (textInputTexture.isInside(x, y))
			{
				textInputX.setActive(false);
				textInputY.setActive(false);
				textInputZ.setActive(false);
				textInputXScale.setActive(false);
				textInputYScale.setActive(false);
				textInputTexture.setActive(true);
			}
			else
			{
				textInputX.setActive(false);
				textInputY.setActive(false);
				textInputZ.setActive(false);
				textInputXScale.setActive(false);
				textInputYScale.setActive(false);
				textInputTexture.setActive(false);
			}
		}
	}
	glutPostRedisplay();
}

// Bonus: I did not make the music system :(
void MouseWheel(int wheel, int direction, int x, int y) {
	if (!scrolling) return;
	hierarchyScrollY -= direction * 0.7f;

	if (hierarchyScrollY < 0.0f)
		hierarchyScrollY = 0.0f;

	glutPostRedisplay();
}


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

	// RGB mode with double buffering and Z axis
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

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
	glutMouseWheelFunc(MouseWheel);


	// Terminal Msgs
	std::cout << std::endl;
	std::cout << "Credits:\n";
	std::cout << "The Legend of Zelda: Majora's Mask\n";
	std::cout << "Bakudas: Generic Dungeon Pack\n";
	std::cout << "Wulax:  Medieval fantasy character sprites\n";
	std::cout << "\nBonus Feature: Hierarchy Scrolling\n";

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