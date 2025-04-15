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

class GameObject;

#define WindowX 0
#define WindowY 0
#define WIN_W 1200
#define WIN_H 675

#define SIDE_PANEL_W 250       
#define TOP_PANEL_H   30
#define MIDDLE_PANEL_H 600
#define BOTTOM_PANEL_H 175        
#define MAIN_PANEL_W (WIN_W - 2*SIDE_PANEL_W)
#define MAIN_PANEL_H (WIN_H - TOP_PANEL_H - BOTTOM_PANEL_H)
//-------------------------------------------------------------------------------------------------------------

// Global Editor Variables


GameObject* selectedObject = nullptr;
bool gridMode = true;
bool dragging = false;
bool displaySceneStateDisplay = true;

float editorCameraX = 0.0f;
float editorCameraY = 0.0f;

//


// Global Viewport Variables

// Frame & Object Counter
int displayFrame = 0, groundNum = 0, coffeeNum = 0;

// Camera Positions
float cameraX = 0.0f, cameraY = 0.0f;
float offsetX = 0.0f, offsetY = 3.0f;

// Movement Values
float speed = 0.15f;					// Horizontal Movement Speed

// Vertical Movement
float gravity = 0.2f;					// Doward Movement Speed
float jumpTimer = 5, resetJumpTimer;	// Jump Reset
float jumpAcceleration = 0.15f;			// Jump Height
float jumpVelocity = 0.8f;				// Upward Movement Speed

// Collision Flags
bool showCollision = false;
bool lt, rt, jump, contact, onGround;

// Toggles
bool axis;

// Mechanic Values
int totalcoffee = 10;					// Collectibles (Total)
int coffeeCollected = 0;				// Collectibles (Gathered)
static int timeLeft = 60;

// Game States
// Resource: https://stackoverflow.com/questions/49966159/using-enum-for-gamestates-in-python | https://medium.com/@jordantkay/understanding-the-concept-of-an-enum-state-machine-b2aa4f49fc87
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

// Player States for animation
enum PlayerState
{
	IdleLeft,
	RunLeft,
	IdleRight,
	RunRight,
	JumpLeft,
	JumpRight
};

// Player Animation
PlayerState currentState = IdleRight;	// Start Looking Right
int playerAnimFrame = 0;                // Cycle Frames
bool facingLeft = false;                // Check Left Idle from Right Idle


// Coffee Animation
int coffeeAnimFrame = 0;      // Similar Index to Cyccle Frames
int coffeeAnimDelay = 200;    // Frame Cycle Speed

// Creates sound engine
ISoundEngine* SoundEngine = createIrrKlangDevice();

// Texture IDs
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

// Sound System: BGM
//SoundEngine->play2D("audio/Game.mp3", true); Platform Example
char* backgroundMusic[4] =
{
	(char*)"audio/Menu.mp3",
	(char*)"audio/Game.mp3",
	(char*)"audio/Lose.mp3",
	(char*)"audio/Win.mp3"
};

// Sound System: SFX
char* sfxMusic[1] =
{
	(char*)"audio/Jump.ogg"
};

//-------------------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------------------
// Global Objects

// Player and its Collision Boxes
GameObject player, bottomCheck, leftCheck, rightCheck, topCheck;

// Total Tiles, Collectibles, Hazards, Exits
std::list<GameObject> platforms;
std::list<GameObject> collectibles;
std::list<GameObject> hazards;
std::list<GameObject> exits;





// Create Ground (Visable/Invisable)
GameObject CreateGround(float x, float y, float width, float height, bool collider = true, bool visable = true, int texIndex = -1)
{
	GameObject ground;
	ground.x = x;						// X Axis Origin
	ground.y = y;						// Y Axis Origin
	ground.sizeX = width;				// Width
	ground.sizeY = height;				// Height
	ground.isSolid = collider;			// Collision
	ground.canSee = visable;			// Visibility
	ground.textureIndex = texIndex;		// Store Texture Index
	return ground;
}

