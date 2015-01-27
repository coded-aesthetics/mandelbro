#pragma once

#include "ofMain.h"
#include "mandelbrot.h"
#include <thread>
#include "ofxBlur.h"



class testApp : public ofBaseApp
{
	mandelbrot m;
	mandelbrot m2;
	mandelbrot m3;
	mandelbrot m4;
public:
	testApp(int windowW, int windowH, int w, int h, double iX, double iY, int mIts):m(w/2,h/2,iX,iY,mIts),m2(w/2,h/2,iX,iY,mIts),m3(w/2,h/2,iX,iY,mIts),m4(w/2,h/2,iX,iY,mIts){
		image = ofImage();
		image.allocate(w/2, h/2, OF_IMAGE_COLOR);
		image2 = ofImage();
		image2.allocate(w/2, h/2, OF_IMAGE_COLOR);
		image3 = ofImage();
		image3.allocate(w/2, h/2, OF_IMAGE_COLOR);
		image4 = ofImage();
		image4.allocate(w/2, h/2, OF_IMAGE_COLOR);

		wW = windowW;
		wH = windowH;
	}
    void setup();
    void update();
    void draw();
	void reset();
    void resize(int w, int h, bool moveViewport);
	void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

	void recalcOffset();

	void moveViewport(int x, int y, double factor);
	void zoom(double factor);

	void collectTracePoints();

	ofImage image;
	ofImage image2;
	ofImage image3;
	ofImage image4;
	std::thread thread;

	int offsetX;
	int offsetY;

	std::queue <double> fpsMedian;

	int width;
	int height;

	int dirX;
	int dirY;

	int wH;
	int wW;

	int mouseX;
	int mouseY;

	double zoomFactor;

	enum Modes
	{
		ZOOM, MORPH, SNAPSHOT
	} mode;

	bool zoomAuto;
	bool morph;
	bool movingColors;
	bool fitToWindow;
	bool stereo;
	bool julia;
	bool trace;
	bool smoothOut;
	bool quizzical;
	double quizDirX;
	double quizDirY;
	double quizX;
	double quizY;

	int tickCtr;
	int itsMultiplier;
	double timeTaken;
	double timeTaken2;

	clock_t lastTickTime;

	vector<ofPoint> tracePoints;

	int ctr;
    float 	counter;
    bool	bSmooth;
};

