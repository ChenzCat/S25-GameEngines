// Computer Graphics  | Project 1: Sprite Cube
// Vincenzo Cavallaro | Professor Meldin Bektic
// Feb 11, 2025

#include <GL/glut.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <IrrKlang/irrKlang.h>
#include <iostream>

using namespace irrklang;
using namespace std;

// Global toggles and states
bool solid = true;                  // Solid
bool wireframe = false;             // Wireframe
bool bonusMode = false;             // Simple Logo
bool extras = true;                 // Square and Triangle
bool colorSwap = false;             // Swap Triangle and Square Colors
bool displayOn = true;              // Toggle Entire Screen
bool axis = false;                  // Bicolored Axis

// Custom Colors (Extra Feature)
float r = 1.0, g = 1.0, b = 1.0;

// Global Movement for the cube
float squareX = 0.0f, squareY = 0.0f, speed = 1.0f; // Global for square movement.
float zoomFactor = 1.0f;

// Physics globals for gravity
float velocityY = 0.0f;             // Vertical velocity for gravity
const float gravity = -0.005f;      // Gravity acceleration (negative = downward)
const float cubeHalfHeight = 0.1f;    // Half-height of the cube (since width/height=10/100 = 0.1)
const float floorY = -0.9f;         // Y coordinate of the floor

// Creates sound engine
ISoundEngine* SoundEngine = createIrrKlangDevice();
const char* audioTracks[3] = { "audio/TheBottom.mp3", "audio/ThePrison.mp3", "audio/ClockwiseOperetta.mp3" };
int currentTrack = 0;
bool audio = false;

// Textures
int frame = 0;
int frameDelay = 100; // in milliseconds

GLuint texID[4];  // Number of Textures

char* textureFileNames[4] =
{
    (char*)"sprite/coffee0.png",
    (char*)"sprite/coffee1.png",
    (char*)"sprite/coffee2.png",
    (char*)"sprite/coffee3.png",
};

