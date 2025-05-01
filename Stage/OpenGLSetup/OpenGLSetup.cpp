#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <FreeImage/FreeImage.h>
#include <stdio.h>
#include <math.h>
#include <array>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>
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

int frame = 0, groundNum = 2;

float speed = 0.15, gravity = 0.2;

float jumpTimer = 10, resetJumpTimer;
float jumpAcceleration = 1.0f; // Adjust this value for jump height
float jumpVelocity = 0.5f; // Adjust this value for initial jump velocity

bool showCollision = true;

bool gravityOn = false; // Toggle the gravity. I'm just doing it like this for the example but you should do it per object (Hint: maybe in the class)

bool lt, rt, jump, contact, onGround;

bool dropDown; // Toggle the drop down menu
bool fileDropDown, helpDropDown;
bool buttonClicked = false; // Check if drop down button was clicked so button behind doesn't click

int textInputType;

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
	bool	  collision; // Use this to toggle collision on/off for gameObjects.
	bool      gravity; // use the y variable and apply the gravity scale value we have to it.


	// Default constructor
	GameObject()
		: x(0), y(0), z(0),
		sizeX(1), sizeY(1),
		colorR(1), colorG(1), colorB(1),
		mass(0), canSee(true), isSolid(false),
		destroyed(false), collision(true), gravity(false) {}

	void DrawGameObject(bool sprite, int textureNum) {
		glPushMatrix();
		glTranslatef(x, y, z);

		if (canSee && !destroyed)
		{
			if (sprite)
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

				glEnable(GL_TEXTURE_2D); // Enable texturing

				glBindTexture(GL_TEXTURE_2D, texID[textureNum]); // Which texture

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

	void DrawPlayer(bool sprite) {
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
};

// Gameobjects on the screen
GameObject player, bottomCheck, leftCheck, rightCheck, topCheck, collectible;

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
		text(txt), active(false), gameObject(nullptr) {}

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
			if (gameObject != nullptr) {
				// Update the x value whenever a valid character is entered
				switch (textInputType)
				{
				case 0:
					gameObject->x = textToFloat();
					break;
				case 1:
					gameObject->y = textToFloat();
					break;
				case 2:
					gameObject->z = textToFloat();
					break;
				case 3:
					gameObject->sizeX = textToFloat();
					break;
				case 4:
					gameObject->sizeY = textToFloat();
					break;
				}
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
	bool toggled; // Indicates whether the button has been clicked


	// x, y, w, h, windowX, windowY, windowWidth, windowHeight, text, buttonAction, gameObject pointer
	// Updated to allow for the passing of a gameObject reference but is not necessary
	Button(float posX, float posY, float w, float h,
		float winX, float winY, float winW, float winH,
		function<void(Button&)> act, const string& txt, GameObject* obj = nullptr, int texture = 0, bool pic = true, bool tog = false)
		: x(posX), y(posY), width(w), height(h),
		windowX(winX), windowY(winY), windowWidth(winW), windowHeight(winH),
		text(txt), buttonAction(act), clicked(false), gameObject(obj), textureNum(texture), sprite(pic), toggled(tog) {}

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
		toggled = !toggled;
		if (buttonAction) {
			buttonAction(*this); // Invoke the action handler
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

	void drawToggleButton()
	{
		// Calculate the position to center the button
		float centerX = x - width / 2;
		float centerY = y - height / 2;

		// Draw the button background using the calculated center position
		glColor3f(0.6f, 0.6f, 0.6f); // Light gray color for the button
		glBegin(GL_QUADS);
		glVertex3f(centerX, centerY, 0);
		glVertex3f(centerX + width, centerY, 0);
		glVertex3f(centerX + width, centerY + height, 0);
		glVertex3f(centerX, centerY + height, 0);
		glEnd();

		// Define separate width and height for the smaller square
		float smallSquareWidth = width * 0.6f;
		float smallSquareHeight = height * 0.6f;

		// Calculate the position to center the smaller square
		float smallSquareX = centerX + (width - smallSquareWidth) / 2;
		float smallSquareY = centerY + (height - smallSquareHeight) / 2;

		// Draw the button background using the calculated center position
		if (toggled) {
			// Draw a smaller filled square inside the button
			glColor3f(0.0f, 1.0f, 0.0f); // Green color for the small square
			glBegin(GL_QUADS);
			glVertex3f(smallSquareX, smallSquareY, 0);
			glVertex3f(smallSquareX + smallSquareWidth, smallSquareY, 0);
			glVertex3f(smallSquareX + smallSquareWidth, smallSquareY + smallSquareHeight, 0);
			glVertex3f(smallSquareX, smallSquareY + smallSquareHeight, 0);
			glEnd();
		}
		else {
			// Draw a smaller filled square inside the button
			glColor3f(1.0f, 1.0f, 1.0f); // White color for the small square
			glBegin(GL_QUADS);
			glVertex3f(smallSquareX, smallSquareY, 0);
			glVertex3f(smallSquareX + smallSquareWidth, smallSquareY, 0);
			glVertex3f(smallSquareX + smallSquareWidth, smallSquareY + smallSquareHeight, 0);
			glVertex3f(smallSquareX, smallSquareY + smallSquareHeight, 0);
			glEnd();
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
list<Button> leftPanelButtons, topPanelButtons, bottomPanelButtons, fileDropDownButtons, helpDropDownButtons, rightPanelButtons;

list<GameObject> platforms;
list<GameObject> collectables;
list<TextInput> textInputs;
list<GameObject> flags;


// Text input variable
TextInput textInputX(-5.8, 5.4, 4.5, 0.4, 850, 109, 66, 14, "");
TextInput textInputY(-5.8, 4.9, 4.5, 0.4, 850, textInputX.windowY + 17, 66, 14, "");
TextInput textInputZ(-5.8, 4.4, 4.5, 0.4, 850, textInputY.windowY + 17, 66, 14, "");
TextInput textInputScaleX(-3.5, 3.9, 4, 0.4, 879, 173, 60, 16, "");
TextInput textInputScaleY(-3.5, 3.4, 4, 0.4, 879, textInputScaleX.windowY + 22, 60, 16, "");

void collisionToggleAction(Button&);
void gravityToggleAction(Button&);

Button toggleButton1(0, 3.1, 1.5, 0.5, 900, 214, 20, 20, collisionToggleAction, "");
Button toggleButton2(0, 2.6, 1.5, 0.5, 900, toggleButton1.windowY + 15, 20, 20, gravityToggleAction, "");

static void addToList(list<Button>& buttonList, float x, float y, float w, float h,
	float winX, float winY, float winW, float winH,
	function<void(Button&)> action, const string& text, GameObject* obj) {

	// Create a new button instance and add it to the list
	buttonList.emplace_back(x, y, w, h, winX, winY, winW, winH, action, text, obj);
}

// Give buttons functions by making functions and 
// assiging them when the button is made.
void hierarchyButton(Button& button) {
	if (button.gameObject != nullptr) {
		// Update textInput with gameObject's x-coordinate
		textInputX.text = to_string(button.gameObject->x);
		textInputY.text = to_string(button.gameObject->y);
		textInputZ.text = to_string(button.gameObject->z);
		textInputScaleX.text = to_string(button.gameObject->sizeX);
		textInputScaleY.text = to_string(button.gameObject->sizeY);


		textInputX.gameObject = button.gameObject;
		textInputY.gameObject = button.gameObject;
		textInputZ.gameObject = button.gameObject;
		textInputScaleX.gameObject = button.gameObject;
		textInputScaleY.gameObject = button.gameObject;

		toggleButton1.gameObject = button.gameObject;
		toggleButton2.gameObject = button.gameObject;

		toggleButton1.toggled = button.gameObject->collision;
		toggleButton2.toggled = button.gameObject->gravity;
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

void fileButtonAction(Button& button) {
	// Define actions for buttons
	dropDown = true;
	fileDropDown = true;
	helpDropDown = false;
	printf("Completed file button action \n");
}

void helpButtonAction(Button& button) {
	// Define actions for buttons
	dropDown = true;
	fileDropDown = false;
	helpDropDown = true;
	printf("Completed help button action \n");
}

void exitButton(Button& button) {
	// Define actions for buttons
	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	exit(0);
	printf("Completed exiting action \n");
}

// Save game objects that you've added to the main window in a list you've created.
void saveGameObjects(list<GameObject>& gameObjects, const string& filename) {
	string fullName = "save/" + filename + ".txt"; // File path relative to current directory
	ofstream outputFile(fullName, ios::out | ios::trunc); // Open file in write mode, truncate if exists
	if (!outputFile.is_open()) {
		cout << "Error: Unable to open file for writing: " << fullName << endl;
		return;
	}

	// Write the number of game objects
	int numGameObjects = gameObjects.size(); // Assuming platforms store all game objects
	outputFile << numGameObjects << endl;

	// Write game object data
	for (const auto& gameObject : gameObjects) {
		// Write each game object's data to the file
		outputFile << gameObject.x << " " << gameObject.y << " " << gameObject.z << " "
			<< gameObject.sizeX << " " << gameObject.sizeY << " "
			<< gameObject.colorR << " " << gameObject.colorG << " " << gameObject.colorB << " "
			<< gameObject.mass << " " << gameObject.canSee << " " << gameObject.isSolid << " "
			<< gameObject.destroyed << " " << gameObject.collision << " " << gameObject.gravity << endl;
	}

	outputFile.close();
}

// Load game objects to the main window to a list you've created.
void loadGameObjects(list<GameObject>& gameObjects, const string& filename) {
	string fullName = "save/" + filename + ".txt"; // File path relative to current directory
	ifstream inputFile(fullName);

	if (!inputFile.is_open()) {
		cerr << "Failed to open file for reading: " << fullName << endl;
		return;
	}

	// Clear the existing game objects list
	gameObjects.clear();

	int numGameObjects;
	inputFile >> numGameObjects;

	// Load each game object from the file
	for (int i = 0; i < numGameObjects; ++i) {
		GameObject gameObject;
		inputFile >> gameObject.x >> gameObject.y >> gameObject.z
			>> gameObject.sizeX >> gameObject.sizeY
			>> gameObject.colorR >> gameObject.colorG >> gameObject.colorB
			>> gameObject.mass >> gameObject.canSee
			>> gameObject.isSolid >> gameObject.destroyed
			>> gameObject.collision >> gameObject.gravity;
		gameObjects.emplace_back(gameObject);

		addToList(leftPanelButtons, 0, leftPanelButtons.back().y - 0.7, 14, 0.5,
			100, leftPanelButtons.back().windowY + 30, 200, 20,
			hierarchyButton, "Ground " + to_string(leftPanelButtons.size()), &gameObjects.back());
	}

	inputFile.close();
	cout << "Game objects loaded from file: " << fullName << endl;
}

void saveButton(Button& button) {
	// Define actions for buttons
	saveGameObjects(platforms, "GroundAssets");

	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	printf("Completed save action \n");
}

void loadButton(Button& button) {
	// Define actions for buttons
	loadGameObjects(platforms, "GroundAssets");

	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	printf("Completed load action \n");
}

void tempButton(Button& button) {
	// Define actions for buttons
	dropDown = false;
	fileDropDown = false;
	helpDropDown = false;
	printf("Completed action 5 \n");
}

void collisionToggleAction(Button& button) {
	// Define actions for buttons
	if (toggleButton1.gameObject != nullptr) {
		toggleButton1.toggled = !toggleButton1.toggled;
		toggleButton1.gameObject->collision = toggleButton1.toggled;
		printf("Toggle collision action \n");
	}
	else
		printf("Collision Toggle Failed \n");
}

void gravityToggleAction(Button& button) {
	// Define actions for buttons
	if (toggleButton2.gameObject != nullptr) {
		toggleButton2.toggled = !toggleButton2.toggled;
		toggleButton2.gameObject->gravity = toggleButton2.toggled;
		printf("Toggle gravity action \n");
	}
	else
		printf("Gravity Toggle Failed \n");
}

void init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the window screen and change the background color
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-7.0, 7.0, -7.0, 7.0, -10.0, 10.0);

	resetJumpTimer = jumpTimer;

	player.y = 1;

	bottomCheck.colorR = 0;
	leftCheck.colorR = 0;
	rightCheck.colorR = 0;
	topCheck.colorR = 0;

	SoundEngine->play2D("audio/The Return of Caped Crusader Cat.mp3", true);

	// Create buttons and add them to the list
	Button button1(0, 6, 14, 0.5, 100, 92, 200, 20, hierarchyButton, "Player", &player);
	// Add buttons to list
	leftPanelButtons.push_back(button1);

	Button button3(-6.5, 0, 1, 20, 33, 25, 66, 50, fileButtonAction, "    File");
	topPanelButtons.push_back(button3);

	Button button4(-5.5, 0, 1, 20, 107, 25, 66, 50, helpButtonAction, "    Help");
	topPanelButtons.push_back(button4);

	// Save Button
	Button fileButton1(-5.6, 5.95, 2, 0.5, 100, 64, 142, 26, saveButton, "             Save");
	fileDropDownButtons.push_back(fileButton1);

	// Load Button
	Button fileButton2(-5.6, fileButton1.y - 0.5, 2, 0.5, fileButton1.windowX, fileButton1.windowY + 30, 142, 26, loadButton, "             Load");
	fileDropDownButtons.push_back(fileButton2);

	Button fileButton3(-5.6, fileButton2.y - 0.5, 2, 0.5, fileButton2.windowX, fileButton2.windowY + 30, 142, 26, exitButton, "             Exit");
	fileDropDownButtons.push_back(fileButton3);

	Button helpButton1(-4.6, 5.95, 2, 0.5, 173, 64, 142, 26, tempButton, "           About");
	helpDropDownButtons.push_back(helpButton1);

	Button helpButton2(-4.6, helpButton1.y - 0.5, 2, 0.5, helpButton1.windowX, helpButton1.windowY + 30, 142, 26, tempButton, "           Controls");
	helpDropDownButtons.push_back(helpButton2);

	// Bottom buttons
	Button bottomButton(-6, -1, 1, 8, 70, 763, 70, 114, buttonAddGround, "  Ground");
	// Makes sprite false on ground button
	bottomButton.sprite = false;
	bottomPanelButtons.push_back(bottomButton);

	rightPanelButtons.push_back(toggleButton1);
	rightPanelButtons.push_back(toggleButton2);
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

	bottomCheck.DrawGameObject(false, 0);
	leftCheck.DrawGameObject(false, 0);
	rightCheck.DrawGameObject(false, 0);
	topCheck.DrawGameObject(false, 0);
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
void gravityCheck() {
	onGround = false;

	for (auto& ground : platforms) {
		if (CheckCollision(bottomCheck, ground)) {
			onGround = true;
			break;
		}
	}

	for (auto& ground : platforms) {
		if (CheckCollision(leftCheck, ground)) {
			player.x += speed;
			break;
		}
	}

	for (auto& ground : platforms) {
		if (CheckCollision(rightCheck, ground)) {
			player.x -= speed;
			break;
		}
	}

	for (auto& ground : platforms) {
		if (CheckCollision(topCheck, ground) && jump) {
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

void drawTextWithBG(string text, float x, float y, float width, float height, bool showBG) {
	// Calculate the position to center the text with background
	float centerX = x - width / 2;
	float centerY = y - height / 2;

	// Draw the text background using the calculated center position
	if (showBG) { // Turn background on or off
		glColor3f(0.75f, 0.75f, 0.75f); // Light gray color for the background
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
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for the text
	glRasterPos3f(textX, textY, 1);
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
	}
}

void drawLeftPanel() {

	glPushMatrix();
	glViewport(0, BOTTOM_PANEL_H, SIDE_PANEL_W, MIDDLE_PANEL_H);

	// Dark Grey background
	drawSquare(15, 14, 1, 0, 0, 0, 0.5, 0.5, 0.5);

	// Draw text box to label panel
	drawTextWithBG("                 Hierarchy",
		0, 6.6, 14, 0.8, true);

	// Draw the buttons
	for (auto& button : leftPanelButtons) {
		button.draw();
	}

	glPopMatrix();
}

void drawMainPanel() {
	glPushMatrix();
	glViewport(SIDE_PANEL_W, BOTTOM_PANEL_H, MAIN_PANEL_W, MIDDLE_PANEL_H);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Makes the player
	CreatePlayer(showCollision);

	// Make the ground
	for (auto& ground : platforms) {
		ground.DrawGameObject(false, 0);
	}

	// Makes the collectible
	collectible.DrawGameObject(false, 0);

	// Removes the collectible
	if (CheckCollision(player, collectible))
		collectible.destroyed = true;

	if (gravityOn)
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
	glViewport(SIDE_PANEL_W + MAIN_PANEL_W, BOTTOM_PANEL_H, SIDE_PANEL_W, MIDDLE_PANEL_H);

	// Dark Grey background
	drawSquare(15, 14, 1, 0, 0, 0, 0.7, 0.7, 0.7);

	// Draw text box to label panel
	drawTextWithBG("                 Inspector",
		0, 6.6, 14, 0.8, true);

	// Draw text box for x coordinate label
	drawTextWithBG("X:",
		-4, 5.6, 6, 0.5, true);

	// Draw text box for y coordinate label
	drawTextWithBG("Y:",
		-4, 5.1, 6, 0.5, true);

	// Draw text box for z coordinate label
	drawTextWithBG("Z:",
		-4, 4.6, 6, 0.5, true);

	// Draw text box for x scale label
	drawTextWithBG("Scale X:",
		-3, 4.1, 8, 0.5, true);

	// Draw text box for y scale label
	drawTextWithBG("Scale Y:",
		-3, 3.6, 8, 0.5, true);

	// Draw text box for show player colliders toggle
	drawTextWithBG("Collison:",
		-4, 3.1, 6, 0.5, true);

	// Draw text box for show player gravity toggle
	drawTextWithBG("Gravity:",
		-4, 2.6, 6, 0.5, true);

	// Draw the text box
	textInputX.draw();
	textInputY.draw();
	textInputZ.draw();
	textInputScaleX.draw();
	textInputScaleY.draw();

	toggleButton1.drawToggleButton();
	toggleButton2.drawToggleButton();

	glPopMatrix();
}

void drawTopPanel() {
	glPushMatrix();
	glViewport(0, WIN_H - TOP_PANEL_H, WIN_W, TOP_PANEL_H);

	//glScalef(1, 20, 1);

	// Blue background 
	drawSquare(20, 20, 1, 0, 0, 0, 0.5, 0.5, 0.5);

	// Draw the buttons
	for (auto& button : topPanelButtons) {
		button.draw();
	}

	glPopMatrix();
}

void drawBottomPanel() {
	glPushMatrix();
	glViewport(0, 0, WIN_W, BOTTOM_PANEL_H);

	//glScalef(1, 5, 1);

	// Dark Grey background
	drawSquare(14, 15, 1, 0, 0, 0, 0.5, 0.5, 0.5);

	drawTextWithBG("                  Assets",
		0, 5.7, 14, 2.6, true);

	// Draw the buttons
	for (auto& button : bottomPanelButtons) {
		button.drawAssetButton();
	}

	glPopMatrix();
}

// Draws a viewport over the entire window that we can draw our menu on
void drawDropDownViewport() {
	glPushMatrix();
	glViewport(0, 0, WIN_W, WIN_H);

	if (dropDown)
	{
		// Draw the buttons for file drop down menu
		if (fileDropDown)
			for (auto& button : fileDropDownButtons) {
				button.draw();
			}

		// Draw the buttons for help drop down menu
		if (helpDropDown)
			for (auto& button : helpDropDownButtons) {
				button.draw();
			}
	}

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
	drawDropDownViewport();

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
	// Handle key press events for input boxes
	textInputX.handleKeyPress(key);
	textInputY.handleKeyPress(key);
	textInputZ.handleKeyPress(key);
	textInputScaleX.handleKeyPress(key);
	textInputScaleY.handleKeyPress(key);

	switch (key)
	{
	case 's': // Show ground check
		showCollision = !showCollision;
		break;
	case 'g': // Toggle gravity
		gravityOn = !gravityOn;
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

		// Checks if drop down menu is up
		if (dropDown)
		{
			buttonClicked = false;

			// Check if any button was clicked
			for (auto& button : fileDropDownButtons) {
				if (button.isInside(x, y) && fileDropDown) {
					buttonClicked = true;
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}

			for (auto& button : helpDropDownButtons) {
				if (button.isInside(x, y) && helpDropDown) {
					buttonClicked = true;
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}

			// If no button was clicked, then enables other editor buttons
			if (!buttonClicked) {
				dropDown = false;
				fileDropDown = false;
				helpDropDown = false;
			}
		}

		// Doesn't allow other buttons to be clicked if drop down is up.
		if (!dropDown && !buttonClicked)
		{
			// Check if any button was clicked
			for (auto& button : leftPanelButtons) {
				if (button.isInside(x, y)) {
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}

			for (auto& button : topPanelButtons) {
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

			for (auto& button : rightPanelButtons) {
				if (button.isInside(x, y)) {
					button.handleClick(); // Trigger the action associated with the button
					break; // Exit the loop after handling the click for one button
				}
			}
		}

		// Allows for none drop down buttons to be clickable again
		if (buttonClicked)
			buttonClicked = false;
	}

	// Check if clicked in or out of text box to set as active or not.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (textInputX.isInside(x, y)) {
			textInputX.setActive(true);
			textInputType = 0;
		}

		else {
			textInputX.setActive(false);
		}

		if (textInputY.isInside(x, y)) {
			textInputY.setActive(true);
			textInputType = 1;
		}

		else {
			textInputY.setActive(false);
		}

		if (textInputZ.isInside(x, y)) {
			textInputZ.setActive(true);
			textInputType = 2;
		}

		else {
			textInputZ.setActive(false);
		}

		if (textInputScaleX.isInside(x, y)) {
			textInputScaleX.setActive(true);
			textInputType = 3;
		}

		else {
			textInputScaleX.setActive(false);
		}

		if (textInputScaleY.isInside(x, y)) {
			textInputScaleY.setActive(true);
			textInputType = 4;
		}

		else {
			textInputScaleY.setActive(false);
		}

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
	glutCreateWindow("Save Load Example");

	glutTimerFunc(0, timer, 0);

	init();

	loadTextures();

	glutDisplayFunc(MyDisplay); // call the drawing function

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(specialKeyboard);
	glutSpecialUpFunc(specialKeyboardRelease);
	glutMouseFunc(MouseControl);

	glutMainLoop();
	return 0;
}