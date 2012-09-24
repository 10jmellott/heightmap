//CMPSC 458 Heightmap Starter Code
#ifdef WIN32
// For VC++ you need to include this file as glut.h and gl.h refer to it
#include <windows.h>
// disable the warning for the use of strdup and friends
#pragma warning(disable:4996) 
#endif

#include <stdio.h>		// Standard Header For Most Programs
#include <stdlib.h>		// Additional standard Functions (exit() for example)

#include <iostream>

#include <GL/gl.h>		// The GL Header File
#include <GL/glu.h>		// The GL Utilities (GLU) header
#include <GL/glut.h>	// The GL Utility Toolkit (Glut) Header

#include <pic.h>		// Interface to libpicio, provides functions to load/save jpeg files

using namespace std;

int g_iMenuId;

/* State of the mouse */
int mousePos[2] = {0, 0};
int leftMouseButtonState = 0;    /* 1 if pressed, 0 if not */
int middleMouseButtonState = 0;
int rightMouseButtonState = 0;

/* Current control mode the program is in (what will happen when the mouse is moved */
typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;
CONTROLSTATE currentControlState = ROTATE;

/* transformation variables */
GLfloat currentRotation[3] = {0.0, 0.0, 0.0};
GLfloat currentTranslation[3] = {0.0, 0.0, 0.0};
GLfloat currentScaling[3] = {1.0, 1.0, 1.0};

#define PI 3.14159265359
GLfloat currentViewAngle[2] = {0.0, PI / 2};
GLfloat currentView[3] = {0.0, 0.0, 0.0};

/* source image for heightmap */
Pic * sourceImage;

/* Texture ids */
GLuint frontTextureId;
GLuint leftTextureId;
GLuint backTextureId;
GLuint rightTextureId;
GLuint topTextureId;
GLuint bottomTextureId;

// default window width and height
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

// default maximum height in pixels of the heightmap
#define HEIGHTMAP_RANGE 100

// options to enable(1) and or disable(0)
#define DRAW_SKYBOX 1
#define DRAW_HEIGHTMAP 1

// if the skybox is to be scaled
#define SCALE_SKYBOX 0

// defined to remove the black edges along the textures
#define GL_CLAMP_TO_EDGE 0x812F

/* ----------------------------------- FUNCTION PROTOTYPES ------------------------------------------ */

/* This function takes the name of a jpg file and a texture ID (by reference)
* It creates a texture from the image specified and sets the ID specified to the value OpenGL generates for that texture */
void loadTexture (char *filename, GLuint &textureID);

/* initGL will perform the one time initialization by
* setting some state variables that are not going to
* be changed */
void InitGL ( GLvoid );

/* sets the reshape callback for the current window */
void reshape(int w, int h);	

/* Main GL display loop.
* This function is called by GL whenever it wants to update the display.
* All of the drawing you do should be done within this function, or functions
* that this calls
* Create The Display Function. model, view and lighting goes here */
void display ( void );   

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename);

/* converts mouse drags into information about rotation/translation/scaling
* This is run by GL whenever the mouse is moved and a mouse button is being
* held down. */
void mousedrag(int x, int y);

/* This function is called by GL when the mouse moves passively.
* "Passively" means that no mouse button is being held down */
void mouseidle(int x, int y);

/* This is called by GL whenever a mouse button is pressed. */
void mousebutton(int button, int state, int x, int y);

/* This function will be called by GL whenever a keyboard key is pressed.
* It recieves the ASCII value of the key that was pressed, along with the x
* and y coordinates of the mouse at the time the key was pressed. */
void keyboard (unsigned char key, int x, int y);

/* Function that GL runs once a menu selection has been made.
* This receives the number of the menu item that was selected */
void menufunc(int value);

/* This function is called by GL whenever it is idle, usually after calling display */
void doIdle();

/* this function will gather the height */
GLfloat getHeight(int i, int j);

GLvoid drawSkybox();

GLvoid drawHeightmap();

/* ------------------------------------------ MAIN FUNCTION ------------------------------------------- */

