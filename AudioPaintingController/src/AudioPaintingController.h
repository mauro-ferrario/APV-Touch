//
//  AudioPaintingController.h
//  AudioPaintingController
//
//  Created by Mauro on 27/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AudioPaintingController_AudioPaintingController_h
#define AudioPaintingController_AudioPaintingController_h

#include "ofMain.h"
#include "ofxOsc.h"


#define PORT 12345
#define PORT_AKAI 12346

class AudioPaintingController
{
private:
public:
    AudioPaintingController();
    void                        init();
    void                        sendMessagges();  
    void                        initVariables();
    void                        initGUI();
    void                        draw();
    void                        keyPressed (ofKeyEventArgs &e);
    void                        keyReleased (ofKeyEventArgs &e);
    void                        mouseDragged(ofMouseEventArgs &e);
    void                        mousePressed(ofMouseEventArgs &e);
    void                        mouseReleased(ofMouseEventArgs &e);
    void                        initEvents();
    void                        addSinglePoint(int x, int y);
    void                        sendSinglePoint(ofPoint* tempPoint);
    void                        sendSinglePoint(float x, float y);
    void                        clear();
    void                        loadGraphic();
    void                        checkAkaiMessage();
    void                        loadFromSVG(string url);
    void                        loadLetterFunction(string letters);
    void                        initSVGs();
    void                        loadFont();
    
    ofxOscSender                sender;
    ofxOscSender                senderToTouch;
    ofxOscSender                senderToAkai;
    
    bool                        fullscreen;
    bool                        fullscreenState;
    bool                        drawPoint;
    bool                        sameColorTriangle;
    bool                        timeTriangle;
    bool                        drawTriangle;
    bool                        drawLinePointToPoint;
    bool                        connectToPrevPoint;
    bool                        sameSpring;
    bool                        sameFriction;
    bool                        sameSpeed;
    bool                        loadGeometricGraphic;
    bool                        loadSVG;
    bool                        loadLetter;
    bool                        letterMode;
    bool                        emitterMode;
    bool                        oldEmitterMode;
    bool                        repulsionFromTarget;
    bool                        directDraw;
    bool                        prevDirectDraw;
    bool                        touchBridge;
    bool                        cleanMode;
    bool                        clearAll;
    
    
    string                      actualWords;
    int                         outputWidth;
    int                         outputHeight;
    int                         oldOutputWidth;
    int                         oldOutputHeight;
    int                         sameSpeedValue;
    int                         sameFrictionValue;
    int                         sameSpringValue;
    float                       maxDistance;
    float                       repulsionForce;
    float                       triangleCoefficent;
    float                       maxTriangleCoefficent;
    float                       maxDistancePointToPoint;
    float                       volumeLimitToInvert;
    vector<int>                 spiralParams;
    bool                        clearPoints;
    int                         geometricShape;
    int                         svgShape;
    bool                        OSCAkaiActive;
    
    int                         colorTriangleR;
    int                         colorTriangleG;
    int                         colorTriangleB;
    
    string                      hostIp;
    string                      touchIp;
    
    bool                        guiVisible;
    ofPath*                     path;
    vector<ofPoint*>            points;
    string  pointsString;
    ofxOscReceiver              receiver;
    vector<string>              svgUrl;   
    ofTrueTypeFont              font;
    ofPoint                     cleanPosition;
    float                       visualFrameRate;
    bool                        secureLimit;
    
    bool                        straightLineMode;
    ofPoint                     pointStraight;
    string                      straightDirection;
    ofPoint                     startPointStraight;
    bool                        shiftMode;
    
    string                      letters;
    bool                        randomLetter;
    
    int                         maxPointTouch;
    
};

#endif