// Create Collectible
GameObject CreateCollectible(float x, float y, bool collider = true)
{
	GameObject Collectible;
	Collectible.x = x;
	Collectible.y = y;
	Collectible.sizeX = 0.3f;
	Collectible.sizeY = 0.4f;
	Collectible.isSolid = collider;
	Collectible.canSee = true;
	Collectible.destroyed = false;
	Collectible.textureIndex = 18;		// Fixed animation
	return Collectible;
}

// Create Hazard
GameObject CreateHazard(float x, float y, float width, float height, bool collider, bool visable, int texIndex = -1)
{
	GameObject Hazard;
	Hazard.x = x;
	Hazard.y = y;
	Hazard.sizeX = width;
	Hazard.sizeY = height;
	Hazard.isSolid = collider;
	Hazard.canSee = visable;
	Hazard.destroyed = false;
	Hazard.textureIndex = -1;		// Fixed animation

	Hazard.colorR = 1.0f;
	Hazard.colorG = 0.0f;
	Hazard.colorB = 0.0f;

	return Hazard;
}

GameObject CreateExit(float x, float y, float width, float height, bool collider, bool visable, int texIndex = -1)
{
	GameObject Exit;
	Exit.x = x;
	Exit.y = y;
	Exit.sizeX = width;
	Exit.sizeY = height;
	Exit.isSolid = collider;
	Exit.canSee = visable;
	Exit.destroyed = false;
	Exit.textureIndex = -1;

	Exit.colorR = 0.0f;
	Exit.colorG = 1.0f;
	Exit.colorB = 0.0f;

	return Exit;
}

class TextInput {
public:
	float x, y; // Position of the text input box in the viewport
	float width, height; // Dimensions of the text input box in the viewport
	float windowX, windowY; // Position of the button in the window
	float windowWidth, windowHeight; // Dimensions of the button in the window
	string text; // Text to be displayed in the text input box
	GameObject* gameObject; // Used to get and set the variables of the game object
	bool active; // Indicates whether the text input box is active (selected for input)

	TextInput(float posX, float posY, float w, float h,
		float winX, float winY, float winW, float winH,
		const string& txt)
		: x(posX), y(posY), width(w), height(h),
		windowX(winX), windowY(winY), windowWidth(winW), windowHeight(winH),
		text(txt), active(false), gameObject(nullptr) {
	}

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
				// Update the x value whenever a valid character is entered
				gameObject->x = textToFloat();
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
		glColor3f(0.9f, 0.9f, 0.9f); // Light gray color for the text input box
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
		glEnd();

		// Draw the text inside the text input box
		glColor3f(0.0f, 0.0f, 0.0f); // Black color for the text
		glRasterPos2f(x + 0.05f, y + (height / 2) - 0.05f); // Adjust position for centering
		for (char c : text) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
		}
	}
};



//------------------------------------------------------------------------------------------------------------
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
		glColor3f(0.9f, 0.9f, 0.9f); // Light gray color for the button
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
		glColor3f(0.0f, 0.0f, 0.0f); // Black color for the text
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
		glColor3f(0.9f, 0.9f, 0.9f); // Light gray color for the button
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
		glColor3f(0.0f, 0.0f, 0.0f); // Black color for the text
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
list<Button> leftPanelButtons;
list<Button> rightPanelButtons;
list<Button> bottomPanelButtons;
list<Button> topPanelButtons;



TextInput textInput(-5.8, 5.4, 4.5, 0.4, 850, 109, 66, 14, "");

