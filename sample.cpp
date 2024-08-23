#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

const char *WINDOWTITLE = "OpenGL / GLUT Sample -- Amogh Sawant";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;
const int INIT_WINDOW_WIDTH = 900;
const int INIT_WINDOW_HEIGHT = 900;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.01f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

struct Point {
	float x;
	float y;
};

struct xyzCoor {
	GLfloat x, y, z;
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE     = GL_LINEAR;
const GLfloat FOGDENSITY  = 0.30f;
const GLfloat FOGSTART    = 1.5f;
const GLfloat FOGEND      = 4.f;

// for lighting:

const float	WHITE[ ] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 700000;		// 10000 milliseconds = 10 seconds


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER
// 
// 
// constant physics parameters
const int	PLAYER_EYE = 5;
const float	SPEEDCONST = 0.5;
float		VELOCITY_X = 0;
float		VELOCITY_Z = 0;
float		VELOCITY_Y = 0.0;
float		FRICTION = -0.02;
float		G = -9.8 * 0.003;

// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
GLuint  Wall, Hands;
int		AxesOn;					// != 0 means to draw the axes

GLuint  LightSource;            // light source
GLuint  Map, testMap;					// Map

bool    spot_light;
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		    // ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

float   xcam = -10;
float   ycam = PLAYER_EYE;
float   zcam = -10;

float	xlook = 10.;
float	ylook = 5.;
float	zlook = 10.;
float   thta = 0.;
float	gma = 0.;
const float LOOKRADIUS = 5000.;

// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );
void	MoveForward( );
void	MoveBackward( );
void	MoveRight();
void	MoveLeft();
void	Jump( );
void	Gravity( );
void	Physics( );
void	Collision( );

void			Axes( float );
void			HsvRgb( float[3], float [3] );
void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);
float			Unit(float [3]);

Point	pointOnCircle(float, float, float, float);

GLuint	handTex, floorTex, wallTex1, wallTex2, wallTex3, pillarTex, ceilingTex, bwallTex, skyTex, wallTex4, wallTex5;

GLuint	skysphere, gunlist;

// utility to create an array from 3 separate values:

float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
#include "osucone.cpp"
#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"


// main program:

int
main( int argc, char *argv[ ] )
{
	glutInit( &argc, argv );

	InitGraphics( );

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );

	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}

void
Animate( )
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow( MainWindow );

	// erase the background:
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif


	// specify shading to be flat:

	glShadeModel( GL_FLAT );

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( NowProjection == ORTHO )
		glOrtho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
	else
		gluPerspective( 70.f, 1.f,	0.1f, 1000.f );

	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	//      set the eye position,      look-at position, and    up-vector:
	gluLookAt( xcam, ycam, zcam,    xlook, ylook, zlook,     0.f, 1.f, 0.f );    

	// rotate the scene: 

	glRotatef( (GLfloat)Yrot, 0.f, 1.f, 0.f );
	glRotatef( (GLfloat)Xrot, 1.f, 0.f, 0.f );

	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// possibly draw the axes:

	if( AxesOn != 1 )
	{
		glColor3fv( &Colors[NowColor][0] );
		glCallList( AxesList );
	}


	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable( GL_NORMALIZE );



	// draw the Light:
	SetPointLight(GL_LIGHT0, 0, 8, 0, 1.f, 0, 0);
	SetPointLight(GL_LIGHT1, 0, 5, 120, 0.f, 1, 0.5);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MulArray3(.85f, .80f, 0.80f, 0.80f));
	

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	glTranslatef(xcam, ycam-3, zcam);
	glRotatef(-thta,0, 1, 0);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(90+180, 0, 1, 0);
	glCallList(gunlist);
	glPopMatrix();

	glCallList(Map);
	glPushMatrix();
		glRotatef(Time * 360, 0, 1, 0);
		glCallList(skysphere);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	Physics();

#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.f,   0.f, 1.f, 0.f );
			glCallList( BoxList );
		glPopMatrix( );
	}
#endif

	glDisable( GL_DEPTH_TEST );


	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1.f, 1.f, 1.f );

	glutSwapBuffers( );


	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}


void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");


	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

}


