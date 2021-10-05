#include "testApp.h"
#include <ctime>

//--------------------------------------------------------------
void testApp::setup()
{
    counter = 0;
    ofSetCircleResolution(50);
    ofBackground(255,255,255);
    bSmooth = false;
    ofSetWindowTitle("Hezelgeht v0.2");
	
	tracePoints.clear();

    ofSetFrameRate(60); 

	quizzical = false;
	smoothOut = true;

	m.setImage(image, offsetX, offsetY);
	m2.setImage(image2, offsetX, offsetY);
	m3.setImage(image3, offsetX, offsetY);
	m4.setImage(image4, offsetX, offsetY);

	ctr = 0;

	dirX = m.bitmapWidth;
	dirY = m.bitmapHeight;
	
	mouseX = dirX;
	mouseY = dirY;
	
	width = m.bitmapWidth * 2;
	height = m.bitmapHeight * 2;

	recalcOffset();

	zoomFactor = 1.0;

	stereo = true;
	morph = false;
	zoomAuto = true;
	movingColors = false;
	fitToWindow = true;
	julia = false;
	trace = false;

	if (fitToWindow && !stereo) {
		windowResized(wW, wH);
	}

	reset();

	//blur.allocate(m.bitmapWidth, m.bitmapHeight);
	//blur.setPasses(50);
	timeTaken2 = 0;
	timeTaken = 0;
	tickCtr = 0;
	lastTickTime = clock();

	ofBackground(0,0,0);
	//ofSetHexColor(0x000000);

	mode = Modes::ZOOM;
	
}

//--------------------------------------------------------------
void testApp::update()
{
    counter = counter + 0.033f;
	m.fac = m2.fac = m3.fac= m4.fac = sin(counter);
	
	m.lock();
	image = m.image;
	m.unlock();
	m2.lock();
	image2 = m2.image;
	m2.unlock();
	m3.lock();
	image3 = m3.image;
	m3.unlock();
	m4.lock();
	image4 = m4.image;
	m4.unlock();
	
	

    //ofSetColor(ofNoise( ofGetFrameNum() ) * 255 * 5, 255);
	/*
	blur.getBackBuffer()->begin();
	ofSetColor(255);
	image.draw(0, 0);
	blur.getBackBuffer()->end();
	
	blur.setTexture(image.getTextureReference());

    blur.update();
	*/
	
}