// Give buttons functions by making functions and 
// assiging them when the button is made.
void hierarchyButton(Button& button) {
	if (button.gameObject != nullptr) {
		// Update textInput with gameObject's x-coordinate
		textInput.text = to_string(button.gameObject->x);
		textInput.gameObject = button.gameObject;
	}
	else {
		cout << "Button's gameObject is null" << endl;
	}

	cout << "Completed button action" << endl;
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

void buttonDrawGrid()
{

}

void buttonAddPlatform(Button& button)
{
	platforms.emplace_back(CreateGround(0.0f, 0.0f, 1.0f, 1.0f, true, 54));
	printf("Platform added\n");
}

void buttonAddCollectible(Button& button)
{
	collectibles.emplace_back(CreateCollectible(0.0f, 0.0f));
	printf("Collectible added\n");
}

void buttonAddHazard(Button& button)
{
	hazards.emplace_back(CreateHazard(0.0f, 0.0f, 1.0f, 1.0f, true, false));
	printf("Hazard added\n");
}

void buttonAddExit(Button& button)
{
	exits.emplace_back(CreateExit(0.0f, 0.0f, 1.0f, 1.0f, true, false));
	printf("Exit added\n");
}



// Create Button Function (Automatic Panel Adjustment)
void addButtonToPanel(std::list<Button>& panel,
	float posX, float posY,					// Button position (logical coordinates)
	float width, float height,				// Button size (logical dimensions)
	float windowX, float windowY,			// Button position in window coordinates (for hit testing)
	float windowWidth, float windowHeight,	// Button window dimensions
	std::function<void()> buttonAction,		// Action when the button is clicked
	const std::string& text,				// Button text to display
	bool toggleButton = false)				// Is this a toggle button? (default false)
{
	//Button newButton(posX, posY, width, height, windowX, windowY, windowWidth, windowHeight, buttonAction, text, toggleButton);
	//panel.push_back(newButton);
}

void populateHierarchyButtons()
{
	leftPanelButtons.clear();
	const float buttonWidth = 14.0f;    // Button Width: (Equal to Panel Width)
	const float buttonHeight = 0.5f;    // Button Height
	const float spacing = 0.1f;         // Vertical Spacing: (Between Buttons)
	float currentY = 9.0f;              // Starting Point: Y
	const float centerX = 0.0f;         // Starting Point: X

	// Create a button for the player.
	addButtonToPanel(leftPanelButtons,
		centerX, currentY,           // Logical position.
		buttonWidth, buttonHeight,   // Button size.
		100.0f, 92.0f,               // Window coordinates for hit test (adjust as needed).
		200.0f, 20.0f,               // Window dimensions for hit test.
		[]() { printf("Player selected\n"); },
		"Player");
	currentY -= (buttonHeight + spacing);

	// Create a button for each ground object.
	for (int i = 0; i < groundNum; ++i) {
		char label[50];
		sprintf_s(label, "Ground %d", i);
		addButtonToPanel(leftPanelButtons,
			centerX, currentY,
			buttonWidth, buttonHeight,
			100.0f, 92.0f,
			200.0f, 20.0f,
			[i]() { printf("Ground %d selected\n", i); },
			label);
		currentY -= (buttonHeight + spacing);
	}

	// Create a button for each collectible.
	for (int i = 0; i < coffeeNum; ++i) {
		char label[50];
		sprintf_s(label, "Collectible %d", i);
		addButtonToPanel(leftPanelButtons,
			centerX, currentY,
			buttonWidth, buttonHeight,
			100.0f, 92.0f,
			200.0f, 20.0f,
			[i]() { printf("Collectible %d selected\n", i); },
			label);
		currentY -= (buttonHeight + spacing);
	}

	// Create a button for the Hazard object.
	addButtonToPanel(leftPanelButtons,
		centerX, currentY,
		buttonWidth, buttonHeight,
		100.0f, 92.0f,
		200.0f, 20.0f,
		[]() { printf("Hazard selected\n"); },
		"Hazard");
	currentY -= (buttonHeight + spacing);

	// Create a button for the Map Exit.
	addButtonToPanel(leftPanelButtons,
		centerX, currentY,
		buttonWidth, buttonHeight,
		100.0f, 92.0f,
		200.0f, 20.0f,
		[]() { printf("Map Exit selected\n"); },
		"Map Exit");
	// You may continue to add more buttons if additional game objects exist.
}



// Give buttons functions by making functions and 
// assiging them when the button is made.
void buttonAction1() {
	// Define actions for buttons
	for (int i = 0; i < 3; ++i) {
		//ground[i].colorR = 0;
		//ground[i].colorG = 1;
		//ground[i].colorB = 0;
	}
	printf("Completed action 1 \n");
}

void buttonAction2() {
	// Define actions for buttons
	for (int i = 0; i < 3; ++i) {
		//ground[i].colorR = 1;
		//ground[i].colorG = 0;
		//ground[i].colorB = 1;
	}
	printf("Completed action 2 \n");
}

void toggleAction() {
	// Define actions for buttons
	showCollision = !showCollision;
	printf("Toggle action \n");
}

void spawnBlockAction() {
	// Create a new block using your helper (change parameters as needed).
//	ground[groundNum] = CreateGround(0.0f, 0.0f, 1.0f, 1.0f, true, 54);
//	groundNum++;
	// Refresh the hierarchy so that a new button appears.
	populateHierarchyButtons();
	printf("Spawned new block\n");
}




// Sound Library Functions
void playMenuMusic()
{
	SoundEngine->stopAllSounds();
	//SoundEngine->play2D(backgroundMusic[0], true);
}

void playGameMusic()
{
	SoundEngine->stopAllSounds();
	//SoundEngine->play2D(backgroundMusic[1], true);
}

void playLoseMusic()
{
	SoundEngine->stopAllSounds();
	//SoundEngine->play2D(backgroundMusic[2], false);
}

void playWinMusic()
{
	SoundEngine->stopAllSounds();
	//SoundEngine->play2D(backgroundMusic[3], false);
}

void playJumpSFX()
{
	//SoundEngine->play2D(sfxMusic[0], false);
}

// Implementing Collision Detection | Source: Platform Example
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

//-------------------------------------------------------------------------------------------------------------

// Environment Initialization
void init(void)
{
	// Clear The Window + Set Color (Match Tiles)
	glClearColor((52.0f / 255.0f), (32.0f / 255.0f), (43.0f / 255.0f), 1.0f);


	// 2D Projection Setup
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-10.0, 10.0, -10.0, 5.0, -10.0, 10.0);

	// Prepare jump timer
	//resetJumpTimer = jumpTimer;


	//playMenuMusic();

	// Give collision check boxes a color
	bottomCheck.colorR = 0;
	leftCheck.colorR = 0;
	rightCheck.colorR = 0;
	topCheck.colorR = 0;


	// Create buttons and add them to the list
	Button button1(0, 6, 14, 0.5, 100, 92, 200, 20, hierarchyButton, "Player", &player);
	// Add buttons to list
	leftPanelButtons.push_back(button1);


	// Create buttons and add them to the list
	Button button1(0, 6, 14, 0.5, 100, 92, 200, 20, hierarchyButton, "Player", &player);
	// Add buttons to list
	rightPanelButtons.push_back(button1);





	Button bottomButton(
		0,		// Position X
		0,		// Position Y
		1,		// Width
		8,	// Height
		100,	// WindowX
		92,		// WindowY
		200,	// Window Width
		20,		// Window Height

		buttonAddGround,	// Function
		"Ground"			// Title
	);

	bottomPanelButtons.push_back(bottomButton);
}