// Draw Axis
void drawAxis()
{
    glPushMatrix();
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

// Primitive Functions (Cube, Triangle)
void drawSquare(float width, float height, float renderType)
{
    glBegin(renderType);
    glVertex3f(-width / 100, -height / 100, 0);
    glVertex3f(width / 100, -height / 100, 0);
    glVertex3f(width / 100, height / 100, 0);
    glVertex3f(-width / 100, height / 100, 0);
    glEnd();
}

void drawTriangle(float width, float height, float renderType)
{
    glBegin(renderType);
    glVertex3f(-width / 100, -height / 100, 0);
    glVertex3f(width / 100, -height / 100, 0);
    glVertex3f(0, height / 100, 0);
    glEnd();
}

// BonusShapes
void drawLetterV(float width, float height, float renderType, float red, float green, float blue)
{
    glColor3f(red, green, blue);
    glBegin(renderType);
    // Left leg of the V
    glVertex3f(-width / 100, height / 100, 0);
    glVertex3f(0.0f, -height / 100, 0);
    // Right leg of the V
    glVertex3f(width / 100, height / 100, 0);
    glVertex3f(0.0f, -height / 100, 0);
    glEnd();
}

void drawWitchHat(float width, float height, float renderType, float red, float green, float blue)
{
    glColor3f(red, green, blue); // Cone (fancy triangle)
    glBegin(renderType);
    glVertex3f(-width / 100, -height / 100, -1);
    glVertex3f(width / 100, -height / 100, -1);
    glVertex3f(0, height / 100, -1);
    glEnd();

    glColor3f(red / 2, green / 2, blue / 2); // Brim
    glBegin(renderType);
    glVertex3f((-width + 1) / 100, -height / 100, 0);
    glVertex3f((width + 1) / 100, -height / 100, 0);
    glVertex3f(width / 50, -height / 80, 0);
    glVertex3f(-width / 50, -height / 80, 0);
    glEnd();
}

void drawLines(float width, float height, float renderType)
{
    for (int i = 0; i < 4; i++) {
        if (i == 0) glColor3f(0.0f, 0.5f, 1.0f);
        if (i == 1) glColor3f(0.0f, 1.0f, 0.0f);
        if (i == 2) glColor3f(1.0f, 1.0f, 0.0f);
        if (i == 3) glColor3f(1.0f, 0.0f, 0.0f);

        glBegin(renderType);
        glVertex3f(-width * (i + 0.5f) / 140, (height - 1) / 200.0f * i, 0);
        glVertex3f(width * (i + 0.5f) / 140, (height + 1) / 200.0f * i, 0);
        glEnd();
    }
}

// Textured Shape (Cube with coffee texture)
void drawCoffee(float width, float height, float renderType)
{
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID[frame]);

    glColor3f(r, g, b);

    glBegin(renderType);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-width / 100, -height / 100, 0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(width / 100, -height / 100, 0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(width / 100, height / 100, 0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-width / 100, height / 100, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// Required Functions (Color Swapping Shapes)
void drawSpecialItems()
{
    glPushMatrix();
    if (!colorSwap)
        glColor3f(0.8f, 0.1f, 0.1f);
    else
        glColor3f(0.1f, 0.8f, 0.1f);
    glTranslatef(0.5f, 0.0f, 0.0f);
    drawSquare(10, 10, GL_POLYGON);
    glPopMatrix();

    glPushMatrix();
    if (!colorSwap)
        glColor3f(0.1f, 0.8f, 0.1f);
    else
        glColor3f(0.8f, 0.1f, 0.1f);
    glTranslatef(-0.5f, 0.0f, 0.0f);
    drawTriangle(10, 10, GL_POLYGON);
    glPopMatrix();
}

// Bonus Object (Simple logo)
void drawBonusLogo()
{
    glPushMatrix();
    glLineWidth(4.0f);
    glTranslatef(0.80f, -0.88f, 0.0f);
    glRotatef(10.0f, 0.0f, 0.0f, 1.0f);
    drawLetterV(2.4f, 2.4f, GL_LINES, 0.8f, 0.8f, 0.1f);
    glLineWidth(1.0f);
    glPopMatrix();

    glPushMatrix();
    glLineWidth(2.0f);
    glTranslatef(0.91f, -0.975f, -0.1f);
    drawLines(6.0f, 3.0f, GL_LINES);
    glLineWidth(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.9f, -0.8f, 0.0f);
    glRotatef(8.0f, 0.0f, 0.0f, 1.0f);
    drawWitchHat(10.0f, 10.0f, GL_POLYGON, 0.0f, 0.0f, 1.0f);
    glPopMatrix();
}

// Draw a simple floor as a rectangle that serves as a collider.
void drawFloor() {
    glColor3f(0.3f, 0.3f, 0.3f); // Dark grey
    glBegin(GL_POLYGON);
    glVertex3f(-1.0f, floorY, 0);
    glVertex3f(1.0f, floorY, 0);
    glVertex3f(1.0f, floorY - 0.05f, 0);
    glVertex3f(-1.0f, floorY - 0.05f, 0);
    glEnd();
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Clear the window screen
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
}

void MyDisplay() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-1.0 / zoomFactor, 1.0 / zoomFactor, -1.0 / zoomFactor, 1.0 / zoomFactor, -1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the floor first
    if (displayOn)
        drawFloor();

    // Draw the moving cube (using coffee texture) with gravity applied
    glPushMatrix();
    glTranslatef(squareX, squareY, 0.0f);
    if (solid && !wireframe && displayOn)
        drawCoffee(10, 10, GL_POLYGON);
    else if (wireframe && !solid && displayOn) {
        glColor3f(r, g, b);
        drawSquare(10, 10, GL_LINE_LOOP);
    }
    glPopMatrix();

    // Bonus and extra items
    if (bonusMode && displayOn)
        drawBonusLogo();
    if (axis && displayOn) {
        glPushMatrix();
        drawAxis();
        glPopMatrix();
    }
    if (extras && displayOn) {
        glPushMatrix();
        drawSpecialItems();
        glPopMatrix();
    }

    glFlush();
    glutSwapBuffers();
}

void loadTextures() {
    int i;
    glGenTextures(4, texID); // Get the texture object IDs.
    for (i = 0; i < 4; i++)
    {
        void* imgData; // Pointer to image color data read from the file.
        int imgWidth;  // Image width.
        int imgHeight; // Image height.
        FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
        if (format == FIF_UNKNOWN) {
            printf("Unknown file type for texture image file %s\n", textureFileNames[i]);
            continue;
        }
        FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], 0);
        if (!bitmap) {
            printf("Failed to load image %s\n", textureFileNames[i]);
            continue;
        }
        FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap);
        FreeImage_Unload(bitmap);
        imgData = FreeImage_GetBits(bitmap2);
        imgWidth = FreeImage_GetWidth(bitmap2);
        imgHeight = FreeImage_GetHeight(bitmap2);
        if (imgData) {
            printf("Texture image loaded from file %s, size %dx%d\n",
                textureFileNames[i], imgWidth, imgHeight);
            glBindTexture(GL_TEXTURE_2D, texID[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_BGR_EXT,
                GL_UNSIGNED_BYTE, imgData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        }
        else {
            printf("Failed to get texture data from %s\n", textureFileNames[i]);
        }
    }
}

void timer(int v)
{
    // Animation frame update
    frame = (frame + 1) % 4;

    // Physics update: Apply gravity and update vertical position.
    // Convert frameDelay (ms) to seconds.
    float dt = frameDelay / 1000.0f;
    velocityY += gravity * dt;      // Update vertical velocity by gravity
    squareY += velocityY * dt;      // Update vertical position

    // Floor collision: if the bottom of the cube (squareY - cubeHalfHeight) goes below floorY, clamp it.
    if (squareY - cubeHalfHeight < floorY) {
        squareY = floorY + cubeHalfHeight;
        velocityY = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(frameDelay, timer, 0); // Frame delay in milliseconds
}

void procSpecialKeys(int key, int x, int y)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (key == GLUT_KEY_LEFT)
        squareX -= 0.1 * speed;
    if (key == GLUT_KEY_RIGHT)
        squareX += 0.1 * speed;
    if (key == GLUT_KEY_UP)
        squareY += 0.1 * speed; // Manual upward movement (in addition to gravity)
    if (key == GLUT_KEY_DOWN)
        squareY -= 0.1 * speed;
    glOrtho(-1 / zoomFactor, 1 / zoomFactor, -1 / zoomFactor, 1 / zoomFactor, -1, 1);
}

void normalKeys(unsigned char key, int x, int y)
{
    float movementConstant = 0.3;
    switch (key)
    {
    case '1': // Mode Toggle
        wireframe = !wireframe;
        solid = !solid;
        break;
    case '2': // Slow down animation
        frameDelay += 50;
        if (frameDelay > 500) frameDelay = 500;
        break;
    case '3': // Speed up animation
        frameDelay -= 50;
        if (frameDelay < 50) frameDelay = 50;
        break;
    case '4': // Bonus Toggle
        bonusMode = !bonusMode;
        break;
    case 'a': // Axis Toggle 
        axis = !axis;
        break;
    case 'c': // Disable Display
        displayOn = false;
        break;
    case 'm': // Enable Display
        displayOn = true;
        break;
    case 'p': // Toggle audio
        audio = !audio;
        if (audio)
            SoundEngine->play2D(audioTracks[currentTrack], true);
        else
            SoundEngine->stopAllSounds();
        break;
        // Bonus color adjustments
    case 'r': r = r - 0.1; break;
    case 'g': g = g - 0.1; break;
    case 'b': b = b - 0.1; break;
    case 't': r = r + 0.1; break;
    case 'h': g = g + 0.1; break;
    case 'n': b = b + 0.1; break;
    case 27: // ESC key to exit
        exit(0);
    }
}

void procMouse(int button, int state, int x, int y)
{
    // Handle Zoom
    if (button == 4 && state == GLUT_UP) { // Zoom Out
        zoomFactor -= 0.1;
        if (zoomFactor < 0.5) zoomFactor = 0.5;
    }
    else if (button == 3 && state == GLUT_UP) { // Zoom In
        zoomFactor += 0.1;
        if (zoomFactor > 10.0) zoomFactor = 10.0;
    }
    // Update Projection Matrix
    if (button == 4 || button == 3) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1 / zoomFactor, 1 / zoomFactor, -1 / zoomFactor, 1 / zoomFactor, -1, 1);
        glMatrixMode(GL_MODELVIEW);
    }
    // Music change + Color Swap
    if (state == GLUT_DOWN) {
        if (button == GLUT_RIGHT_BUTTON && audio) {
            currentTrack = (currentTrack + 1) % 3;
            SoundEngine->stopAllSounds();
            SoundEngine->play2D(audioTracks[currentTrack], true);
        }
        else if (button == GLUT_LEFT_BUTTON) {
            colorSwap = !colorSwap;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    // Updated to use double buffering along with RGB mode.
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("V Cavallaro | 811097945");

    glutTimerFunc(0, timer, 0);

    init();
    loadTextures();

    // Callbacks
    glutDisplayFunc(MyDisplay);
    glutSpecialFunc(procSpecialKeys);
    glutMouseFunc(procMouse);
    glutKeyboardFunc(normalKeys);

    // Print controls to terminal
    std::cout << std::endl;
    std::cout << "Controls:\n";
    std::cout << "Arrow Keys: Move the player\n";
    std::cout << "'a': Toggle axis\n";
    std::cout << "'c': Clear screen\n";
    std::cout << "'m': Show screen\n";
    std::cout << "'p': Toggle audio\n";
    std::cout << "Left-click: Swap Red/Green colors\n";
    std::cout << "Right-click: Cycle audio (if playing)\n";
    std::cout << "Mouse Scroll: Zoom in/out\n";
    std::cout << "ESC: Exit\n\n";
    std::cout << "Bonus Features:\n";
    std::cout << "'1': Toggle wireframe/solid\n";
    std::cout << "'2': Slow down animation\n";
    std::cout << "'3': Speed Up animation\n";
    std::cout << "'4': Toggle Bonus Logo/solid\n\n";
    std::cout << "'r': Reduce Wireframe Red\n";
    std::cout << "'g': Reduce Wireframe Green\n";
    std::cout << "'b': Reduce Wireframe Blue\n";
    std::cout << "'t': Increase Wireframe Red\n";
    std::cout << "'h': Increase Wireframe Green\n";
    std::cout << "'n': Increase Wireframe Blue\n";

    glutMainLoop();
    return 0;
}