int main ( int argc, char** argv )
{
	/* aquire filename from option or prompt */
	if (argc<2)
	{
		char inputFile[100];
		printf("Input height file:");
		cin>>inputFile;
		sourceImage = jpeg_read(inputFile, NULL);
	}
	else
	{
		sourceImage = jpeg_read(argv[1], NULL);
	}

	// if the heightmap image couldn't be found
	if(sourceImage == NULL)
		return EXIT_FAILURE;

	/* Initialize glut */
	glutInit(&argc, argv);

	/* Set up window modes */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	/* Set window position (where it will appear when it opens) */
	glutInitWindowPosition(0,0);

	/* Set size of window */
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	/* Create the window */
	glutCreateWindow( "CMPSC 458: Heightmap" );

	/**** Call to our initialization routine****/
	InitGL ();

	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);

	/*sets the reshape callback for the current window*/
	glutReshapeFunc(reshape);

	/** allow the user to quit using the right mouse button menu **/
	/* Set menu function callback */
	g_iMenuId = glutCreateMenu(menufunc);

	/* Set identifier for menu */
	glutSetMenu(g_iMenuId);

	/* Add quit option to menu */
	glutAddMenuEntry("Save Image", 0);
	glutAddMenuEntry("Quit", 1);

	/* Attach menu to right button clicks */
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	/* Set idle function.  You can change this to call code for your animation,
	* or place your animation code in doIdle */
	glutIdleFunc(doIdle);

	/* callback for keyboard input */
	glutKeyboardFunc(keyboard);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);

	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);

	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);

	glutMainLoop();          // Initialize The Main Loop

	return 0;
}



/* -------------------------------------- FUNCTION DEFINITIONS ---------------------------------------- */

void loadTexture (char *filename, GLuint &textureID)
{
	Pic *pBitMap = pic_read(filename, NULL);

	if(pBitMap == NULL)
	{
		printf("Could not load the texture file\n");
		exit(1);
	}

	glEnable(GL_TEXTURE_2D); // Enable texturing
	glGenTextures(1, &textureID); // Obtain an id for the texture

	glBindTexture(GL_TEXTURE_2D, textureID); // Set as the current texture

	/* set some texture parameters */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	/* Load the texture into OpenGL as a mipmap. !!! This is a very important step !!! */
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, pBitMap->nx, pBitMap->ny, GL_RGB, GL_UNSIGNED_BYTE, pBitMap->pix);
	glDisable(GL_TEXTURE_2D);
	pic_free(pBitMap); // now that the texture has been copied by OpenGL we can free our copy
}

void InitGL ( GLvoid )
{

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background	

	// load textures
	loadTexture("texture/sky25/front.jpg", frontTextureId);
	loadTexture("texture/sky25/left.jpg", leftTextureId);
	loadTexture("texture/sky25/back.jpg", backTextureId);
	loadTexture("texture/sky25/right.jpg", rightTextureId);
	loadTexture("texture/sky25/up.jpg", topTextureId);
	loadTexture("texture/sky25/down.jpg", bottomTextureId);
}

void reshape(int w, int h)
{
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, aspect, 0.1, 10000.0);
	glMatrixMode(GL_MODELVIEW);
}

void display ( void )
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* set view of the camera, using polar coordinates of a sphere */

	currentView[0] = cos(currentViewAngle[0]) * sin(currentViewAngle[1]);
	currentView[1] = sin(currentViewAngle[0]) * sin(currentViewAngle[1]);
	currentView[2] = cos(currentViewAngle[1]);

	gluLookAt(currentTranslation[0], currentTranslation[1], currentTranslation[2], currentView[0] + currentTranslation[0], currentView[1] + currentTranslation[1], currentView[2] + currentTranslation[2], 0, 0, 1);
	glScalef(currentScaling[0], currentScaling[1], currentScaling[2]);

	/* Clear buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw skybox */

	if(DRAW_SKYBOX)
		drawSkybox();

	if(DRAW_HEIGHTMAP)
		drawHeightmap();

	

	/* Swap buffers, so one we just drew is displayed */
	glutSwapBuffers();
}

void saveScreenshot (char *filename)
{
	int i;
	Pic *in = NULL;

	if (filename == NULL)
		return;

	/* Allocate a picture buffer */
	in = pic_alloc(WINDOW_WIDTH, WINDOW_HEIGHT, 3, NULL);

	printf("File to save to: %s\n", filename);

	/* Loop over each row of the image and copy into the image */
	for (i=WINDOW_HEIGHT-1; i>=0; i--) {
		glReadPixels(0, WINDOW_HEIGHT-1-i, WINDOW_WIDTH, 1, GL_RGB,
			GL_UNSIGNED_BYTE, &in->pix[i*in->nx*in->bpp]);
	}

	/* Output the file */
	if (jpeg_write(filename, in)) {
		printf("File saved Successfully\n");
	} else {
		printf("Error in Saving\n");
	}

	/* Free memory used by image */
	pic_free(in);
}

