#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "graphicPoint.h"

// listen on port 12345
#define PORT 12347
#define NUM_MSG_STRINGS 20



struct specialPoint
{
    ofPoint point;
    int id;
};

class testApp : public ofBaseApp{
	public:

		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
        void Delete(int id, vector<graphicPoint>& tempPoints);
    bool showGrid;
    bool    showOriginalLine;
    long int    countPoint;
    ofColor             color;
    ofxOscReceiver	receiver;    
    vector<graphicPoint> points;
    vector<ofPoint>       reversePoint;
    vector<graphicPoint>    globalSmoothedPoint;
    ofPolyline              line;
    
    bool        drawingPoint;
    bool        removePoint;
    float       lineWeight;
    graphicPoint mainPositionPoint;
    
    int windowHeight;
    int windowWidth;
    
    int posX;
    int posY;
    
};
