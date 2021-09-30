#include "ofMain.h"
#include "testApp.h"


//========================================================================
int main( )
{

    ofSetupOpenGL(1000,650, OF_WINDOW);			// <-------- setup the GL context

    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp( new testApp(1000, 750, 700, 700, 0.0, 0.0, 60));

}