//--------------------------------------------------------------
void testApp::draw()
{
	clock_t now = clock();
	double elapsed_millis = double(now - lastTickTime);
	timeTaken += elapsed_millis;
	timeTaken2 += elapsed_millis;
	lastTickTime = now;
	tickCtr++;
	if (timeTaken2 > 100.0) {
		timeTaken2 = 0;
		if (movingColors) {
			if (zoomFactor < 1.0) {
				m.colorOffset = m2.colorOffset = m3.colorOffset = ++m4.colorOffset;
			} else {
				m.colorOffset = m2.colorOffset = m3.colorOffset = --m4.colorOffset;
			}
		}

	}
	if (timeTaken > 1000.0) {
		//cout << tickCtr << "\n";
		//if (tickCtr < 15) {
		//	resize(width / 1.2, height / 1.2, true);
		//}
		//if (tickCtr > 25) {
		//	resize(width * 1.2, height * 1.2, true);
		//}
		
		timeTaken = 0.0;
		tickCtr = 0;
	}
	
	double zF = zoomFactor;
	if (zoomAuto) {
		if (!(zoomFactor == 1.0 && morph)) { // don't morph if in zoom mode, this is confusing
			/*
			run constantly at zoomFactor as if 24 fps
			*/
			// zF = pow(zoomFactor, elapsed_millis / 42.0); //this becomes unplayable
			zF = 1.0 + (elapsed_millis / 42.0 * (zoomFactor - 1.0));
			moveViewport(dirX, dirY, zF);
		}
		//cout << "m " << m.bufferOld << "m2 " << m2.bufferOld << "m3 " << m3.bufferOld << "m4 " << m4.bufferOld;
	}
	//cout << elapsed_millis << "\n";
	//gaussianblur.setTexture(image.getTextureReference());
	//blur.setTexture(image.getTextureReference());
	//blur.draw(offsetX, offsetY);
	if (!quizzical) {
		image.draw(offsetX, offsetY);
		image2.draw(offsetX + width/2, offsetY);
		image3.draw(offsetX, offsetY + height/2);
		image4.draw(offsetX+width/2, offsetY +height/2);
	}
	if (!m.isThreadRunning() && !m2.isThreadRunning() && !m3.isThreadRunning() && !m4.isThreadRunning()) {
		m.dirX = m2.dirX = m3.dirX = m4.dirX = (double)dirX;
		m.dirY = m2.dirY = m3.dirY = m4.dirY = (double)dirY;
		m2.dirX -= (double)m.bitmapWidth;
		m3.dirY -= (double)m.bitmapHeight;
		m4.dirX -= (double)m.bitmapWidth;
		m4.dirY -= (double)m.bitmapHeight;
		

		m.zoomFactor = m2.zoomFactor = m3.zoomFactor = m4.zoomFactor = zF;

		m.startThread();
		m2.startThread();
		m3.startThread();
		m4.startThread();

		m.waitForThread(true);
		m2.waitForThread(true);
		m3.waitForThread(true);
		m4.waitForThread(true);
	}
	
	if (quizzical) {
		quizX += elapsed_millis / 1000 * quizDirX;
		quizY += elapsed_millis / 1000 * quizDirY;
		collectTracePoints();
	}

	if (trace) {
		if (tracePoints.size() > 0) {
			double delta = 255.0 / (double)tracePoints.size();
			for (int i = 0; i < tracePoints.size() - 1; i++) {
				ofPoint cur = tracePoints.at(i);
				ofPoint curTo = tracePoints.at(i+1);
				double xFrom =offsetX+ ofMap(cur.x, m.viewportX, m.viewportX+2.0*m.viewportSize, 0, width);
				double xTo = offsetX+ofMap(curTo.x, m.viewportX, m.viewportX+2.0*m.viewportSize, 0, width);
				double yFrom = offsetY+ofMap(cur.y, m.viewportY, m.viewportY+2.0*m.viewportSizeY, 0, height);
				double yTo = offsetY+ofMap(curTo.y, m.viewportY, m.viewportY+2.0*m.viewportSizeY, 0, height);
				
				int a = (i+ctr++)%tracePoints.size();

				//ofSetColor(ofColor(delta * a));
				ofLine(xFrom, yFrom, xTo, yTo);
			}
		}
	}

	ofLine(wW / 2 - 5, wH / 2, wW/2 + 5, wH/2);
	ofLine(wW / 2 , wH / 2-5, wW/2, wH/2+5);

	for (int i = 0; i < 10; i++) {
		double pixX = (double)(rand() % m.bitmapWidth*2);
		double pixY = (double)(rand() % m.bitmapHeight*2);
		double x2 = zoomFactor * ((double)pixX-dirX) + (double)m.bitmapWidth;
		double y2 = zoomFactor * ((double)pixY-dirY) + (double)m.bitmapHeight;

		//ofLine(pixX+offsetX, pixY+offsetY, x2+offsetX, y2+offsetY);
	}

	ofSetColor(255);
	string fpsStr = "fps: " + ofToString(ofGetFrameRate(), 2);
	ofDrawBitmapString(fpsStr, 10, 10);

	string maxItsStr = "max. iterations: " + ofToString(m.maxIts);
	ofDrawBitmapString(maxItsStr, 100, 10);

	maxItsStr = "zoom: " + ofToString(1.5 / m.viewportSize);
	ofDrawBitmapString(maxItsStr, 300, 10);

	maxItsStr = "c1: " + ofToString(m.initX, 3) + ", c2: " + ofToString(m.initY, 3);
	ofDrawBitmapString(maxItsStr, 450, 10);

	maxItsStr =  "'r': reset";
	maxItsStr += "  'm': morphing ";
	maxItsStr += (morph) ? "off" : "on";
	maxItsStr += "  'z': auto-zoom ";
	maxItsStr += (zoomAuto) ? "off" : "on";
	maxItsStr += "  'l': moving colors ";
	maxItsStr += (movingColors) ? "off" : "on";
	maxItsStr += "  'l/r mouse': zoom in/out  '+/-': size";
	maxItsStr += "  'c': random colors";
	ofDrawBitmapString(maxItsStr, 10, wH - 10);
}


void testApp::reset() {
	m.reset();
	m2.reset();
	m3.reset();
	m4.reset();
	m.setViewportSize(m.viewportSize / 2.0);
	m2.setViewportSize(m2.viewportSize / 2.0);
	m3.setViewportSize(m3.viewportSize / 2.0);
	m4.setViewportSize(m4.viewportSize / 2.0);
	m2.viewportX += m2.viewportSize;
	m3.viewportY += m3.viewportSizeY;
	m4.viewportX += m4.viewportSize;
	m4.viewportY += m4.viewportSizeY;

	itsMultiplier = 2;

	m2.initX = m3.initX = m4.initX = m.initX;
	m2.initY = m3.initY = m4.initY = m.initY;
	m2.colorAmount = m3.colorAmount = m4.colorAmount = m.colorAmount;
			m.offsetX = m.bitmapWidth;
		m.offsetY = m.bitmapHeight;
		
		m2.offsetX = 0.0;
		m2.offsetY = m.bitmapHeight;
		m3.offsetX = m.bitmapWidth;
		m3.offsetY = 0.0;
		m4.offsetX = 0.0;
		m4.offsetY = 0.0;
}

