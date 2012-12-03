#include "GLStuff.h"
#include "Harp.h"

#include <cmath>

#include "Leap.h"

#include <iostream>

#define _USE_MATH_DEFINES // To get definition of M_PI
#include <math.h>

class HarpListener : public Leap::Listener {
public:
    virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    float xPos;
    float yPos;
};

void HarpListener::onInit(const Leap::Controller& controller) {
    std::cout << "Initialized" << std::endl;
    xPos = yPos = 0;
}

void HarpListener::onConnect(const Leap::Controller& controller) {
    std::cout << "Connected" << std::endl;
}

void HarpListener::onDisconnect(const Leap::Controller& controller) {
    std::cout << "Disconnected" << std::endl;
}

void HarpListener::onFrame(const Leap::Controller& controller) {
    
    GLStuff::gLock.lock();
    for (std::map<int,GLStuff::Finger>::iterator i = GLStuff::gFingers.begin(); i != GLStuff::gFingers.end(); ++i)
    {
        (*i).second.invalid = true;
    }
    GLStuff::gLock.unlock();
    // Get the most recent frame and report some basic information
    const Leap::Frame frame = controller.frame();
    const std::vector<Leap::Hand>& hands = frame.hands();
    const size_t numHands = hands.size();
    //    std::cout << "Frame id: " << frame.id()
    //    << ", timestamp: " << frame.timestamp()
    //    << ", hands: " << numHands << std::endl;
    
    if (numHands >= 1) {
        // Get the first hand
        for (size_t h = 0; h < numHands; ++h) {
            const Leap::Hand& hand = hands[h];
            
            
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
                    
                    std::map<int, GLStuff::Finger>::iterator iter = GLStuff::gFingers.find(finger.id());
                    if (iter == GLStuff::gFingers.end())
                    {
                        GLStuff::gLock.lock();
                        iter = GLStuff::gFingers.insert(std::make_pair(finger.id(), GLStuff::Finger())).first;
                        GLStuff::gLock.unlock();
                        std::cout << "inserted finger: " << finger.id() << std::endl;
                    }
                    else
                        (*iter).second.invalid = false;
                    
                    GLStuff::Finger* f = &(*iter).second;
                    float prevX = f->fX;
                    float prevY = f->fY;
                    float newX = ((tip.position.x + 200) / 400.f);// * GLStuff::gWidth;
                    float newY = ((tip.position.y - 150) / 200.f);// * GLStuff::gHeight;
                    f->SetX(newX);
                    f->SetY(newY);
                    f->scaleFactor = 1 - (tip.position.z / 300.f);
                    GLStuff::airMotion(newX, newY, tip.position.z, prevX , prevY);
                    
                }
                pos = Leap::Vector(pos.x/numFingers, pos.y/numFingers, pos.z/numFingers);
                xPos = pos.x;
                yPos = (1 - ((pos.y - 50) / 200.f)) * GLStuff::gHeight;
                //std::cout << "x: " << xPos << " y: " << yPos << std::endl;
                
                
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
    GLStuff::gLock.lock();
    std::map<int,GLStuff::Finger>::iterator i = GLStuff::gFingers.begin();
    while (i != GLStuff::gFingers.end())
    {
        if ((*i).second.invalid)
            GLStuff::gFingers.erase(i++);
        else
            i++;
    }
    GLStuff::gLock.unlock();
}

HarpListener listener;

namespace GLStuff
{
    // Pentatonic Major
    const int gPentatonicMajor[] = { 0, 2, 5, 7, 9};
    const int gPentatonicMajorIntervals = 5;
    
    // Pentatonic Minor
    const int gPentatonicMinor[] = { 0, 3, 5, 7, 10};
    const int gPentatonicMinorIntervals = 5;
    
    // Whole-tone
    const int gWholeTone[] = { 0, 2, 4, 6, 8, 10};
    const int gWholeToneIntervals = 6;
    
    // Diatonic
    const int gDiatonic[] = { 0, 2, 4, 5, 7, 9, 11};
    const int gDiatonicIntervals = 7;
    
    const int* gScale = gPentatonicMajor;
    int gScaleIntervals = gPentatonicMajorIntervals;
    
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
        g_light0_pos[0] = .3f;
        g_light0_pos[1] =  .24f;
        g_light0_pos[2] = -1.0f;
        g_light0_pos[3] = 2.0f;
        
        gDisplay = true;
        gFullscreen = false;
        gBufferSize = 1024;
        gFrameRate = 60; // frames/sec
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
        //// load the identity matrix
        glLoadIdentity( );
        // create the viewing frustum
        //gluPerspective( 45.f, (GLfloat) w / (GLfloat) h, 1.0, 1 );
        // set the matrix mode to modelview
//        float aspectRatio = (GLfloat)w / (GLfloat)h;
//        if (w <= h)
//            glOrtho (-1, 1.0, -1/aspectRatio, 1/aspectRatio, 1.0, -1.0);
//        else
//            glOrtho (-1 * aspectRatio, 1 * aspectRatio, -1, 1, 1.0, -1.0);
        glOrtho (0, 1.0, 0, 1, 1.0, -1.0);
        glMatrixMode( GL_MODELVIEW );
        // load the identity matrix
        glLoadIdentity( );
        // position the view point
        //gluLookAt( 0, 0, 0, .5f, .5f, -.5f, 0.0f, 1.0f, 0.0f );
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
            case 'a':
                Harp::GetInstance()->AddString();
                break;
            case 'z':
                Harp::GetInstance()->RemoveString();
                break;
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
                gScale = gPentatonicMajor;
                gScaleIntervals = gPentatonicMajorIntervals;
            }
                break;
            case '2':
            {
                gScale = gPentatonicMinor;
                gScaleIntervals = gPentatonicMinorIntervals;
            }
                break;
            case '3':
            {
                gScale = gDiatonic;
                gScaleIntervals = gDiatonicIntervals;
            }
                break;
            case '4':
            {
                gScale = gWholeTone;
                gScaleIntervals = gWholeToneIntervals;
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
    
    void airMotion(float x, float y, float z, float prevX, float PrevY)
    {
        if (z < 0)
        {
            int numStrings = Harp::GetInstance()->GetNumStrings();
            float columnWidth = 1.f / numStrings;
            for (int i = 0; i < numStrings; ++i)
            {
                float threshold = (columnWidth * i) + (columnWidth / 2);
                if (((prevX <= threshold && x > threshold) ||
                     (prevX > threshold && x <= threshold)) &&
                    fabsf(x - threshold) < columnWidth / 2)
                {
                    printf("trig\n");
                    int idx = i % gScaleIntervals;
                    int mult = (i / (float)gScaleIntervals);
                    int base = 32 + 12*mult;
                    int note = base + gScale[idx];
                    //Harp::GetInstance()->NoteOn(i, note, 30);
                    int bufferSize = 512;
                    float buffer[bufferSize];
                    memset(buffer, 0, bufferSize);
                    int midpoint = y * bufferSize;
                    //std::cout << "gHeight: " << gHeight << " mid: " << y << "\n";
                    for (int x = 0; x < bufferSize; ++x)
                    {
                        if (x < midpoint)
                            buffer[x] = x / (float)midpoint;
                        else
                            buffer[x] = 1.f - (x - midpoint) / (float)(bufferSize - midpoint);
                        
                        if (prevX > threshold)
                            buffer[x] = -buffer[x];
                    }
                    Harp::GetInstance()->ExciteString(i, note, 127, buffer, bufferSize);
                }
            }
        }
    }
    
    
    void motionFunc(int x, int y )
    {
        //if (gLeftButtondown)
        
        
        gMouseOriginX = x;
        gMouseOriginY = y;
    }
    
    void displayFunc( )
    {
        static GLfloat x = 0.0f;
        
        // clear the color and depth buffers
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glLineWidth(2);
        
        int numStrings = Harp::GetInstance()->GetNumStrings();
        float columnWidth = 1.f / numStrings;
        for (int i = 0; i < numStrings; ++i)
        {
            if (i % gScaleIntervals == 0)
                glColor4f(1,.5,.5,1);
            else
                glColor4f(.5,.5,.5,1);

            SampleAccumulator::PeakBuffer peakBuffer = Harp::GetInstance()->GetBuffers().at(i)->Get();
            int numSegments = peakBuffer.size();
            float segmentLength = 1 / (float)numSegments;
            glBegin(GL_LINE_LOOP);//start drawing a line loop
            for (int j = 0; j < numSegments; j++)
            {
                SampleAccumulator::PeakSample samp = peakBuffer.at(j);
                float val = fabsf(samp.first) > fabsf(samp.second) ? samp.first : samp.second;
                float cw = columnWidth;
                float x = (columnWidth * i) + (columnWidth / 2) + val * fmin(columnWidth, .1);
                
                glVertex2f(x,segmentLength*j);//left of window
            }
            int x = (columnWidth * i) + (columnWidth / 2);
            //glVertex2i(x,gHeight);
            glEnd();//end drawing of line loop
        }
        
        //glTranslatef(gMouseOriginX,gHeight - gMouseOriginY,100);
        //  glutSolidSphere(100, 100, 100);
        GLStuff::gLock.lock();
        for (std::map<int,Finger>::iterator i = gFingers.begin(); i != gFingers.end(); ++i)
        {
            (*i).second.Display();
         }
        GLStuff::gLock.unlock();
        
        
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
