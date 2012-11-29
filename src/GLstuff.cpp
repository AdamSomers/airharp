#include "GLStuff.h"
#include "Harp.h"

#include <cmath>

#include "Leap.h"

#include <iostream>

#define _USE_MATH_DEFINES // To get definition of M_PI
#include <math.h>

class SampleListener : public Leap::Listener {
public:
    virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    float xPos;
    float yPos;
};

void SampleListener::onInit(const Leap::Controller& controller) {
    std::cout << "Initialized" << std::endl;
    xPos = yPos = 0;
}

void SampleListener::onConnect(const Leap::Controller& controller) {
    std::cout << "Connected" << std::endl;
}

void SampleListener::onDisconnect(const Leap::Controller& controller) {
    std::cout << "Disconnected" << std::endl;
}

void SampleListener::onFrame(const Leap::Controller& controller) {
    // Get the most recent frame and report some basic information
    const Leap::Frame frame = controller.frame();
    const std::vector<Leap::Hand>& hands = frame.hands();
    const size_t numHands = hands.size();
//    std::cout << "Frame id: " << frame.id()
//    << ", timestamp: " << frame.timestamp()
//    << ", hands: " << numHands << std::endl;
    
    if (numHands >= 1) {
        // Get the first hand
        const Leap::Hand& hand = hands[0];
        
        // Check if the hand has any fingers
        const std::vector<Leap::Finger>& fingers = hand.fingers();
        const size_t numFingers = fingers.size();
        if (numFingers >= 1) {
            // Calculate the hand's average finger tip position
            Leap::Vector pos(0, 0, 0);
            for (size_t i = 0; i < numFingers; ++i) {
                const Leap::Finger& finger = fingers[i];
                const Leap::Ray& tip = finger.tip();
                pos.x += tip.position.x;
                pos.y += tip.position.y;
                pos.z += tip.position.z;
            }
            pos = Leap::Vector(pos.x/numFingers, pos.y/numFingers, pos.z/numFingers);
            xPos = pos.x;
            yPos = 500 - pos.y;
            std::cout << "x: " << xPos << " y: " << yPos << std::endl;
            GLStuff::motionFunc((xPos / 150.f + 0.5) * GLStuff::gWidth, yPos);
            
//            std::cout << "Hand has " << numFingers << " fingers with average tip position"
//            << " (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
        }
        
        // Check if the hand has a palm
        const Leap::Ray* palmRay = hand.palm();
        if (palmRay != NULL) {
            // Get the palm position and wrist direction
            const Leap::Vector palm = palmRay->position;
            const Leap::Vector wrist = palmRay->direction;
//            std::cout << "Palm position: ("
//            << palm.x << ", " << palm.y << ", " << palm.z << ")" << std::endl;
            
            // Check if the hand has a normal vector
            const Leap::Vector* normal = hand.normal();
            if (normal != NULL) {
                // Calculate the hand's pitch, roll, and yaw angles
                double pitchAngle = -atan2(normal->z, normal->y) * 180/M_PI + 180;
                double rollAngle = -atan2(normal->x, normal->y) * 180/M_PI + 180;
                double yawAngle = atan2(wrist.z, wrist.x) * 180/M_PI - 90;
                // Ensure the angles are between -180 and +180 degrees
                if (pitchAngle > 180) pitchAngle -= 360;
                if (rollAngle > 180) rollAngle -= 360;
                if (yawAngle > 180) yawAngle -= 360;
//                std::cout << "Pitch: " << pitchAngle << " degrees,  "
//                << "roll: " << rollAngle << " degrees,  "
//                << "yaw: " << yawAngle << " degrees" << std::endl;
            }
        }
        
        // Check if the hand has a ball
        const Leap::Ball* ball = hand.ball();
        if (ball != NULL) {
//            std::cout << "Hand curvature radius: " << ball->radius << " mm" << std::endl;
        }
    }
}

SampleListener listener;

namespace GLStuff
{
    
    //const int notes[] = { 32, 34, 37, 39, 41, 44, 46, 49, 51, 53 };
    const int intervals[] = { 0, 2, 5, 7, 9};
    //const int intervals[] = { 0, 2, 4, 5, 7, 9, 11};
    const int numIntervals = 5;
    
