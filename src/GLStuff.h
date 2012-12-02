#ifndef h_GlStuff
#define h_GlStuff

// OpenGL
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <cmath>
#include <deque>
#include <map>
#include <iostream>
#include <mutex>

#define __PI    3.1415926

#define ABS(x)  (float) (x < 0 ? -x : x)
#define COS(x)  (float) cos( (double) (x) * __PI / 180.0 )
#define SIN(x)  (float) sin( (double) (x) * __PI / 180.0 )


class Finger;

namespace GLStuff
{
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
        , fZ(0.f)
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
        
        void SetX(float x)
        {
            fX=x;
        }
        
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
        
        //protected:
        float fX, fY, fW, fH, fZ, fR, fG, fB, fA;
        bool fVisible;
        float fInitialZAngle;
    };
    
    class Finger : public GLDisplay
    {
    public:
        Finger()
        : GLDisplay(0,0,1,1)
        , invalid(false)
        , scaleFactor(.5)
        {
        }
        
        void Draw()
        {
            if (gWidth > gHeight)
                glScalef(1, gWidth / (float)gHeight, 1);
            else
                glScalef(gHeight / (float)gWidth, 1 , 1);
            float scale = fmin(scaleFactor, 1.f);
            if (scale == 1.f)
            {
                glColor4f(1, 0, 0, .8);
            }
            else
            {
                glColor4f(1, 1, 1, .25);
            }
            glutSolidSphere(.025, 50, 50);

            glColor4f(.7, .7, .7, scale);
            float clippedScale = fmin(.85, scale);
            glScalef(clippedScale, clippedScale, 1);
            glutSolidSphere(.025, 50, 50);         
        }
        
        bool invalid;
        float scaleFactor;
    };
    
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
    void airMotion(float x, float y, float z, float prevX, float prevY);
    
    void go2d();
    void go3d();
    
    static std::map<int, Finger> gFingers;
    static std::mutex gLock;    
}

#endif

