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

float speed = 0.05;

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

// Gameobject class
class GameObject
{
public:
	// object state
	GLfloat   x, y, z, sizeX, sizeY;
	GLfloat   colorR, colorG, colorB;
	GLfloat   mass;
	bool      isSolid;
	bool      destroyed;
	bool      gravity;
	
	GameObject();

	void DrawGameObject(bool, GameObject);
};

// Gameobjects on the screen
GameObject player, obsticle, collectible;

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
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

	obsticle.x = 3;
	obsticle.colorG = 0;

	collectible.x = -3;
	collectible.colorB = 0;

	SoundEngine->play2D("audio/Catsong.mp3", true);
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw player sprite
	glPushMatrix();
	player.DrawGameObject(true, player);
	glPopMatrix();

	glPushMatrix();
	obsticle.DrawGameObject(false, obsticle);
	glPopMatrix();

	glPushMatrix();
	collectible.DrawGameObject(false, collectible);
	glPopMatrix();

	// Removes the collectible
	if (CheckCollision(player, collectible))
		collectible.destroyed = true;

	glFlush();
	glutSwapBuffers();
}

void Keyboard(int key, int x, int y) {

	if (key == GLUT_KEY_UP) {
		player.y += speed;
			if (CheckCollision(player, obsticle))
				player.y -= speed;
	}
	if (key == GLUT_KEY_DOWN) {
			player.y -= speed;
			if (CheckCollision(player, obsticle))
				player.y += speed;
	}
	if (key == GLUT_KEY_LEFT) {
			player.x -= speed;
			if (CheckCollision(player, obsticle))
				player.x += speed;
	}
	if (key == GLUT_KEY_RIGHT) {
			player.x += speed;
			if (CheckCollision(player, obsticle))
				player.x -= speed;
	}
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

	if (frame >= 4) {
		frame = 0;
	}

	glutPostRedisplay();
	glutTimerFunc(100, timer, v); // Creates a frame delay that is counted in miliseconds
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB); // RGB mode
	glutInitWindowSize(WIN_W, WIN_H); // window size
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutCreateWindow("Collision Example");

	glutTimerFunc(0, timer, 0);

	init();

	loadTextures();

	glutDisplayFunc(MyDisplay); // call the drawing function

	glutSpecialFunc(Keyboard);

	glutMainLoop();
	return 0;
}

GameObject::GameObject() {
	x = y = z = 0;
	sizeX = sizeY = 1;
	colorR = colorG = colorB = 1;
	mass = 0;
	isSolid = false;
	destroyed = false;
	gravity = false;
}

void GameObject::DrawGameObject(bool sprite, GameObject parameters)
{
	glPushMatrix();
	glTranslatef(parameters.x, parameters.y, parameters.z);

	if (!parameters.destroyed)
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
			glVertex3f(0.5, -0.5, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(0.5, 0.5, 0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(-0.5, 0.5, 0);
			glEnd();

			glDisable(GL_TEXTURE_2D); // Turn texturing off
		}
		else
		{
			glColor3f(parameters.colorR, parameters.colorG, parameters.colorB);

			glBegin(GL_POLYGON);
			glVertex3f(-0.5, -0.5, 0);
			glVertex3f(0.5, -0.5, 0);
			glVertex3f(0.5, 0.5, 0);
			glVertex3f(-0.5, 0.5, 0);
			glEnd();
		}

		glPopMatrix();
	}
}