   std::deque<GLDisplay*> gDisplays;

//-----------------------------------------------------------------------------
// name: rand2f()
// desc: generate a random float
//-----------------------------------------------------------------------------
GLfloat rand2f( float a, float b )
{
   return a + (b-a)*(rand() / (GLfloat)RAND_MAX);
}

void UpdateControls()
{
}


void GLStartup(int argc, const char** argv)
{
    Leap::Controller controller(&listener);
    
	std::cout << "Hit q to quit.\n";
 
   g_X = 0.0; g_Y = 0.0; g_ZOOM = 1500; //initial states for the gl_view
   // width and height of the window
   gWidth = 800;
   gHeight = 600;
   gLastWidth = gWidth;
   gLastHeight = gHeight;
   // light 0 position
   g_light0_pos[0] = 2.0f;
   g_light0_pos[1] =  8.2f;
   g_light0_pos[2] = 4.0f;
   g_light0_pos[3] = 1.0f;
   
   gDisplay = true;
   gFullscreen = false;
   gBufferSize = 1024;
   gFrameRate = 40; // frames/sec
   gRefreshRate = 1000 / gFrameRate; // ms
   gMenuItem = 0;
   gInc_val_mouse = 360 / gFrameRate;
   gLeftButtondown = false;
   gMouseOriginX = 0;
   gMouseOriginY = 0;
   // experimental
   gShowHUD = true;
   
   std::deque<GLDisplay*> gDisplays;
   
#ifdef __OS_MACOSX__
   //save working dir
   char * cwd = getcwd( NULL, 0 );
#endif
   // initialize GLUT
   glutInit( &argc, (char**)argv );
   
#ifdef __OS_MACOSX__
   //restore working dir
   chdir( cwd );
   free( cwd );
#endif
   
   glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
   glutInitWindowSize( gWidth, gHeight );
   glutInitWindowPosition( 100, 100 );
   glutCreateWindow( "AirHarp" );
   //glutIdleFunc( idleFunc );
   glutDisplayFunc( displayFunc );
   glutReshapeFunc( reshapeFunc );
   glutKeyboardFunc( keyboardFunc );
   glutMouseFunc( mouseFunc );
   glutPassiveMotionFunc(motionFunc);
   glutMotionFunc(motionFunc);
   glutTimerFunc(gRefreshRate,timerCallback,0);
   createMenu();
   if( gFullscreen )
      glutFullScreen();
   
   // do our own initialization
   initialize_graphics( );
   
   glutMainLoop();
   
}

void initialize_graphics()
{
   // set the GL clear color - use when the color buffer is cleared
   glClearColor( 0, 0, 0, 1.0f );
   // set the shading model to 'smooth'
   glShadeModel( GL_SMOOTH );
   // enable depth
   //glEnable( GL_DEPTH_TEST );
   // set the front faces of polygons
   glFrontFace( GL_CCW );
   // draw in wireframe
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	
	glEnable( GL_LINE_SMOOTH );
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	glEnable(GL_BLEND);              //activate blending mode
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(4.0);
	
	
	// enable lighting
   glEnable( GL_LIGHTING );
   // enable lighting for front
   glLightModeli( GL_FRONT_AND_BACK, GL_TRUE );
   // material have diffuse and ambient lighting 
   glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
   // enable color
   glEnable( GL_COLOR_MATERIAL );
	
	// enable light 0
   glEnable( GL_LIGHT0 );    
   // set the position of the lights
   glLightfv( GL_LIGHT0, GL_POSITION, g_light0_pos );
   
}


//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( int w, int h )
{
   // save the new window size
   gWidth = w; gHeight = h;
   // map the view port to the client area
   glViewport( 0, 0, w, h );
   // set the matrix mode to project
   glMatrixMode( GL_PROJECTION );
   // load the identity matrix
   glLoadIdentity( );
   // create the viewing frustum
   gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0, 1550.0 );
   // set the matrix mode to modelview
   glMatrixMode( GL_MODELVIEW );
   // load the identity matrix
   glLoadIdentity( );
   // position the view point
   gluLookAt( g_X, g_Y, g_ZOOM, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}

void go2d() {
   
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0, gWidth, 0.0, gHeight);
   
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   
   glDisable(GL_TEXTURE);
   glDisable(GL_TEXTURE_2D);   
   
}

void go3d() {
   glEnable(GL_TEXTURE);
   glEnable(GL_TEXTURE_2D);
   
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   
   reshapeFunc(gWidth, gHeight);
}

//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
   switch( key )
   {
      case 'f':
		{
			if( !gFullscreen )
			{
				gLastWidth = gWidth;
				gLastHeight = gHeight;
				glutFullScreen();
			}
			else
				glutReshapeWindow( gLastWidth, gLastHeight );
			
			gFullscreen = !gFullscreen;
		}
         break;
      case 'h':
         gShowHUD = !gShowHUD;
         break;
      case 'w':
         gFrameRate++;
         gRefreshRate = 1000 / gFrameRate;
         std::cout << "refresh rate: " << gFrameRate << std::endl;
         break;
         
      case 's':
         if (gFrameRate > 1)
         {
            gFrameRate--;
            gRefreshRate = 1000 / gFrameRate;
         }
         std::cout << "refresh rate: " << gFrameRate << std::endl;
         break;
         
      case 'e':
         break;
         
      case 'd':
         break;
      case '1':
      {
      }
         break;
      case '2':
      {
      }
         break;
		case '3':
      {
      }
			break;
      case '4':
      {
      }
			break;
      case 'r':
         break;
      case 'c':
         break;
         
		case 'q':
		{
			exit(0);
		}
			break;
		default:
			break;
   }
	
   // do a reshape since gEyeY might have changed
   reshapeFunc( gWidth, gHeight );
   glutPostRedisplay( );
}