void testApp::resize(int w, int h, bool doMoveViewport) {
	w = max(w, 50);
	h = max(h, 50);

	h = min(h, wH);
	w = min(w, wW);

	m.resize(w / 2, h / 2);
	m2.resize(w / 2, h / 2);
	m3.resize(w / 2, h / 2);
	m4.resize(w / 2, h / 2);
	image4.allocate(w/2, h/2, OF_IMAGE_COLOR);
			m.offsetX = m.bitmapWidth;
		m.offsetY = m.bitmapHeight;
		
		m2.offsetX = 0.0;
		m2.offsetY = m.bitmapHeight;
		m3.offsetX = m.bitmapWidth;
		m3.offsetY = 0.0;
		m4.offsetX = 0.0;
		m4.offsetY = 0.0;

	width = w;
	height = h;

	recalcOffset();
		
	if (doMoveViewport) {

		dirX = (m.bitmapWidth) + ((mouseX-offsetX) - m.bitmapWidth) / 10;
		dirY= (m.bitmapHeight)+((mouseY-offsetY) - m.bitmapHeight) / 10;
		
		moveViewport(dirX, dirY, 1.0);
	}
	//blur.allocate(m.bitmapWidth, m.bitmapHeight);
	//blur.setPasses(10);
}

void testApp::recalcOffset() {
	offsetX = (wW - width) / 2;
	offsetY = (wH - height) / 2;
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{
	if (key == 'r') {
		reset();
	}

	if (key == 't') {
		trace = !trace;
	}

	if (key == 'j') {
		julia = !julia;
		m.julia = m2.julia = m3.julia = m4.julia = julia;
	}

	if (false){//key == 'c') {
		m.randomizeColors();
		m2.randomizeColors();
		m3.randomizeColors();
		m4.randomizeColors();
	}

	cout << key << " ";

	if (key == 356) {
		itsMultiplier--;
		itsMultiplier %= 10;
		m.first = m2.first = m3.first = m4.first = true;
	}

	if (key == 358) {
		itsMultiplier++;
		itsMultiplier %= 10;
		m.first = m2.first = m3.first = m4.first = true;
	}

	if (key == 61) {
		cout << "+!!!";
		resize(width * 1.2, height * 1.2, true);
	}

	if (key == '-') {
		resize(width / 1.2, height / 1.2, true);
	}

	if (key == 'c') {
		m.randomizeColors();
		m2.colorAmount = m3.colorAmount = m4.colorAmount = m.colorAmount;
		cout << "randomColor: " << m.colorAmount << "\n";
	}

	if (key == 's') {
		smoothOut = !smoothOut;
		m.smoothOut = m2.smoothOut = m3.smoothOut = m4.smoothOut = smoothOut;
	}

	if (key == 'l') {
		movingColors = !movingColors;
	}

    if (key == 'm')
    {
        morph = !morph;
    }

	if (key == 'z')
    {
        zoomAuto = !zoomAuto;
    }

	if (key == 'q')
    {
        quizzical = !quizzical;
		quizDirX = 10.0;
		quizDirY = 10.0;
		quizX = 0.0;
		quizY = 0.0;
		if (quizzical) {
			morph = false;
			zoomAuto = false;
			trace = true;
		} else {
			trace = false;
			morph = true;
		}
    }
	
	m.optimizeZoom = m2.optimizeZoom = m3.optimizeZoom = m4.optimizeZoom = zoomAuto && !morph;
	
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{
	mouseX = x;
	mouseY = y;

	x -= offsetX;
	y -= offsetY;
	//blur.setPasses(ofMap(y, 0, ofGetHeight(),0, 10, true));
	dirX = (m.bitmapWidth) + (x - m.bitmapWidth) / 10;
	dirY = (m.bitmapHeight)+(y - m.bitmapHeight) / 10;

	
	double vX = m.viewportX;
	double vY = m.viewportY;
	double vS = m.viewportSize * 2.0;
	double vSY = m.viewportSizeY * 2.0;

	if (morph) {
		m.initX = vX + ((double) x / (double) (m.bitmapWidth * 2.0)) * vS;
		m.initY = vY + ((double) y / (double) (m.bitmapHeight * 2.0)) * vSY;
	}
	if (trace) {
		collectTracePoints();
	}
	

	m2.initX = m3.initX = m4.initX = m.initX;
	m2.initY = m3.initY = m4.initY = m.initY;
	//cout << "dirX: " << dirX << ", dirY:" << dirY;
	//moveViewport(xReduced, yReduced);
}

void testApp::collectTracePoints() {
		int x = mouseX - offsetX;
		int y = mouseY - offsetY;
		double xx = (double)x;
		double yy = (double)y;
		if (quizzical) {
			xx = quizX;
			yy = quizY;
		}
		double traceX = m.viewportX + (xx / (double) (m.bitmapWidth * 2)) * m.viewportSize * 2.0;
		double traceY = m.viewportY + (yy / (double) (m.bitmapHeight * 2)) * m.viewportSizeY * 2.0;

		int its = m.doTheMandelbrot(traceX, traceY, &tracePoints, true, false);
		if (its != -1 && quizzical) {
			double ab = quizDirX / abs(quizDirX);
			quizDirX = rand() % 100 - 50;
			ab = quizDirY / abs(quizDirY);
			quizDirY = rand() % 100 - 50;
		}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
}

void testApp::zoom(double factor) {
	if (!(factor < 1.0 && m.viewportSize > 2.5)) {
		m.setViewportSize(m.viewportSize / factor);
		m2.setViewportSize(m.viewportSize);
		m3.setViewportSize(m.viewportSize);
		m4.setViewportSize(m.viewportSize);
	} else {
		zoomFactor = 1.0;
	}
}

void testApp::moveViewport(int x, int y, double factor) {
	double vX = m.viewportX;
	double vY = m.viewportY;
	double vS = m.viewportSize * 2.0;
	double vSY = m.viewportSizeY * 2.0;

	m.viewportX = vX + ((double) x / (double) (m.bitmapWidth * 2.0)) * vS;
	m.viewportY = vY + ((double) y / (double) (m.bitmapHeight * 2.0)) * vSY;
	
	zoom(factor);

	m.maxIts = 60 - (int)(log10(m.viewportSize) * abs(itsMultiplier) * 10);

	m.viewportX -= m.viewportSize;
	m.viewportY -= m.viewportSizeY;

	m2.viewportX = m.viewportX + m.viewportSize;
	m2.viewportY = m.viewportY;
	
	m3.viewportX = m.viewportX;
	m3.viewportY = m.viewportY + m.viewportSizeY;

	m4.viewportX = m.viewportX + m.viewportSize;
	m4.viewportY = m.viewportY + m.viewportSizeY;

	m2.maxIts = m.maxIts;
	m3.maxIts = m.maxIts;
	m4.maxIts = m.maxIts;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	x -= offsetX;
	y -= offsetY;
	/*
	var vX = bigMandelSprite.viewportX;
	var vY = bigMandelSprite.viewportY;
	var vS = bigMandelSprite.viewportSize;
	bigMandelSprite.viewportX = vX + (e.localX / bitmapWidth) * vS;
	bigMandelSprite.viewportY = vY + (e.localY / bitmapHeight) * vS;
	bigMandelSprite.viewportSize /= 10;
			
	bigMandelSprite.viewportX -= (bigMandelSprite.viewportSize / 2);
	bigMandelSprite.viewportY -= (bigMandelSprite.viewportSize / 2);
			
	bigMandelSprite.maxIts += 9;
	*/

	if (button == 0) {
		if (!zoomAuto && !quizzical) {
			if (false) {
				moveViewport(dirX, dirY, 1.03);
			} else {
				moveViewport(x, y, 10.0);
			}
		} else if (!quizzical) {
			if (zoomFactor != 1.0) {
				zoomFactor = 1.0;
			} else {
				zoomFactor = 1.03;
			}
		 } else {
			quizX = x;
			quizY = y;
		}
	}

	if (button == 1) {
		zoomFactor = 1.0;
		zoomAuto = false;
	}

	if (button == 2) {
		if (!zoomAuto) {
			if (false) {
				moveViewport(dirX, dirY, 1.0/1.03);
			} else {
				moveViewport(x, y, 1.0/1.03);
			}
		} else {
			if (zoomFactor != 1.0) {
				zoomFactor = 1.0;
			} else {
				zoomFactor = 1.0/1.03;
			}
		}
	}

	if (ctr++ %2 == 0) {
		m.maxIts += 1;
		m2.maxIts += 1;
		m3.maxIts += 1;
		m4.maxIts += 1;
	}
	//using namespace std;
	//clock_t begin = clock();
	
	//m.bakeMandelbrot(&image);
	//clock_t end = clock();
	//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//cout << "took: " << elapsed_secs << "\n";
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{
	wW = w;
	wH = h;
	//cout << w << " " << h;
	if (fitToWindow) {
		resize(wW, wH, false);
		dirX = wW/2;
		dirY = wH/2;
		
		moveViewport(dirX, dirY, 1.0);
	}
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}