#define PI 3.14159
void
InitLists( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	glutSetWindow( MainWindow );
	
	#define XSIDE	1			
	#define X0      (-XSIDE/2.)		
	#define NX		(XSIDE*200)			
	#define DX		( XSIDE/(float)NX )	

	#define YGRID	0.f

	#define ZSIDE	1			
	#define Z0      (-ZSIDE/2.)		
	#define NZ		(ZSIDE*200)			
	#define DZ		( ZSIDE/(float)NZ )	

	Wall = glGenLists(1);
	glNewList(Wall, GL_COMPILE);
	SetMaterial(1.f, 1.f, 1.f, 2.f);
	glShadeModel(GL_SMOOTH);
	for (int i = 0; i < NZ; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NX; j++)
		{
			float s = (i - X0) / (NX - X0);
			float t = (j - Z0) / (NZ - Z0);
			glNormal3f(0., 1., 0.);
			glTexCoord2f(s, t);
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 0));
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 1));
		}
		glEnd();
	}
	glEndList();

	LightSource = glGenLists(1);
	glNewList(LightSource, GL_COMPILE);
	OsuSphere(0.1, 10, 10);
	glEndList();

	int Fw, Fh;
	unsigned char* floortexture = BmpToTexture("floor.bmp", &Fw, &Fh);
	glGenTextures(1, &floorTex);
	glBindTexture(GL_TEXTURE_2D, floorTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Fw, Fh, 0, GL_RGB, GL_UNSIGNED_BYTE, floortexture);

	int W1w, W1h;
	unsigned char* walltexture1 = BmpToTexture("wall1.bmp", &W1w, &W1h);
	glGenTextures(1, &wallTex1);
	glBindTexture(GL_TEXTURE_2D, wallTex1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, W1w, W1h, 0, GL_RGB, GL_UNSIGNED_BYTE, walltexture1);

	int W2w, W2h;
	unsigned char* walltexture2 = BmpToTexture("wall2.bmp", &W2w, &W2h);
	glGenTextures(1, &wallTex2);
	glBindTexture(GL_TEXTURE_2D, wallTex2);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, W2w, W2h, 0, GL_RGB, GL_UNSIGNED_BYTE, walltexture2);

	int W3w, W3h;
	unsigned char* walltexture3 = BmpToTexture("wall3.bmp", &W3w, &W3h);
	glGenTextures(1, &wallTex3);
	glBindTexture(GL_TEXTURE_2D, wallTex3);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, W3w, W3h, 0, GL_RGB, GL_UNSIGNED_BYTE, walltexture3);

	int W4w, W4h;
	unsigned char* walltexture4 = BmpToTexture("wall4.bmp", &W4w, &W4h);
	glGenTextures(1, &wallTex4);
	glBindTexture(GL_TEXTURE_2D, wallTex4);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, W4w, W4h, 0, GL_RGB, GL_UNSIGNED_BYTE, walltexture4);

	int W5w, W5h;
	unsigned char* walltexture5 = BmpToTexture("wall5.bmp", &W5w, &W5h);
	glGenTextures(1, &wallTex5);
	glBindTexture(GL_TEXTURE_2D, wallTex5);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, W5w, W5h, 0, GL_RGB, GL_UNSIGNED_BYTE, walltexture5);

	int Pw, Ph;
	unsigned char* pillartexture = BmpToTexture("pillar.bmp", &Pw, &Ph);
	glGenTextures(1, &pillarTex);
	glBindTexture(GL_TEXTURE_2D, pillarTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Pw, Ph, 0, GL_RGB, GL_UNSIGNED_BYTE, pillartexture);

	int Cw, Ch;
	unsigned char* ceilingtexture = BmpToTexture("ceiling.bmp", &Cw, &Ch);
	glGenTextures(1, &ceilingTex);
	glBindTexture(GL_TEXTURE_2D, ceilingTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Cw, Ch, 0, GL_RGB, GL_UNSIGNED_BYTE, ceilingtexture);

	int BWw, BWh;
	unsigned char* bwalltexture = BmpToTexture("gunt2.bmp", &BWw, &BWh);
	glGenTextures(1, &bwallTex);
	glBindTexture(GL_TEXTURE_2D, bwallTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, BWw, BWh, 0, GL_RGB, GL_UNSIGNED_BYTE, bwalltexture);

	int Sw, Sh;
	unsigned char* skytexture = BmpToTexture("sky2.bmp", &Sw, &Sh);
	glGenTextures(1, &skyTex);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Sw, Sh, 0, GL_RGB, GL_UNSIGNED_BYTE, skytexture);

	glDisable(GL_TEXTURE_2D);

	gunlist = glGenLists(1);
	glNewList(gunlist, GL_COMPILE);

	SetMaterial(1.f, 1.f, 1.f, 5.f);
	glShadeModel(GL_SMOOTH);
	glNormal3f(0., 1., 0.);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, bwallTex);
	LoadObjFile((char*)"gun.obj");
	glDisable(GL_TEXTURE_2D);
	glEndList();
	
	skysphere = glGenLists(1);
	glNewList(skysphere, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	OsuSphere(500, 500, 500);
	glEndList();
	
	glDisable(GL_TEXTURE_2D);


	Map = glGenLists(1);
	glNewList(Map, GL_COMPILE);
	// floor 
	glPushMatrix();
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, floorTex);
	glScalef(100, 1, 100);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//entrance flior
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, floorTex);
	glTranslatef(0, 0, 65);
	glScalef(25, 1, 30);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// base floir
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, floorTex);
	glTranslatef(0, 0, 105);
	glScalef(50, 1, 50);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// wall 1
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex1);
	
	glTranslatef(0, 12, -50);
	glRotatef(90, 1, 0, 0);

	glScalef(100, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// wall 2 right
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex1);
	glTranslatef(31, 12, 50);
	glRotatef(90, 1, 0, 0);
	glScalef(50-12.5, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// wall 2 left
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex1);
	glTranslatef(-31, 12, 50);
	glRotatef(90, 1, 0, 0);
	glScalef(50-12.5, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();


	// wall 3
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex1);
	glTranslatef(50, 12, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(100, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// wall 4
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex1);
	glTranslatef(-50, 12, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(100, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall 5
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex3);
	glRotatef(45, 0, 1, 0);
	glTranslatef(0, 12, -50);
	glRotatef(90, 1, 0, 0);
	glScalef(42, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall6
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex3);
	glRotatef(135, 0, 1, 0);
	glTranslatef(0, 12, -50);
	glRotatef(90, 1, 0, 0);
	glScalef(42, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//wall7
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex3);
	glRotatef(225, 0, 1, 0);
	glTranslatef(0, 12, -50);
	glRotatef(90, 1, 0, 0);
	glScalef(42, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// wall 8
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex3);
	glRotatef(315, 0, 1, 0);
	glTranslatef(0, 12, -50);
	glRotatef(90, 1, 0, 0);
	glScalef(42, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();


	//entrance ceil
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, ceilingTex);
	glTranslatef(0, 24, 65);
	glScalef(25, 1, 30);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//entrance wall left wall
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex2);
	glTranslatef(-12.5, 12, 65);
	glRotatef(90, 0, 0, 1);
	glScalef(25, 1, 30);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//entrance wall right wall
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex2);
	glTranslatef(12.5, 12, 65);
	glRotatef(90, 0, 0, 1);
	glScalef(25, 1, 30);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// base top right
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex5);
	glTranslatef(18.8, 12, 80);
	glRotatef(90, 1, 0, 0);
	glScalef(12.7, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// base top left
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex5);
	glTranslatef(-18.8, 12, 80);
	glRotatef(90, 1, 0, 0);
	glScalef(12.7, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// base ceiling
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, ceilingTex);
	glTranslatef(0, 24, 105);
	glScalef(50, 1, 50);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// base left
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex2);
	glTranslatef(-24, 12.5, 105);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(51, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// base right
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex2);
	glTranslatef(24, 12.5, 105);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(51, 1, 25);
	glCallList(Wall);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, wallTex4);
	// right first
	glPushMatrix();
	glTranslatef(17.5, 12, 120);
	glRotatef(90, 1, 0, 0);
	glScalef(15, 1, 25);
	glCallList(Wall);
	glPopMatrix();

	// left first
	glPushMatrix();
	glTranslatef(-17.5, 12, 120);
	glRotatef(90, 1, 0, 0);
	glScalef(15, 1, 25);
	glCallList(Wall);
	glPopMatrix();

	// right second
	glPushMatrix();
	glTranslatef(17.5, 12, 110);
	glRotatef(90, 1, 0, 0);
	glScalef(15, 1, 25);
	glCallList(Wall);
	glPopMatrix();

	// left second
	glPushMatrix();
	glTranslatef(-17.5, 12, 110);
	glRotatef(90, 1, 0, 0);
	glScalef(15, 1, 25);
	glCallList(Wall);
	glPopMatrix();

	// right third
	glPushMatrix();
	glTranslatef(10, 12, 115);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 25);
	glCallList(Wall);
	glPopMatrix();

	// left third
	glPushMatrix();
	glTranslatef(-10, 12, 115);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 25);
	glCallList(Wall);
	glPopMatrix();

	// big wall first
	glPushMatrix();
	glTranslatef(5, 12, 100);
	glRotatef(90, 1, 0, 0);
	glScalef(40, 1, 25);
	glCallList(Wall);
	glPopMatrix();

	// big wall second
	glPushMatrix();
	glTranslatef(5, 12, 90);
	glRotatef(90, 1, 0, 0);
	glScalef(40, 1, 25);
	glCallList(Wall);
	glPopMatrix();

	// big wall third
	glPushMatrix();
	glTranslatef(-15, 12, 95);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 25);
	glCallList(Wall);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//pillar 1
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, pillarTex);
	glTranslatef(-20, 0, -20);

	glPushMatrix();
	glTranslatef(0, 10, -5);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 10, 5);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5, 10, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, 10, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//pillar 2
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, pillarTex);
	glTranslatef(20, 0, -20);

	glPushMatrix();
	glTranslatef(0, 10, -5);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 10, 5);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5, 10, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, 10, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//pillar 3
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, pillarTex);
	glTranslatef(0, 0, 20);

	glPushMatrix();
	glTranslatef(0, 10, -5);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 10, 5);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5, 10, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, 10, 0);
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(10, 1, 20);
	glCallList(Wall);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glEndList();

	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			NowProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			NowProjection = PERSP;
			break;

		case 'w':
		case 'W':
			MoveForward();
			break;

		case 'a':
		case 'A':
			MoveLeft();
			break;

		case 'S':
		case 's':
			MoveBackward();
			break;

		case 'D':
		case 'd':
			MoveRight();
			break;

		case ' ':
			Jump();
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}