void CreateMechanics()
{
	// Simply display the timer and coin count without modifying the timer value
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(player.x - 9.3f, player.y + 8.0f);
	string timerText = "Time: " + std::to_string(timeLeft);
	for (char c : timerText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

	glRasterPos2f(player.x + 4.3f, player.y + 8.0f);
	string coinText = "Espresso: " + to_string(coffeeCollected) + "/" + to_string(totalcoffee);
	for (char c : coinText)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
}

void coffeeAnimationTimer(int value)
{
	coffeeAnimFrame = (coffeeAnimFrame + 1) % 4;
	glutPostRedisplay();  // Redraw the scene with the new coffee frame
	glutTimerFunc(coffeeAnimDelay, coffeeAnimationTimer, 0);  // Schedule next frame update
}

// Draw Player Character + Colliders
void CreatePlayer(bool show)
{
	glPushMatrix();
	player.DrawPlayer(true);
	bottomCheck.x = player.x + 0.3f;
	bottomCheck.y = player.y;
	bottomCheck.sizeX = 0.4f;
	bottomCheck.sizeY = 0.2f;

	bottomCheck.canSee = show;

	leftCheck.x = player.x + 0.25f;
	leftCheck.y = player.y + 0.3f;
	leftCheck.sizeX = 0.15f;
	leftCheck.sizeY = 0.4f;

	leftCheck.canSee = show;

	rightCheck.x = player.x + 0.60f;
	rightCheck.y = player.y + 0.3f;
	rightCheck.sizeX = 0.15f;
	rightCheck.sizeY = 0.4f;

	rightCheck.canSee = show;

	topCheck.x = player.x + 0.3f;
	topCheck.y = player.y + 0.75f;
	topCheck.sizeX = 0.4f;
	topCheck.sizeY = 0.15f;

	topCheck.canSee = show;

	bottomCheck.DrawGameObject(false);
	leftCheck.DrawGameObject(false);
	rightCheck.DrawGameObject(false);
	topCheck.DrawGameObject(false);
	glPopMatrix();
}

// Draw Axis
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

//-------------------------------------------------------------------------------------------------------------
// Laws

// Physics
void gravityCheck()
{
	if (currentScene != GameScene) return;
	onGround = false;

	// Bottom Check – detect standing on ground
	for (auto& ground : platforms)
	{
		if (ground.isSolid && CheckCollision(bottomCheck, ground))
		{
			onGround = true;
			break;
		}
	}

	// Left Check – push player right
	for (auto& ground : platforms)
	{
		if (ground.isSolid && CheckCollision(leftCheck, ground))
		{
			player.x += speed;
			break;
		}
	}

	// Right Check – push player left
	for (auto& ground : platforms)
	{
		if (ground.isSolid && CheckCollision(rightCheck, ground))
		{
			player.x -= speed;
			break;
		}
	}

	// Top Check – stop jump if hitting ceiling
	for (auto& ground : platforms)
	{
		if (ground.isSolid && CheckCollision(topCheck, ground) && jump)
		{
			jump = false;
			jumpTimer = resetJumpTimer;
			break;
		}
	}

	// If not on ground: apply gravity
	if (!onGround)
	{
		player.y -= gravity;
	}

	// Fall out of bounds
	if (player.y < -10.0)
	{
		currentScene = LoseScene;
		playLoseMusic();
	}
}

//-------------------------------------------------------------------------------------------------------------
// UI Shapes

// Source: Panel Example
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

//-------------------------------------------------------------------------------------------------------------

// Window Logic
void onReshape(int w, int h)
{
	glutPostRedisplay();
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
	cameraX = player.x + player.sizeX / 2 + offsetX;
	cameraY = player.y + player.sizeY / 2 + offsetY;

	// Camera Rules
		// Out of Bounds Camera: Extra
	if (player.y == -3.0f) cameraY = -3.0f;
}


// Draw Editor Panels

void drawTextWithBG(string text, float x, float y, float width, float height, bool showBG) {
	// Calculate the position to center the text with background
	float centerX = x - width / 2;
	float centerY = y - height / 2;

	// Draw the text background using the calculated center position
	if (showBG) { // Turn background on or off
		glColor3f(/* Red */  44.5f / 255.0f, /* Green */ 30.5f / 255.0f, /* Blue */ 41.5f / 255.0f);
		// You can add more to change the color
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

// Left Panel Functions
void drawLeftPanelBackground()
{
	glPushMatrix();
	glViewport(0, BOTTOM_PANEL_H, SIDE_PANEL_W, MAIN_PANEL_H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawSquare(4.0f, 9.5f, 1.0f, 0.0f, 0.5f, 0.0f, /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);
	glPopMatrix();
}

void drawLeftPanelHierarchyTitle()
{
	glPushMatrix();
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	glOrtho(-7.0, 7.0, 0.0, 9.5, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
	drawTextWithBG("                       Hierarchy", 0.0f, 9.3, 14.0f, 0.4f, true);

	for (auto& button : leftPanelButtons) {
		button.draw();
	}
	glPushMatrix();
}

// Left Panel | Source: Panel Example

void drawLeftPanel()
{
	drawLeftPanelBackground();
	drawLeftPanelHierarchyTitle();
	//drawLeftPanelAdaptiveHierarchy();
}

// Right Panel

void drawRightPanelBackground()
{
	glPushMatrix();
	glViewport(SIDE_PANEL_W + MAIN_PANEL_W, BOTTOM_PANEL_H, SIDE_PANEL_W, MAIN_PANEL_H);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
	drawSquare(4.0f, 9.5f, 1.0f, 0.0f, 0.0f, 0.0f, /* Red */ 46.0f / 255.0f, /* Green */ 32.0f / 255.0f, /* Blue */ 43.0f / 255.0f);
	glPopMatrix();
}

void drawRightPanelInspectorTitle()
{
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-7.0, 7.0, 0.0, 9.5, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
	drawTextWithBG("                         Inspector", 0.0f, 9.3, 14.0f, 0.4f, true);

	for (auto& button : rightPanelButtons) {
		button.draw();
	}
	glPushMatrix();
}

void drawRightPanel() {
	drawRightPanelBackground();
	drawRightPanelInspectorTitle();
}

// Top Panel

void drawTopPanelBackground()
{
	glPushMatrix();
	glViewport(0, WIN_H - TOP_PANEL_H, WIN_W, TOP_PANEL_H);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
	drawSquare(2.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, /* Red */ 42.0f / 255.0f, /* Green */ 30.0f / 255.0f, /* Blue */ 40.0f / 255.0f);
	glPopMatrix();

}


void drawTopPanelMessages()
{
	if (displaySceneStateDisplay)
	{
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, 7.0, 0.0, 9.5, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
		switch (currentScene)
		{
		case EditScene:
			drawTextWithBG("Scene: Edit", 13.5f, 3.9, 14.0f, 0.4f, false);
			break;

		case MenuScene:
			drawTextWithBG("Scene: Menu", 13.5f, 3.9, 14.0f, 0.4f, false);
			break;

		case GameScene:
			drawTextWithBG("Scene: Game", 13.5f, 3.9, 14.0f, 0.4f, false);
			break;

		case WinScene:
			drawTextWithBG("Scene: Win", 13.5f, 3.9, 14.0f, 0.4f, false);
			break;

		case LoseScene:
			drawTextWithBG("Scene: Lose", 13.5f, 3.9, 14.0f, 0.4f, false);
			break;
		}

		glPopMatrix();
	}
}

void drawTopPanelButtons()
{
	// Grid toggle button (top-right corner)
		//Button toggleGridButton(
		//	0.93f, 0.4f,     // x, y in ortho space (0.0 to 1.0)
		//	0.06f, 0.25f,    // width, height
		//	WIN_W - 60.0f, WIN_H - 40.0f,
		//	100.0f, 30.0f,
		//	[]() { gridMode = !gridMode; }, "Grid", true
		//);
		//toggleGridButton.toggled = gridMode;
		//toggleGridButton.draw();

		//Button togglePlayButton( 0.86f, 0.4f,	0.06f, 0.25f,WIN_W - 120.0f, WIN_H - 40.0f,	100.0f, 30.0f,[]() {
		//if (currentScene == EditScene)
	//				currentScene = GameScene;
	//		else
	//				currentScene = EditScene;
	//		}, "Play", false
	//	);
	//	togglePlayButton.draw();
}

void drawTopPanel() 
{
	drawTopPanelBackground();
	drawTopPanelMessages();

}



void drawBottomPanelBackground()
{
	glPushMatrix();
	glViewport(0, 0, WIN_W, BOTTOM_PANEL_H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawSquare(14, 3, 1, 0, 0, 0, /* Red */ 42.0f / 255.0f, /* Green */ 30.0f / 255.0f, /* Blue */ 40.0f / 255.0f);
}

void drawBottomPanelTitle()
{
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-7.0, 7.0, 5.0, 9.2, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
	drawTextWithBG("                         Assets", -0.04f, 8.8, 14.0f, 0.8f, true);

}

void drawBottomPanelButtons()
{
	// Draw all bottom panel buttons.
	for (auto& button : bottomPanelButtons) 
	{
		button.drawAssetButton();
	}
}

// Bottom Panel |  Source: Panel Example
void drawBottomPanel() 
{
	drawBottomPanelBackground();
	drawBottomPanelTitle();
	drawBottomPanelButtons();


}

// Draw Complete Editor
void drawEditor()
{
	drawLeftPanel();
	drawRightPanel();
	drawTopPanel();
	drawBottomPanel();
	// drawFill
}



// Viewport Scenes

void MainMenu()
{
	glColor3f(1.0, 1.0, 1.0);

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
}

void PauseGame()
{
	glColor3f(1.0, 1.0, 1.0);

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
}

void LoseGame()
{
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(0.8f, 1.5f);
	const char* message = "Game Over";
	for (int i = 0; message[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
	}
	glRasterPos2f(player.x - 0.6f, player.y + 1.0f);
	const char* gameOverMsg = "You were too eepy :(";
	for (int i = 0; gameOverMsg[i] != '\0'; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, gameOverMsg[i]);
	}
}

void WinGame()
{
	glColor3f(1.0f, 1.0f, 1.0f);
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
}


void EditGame()
{

}


void ActiveGame()
{
	// Player Movement
	if (lt)
		player.x -= speed;
	if (rt)
		player.x += speed;

	// Physics + Collision
	gravityCheck();

	// Check collision with hazards (dynamic list)
	for (auto& hz : hazards)
	{
		if (CheckCollision(player, hz))
		{
			currentScene = LoseScene;
			playLoseMusic();
			break;
		}
	}

	// Check win condition (touching exit & all coffee collected)
	for (auto& Ex : exits)
	{
		if(CheckCollision(player, Ex) && coffeeCollected >= totalcoffee)
		{
			currentScene = WinScene;
			playWinMusic();
		}
	}
}


void editorVisualized()
{
	// Background/Environment/Props
	for (auto& ground : platforms)
	{
		if (!ground.isSolid)
			ground.DrawGameObject(true);
	}

	// Special Props - Collectibles
	for (auto& coffee : collectibles)
	{
		if (!coffee.destroyed)
		{
			coffee.DrawGameObject(true);
			if (CheckCollision(player, coffee))
			{
				coffee.destroyed = true;
				++coffeeCollected;
			}
		}
	}

	// Hazards
	for (auto& hz : hazards)
	{
		hz.DrawGameObject(true);
	}

	// Exits
	for (auto& ex : exits)
	{
		ex.DrawGameObject(false); // Green Box
	}

	// Player
	CreatePlayer(showCollision);

	// Debug
	if (axis) drawAxis();
	if (gridMode) drawGrid();
}



void GlobalValues()
{
	// Game Camera
	if (currentScene == GameScene)
		gameCameraLogic();

	gluLookAt(cameraX, cameraY, 5, cameraX, cameraY, 0, 0, 1, 0);

	// ------------------------------------------------------------------------
	// Render World

	// Background / Platforms
	for (auto& platform : platforms)
	{
		if (!platform.isSolid)
			platform.DrawGameObject(true);
	}

	// Collectibles
	for (auto& coffee : collectibles)
	{
		if (!coffee.destroyed)
		{
			coffee.DrawGameObject(true);
			if (CheckCollision(player, coffee))
			{
				coffee.destroyed = true;
				++coffeeCollected;
			}
		}
	}

	// Hazards
	for (auto& hz : hazards)
	{
		hz.DrawGameObject(false);
		if (CheckCollision(player, hz))
		{
			currentScene = LoseScene;
			playLoseMusic();
		}
	}

	// Exit Tiles (Win)
	for (auto& ex : exits)
	{
		ex.DrawGameObject(false);
		if (CheckCollision(player, ex) && coffeeCollected >= totalcoffee)
		{
			currentScene = WinScene;
			playWinMusic();
		}
	}

	// Player
	CreatePlayer(showCollision);

	// UI
	CreateMechanics();

	// Debug
	if (axis) drawAxis();
}


void EditorScene()
{
	
	editorCameraLogic();

	editorVisualized();



}

// Viewport Panel
void drawViewport()
{
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MAIN_PANEL_H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 5, -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Scenes
	if (currentScene == MenuScene) MainMenu();
	else if (currentScene == PauseScene) PauseGame();
	else if (currentScene == LoseScene) LoseGame();
	else if (currentScene == WinScene) WinGame();
	else if (currentScene == GameScene) { ActiveGame(); GlobalValues(); }
	else if (currentScene == EditScene) EditorScene();
}

// Main Display
void MyDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawViewport();
	drawEditor();
	glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------
// Controls

void specialKeyboard(int key, int x, int y)
{
	if (currentScene == EditScene) {
		switch (key)
		{
		case GLUT_KEY_LEFT:
			editorCameraX -= 0.5f;
			break;
		case GLUT_KEY_RIGHT:
			editorCameraX += 0.5f;
			break;
		case GLUT_KEY_UP:
			editorCameraY += 0.5f;
			break;
		case GLUT_KEY_DOWN:
			editorCameraY -= 0.5f;
			break;
		}
	}
	else if (currentScene == GameScene) {
		switch (key)
		{
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
	}

	glutPostRedisplay(); // Redraw with the new camera position
}

void specialKeyboardRelease(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		lt = false;
		if (currentState == RunLeft)
		{
			currentState = IdleLeft;
			playerAnimFrame = 0;
		}
		break;
	case GLUT_KEY_RIGHT:
		rt = false;
		if (currentState == RunRight)
		{
			currentState = IdleRight;
			playerAnimFrame = 0;
		}
		break;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
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

	// Check if clicked in or out of text box to set as active or not.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (textInput.isInside(x, y)) {
			textInput.setActive(true);
		}

		else {
			textInput.setActive(false);
		}
	}

	glutPostRedisplay();
}


//-------------------------------------------------------------------------------------------------------------
// Loading Textures
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

//-------------------------------------------------------------------------------------------------------------


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
		displayFrame = playerAnimFrame;
		break;

	case IdleLeft:
		// single frame = 8
		displayFrame = 8;
		break;

	case RunRight:
		// frames [9..16] for running right
		displayFrame = 9 + playerAnimFrame;
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
		playerAnimFrame++;
		if (playerAnimFrame > 7) playerAnimFrame = 0;
	}
	// Running Right
	else if (currentState == RunRight)
	{
		playerAnimFrame++;
		if (playerAnimFrame > 7) playerAnimFrame = 0;
	}
	// Idle states don't cycle. Although it would have been fun to add.
}	// Honestly, I have found such a strong love for this. Hopefully, I am good at coding it. 

// Global Timer
void timer(int v)
{
	// Only update the game timer when in GameScene.
	if (currentScene != GameScene)
	{
		glutTimerFunc(100, timer, 0);
		return;
	}

	static int gameTimerAccumulator = 0;
	gameTimerAccumulator += 100;
	if (gameTimerAccumulator >= 1000)
	{
		timeLeft--;
		gameTimerAccumulator = 0;
		if (currentScene == GameScene && timeLeft <= 0)
		{
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
			player.y += jumpVelocity;
			jumpVelocity -= jumpAcceleration;
			jumpTimer--;
		}
		else
		{
			jump = false;
			jumpTimer = resetJumpTimer;
			jumpVelocity = 0.8f;
			currentState = (lt ? RunLeft : (rt ? RunRight : (facingLeft ? IdleLeft : IdleRight)));
		}
	}
	cycleRunFrames();
	updateAnimationFrame();
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------
// Main

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WIN_W, WIN_H);
	glutInitWindowPosition(WindowX, WindowY);
	glutCreateWindow("V Cavallaro | 811097945");
	glutReshapeFunc(onReshape);

	// Start the coffee animation timer
	glutTimerFunc(coffeeAnimDelay, coffeeAnimationTimer, 0);

	glutTimerFunc(0, timer, 0);
	init();
	loadTextures();

	glutDisplayFunc(MyDisplay);				// Drawing function
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardRelease);
	glutMouseFunc(MouseControl);

	// Print controls to terminal
	std::cout << std::endl;
	std::cout << "Controls:\n";
	std::cout << "Arrow Keys: Move the player\n";
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

//-------------------------------------------------------------------------------------------------------------
// Game Object Implementation

GameObject::GameObject()
{
	x = y = z = 0;
	sizeX = sizeY = 1;
	colorR = colorG = colorB = 1;
	mass = 0;
	canSee = true;
	isSolid = false;
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