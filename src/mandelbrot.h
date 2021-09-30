#pragma once
#include "ofMain.h"
#include "ofImage.h"

class mandelbrot : public ofThread
{
public:
	mandelbrot(void);
	mandelbrot(int w, int h, double iX, double iY, int mIts);
	~mandelbrot(void); 

	ofVec2f oneMandel(double x, double y, double c1, double c2);

	int doTheMandelbrot(double x, double y, vector<ofPoint>* points, bool usePoints, bool gimmeColors);

	void bakeMandelbrot(ofImage* image);

	void reset();
	void resize(int w, int h);

	void setImage(ofImage img, int x, int y);

	void randomizeColors() {
		colorAmount = 45 + rand() % (240 - 45);
	}

	void threadedFunction() {
		double vXMin = viewportX;
		double vXMax = viewportX + viewportSize;
		double vYMin = viewportY;
		double vYMax = viewportY + viewportSizeY;

		double dx = (vXMax - vXMin) / (double) bitmapWidth;
		double dy = (vYMax - vYMin) / (double) bitmapHeight;

		bool gimmeColors = false;

		colorDelta = (double) 0xFFFFFF / (double) colorAmount;
		float colDelta2 = 270.0f / (float) colorAmount;
		int goodOnes = 0;
		double timeCalc = 0.0;
		for (int y = 0; y < bitmapHeight; y++) {
			for (int x = 0; x < bitmapWidth; x++) {
			
				//using namespace std;
				//clock_t begin = clock();
			
				//int its = doTheMandelbrot(c1, c2);
				//clock_t end = clock();
				//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
				//timeCalc += elapsed_secs;
				//cout << its << "\t";
				bool cache = false;
				int its;
				if (!first && optimizeZoom) {
					// calculate, where the point has been in the old picture (bufferOld);
					double xOld = (1.0 / zoomFactor) * ((double)x-(double)offsetX) + dirX;
					double yOld = (1.0 / zoomFactor) * ((double)y-(double)offsetY) + dirY;
					// does it lie within the image's bounds?
					if (xOld >= 0.0 && yOld >= 0.0 && xOld <= (double)(bitmapWidth-1.0) && yOld <= (double)(bitmapHeight-1.0)) {
						int xOld1 = (int)floor(xOld);
						int xOld2 = (int)ceil(xOld);
						int yOld1 = (int)floor(yOld);
						int yOld2 = (int)ceil(yOld);

						//cout << "--> " << xOld1 << " " << xOld2 << " " << yOld1 << " " << yOld2 << "\n";
						/**
						   ___________________
						  |                   |
						  | itsOld1 | itsOld4 |
						  |-------------------|
						  | itsOld2 | itsOld3 |  the new point lies between these four pixels
						  |___________________|  from the old buffer
						**/
						int itsOld1 = bufferOld[xOld1 + yOld1 * bitmapWidth];
						int itsOld2 = bufferOld[xOld1 + yOld2 * bitmapWidth];				
						int itsOld3 = bufferOld[xOld2 + yOld2 * bitmapWidth];				
						int itsOld4 = bufferOld[xOld2 + yOld1 * bitmapWidth];				
				
						//cout << "--> " << itsOld1 << " " << itsOld2 << " " << itsOld3 << " " << itsOld4 << "\n";
					
						if (itsOld1 == itsOld2 && itsOld2 == itsOld3 && itsOld3 == itsOld4) {
							// this means, that all four points out of the old calculation, the new point lies between,
							// had the same number of iteration, so we assume this point must have this amount of iterations also
							its = itsOld1;
							cache = true;
						} else { // if (itsOld1 == itsOld2 && itsOld2 == itsOld3 && itsOld3 == itsOld4)
							cache = false;
						}
					} else { // if (xOld >= 0.0 && yOld >= 0.0 && xOld <= (double)(bitmapWidth-1.0) && yOld <= (double)(bitmapHeight-1.0))
						cache = false;
					}
				} else { // if (!first && optimizeZoom)
					cache = false;
				}

				if (!cache) { // if it could not be drawn from cache, recalculate
					double c1 = (dx * (double) x) + vXMin;
					double c2 = (dy * (double) y) + vYMin;
					its = doTheMandelbrot(c1, c2, NULL, false, gimmeColors);
				}

				if (!first) {
					bufferNew[x + y * bitmapWidth] = its;
				} else {
					bufferNew[x + y * bitmapWidth] = bufferOld[x + y * bitmapWidth] = its;
				}
				//cout << bufferNew[x + y * bitmapWidth] << " " << bufferOld[x + y * bitmapWidth] << " ";
				//clock_t end = clock();
				//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
				//timeCalc += elapsed_secs;
				ofColor col;
				if (true) {//!cache) { // for debugging: draw all points drawn from cache in red
					if (!gimmeColors) {
						if (its == -1) {
							col = ofColor::fromHex(0x000000);
						} else {				
							int colHex = (int) floor(colorDelta * (double) ((its+colorOffset)%colorAmount));
							col = ofColor::fromHex(colHex);
						
							if (its >= maxIts - 30 && smoothOut) {
								int colHex = (int) floor(colorDelta * (double) ((maxIts - 30 + colorOffset)%colorAmount));
								int distance = maxIts - its;
								col = ofColor::fromHex(colHex);
								col.setBrightness(col.getBrightness() / ((float)(31-distance)/10.0f));
							}
						
							//double ratio = ((double)its - 0.0) / ((double)210 - 0.0);
							//colHex = 0xFFFFFF * pow(ratio, -1.0);
						
						
							//col = ofColor::fromHsb(colDelta2*(float)((its+abs(colorOffset))%colorAmount) + 45.0f, 140.0f, 120.0f);
						}
					} else {
						col = ofColor::fromHex(its);
					}
				} else {
					col = ofColor::fromHex(0xFF0000);
				}
				image.setColor(x, y, col);
			}
		}

		// swap buffers...
		int* bO = bufferOld;
		bufferOld = bufferNew;
		bufferNew = bO;
		first = false;	

		image.update();
	}

	void setViewportSize(double s);

	double offsetX;
	double offsetY;
	double dirX;
	double dirY;
	double zoomFactor;

	int colorOffset;

	double initX;
	double initY;

	int colorAmount;

	int bitmapWidth;
	int bitmapHeight;

	double viewportSize;
	double viewportSizeY;
	double viewportX;
	double viewportY;

	int maxIts;

	int* bufferOld;
	int* bufferNew;

	ofImage image;
	int xPos;
	int yPos;

	double colorDelta;
	double fac;

	bool smoothOut;
	bool first;
	bool julia;
	bool optimizeZoom;

	int initMaxIts;
};