void
MouseMotion( int x, int y )
{	
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;
	Point poct;
	Point pocg;
	
	if (dy > 0)
		gma -= 1;
	if (dy < 0)
		gma += 1;
	if (dx > 0)
		thta+= 1;
	if (dx < 0)
		thta-= 1;

	if (gma >= 90)
		gma = 90.;
	if (gma <= -90)
		gma = -90.;

	pocg = pointOnCircle(xcam, ycam, gma, LOOKRADIUS);
	poct = pointOnCircle(xcam, zcam, thta, LOOKRADIUS);
	xlook = poct.x;
	ylook = pocg.y;
	zlook = poct.y;

	glutSetCursor(GLUT_CURSOR_NONE);

	if (x > INIT_WINDOW_WIDTH*0.85)
		glutWarpPointer(INIT_WINDOW_WIDTH * 0.15, y);
	if (x < INIT_WINDOW_WIDTH * 0.15)
		glutWarpPointer(INIT_WINDOW_WIDTH * 0.85, y);
	if (y < INIT_WINDOW_HEIGHT*0.15)
		glutWarpPointer(x, INIT_WINDOW_HEIGHT * 0.85);
	if (y > INIT_WINDOW_HEIGHT * 0.85)
		glutWarpPointer(x, INIT_WINDOW_HEIGHT * 0.15);

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{

	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}

Point 
pointOnCircle(float originX, float originY, float angle, float radius) {
	Point result;
	float radians = angle * M_PI / 180.0;

	result.x = originX + radius * cos(radians);
	result.y = originY + radius * sin(radians);

	return result;
}


void MoveForward() {
	Point coor = pointOnCircle(xcam, zcam, thta + 90., LOOKRADIUS);
	GLfloat dirX = xlook - xcam;
	GLfloat dirZ = zlook - zcam;
	GLfloat length = sqrt(dirX * dirX + dirZ * dirZ);

	dirX /= length;
	dirZ /= length;

	VELOCITY_X = SPEEDCONST * dirX;
	VELOCITY_Z = SPEEDCONST * dirZ;

	xcam += VELOCITY_X ;
	zcam += VELOCITY_Z ;
}

void MoveLeft() {
	Point coor = pointOnCircle(xcam, zcam, thta + 90., LOOKRADIUS);

	GLfloat dirX = coor.x - xcam;
	GLfloat dirZ = coor.y - zcam;
	GLfloat length = sqrt(dirX * dirX + dirZ * dirZ);

	dirX /= length;
	dirZ /= length;

	VELOCITY_X = SPEEDCONST * dirX;
	VELOCITY_Z = SPEEDCONST * dirZ;

	xcam -= VELOCITY_X;
	zcam -= VELOCITY_Z;
}

void MoveRight() {
	Point coor = pointOnCircle(xcam, zcam, thta + 90., LOOKRADIUS);
	GLfloat dirX = coor.x - xcam;
	GLfloat dirZ = coor.y - zcam;
	GLfloat length = sqrt(dirX * dirX + dirZ * dirZ);

	dirX /= length;
	dirZ /= length;

	VELOCITY_X = SPEEDCONST * dirX;
	VELOCITY_Z = SPEEDCONST * dirZ;

	xcam += VELOCITY_X;
	zcam += VELOCITY_Z;
}


void MoveBackward() {
	GLfloat dirX = xlook - zcam;
	GLfloat dirZ = zlook - zcam;
	GLfloat length = sqrt(dirX * dirX + dirZ * dirZ);

	dirX /= length;
	dirZ /= length;

	VELOCITY_X = SPEEDCONST * dirX;
	VELOCITY_Z = SPEEDCONST * dirZ;

	xcam -= VELOCITY_X;
	zcam -= VELOCITY_Z;
}

void Jump() {
	
	VELOCITY_Y = 1;
}

void Gravity() {
	if (VELOCITY_Y != 0)
		ycam += VELOCITY_Y;

	if (ycam > PLAYER_EYE) {
		VELOCITY_Y += G;
	}
	else {
		VELOCITY_Y = 0;
		ycam = PLAYER_EYE;
	}
}

void Physics() {
	Gravity();
}