//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
    gMouseOriginX = x;
    gMouseOriginY = y;

   if( button == GLUT_LEFT_BUTTON )
   {
      // rotate
      if( state == GLUT_DOWN )
      {
         gLeftButtondown = true;
      }
      else if (state == GLUT_UP)
      {
         gLeftButtondown = false;
      }
      // gInc -= gInc_val_mouse;
      else {
         //gInc += gInc_val_mouse;
         
         
      }
   }
   glutPostRedisplay( );
}

void motionFunc(int x, int y )
{   
   //if (gLeftButtondown)
   {
       int numStrings = Harp::GetInstance()->GetNumStrings();
       int columnWidth = gWidth / numStrings;
       for (int i = 0; i < numStrings; ++i)
       {
           int threshold = (columnWidth * i) + (columnWidth / 2);
           if ((gMouseOriginX <= threshold && x > threshold) ||
               (gMouseOriginX > threshold && x <= threshold))
           {
               printf("trig\n");
               int idx = i % numIntervals;
               int mult = (i / (float)numIntervals);
               int base = 32 + 12*mult;
               int note = base + intervals[idx];
               //Harp::GetInstance()->NoteOn(i, note, 30);
               int bufferSize = 512;
               float buffer[bufferSize];
               memset(buffer, 0, bufferSize);
               int midpoint = (gMouseOriginY / (float)gHeight) * bufferSize;
               for (int x = 0; x < bufferSize; ++x)
               {
                   if (x < midpoint)
                       buffer[x] = x / (float)midpoint;
                   else
                       buffer[x] = 1.f - (x - midpoint) / (float)(bufferSize - midpoint);
               }
               Harp::GetInstance()->ExciteString(i, note, 127, buffer, bufferSize);
           }
       }
   }
   
   gMouseOriginX = x;
   gMouseOriginY = y;
}

void displayFunc( )
{
   static GLfloat x = 0.0f;
    
   // clear the color and depth buffers
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    go2d();
    
    glColor4f(1,1,1,1);

    glLineWidth(2);
    
    int numStrings = Harp::GetInstance()->GetNumStrings();
    int columnWidth = gWidth / numStrings;
    for (int i = 0; i < numStrings; ++i)
    {
        SampleAccumulator::PeakBuffer peakBuffer = Harp::GetInstance()->GetBuffers().at(i).Get();
        int numSegments = peakBuffer.size();
        float segmentLength = gHeight / (float)numSegments;
        glBegin(GL_LINE_LOOP);//start drawing a line loop
        for (int j = 0; j < numSegments; j++)
        {
            SampleAccumulator::PeakSample samp = peakBuffer.at(j);
            float val = fabsf(samp.first) > fabsf(samp.second) ? samp.first : samp.second;
            int x = (columnWidth * i) + (columnWidth / 2) + val * 1 * columnWidth;
            glVertex2i(x,segmentLength*j);//left of window
        }
        int x = (columnWidth * i) + (columnWidth / 2);
        //glVertex2i(x,gHeight);
        glEnd();//end drawing of line loop
    }
	glPushMatrix();
	glTranslatef(gMouseOriginX,gHeight - gMouseOriginY,100);
    glutSolidSphere(100, 100, 100);
    
	glPopMatrix();
       
   //UpdateControls();
   
   if(gShowHUD)
   {      
      glColor4f(1,1,1,1);
      std::string s = "";      
      glLineWidth(2);
      s = "AirHarp 0.0.1";
      DrawString(gWidth - 500, gHeight-20, s, GLUT_BITMAP_TIMES_ROMAN_10, 0.15);
   }
   
   glFlush( );
   glutSwapBuffers( );
}

void timerCallback(int v)
{
   //gAngleY += gInc;
   glutPostRedisplay();
   glutTimerFunc(gRefreshRate,timerCallback,0);
}

void createMenu(void){
   //////////
   // MENU //
   //////////
   
   // Create a submenu, this has to be done first.
   int submenid = glutCreateMenu(menu);
   
   // Add sub menu entry
   glutAddMenuEntry("Teapot", 2);
   glutAddMenuEntry("Cube", 3);
   glutAddMenuEntry("Torus", 4);
   
   // Create the menu, this menu becomes the current menu
   int menid = glutCreateMenu(menu);
   
   // Create an entry
   glutAddMenuEntry("Clear", 1);
   
   glutAddSubMenu("Draw", submenid);
   // Create an entry
   glutAddMenuEntry("Quit", 0);
   
   // Let the menu respond on the right mouse button
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   
   
}


void menu(int value){
   if(value == 0){
      exit(0);
   }else{
      gMenuItem = value;
   }
   
   // you would want to redraw now
   glutPostRedisplay();
}

//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
   // render the scene
   //glutPostRedisplay( );
}

void DrawString(int x, int y, std::string& s, void* font, float scaleFactor/* = 0.2f*/)
{
   go2d();
   
   glRasterPos3f(10, 10, 0);
   
   glTranslatef(x,y,0); 
   glScalef(scaleFactor,scaleFactor,1);
   
   glColor4f(1.f,0.f,1.f,1.f);
   for (std::string::iterator i = s.begin(); i != s.end(); ++i)
   {
      glutStrokeCharacter( GLUT_STROKE_ROMAN, *i );
      //glutBitmapCharacter(font, *i);
   }
   
   go3d();
}

}

