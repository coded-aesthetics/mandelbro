#include "./mandelbrot.h"

mandelbrot::mandelbrot() {
}

mandelbrot::mandelbrot(int w, int h, double iX, double iY, int mIts)
{
	bitmapWidth = w;
	bitmapHeight = h;	
	   
	initX = iX;
	initY = iY;
	fac = 1.0;
	colorOffset = 0;

	initMaxIts = mIts;
	maxIts = mIts;
	srand( (unsigned) time(NULL) ) ; 
	randomizeColors();

	reset();

	bufferOld = new int[w*h];
	bufferNew = new int[w*h];

	optimizeZoom = true;
	smoothOut = true;
}

void mandelbrot::setViewportSize(double s) {
	viewportSize = s;
	viewportSizeY = s*(double)bitmapHeight/(double)bitmapWidth;
}

void mandelbrot::resize(int w, int h) {
	image.allocate(w, h, OF_IMAGE_COLOR);
	bitmapWidth = w;
	bitmapHeight = h;
	bufferOld = new int[w*h];
	bufferNew = new int[w*h];
	first = true;
	setViewportSize(viewportSize);
}

void mandelbrot::reset() {
	viewportX = -2.0;
	viewportY = -1.5;
	setViewportSize(3.0);
	
	viewportY = -(viewportSizeY / 2.0);

	maxIts = initMaxIts;
	julia = false;
	//randomizeColors();

	colorAmount = 62;

	initX = 0.0;//(double)(rand() % 100000) / 100000.0 * 3.0 - 1.5;
	initY = 0.0;//(double)(rand() % 100000) / 100000.0 * 3.0 - 1.5;
	first = true;
	bufferOld = new int[bitmapWidth*bitmapHeight];
	bufferNew = new int[bitmapWidth*bitmapHeight];
	if (optimizeZoom) {

	}
}


mandelbrot::~mandelbrot(void)
{
}

void mandelbrot::setImage(ofImage img, int x, int y) {
	image = img;
	xPos = x;
	yPos = y;
}

ofVec2f mandelbrot::oneMandel(double x, double y, double c1, double c2) {
	return ofVec2f(x*x - y*y + c1, 2*x*y + c2); 
}

double inline  sqrtX(double n)
{
	return std::sqrt(n);
} 

int mandelbrot::doTheMandelbrot(double x, double y, vector<ofPoint>* points, bool usePoints, bool gimmeColors) {
	//ofVec2f z = ofVec2f(initX, initY);
	double zX;
	double zY;
	
	if (usePoints && points != NULL) {
		points->clear();
		points->push_back(ofPoint(x, y));
	}

	if (julia) {
		zX = x;
		zY = y;
	} else {
		zX = initX;
		zY = initY;
	}
	double zX2 = zX * zX;
	double zY2 = zY * zY;

	double high = fac * 8.0 + 5.0;

	for (int i = 0; i < maxIts; i++) {
		double zXOld = zX;
		
		//(z^3 - 1 + c)
		
 	    //zX = zX*zX*zX - 3*zX*zY*zY;
		//zY = 1.0*(3*zXOld*zXOld*zY-zY*zY*zY);

		// k^5+5 i k^4 t-10 k^3 t^2-10 i k^2 t^3+5 k t^4+i t^5
		//zX = /*k^5*/zX*zX*zX*zX*zX /*-10 k^3 t^2*/- 10*zX*zX*zX*zY*zY /*+5 k t^4*/ + 5*zX*zY*zY*zY*zY ;
		//zY = 1.0*(/*+5 i k^4 t*/ 5*zXOld*zXOld*zXOld*zXOld*zY/*-10 i k^2 t^3*/-10*zXOld*zXOld*zY*zY*zY /*+i t^5*/ +  zY*zY*zY*zY*zY);
		/*
		double r = sqrtX(zX * zX + zY * zY);
		double alpha = atan(zY / zX);

		r = pow(r, high);

		zX = r*cos(high*alpha);
		zY = r*sin(high*alpha);
		*/

		//(z^2 + c)
		zY = 2.0 * zX * zY;
		zX = zX2 - zY2;
		

		if (julia) {
			zX += initX;
			zY += initY;
		} else {
			zX += x;
			zY += y;
		}
		zX2 = zX*zX;
		zY2 = zY*zY;
		//zX2 = zX * zX;
		//zY2 = zY * zY;
		//(z^4 + c)
		//zX = zX*zX*zX*zX - zY*zY+zY+zY - 6* zX*zX*zY*zY+ x;
		//zY = 4*zXOld*zXOld*zXOld * zY - 4*zXOld*zY*zY*zY + y;
		//k^4 + t^4 - 6 k^2 t^2        + 4 i k^3 t   -4 i k t^3
		
		if (usePoints && points != NULL) {
			points->push_back(ofPoint(zX, zY));
		}
		
		if (zX2 + zY2 > 4.0) {
			if (gimmeColors) {
				double di=(double )i;
                double zn;
                double hue;

                zn = sqrtX(zX2 + zY2);
                hue = di + 1.0 - log(log(abs(zn))) / log(2.0);  // 2 is escape radius
                hue = 0.95 + 10.0 * hue; // adjust to make it prettier
                // the hsv function expects values from 0 to 360
                while (hue > 360.0){

                    hue -= 360.0;
				}
                while (hue < 0.0){
                    hue += 360.0;
				}

				ofColor c = ofColor::fromHsb(hue, 180.0, 180.0);
				return c.getHex();
				return (int) floor(colorDelta * (double) ((i+colorOffset)%colorAmount));
			} else {
				return i;
			}
		}
	}
	if (gimmeColors) {
		return 0x000000;
	} else {
		return -1;
	}
}