void mousedrag(int x, int y)
{
	int mousePosChange[2] = {x-mousePos[0], y-mousePos[1]};

	/* Check which state we are in. */
	switch (currentControlState)
	{
	case TRANSLATE:
		if (leftMouseButtonState)
		{
			float theta = PI - currentViewAngle[0];
			
			// move mouse left or right
			currentTranslation[0] -= (mousePosChange[0] * sin(theta)) * 0.3;
			currentTranslation[1] -= (mousePosChange[0] * cos(theta)) * 0.3;

			theta = currentViewAngle[0];

			// move mouse up or down
			currentTranslation[0] += (mousePosChange[1] * cos(theta)) * 0.3;
			currentTranslation[1] += (mousePosChange[1] * sin(theta)) * 0.3;
		}
		if (middleMouseButtonState)
		{
			currentTranslation[2] += mousePosChange[1]*0.3;
		}
		break;
	case ROTATE:
		if (leftMouseButtonState)
		{
			currentViewAngle[0] -= mousePosChange[0]*0.004;
			currentViewAngle[1] += mousePosChange[1]*0.004;
		}
		if (middleMouseButtonState)
		{
			currentViewAngle[1] += mousePosChange[1]*0.005;
		}

		// limits the view angles
		if(currentViewAngle[1] > PI)
			currentViewAngle[1] = PI - .0001;
		if(currentViewAngle[1] < 0)
			currentViewAngle[1] = .0001;

		break;
	case SCALE:
		if (leftMouseButtonState)
		{
			currentScaling[0] *= 1.0+mousePosChange[0]*0.001;
			currentScaling[1] *= 1.0-mousePosChange[1]*0.001;
		}
		if (middleMouseButtonState)
		{
			currentScaling[2] *= 1.0-mousePosChange[1]*0.001;
		}
		break;
	}

	/* Update stored mouse position */
	mousePos[0] = x;
	mousePos[1] = y;
}

