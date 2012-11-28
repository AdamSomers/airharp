#ifndef h_GlStuff
#define h_GlStuff

// OpenGL
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <cmath>
#include <deque>
#include <iostream>

#define __PI    3.1415926

#define ABS(x)  (float) (x < 0 ? -x : x)
#define COS(x)  (float) cos( (double) (x) * __PI / 180.0 )
#define SIN(x)  (float) sin( (double) (x) * __PI / 180.0 )


class CloudEvent;

namespace GLStuff
{

// gl callbacks
void GLStartup(int argc, const char** argv);
void idleFunc( );
void displayFunc( );
void reshapeFunc( int width, int height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void motionFunc(int x, int y);
void timerCallback(int v);
void initialize_graphics( );
void createMenu(void);
void menu(int value);

void DrawString(int x,int y, std::string& s, void* font, float scaleFactor = 0.2f);
void createcircle (int k, int r, int h);
void generateBalls();
GLfloat rand2f( float a, float b );
void addBall();

void go2d();
void go3d();

//-----------------------------------------------------------------------------
// global variables and #defines
//-----------------------------------------------------------------------------

static float g_X ;
static float g_Y ;
static float g_ZOOM; //initial states for the gl_view


// width and height of the window
static GLsizei gWidth;
static GLsizei gHeight;
static GLsizei gLastWidth;
static GLsizei gLastHeight;

// light 0 position
static GLfloat g_light0_pos[4];

static GLboolean gDisplay;
static GLboolean gFullscreen;

static int gBufferSize;

static float gFrameRate;
static float gRefreshRate;

static int gMenuItem;
static GLfloat gInc_val_mouse;

static bool gLeftButtondown;
static int gMouseOriginX;
static int gMouseOriginY;
    
static bool gShowHUD;

class GLDisplay
{
public:
   GLDisplay(float x, float y, float w, float h)
   : fX(x)
   , fY(y)
   , fW(w)
   , fH(h)
   , fZ(0)
   , fR(1.0)
   , fG(0)
   , fB(0)
   , fA(1.1)
   , fVisible(true)
   , fInitialZAngle(0)
   {
   }
   
   void Display()
   {
      glPushMatrix();
      glTranslatef(fX, fY, fZ);
      glScalef(fW,fH, 1);
      glColor4f(fR, fG, fB, fA);
      Draw();
      glPopMatrix();
   }
   
   virtual void Draw() = 0;
   
   void SetY(float y)
   {
      fY=y;
   }
   
   void SetH(float h)
   {
      fH = h;
   }
   
   void SetZ(float z)
   {
      fZ = z;
   }
   
   void SetInitialZAngle(float phi)
   {
      fInitialZAngle = phi;
   }
   
   void SetColor(float r, float g, float b, float a)
   {
      fR = r; fG = g; fB = b; fA = a;
   }
   
   void SetVisible(bool visible)
   {
      fVisible = visible;
   }
   
   bool Visible() const { return fVisible; }
   
protected:
   float fX, fY, fW, fH, fZ, fR, fG, fB, fA;
   bool fVisible;
   float fInitialZAngle;
};



}

#endif