void mouseidle(int x, int y)
{
	/* Update mouse position */
	mousePos[0] = x;
	mousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

	/* Check which button was pressed and update stored mouse state */
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		leftMouseButtonState = (state==GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		middleMouseButtonState = (state==GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		rightMouseButtonState = (state==GLUT_DOWN);
		break;
	}

	/* Check what modifier keys (shift, ctrl, etc) are pressed and update the
	* control mode based off of which keys are pressed */
	switch(glutGetModifiers())
	{
	case GLUT_ACTIVE_CTRL:
		currentControlState = TRANSLATE;
		break;
	case GLUT_ACTIVE_SHIFT:
		currentControlState = SCALE;
		break;
	default:
		currentControlState = ROTATE;
		break;
	}

	/* Update stored mouse position */
	mousePos[0] = x;
	mousePos[1] = y;
}

void keyboard (unsigned char key, int x, int y)
{

	/* User pressed quit key */
	if(key == 'q' || key == 'Q' || key == 27) {
		exit(0);
	}
}

void menufunc(int value)
{
	switch (value)
	{
	case 0:
		printf("Save file as: \n");
		char outf[100];
		cin >> outf;
		saveScreenshot(outf);
		break;

	case 1:
		exit(0);
		break;
	}
}

void doIdle()
{
	/* do some stuff... */

	/* make the screen update. */
	glutPostRedisplay(); 
}

GLfloat getHeight(int i, int j)
{
	GLfloat red = PIC_PIXEL(sourceImage, i, j, 0);
	GLfloat green = PIC_PIXEL(sourceImage, i, j, 1);
	GLfloat blue = PIC_PIXEL(sourceImage, i, j, 2);

	return (red + green + blue) / (3 * (255 / HEIGHTMAP_RANGE));
}

GLvoid drawSkybox()
{
	// Store the current matrix
    glPushMatrix();
 
    // Reset and transform the matrix.
    glLoadIdentity();

	// sets the viewing angle to the values calculated in display()
	gluLookAt(0, 0, 0, currentView[0], currentView[1], currentView[2], 0, 0, 1);
 
    // Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

	if(SCALE_SKYBOX)
		glScalef(currentScaling[0], currentScaling[1], currentScaling[1]);

	/* Texture coordinates */
	float t1[] = {1.0, 1.0};
	float t2[] = {1.0, 0.0};
	float t3[] = {0.0, 0.0};
	float t4[] = {0.0, 1.0};

	/* vertices of the cube */
	float v1[] = { 1.0, -1.0, -1.0};
	float v2[] = { 1.0, -1.0,  1.0};
	float v3[] = { 1.0,  1.0,  1.0};
	float v4[] = { 1.0,  1.0, -1.0};
	float v5[] = {-1.0, -1.0, -1.0};
	float v6[] = {-1.0, -1.0,  1.0};
	float v7[] = {-1.0,  1.0,  1.0};
	float v8[] = {-1.0,  1.0, -1.0};

	glEnable(GL_TEXTURE_2D); // Enable texturing from now on

	glColor3f(1.0, 1.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, frontTextureId); 
	glBegin(GL_QUADS);
	glTexCoord2fv(t1);
	glVertex3fv(v4);

	glTexCoord2fv(t2);
	glVertex3fv(v3);

	glTexCoord2fv(t3);
	glVertex3fv(v2);

	glTexCoord2fv(t4);
	glVertex3fv(v1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, rightTextureId); 
	glBegin(GL_QUADS);
	glTexCoord2fv(t1);
	glVertex3fv(v1);

	glTexCoord2fv(t2);
	glVertex3fv(v2);

	glTexCoord2fv(t3);
	glVertex3fv(v6);

	glTexCoord2fv(t4);
	glVertex3fv(v5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, backTextureId); 
	glBegin(GL_QUADS);
	glTexCoord2fv(t1);
	glVertex3fv(v5);

	glTexCoord2fv(t2);
	glVertex3fv(v6);

	glTexCoord2fv(t3);
	glVertex3fv(v7);

	glTexCoord2fv(t4);
	glVertex3fv(v8);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, leftTextureId); 
	glBegin(GL_QUADS);
	glTexCoord2fv(t1);
	glVertex3fv(v8);

	glTexCoord2fv(t2);
	glVertex3fv(v7);

	glTexCoord2fv(t3);
	glVertex3fv(v3);

	glTexCoord2fv(t4);
	glVertex3fv(v4);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, topTextureId); 
	glBegin(GL_QUADS);
	glTexCoord2fv(t1);
	glVertex3fv(v2);

	glTexCoord2fv(t2);
	glVertex3fv(v3);

	glTexCoord2fv(t3);
	glVertex3fv(v7);

	glTexCoord2fv(t4);
	glVertex3fv(v6);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, bottomTextureId); 
	glBegin(GL_QUADS);
	glTexCoord2fv(t1);
	glVertex3fv(v4);

	glTexCoord2fv(t2);
	glVertex3fv(v1);

	glTexCoord2fv(t3);
	glVertex3fv(v5);

	glTexCoord2fv(t4);
	glVertex3fv(v8);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);

	 // Restore enable bits and matrix
    glPopAttrib();
    glPopMatrix();
}

GLvoid drawHeightmap()
{
		/* heightmap */
		int width = sourceImage->nx;
		int length = sourceImage->ny;
		GLfloat height;
		GLfloat colorRange = HEIGHTMAP_RANGE;

		// drops the heightmap down to the level of the user
		glTranslatef(0.0, 0.0, - 1 * HEIGHTMAP_RANGE);

		// the following will draw a cross pasttern, as opposed to triangles
		//   and allows for a much more standard mesh appearance
		// initially centered at the location of the user
		for(int i = width - 1; i > 1; i--)
		{
			glBegin(GL_LINE_STRIP);
			for(int j = length - 1; j > 1; j--)
			{
				height = getHeight(i, j);
				glColor3f(height / colorRange, height / colorRange, height / colorRange);
				glVertex3f(i - width / 2, j - length / 2, height);
			}
			glEnd();
		}
	
		for(int i = length - 1; i > 1; i--)
		{
			glBegin(GL_LINE_STRIP);
			for(int j = width - 1; j > 1; j--)
			{
				height = getHeight(j, i);
				glColor3f(height / colorRange, height / colorRange, height / colorRange);
				glVertex3f(j - width / 2, i - length / 2, height);
			}
			glEnd();
		}
		
}