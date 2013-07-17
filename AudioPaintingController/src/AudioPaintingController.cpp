//
//  AudioPaintingController.cpp
//  AudioPaintingController
//
//  Created by Mauro on 27/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AudioPaintingController.h"
#include "ofxSimpleGuiToo.h"
#include "ofxSVG.h"
#include "ofPath.h"
#include <GLUT/GLUT.h>

AudioPaintingController::AudioPaintingController()
{
    cout << "*** Audio Painting Controller" << endl;
}

void AudioPaintingController::init()
{
    this->initVariables();
    this->initSVGs();
    this->initGUI();
    this->initEvents();
    this->loadFont();
    
    ofSetVerticalSync(true);
    path = new ofPath();
    path->setFilled(false);
    path->newSubPath();
    
    sender.setup( hostIp, PORT );
    senderToTouch.setup( touchIp, 12347);
    senderToAkai.setup ( hostIp, PORT_AKAI );
    
    letters = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
    randomLetter = false;
}

void AudioPaintingController::loadFont()
{
    font.loadFont("HelveticaHv.ttf", 660, true, true, true);
}

void AudioPaintingController::initEvents()
{
    ofAddListener(ofEvents().keyReleased, this, &AudioPaintingController::keyReleased);
    ofAddListener(ofEvents().keyPressed, this, &AudioPaintingController::keyPressed);
    ofAddListener(ofEvents().mouseDragged, this, &AudioPaintingController::mouseDragged);
    ofAddListener(ofEvents().mousePressed, this, &AudioPaintingController::mousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &AudioPaintingController::mouseReleased);
}

void AudioPaintingController::initVariables()
{
 //   hostIp = "169.254.174.72";
    
    fullscreen = true;
    fullscreenState = !fullscreen;
    drawPoint = false;
    drawTriangle = true;
    sameColorTriangle = true;
    timeTriangle = true;
    drawLinePointToPoint = false;
    connectToPrevPoint = true;
    sameSpring = false;
    sameFriction = false;
    sameSpeed = true;
    loadGeometricGraphic = false;
    loadSVG = false;
    loadLetter = false;
    emitterMode = false;
    oldEmitterMode = !emitterMode;
    repulsionFromTarget = true;
    letterMode = false;
    
    outputWidth = 1024;
    outputHeight = 768;
    oldOutputWidth = outputWidth - 1;
    oldOutputHeight = outputHeight - 1;
    
    repulsionForce = .5;
    sameSpeedValue = .5;
    sameFrictionValue = 30;
    sameSpringValue = 30;
    maxDistance = .2;
    triangleCoefficent = .5;
    clearPoints = false;
    guiVisible = true;
    directDraw = true;
    prevDirectDraw = false;
    colorTriangleR = 255;
    colorTriangleG = 255;
    colorTriangleB = 255;
    
    maxDistancePointToPoint = 1000;
    maxTriangleCoefficent = 3;
    
    OSCAkaiActive = false;
    
    geometricShape = 0;
    svgShape = 0;
    
    spiralParams.push_back(10);
    spiralParams.push_back(10);   
    
    touchBridge = false;
    visualFrameRate = 0;
    
    secureLimit = true;
    
    straightLineMode = false;
    straightDirection = "";
    shiftMode = false;
    pointStraight.x = -100;
    pointStraight.y = -100;
    
    maxPointTouch = 800;
    
    volumeLimitToInvert = .5;
    
}

void AudioPaintingController::checkAkaiMessage()
{
    while( receiver.hasWaitingMessages() )
	{
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
        if(m.getAddress() != "/frameRate" )
        {
         //   cout << m.getAddress() << endl;
           // cout << m.getArgAsInt32( 0 )  << endl;
        }
        
        if(m.getAddress() == "/frameRate" )
        {
            visualFrameRate = m.getArgAsFloat( 0 );   
            if(secureLimit)
            {
                if(visualFrameRate < 23)
                {
                    if(drawLinePointToPoint)
                    {
                        maxDistance -= .01;
                        if(maxDistance <= 0)
                            cout << "*** WARNING ***" << endl;
                    }
                    if(drawTriangle)
                    {
                        triangleCoefficent -= .01;
                        if(triangleCoefficent <= 0)
                            cout << "*** WARNING ***" << endl;
                    }
                }
                else if(visualFrameRate > 26)
                {
                    if(drawLinePointToPoint && maxDistance < .7)
                        maxDistance += .01;
                    if(drawTriangle && triangleCoefficent < .7)
                        triangleCoefficent += .01;
                }
            }
        }
        else if ( m.getAddress() == "/addSinglePoint" || m.getAddress() == "/removePoint")
        {
            ofMouseEventArgs e;
            e.x = m.getArgAsFloat( 0 ) * ofGetWindowWidth();
            e.y = m.getArgAsFloat( 1 ) * ofGetWindowHeight();
            
            int x = e.x;
            int y = e.y;
            
            if( m.getAddress() == "/removePoint" )
                cleanMode = true;
            else
                cleanMode = false;
            
            if(!cleanMode)
            {
                if(!emitterMode)
                {
                    path->lineTo(x, y);
                    addSinglePoint(x, y);                       
                }     
            }
            else
            {
                cleanPosition.x = x;
                cleanPosition.y = y;
            }
            if(!cleanMode)
            {
               this->sendSinglePoint(points[points.size() - 1]);
            }
            if(cleanMode&&points.size() > 0)
                this->sendSinglePoint(x, y);
            
        }
        else if ( (m.getAddress() == "/midi/note/1" &&  m.getArgAsInt32( 0 ) == 53) || (m.getAddress() == "/1/push1") )
            randomLetter = true;
        else if ( m.getAddress() == "/clear")
            clearPoints = true;
        else if ( m.getAddress() == "/midi/note/1" &&  m.getArgAsInt32( 0 ) == 48 || (m.getAddress() == "/1/toggle1") )
        {
            if(m.getAddress() == "/1/toggle1")
                drawPoint = m.getArgAsInt32( 0 );
            else
                drawPoint = m.getArgAsInt32( 2 );
        }
        else if ( m.getAddress() == "/2/toggle3")
            drawPoint = m.getArgAsInt32( 0 );
        else if ( m.getAddress() == "/midi/note/2" &&  m.getArgAsInt32( 0 ) == 48 )
            connectToPrevPoint = m.getArgAsInt32( 2 );
        else if ( m.getAddress() == "/2/toggle4")
            connectToPrevPoint = m.getArgAsInt32( 0 );
        else if ( m.getAddress() == "/midi/note/3" &&  m.getArgAsInt32( 0 ) == 48 )
            drawLinePointToPoint = m.getArgAsInt32( 2 );
        else if ( m.getAddress() == "/2/toggle1")
            drawLinePointToPoint = m.getArgAsInt32( 0 );
        else if ( m.getAddress() == "/midi/note/4" &&  m.getArgAsInt32( 0 ) == 48 )
            drawTriangle = m.getArgAsInt32( 2 );
        else if ( m.getAddress() == "/2/toggle2")
            drawTriangle = m.getArgAsInt32( 0 );
        else if ( m.getAddress() == "/midi/note/1" &&  m.getArgAsInt32( 0 ) == 49 )
            directDraw = m.getArgAsInt32( 2 );
        else if ( m.getAddress() == "/midi/note/2" &&  m.getArgAsInt32( 0 ) == 49 )
            emitterMode = m.getArgAsInt32( 2 );
        else if ( m.getAddress() == "/2/toggle5" )
            emitterMode = m.getArgAsInt32( 0 );

        else if ( m.getAddress() == "/midi/note/3" &&  m.getArgAsInt32( 0 ) == 49 )
            repulsionFromTarget = m.getArgAsInt32( 2 );

        
        else if ( m.getAddress() == "/midi/note/1" &&  m.getArgAsInt32( 0 ) == 52 )
            clearPoints = true;        
        else if ( m.getAddress() == "/midi/note/2" &&  m.getArgAsInt32( 0 ) == 52 )
            clearAll = true;
        else if ( m.getAddress() == "/midi/note/3" &&  m.getArgAsInt32( 0 ) == 52 )
            loadGeometricGraphic = true;
        else if ( m.getAddress() == "/midi/note/4" &&  m.getArgAsInt32( 0 ) == 52 )
            loadSVG = true;
        
        else if ( m.getAddress() == "/midi/note/5" &&  m.getArgAsInt32( 0 ) == 49 )
            clearAll = m.getArgAsInt32( 2 );
        
        else if ( m.getAddress() == "/midi/note/1" &&  m.getArgAsInt32( 0 ) == 50 )
            sameColorTriangle = m.getArgAsInt32( 2 );
        else if ( m.getAddress() == "/midi/note/2" &&  m.getArgAsInt32( 0 ) == 50 )
             timeTriangle = m.getArgAsInt32( 2 );
       
        
        /*
        else if ( m.getAddress() == "/midi/note/2" &&  m.getArgAsInt32( 0 ) == 52 )
            clearPoints = true;
        else if ( m.getAddress() == "/midi/note/3" &&  m.getArgAsInt32( 0 ) == 52 )
            loadGeometricGraphic = true;
        else if ( m.getAddress() == "/midi/note/4" &&  m.getArgAsInt32( 0 ) == 52 )
            loadSVG = true;
        */

        else if ( m.getAddress() == "/midi/cc7/1" || m.getAddress() == "/2/fader5")
            sameSpeedValue =  m.getArgAsFloat( 0 ) * 100;
        else if ( m.getAddress() == "/midi/cc7/2" || m.getAddress() == "/2/fader6")
            repulsionForce =  m.getArgAsFloat( 0 );
        else if ( m.getAddress() == "/midi/cc7/3" || m.getAddress() == "/2/fader7")
            sameSpringValue =  m.getArgAsFloat( 0 ) * 100;
        else if ( m.getAddress() == "/midi/cc7/4" || m.getAddress() == "/2/fader8")
            sameFrictionValue =  m.getArgAsFloat( 0 ) * 100;
        else if ( m.getAddress() == "/midi/cc48/1")
            maxDistance =  m.getArgAsFloat( 0 );
        else if ( m.getAddress() == "/changeDistancePointToPoint")
        {
            maxDistance =  m.getArgAsFloat( 0 );
            //cout << maxDistance << endl;
        }
        else if ( m.getAddress() == "/midi/cc52/1" || m.getAddress() == "/2/fader1")
            maxDistancePointToPoint = ofMap(m.getArgAsFloat( 0 ), 0, 1, 10.0, 1000.0);
        else if ( m.getAddress() == "/midi/cc49/1")
            triangleCoefficent =  ofClamp(m.getArgAsFloat( 0 ), 0, .9);
        else if ( m.getAddress() == "/changeTriangleCoefficent")
        {
            triangleCoefficent =  ofClamp(m.getArgAsFloat( 0 ), 0, .9);
            //cout << triangleCoefficent << endl;
        }
        else if ( m.getAddress() == "/midi/cc53/1" || m.getAddress() == "/2/fader2")
            maxTriangleCoefficent =  ofMap(m.getArgAsFloat( 0 ), 0, 1, 3, 10);
        
        else if ( m.getAddress() == "/midi/cc16/1" )
            colorTriangleR =  m.getArgAsFloat( 0 ) * 255;
        else if ( m.getAddress() == "/midi/cc17/1" )
            colorTriangleG =  m.getArgAsFloat( 0 ) * 255;
        else if ( m.getAddress() == "/midi/cc18/1" )
            colorTriangleB =  m.getArgAsFloat( 0 ) * 255;
        
        else if ( m.getAddress() == "/midi/cc54/1" )
        {
            spiralParams[0] =  m.getArgAsFloat( 0 ) * 100;
            if(!directDraw)
                loadGeometricGraphic = true;
        }
        else if ( m.getAddress() == "/midi/cc55/1" )
        {
            spiralParams[1] =  m.getArgAsFloat( 0 ) * 100;
            if(!directDraw)
                loadGeometricGraphic = true;
        }

        else if ( m.getAddress() == "/midi/cc50/1" )
        {
            svgShape =  (int)ofMap(m.getArgAsFloat( 0 ), 0, 1, 0, (svgUrl.size() - 1));
            if(!directDraw)
                loadSVG = true;
        }
        else if ( m.getAddress() == "/midi/cc51/1" )
        {
            geometricShape =  (int) ofMap(m.getArgAsFloat( 0 ), 0, 1, 0, 4);
            if(!directDraw)
                    loadGeometricGraphic = true;
        }
	}
}

void AudioPaintingController::clear()
{
    path->close();
    path->clear();
    int size = points.size();
    for(int a = 0; a < size; a++)
    {
        ofPoint* pParticle = points[points.size() - 1];
        points.pop_back();
        delete pParticle;
    }
}

void AudioPaintingController::initGUI()
{
    gui.addTitle("Output");
    
	gui.addVar("ip", hostIp);
	gui.addVar("ip touch", touchIp);
	gui.addToggle("Fullscreen", fullscreen);
    gui.addSlider("Output width", outputWidth, 200, 3000);
    gui.addSlider("Output height", outputHeight, 200, 3000);
    
	gui.addToggle("Direct draw", directDraw);
    gui.addToggle("Emitter mode", emitterMode);
    gui.addToggle("Repulsion from target", repulsionFromTarget);
    gui.addToggle("Clear All", clearAll);
    gui.addToggle("Clear", clearPoints);
    
    gui.addTitle("Draw");
    gui.addToggle("Draw point", drawPoint);
	gui.addToggle("Draw connect prev point", connectToPrevPoint);
	gui.addToggle("Draw line point to point", drawLinePointToPoint);
	gui.addToggle("Draw triangle", drawTriangle);
    gui.addTitle("Triangle");
    gui.addToggle("Same color triangle", sameColorTriangle);
    gui.addToggle("Time  triangle", timeTriangle);
    gui.addTitle("Particle");
    //	gui.addToggle("Set same speed", sameSpeed);
    gui.addSlider("Same speed value", sameSpeedValue, 0, 100);
    gui.addSlider("Repulsion", repulsionForce, 0, 1);
    //	gui.addToggle("Time spring", sameSpring);
    gui.addSlider("Same spring value", sameSpringValue, 0, 100);
    //	gui.addToggle("Time friction", sameFriction);
    gui.addSlider("Same friction value", sameFrictionValue, 0, 100);
    gui.addToggle("Load Geometric", loadGeometricGraphic);
    gui.addToggle("Load SVG", loadSVG);
    gui.addToggle("Letter Mode", letterMode);
    gui.addToggle("Load Letter", loadLetter);
    gui.addToggle("Random letter", randomLetter);

    gui.addTitle("Graphic");
    gui.addSlider("Min relative distance connect point", maxDistance, 0, 1);
    gui.addSlider("Max value distance connect point", maxDistancePointToPoint, 10.0, 500.0);
    gui.addSlider("Relative Triangle coefficient", triangleCoefficent, 0, .9);
    gui.addSlider("Max Triangle coefficient", maxTriangleCoefficent, 3, 10);
    
    gui.addSlider("single Red", colorTriangleR, 0, 255);
    gui.addSlider("single Green", colorTriangleG, 0, 255);
    gui.addSlider("single Blu", colorTriangleB, 0, 255);
    
        
    /*
    string title [svgUrl.size()];
    
    for(int a = 0; a < svgUrl.size(); a++)
        title[a] = svgUrl[a];
    */
    
   // string title[] = {"fake", "fake", "fake"};
    
    string title[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22"};
    
    gui.addComboBox("SVG", svgShape, svgUrl.size(),  title);
    
    string titleArray[] = {"cerchio", "spirala random", "farfalla", "Lemniscate","random"};
	gui.addComboBox("Geometric", geometricShape, 5,  titleArray);
    gui.addSlider("Param 1 spiral and circle", spiralParams[0], 1, 100);
    gui.addSlider("Param 2 spiral", spiralParams[1], 1, 100);
    
    gui.addTitle("Test");
    gui.addToggle("Touch bridge", touchBridge);
    gui.addToggle("Clean", cleanMode);
    gui.addSlider("Visual Framerate", visualFrameRate, 0.0, 60.0);
    gui.addSlider("Max point touch", maxPointTouch, 500, 1600);
    gui.addSlider("Volume Limit To Invert", volumeLimitToInvert, 0.000, 2.000);
    gui.addToggle("Secure Limit", secureLimit);

    
    gui.loadFromXML(); 
    
    //directDraw = false;
    
    gui.show();
}

void AudioPaintingController::initSVGs()
{
    svgUrl.push_back("1.svg");
    svgUrl.push_back("2.svg");
    svgUrl.push_back("3.svg");
    svgUrl.push_back("4.svg");
    svgUrl.push_back("5.svg");
    svgUrl.push_back("6.svg");
    svgUrl.push_back("7.svg");
    svgUrl.push_back("8.svg");
    svgUrl.push_back("9.svg");
    svgUrl.push_back("10.svg");
    svgUrl.push_back("11.svg");
    svgUrl.push_back("12.svg");
    svgUrl.push_back("13.svg");
    svgUrl.push_back("14.svg");
    svgUrl.push_back("15.svg");
    svgUrl.push_back("16.svg");
    svgUrl.push_back("17.svg");
    svgUrl.push_back("18.svg");
    svgUrl.push_back("19.svg");
    svgUrl.push_back("19.svg");
    svgUrl.push_back("20.svg");
    svgUrl.push_back("21.svg");
    svgUrl.push_back("22.svg");
}

void AudioPaintingController::sendMessagges()
{
    this->checkAkaiMessage();
    vector<ofxOscMessage*> messages;
    vector<int*> totPoints;
    messages.push_back(new ofxOscMessage());
	messages[0]->setAddress( "/output" );
	messages[0]->addIntArg( drawPoint );
    messages[0]->addIntArg( drawTriangle );
    messages[0]->addIntArg( sameColorTriangle );
    messages[0]->addIntArg( timeTriangle );
    messages[0]->addIntArg( drawLinePointToPoint );
    messages[0]->addIntArg( connectToPrevPoint );
    messages[0]->addIntArg( repulsionFromTarget );
    messages[0]->addFloatArg( maxDistance );
    messages[0]->addFloatArg( 1 - triangleCoefficent );
    messages[0]->addFloatArg( maxDistancePointToPoint );
    messages[0]->addFloatArg( maxTriangleCoefficent );
    messages[0]->addFloatArg( volumeLimitToInvert );
    
	sender.sendMessage( *messages[0] );
        
    messages[0]->clear();
    messages[0]->setAddress( "/totPoint" );
    messages[0]->addIntArg( maxPointTouch );
	senderToTouch.sendMessage( *messages[0] );
    
    messages[0]->clear();
    messages[0]->setAddress( "/triangle/sameColor" );
    messages[0]->addIntArg( colorTriangleR );
    messages[0]->addIntArg( colorTriangleG );
    messages[0]->addIntArg( colorTriangleB );
    sender.sendMessage( *messages[0] );
    
    if(fullscreenState != fullscreen)
    {
    
        fullscreenState = fullscreen;
        messages[0]->clear();
        messages[0]->setAddress( "/setFullscreen" );
        messages[0]->addIntArg(fullscreen);
        sender.sendMessage( *messages[0] );
        cout << "Change " << endl;
    }
    
    if((oldOutputWidth != outputWidth)|| (oldOutputHeight != outputHeight))
    {
        oldOutputWidth = outputWidth;
        oldOutputHeight = outputHeight;
        messages[0]->clear();
        messages[0]->setAddress( "/setWindowSize" );
        messages[0]->addIntArg(outputWidth);
        messages[0]->addIntArg(outputHeight);
        sender.sendMessage( *messages[0] );
        cout << "Change " << endl;
    }
    
    messages[0]->clear();
    messages[0]->setAddress( "/particle/timeSpring" );
    messages[0]->addIntArg( sameSpring);
    sender.sendMessage( *messages[0] );

    
    if(!sameSpring)
    {
        //   sameSpring = false;
        messages[0]->clear();
        messages[0]->setAddress( "/particle/newSpring" );
        float tempSameSpring = sameSpringValue;
        if(sameSpringValue != 0)
            tempSameSpring /= 100.0;
        messages[0]->addFloatArg( tempSameSpring);
        sender.sendMessage( *messages[0] );
    }
    
    messages[0]->clear();
    messages[0]->setAddress( "/particle/timeFriction" );
    messages[0]->addIntArg( sameFriction);
    sender.sendMessage( *messages[0] );

    if(!sameFriction)
    {
        //  sameFriction = false;
        messages[0]->clear();
        messages[0]->setAddress( "/particle/newFriction" );
        float tempSameFriction = sameFrictionValue;
        if(sameFrictionValue != 0)
            tempSameFriction /= 100.0;
        messages[0]->addFloatArg( tempSameFriction);
        sender.sendMessage( *messages[0] );
    }
    
    if(sameSpeed)
    {
        //  sameFriction = false;
        messages[0]->clear();
        messages[0]->setAddress( "/particle/newSpeed" );
        messages[0]->addFloatArg( sameSpeedValue);
        sender.sendMessage( *messages[0] );
    }
    
    messages[0]->clear();
    messages[0]->setAddress( "/particle/newRepulsionForce" );
    messages[0]->addFloatArg( repulsionForce);
    sender.sendMessage( *messages[0] );
        
    if(clearPoints||clearAll)
    {
        clearPoints = false;
        this->clear();
        messages[0]->clear();
        messages[0]->setAddress( "/clear" );
        senderToTouch.sendMessage( *messages[0] );        
        if(clearAll)
        {
            clearAll = false;            
            sender.sendMessage( *messages[0] );  
        }
    }
    
    if(oldEmitterMode != emitterMode)
    {
        this->clear();
        oldEmitterMode = emitterMode;
        messages[0]->clear();
        messages[0]->setAddress( "/emitterMode" );
        messages[0]->addIntArg(emitterMode);
        sender.sendMessage( *messages[0] );
        senderToTouch.sendMessage( *messages[0]);
        messages[0]->clear();
        messages[0]->setAddress( "/clear" );
        senderToTouch.sendMessage( *messages[0] );  
    }
    
    if(loadGeometricGraphic)
    {
        loadGeometricGraphic = false;
        this->loadGraphic();
        if(directDraw)
        {
            messages[0]->clear();
            messages[0]->setAddress( "/loadShape" );
            cout << "TOT NEW POINT = " << points.size() << endl;
            int cont = 0;
            int contPoint = 0;
            totPoints.push_back(new int());            
            messages[0]->addIntArg(points.size());
            //   sender.sendMessage( *messages[0] );
            cont = -1;
            for(int a = 0; a < points.size(); a++)
            {
                if(a%301 == 0 && a >= 0)
                {
                    //cout << "DENTRO a = " << a << endl;
                    
                    messages[0]->addFloatArg( points[a]->x  / ofGetWindowWidth() );
                    messages[0]->addFloatArg( points[a]->y  / ofGetWindowHeight() );
                    
                    sender.sendMessage( *messages[0] );
                    cont++;
                    messages[0]->clear();
                    messages[0]->setAddress( "/addPoint" );
                    int pointToSend = (points.size() - a);
                    if(pointToSend >= 301)
                        pointToSend = 301;
                    messages[0]->addFloatArg(pointToSend);
                    contPoint = 0;
                    cout << "**************************" << endl;
                    
                }
                messages[0]->addFloatArg( points[a]->x  / ofGetWindowWidth() );
                messages[0]->addFloatArg( points[a]->y  / ofGetWindowHeight() );
                
                senderToTouch.sendMessage( *messages[0] );
                
                contPoint++;
            }
            if(points.size()%301 != 0)
            {
                cout << "SEND FINAL MESSAGE " << endl;
                sender.sendMessage( *messages[0] );
                
                
                senderToTouch.sendMessage( *messages[0] );
            }
        }
    }
    if(loadSVG)
    {
        loadSVG = false;
        this->loadFromSVG(svgUrl[svgShape]);
        if(directDraw)
         {
            messages[0]->clear();
            messages[0]->setAddress( "/loadShape" );
            cout << "TOT NEW POINT = " << points.size() << endl;
            int cont = 0;
            int contPoint = 0;
            totPoints.push_back(new int());            
            messages[0]->addIntArg(points.size());
         //   sender.sendMessage( *messages[0] );
            cont = -1;
            for(int a = 0; a < points.size(); a++)
            {
                if(a%301 == 0 && a >= 0)
                {
                    cout << "DENTRO a = " << a << endl;
                    
                    messages[0]->addFloatArg( points[a]->x  / ofGetWindowWidth() );
                    messages[0]->addFloatArg( points[a]->y  / ofGetWindowHeight() );
                    
                    sender.sendMessage( *messages[0] );
                    cont++;
                    messages[0]->clear();
                    messages[0]->setAddress( "/addPoint" );
                    int pointToSend = (points.size() - a);
                    if(pointToSend >= 301)
                        pointToSend = 301;
                    messages[0]->addFloatArg(pointToSend);
                    contPoint = 0;
                    cout << "**************************" << endl;
                   
                }
                messages[0]->addFloatArg( points[a]->x  / ofGetWindowWidth() );
                messages[0]->addFloatArg( points[a]->y  / ofGetWindowHeight() );
                
                
                contPoint++;
            }
             if(points.size()%301 != 0)
             {
                 cout << "SEND FINAL MESSAGE " << endl;
                 sender.sendMessage( *messages[0] );
             }
        }
    }

  //  if(!letterMode)
    //    actualWords = "";
    
    
    if(randomLetter)
    {
        randomLetter = false;
        loadLetter = true;
        actualWords = "";
        actualWords = letters[ofRandom(0, letters.length() - 1)];
    }
    
    if(loadLetter)
    {
        loadLetter = false;
        this->loadLetterFunction(actualWords);
        if(directDraw)
        {
            messages[0]->clear();
            messages[0]->setAddress( "/loadShape" );
            cout << "TOT NEW POINT = " << points.size() << endl;
            int cont = 0;
            int contPoint = 0;
            totPoints.push_back(new int());            
            messages[0]->addIntArg(points.size());
            //   sender.sendMessage( *messages[0] );
            cont = -1;
            for(int a = 0; a < points.size(); a++)
            {
                if(a%301 == 0 && a >= 0)
                {
                    cout << "DENTRO a = " << a << endl;
                    
                    messages[0]->addFloatArg( points[a]->x  / ofGetWindowWidth() );
                    messages[0]->addFloatArg( points[a]->y  / ofGetWindowHeight() );
                    
                    sender.sendMessage( *messages[0] );
                    cont++;
                    messages[0]->clear();
                    messages[0]->setAddress( "/addPoint" );
                    int pointToSend = (points.size() - a);
                    if(pointToSend >= 301)
                        pointToSend = 301;
                    messages[0]->addFloatArg(pointToSend);
                    contPoint = 0;
                    cout << "**************************" << endl;
                    
                }
                messages[0]->addFloatArg( points[a]->x  / ofGetWindowWidth() );
                messages[0]->addFloatArg( points[a]->y  / ofGetWindowHeight() );
                
                
                contPoint++;
            }
            if(points.size()%301 != 0)
            {
                cout << "SEND FINAL MESSAGE " << endl;
                sender.sendMessage( *messages[0] );
            }
        }
        actualWords = "";
    }

    
    if(directDraw)
    {
        if(prevDirectDraw != directDraw)
        {
            prevDirectDraw = true;
            messages[0]->clear();
            messages[0]->setAddress( "/loadShape" );
            cout << "TOT NEW POINT = " << points.size() << endl;
            int cont = 0;
            int contPoint = 0;
            totPoints.push_back(new int());            
            messages[0]->addIntArg(points.size());
            //   sender.sendMessage( *messages[0] );
            cont = -1;
            for(int a = 0; a < points.size(); a++)
            {
                if(a%301 == 0 && a >= 0)
                {
                    cout << "DENTRO a = " << a << endl;
                    
                    messages[0]->addFloatArg( points[a]->x  / ofGetWindowWidth() );
                    messages[0]->addFloatArg( points[a]->y  / ofGetWindowHeight() );
                    
                    sender.sendMessage( *messages[0] );
                    cont++;
                    messages[0]->clear();
                    messages[0]->setAddress( "/addPoint" );
                    int pointToSend = (points.size() - a);
                    if(pointToSend >= 301)
                        pointToSend = 301;
                    messages[0]->addFloatArg(pointToSend);
                    contPoint = 0;
                    cout << "**************************" << endl;
                    
                }
                messages[0]->addFloatArg( points[a]->x  / ofGetWindowWidth() );
                messages[0]->addFloatArg( points[a]->y  / ofGetWindowHeight() );
                
                
                contPoint++;
            }
            if(points.size()%301 != 0)
            {
                cout << "SEND FINAL MESSAGE " << endl;
                sender.sendMessage( *messages[0] );
            }

        }
    }
    else
    {
        prevDirectDraw = false;
    }
    
    int totMessages = messages.size();
    int totPointsSize = totPoints.size();
    for(int a = 0; a < totMessages; a++)
    {
        ofxOscMessage* pMessage;
        if(totPointsSize > 0)
        {
            int* pInt;
            pInt = totPoints[totPoints.size() - 1];
            totPoints.pop_back();
            delete pInt;
        }
       
        pMessage = messages[messages.size() - 1];
        messages.pop_back();
        delete pMessage;
    }
    totPoints.clear();
    messages.clear();
}


void AudioPaintingController::loadLetterFunction(string letter)
{
    this->clear();
    path->newSubPath();
    ofPoint tempPoint;
    
    ofPoint max;
    ofPoint min;
    
    max.x = 0;
    max.y = 0;
    
    min.x = 0;
    min.y = 0;
    
    ofPolyline newPoly;
    
    
    
    font.setLineHeight(10);
   // string letters = "hey";
    
    vector<ofTTFCharacter> characters = font.getStringAsPoints(letter);
     
    for(int k = 0; k < characters.size(); k++)
        {
            for(int j = 0; j < characters[k].getOutline().size(); j++)
             {
                 for(int i = 0; i < characters[k].getOutline()[j].size(); i++)
                    {
                        newPoly.addVertex(characters[k].getOutline()[j][i].x,  characters[k].getOutline()[j][i].y); 
                        if(characters[k].getOutline()[j][i].x >= max.x)
                            max.x = characters[k].getOutline()[j][i].x;
                        if(characters[k].getOutline()[j][i].y >= max.y)
                            max.y = characters[k].getOutline()[j][i].y;
                        if(characters[k].getOutline()[j][i].x <= min.x)
                            min.x = characters[k].getOutline()[j][i].x;
                        if(characters[k].getOutline()[j][i].y <= min.y)
                            min.y = characters[k].getOutline()[j][i].y;
                    }
                     
            }
        }
    
    ofPoint newPosition;
    newPosition.x = (ofGetWindowWidth() - abs(max.x - min.x)) * .5;
    newPosition.y = (ofGetWindowHeight() - abs(max.y - min.y)) * .5;
    //newPosition.y = (ofGetWindowHeight() - (500)) * .5;
    
   // newPosition.y  = (ofGetWindowHeight() * .5) + 200;
    
    for(int a = 0; a <  newPoly.getVertices().size(); a++)
    {
        tempPoint.x = newPoly[a].x + newPosition.x;
        tempPoint.y = newPoly[a].y + abs(min.y) + newPosition.y;
        
        path->lineTo(tempPoint.x, tempPoint.y);
        addSinglePoint(tempPoint.x, tempPoint.y);
    }
    
    path->close();
}

void AudioPaintingController::loadFromSVG(string url)
{
    url = "21.svg";
    cout << "url : " << url << endl;
    this->clear();
    path->newSubPath();
    ofPoint tempPoint;
    
    ofPoint max;
    ofPoint min;
    
    max.x = -1000;
    max.y = -1000;
    
    min.x = 10000;
    min.y = 10000;
    

    ofxSVG* mySVG;
    mySVG = new ofxSVG();
    mySVG->load(url);
    
    ofPolyline newPoly;
    
    ofPath newPath;
    
    for(int contLayer = 0; contLayer < mySVG->layers.size(); contLayer++)
    {
        for(int contObject = 0; contObject < mySVG->layers[contLayer].objects.size(); contObject++)
        {
            for(int contVertex = 0; contVertex < mySVG->layers[contLayer].objects[contObject]->vertexs.size(); contVertex++)
            {
                mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].x *= 2.5;
                mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].y *= 2.5;
                //cout <<  mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex] << endl;
                if(mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].x > max.x)
                    max.x = mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].x;
                else if(mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].y > max.y)
                    max.y = mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].y;

                if(mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].x < min.x)
                    min.x = mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].x;
                else if(mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].y < min.y)
                    min.y = mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex].y;

                
                newPoly.addVertex(mySVG->layers[contLayer].objects[contObject]->vertexs[contVertex]);  
            }
        }
    }
    
    ofPoint newPosition;
    newPosition.x = (ofGetWindowWidth() - (max.x - min.x)) * .5;
    newPosition.y = (ofGetWindowHeight() - (max.y - min.y)) * .5;
    
    
    newPoly.simplify();
    
    cout << "POINT SIZE = " << newPoly.getVertices().size() << endl;
    
    for(int a = 0; a <  newPoly.getVertices().size(); a++)
    {
        tempPoint.x = newPoly[a].x + newPosition.x;
        tempPoint.y = newPoly[a].y + newPosition.y;
        
        path->lineTo(tempPoint.x, tempPoint.y);
        addSinglePoint(tempPoint.x, tempPoint.y);
    }
    
    delete mySVG;
    
    path->close();
}

void AudioPaintingController::loadGraphic()
{
    this->clear();
    path->newSubPath();
    ofPoint tempPoint;
        
    switch(geometricShape)
    {
        case 0:
            for(int a = 0; a < 360; a += 10)
            {
                               
                tempPoint.x = (ofGetWindowWidth() * .5) + sin(ofDegToRad(a)) *  spiralParams[0] * 10;
                tempPoint.y = (ofGetWindowHeight() * .5) + cos(ofDegToRad(a)) *  spiralParams[0] * 10;
                
                path->lineTo(tempPoint.x, tempPoint.y);
                addSinglePoint(tempPoint.x, tempPoint.y);
            }
            break;
        case 1:     // Spirale
            for(int a = 0; a < 360 * 10; a += 10)
            {
                tempPoint.x = (ofGetWindowWidth() * .5) + sin(ofDegToRad(a)) * spiralParams[1] * (a / (spiralParams[0] * PI));
                tempPoint.y = (ofGetWindowHeight() * .5) + cos(ofDegToRad(a)) * spiralParams[1] * (a / (spiralParams[0] * PI));
                path->lineTo(tempPoint.x, tempPoint.y);
                addSinglePoint(tempPoint.x, tempPoint.y);
            }
            break;
        case 2:     // Curva a farfalla
            for(int a = 0; a < 360 * 5; a += 10)
            {
                tempPoint.x = (ofGetWindowWidth() * .5) + (sin(ofDegToRad(a)) * ( pow(2, cos(ofDegToRad(a))) - 2 * cos(4*ofDegToRad(a)) - pow(sin(ofDegToRad(a)/12), 5) )) * 100;
                tempPoint.y = (ofGetWindowHeight() * .5) + (cos(ofDegToRad(a)) * ( pow(2, cos(ofDegToRad(a))) - 2 * cos(4*ofDegToRad(a)) - pow(sin(ofDegToRad(a)/12), 5) )) * 100;
                path->lineTo(tempPoint.x, tempPoint.y);
                addSinglePoint(tempPoint.x, tempPoint.y);
            }
            break;  
        case 3:     // Curva a farfalla
            for(int a = 0; a < 360 * 5; a += 10)
            {
                tempPoint.x = (ofGetWindowWidth() * .5) + spiralParams[0] * 10 * sin(ofDegToRad(a)) / ( 1 + (sin(ofDegToRad(a)) * sin(ofDegToRad(a))));
                tempPoint.y = (ofGetWindowHeight() * .5) + spiralParams[0] * 10 * sin(ofDegToRad(a)) * cos(ofDegToRad(a)) / ( 1 + (sin(ofDegToRad(a)) * sin(ofDegToRad(a))));
                path->lineTo(tempPoint.x, tempPoint.y);
                addSinglePoint(tempPoint.x, tempPoint.y);
            }
            break;  
        default:
            for(int a = 0; a < 400; a += 1)
            {
                tempPoint.x = ofGetWindowWidth() *  ofRandom(0,1);
                tempPoint.y = ofGetWindowHeight() *  ofRandom(0,1);
                path->lineTo(tempPoint.x, tempPoint.y);
                addSinglePoint(tempPoint.x, tempPoint.y);
            }
            break;
    }
    path->close();
}

void AudioPaintingController::keyReleased (ofKeyEventArgs &e)
{
    char key = e.key;
    
    if(key == 60)
        shiftMode = false;
}

void AudioPaintingController::keyPressed (ofKeyEventArgs &e)
{ 
    
    char key = e.key;
    
    if(key == 60)
        shiftMode = true;
    
    cout << e.key << endl;
    if(letterMode&&e.key != 32 && e.key != 60)
    {
    //    actualWords = ofToString(key);
        actualWords += ofToString(key);
        loadLetter = true;
        return;
    }
    
    
    if(key == ' ')
        guiVisible = !guiVisible;
    else if(key == 'o' && !OSCAkaiActive)
    {
        OSCAkaiActive = true;
        receiver.setup( PORT_AKAI );
    }
	if(key>='0' && key<='9') {
		gui.setPage(key - '0');
		gui.show();
	} else {
		switch(key) {
            
			case ' ': gui.toggleDraw(); break;
			case '[': gui.prevPage(); break;
			case ']': gui.nextPage(); break;
			case 'p': gui.nextPageWithBlank(); break;
		}
	}
}

void AudioPaintingController::mouseDragged(ofMouseEventArgs &e)
{
    if(shiftMode)
    {
        if(pointStraight.x < 0 && straightLineMode)
        {
            if( abs(startPointStraight.x - e.x) >=2 || abs(startPointStraight.y - e.y) >=2  )
            {
              //  cout << "dentoo" << endl;
                if(startPointStraight.x == e.x)
                    straightDirection = "vertical";
                else //if(e.y > (startPointStraight.y) || e.y < (startPointStraight.y))
                    straightDirection = "horizontal";
                
                
                pointStraight.x = e.x;
                pointStraight.y = e.y;                
            }
        }
        if(!straightLineMode)
        {
           // cout << "dentoo 2" << endl;
           // cout << pointStraight.x << endl,
            straightLineMode = true;
            startPointStraight.x = e.x;
            startPointStraight.y = e.y;
        }
        
    }
    
    if(guiVisible) return;
	int x = e.x;
	int y = e.y;
    
    if(straightDirection == "horizontal")
        y = pointStraight.y;
    else if(straightDirection == "vertical")
        x = pointStraight.x;
    
    
    
    
    if(!cleanMode)
    {
        path->lineTo(x, y);
        addSinglePoint(x, y);        
    }
    else
    {
        cleanPosition.x = x;
        cleanPosition.y = y;
    }
    if(directDraw&&!cleanMode)
        this->sendSinglePoint(points[points.size() - 1]);
    if(cleanMode&&points.size() > 0)
        this->sendSinglePoint(x, y);
}

void AudioPaintingController::mousePressed(ofMouseEventArgs &e)
{
	int x = e.x;
	int y = e.y;
    path->newSubPath();
}

void AudioPaintingController::mouseReleased(ofMouseEventArgs &e)
{
	int x = e.x;
	int y = e.y;
    path->close();
    straightLineMode = false;
    straightDirection = "";
    pointStraight.x = -100;
    pointStraight.y = -100;
}

void AudioPaintingController::addSinglePoint(int x, int y)
{
    ofPoint* tempPoint;
    tempPoint = new ofPoint();
    tempPoint->x = x;
    tempPoint->y = y;
    points.push_back(tempPoint);
}

void AudioPaintingController::sendSinglePoint(ofPoint* tempPoint)
{
    ofxOscMessage m;
	m.setAddress( "/addSinglePoint" );
	m.addFloatArg( tempPoint->x  / ofGetWindowWidth() );
	m.addFloatArg( tempPoint->y  / ofGetWindowHeight() );
    
    if(cleanMode)
        m.setAddress( "/removePoint" );
   // if(touchBridge)
    //cout << "send single point 1 " << endl;
    senderToTouch.sendMessage( m );
    if(directDraw)
        sender.sendMessage( m );
}

void AudioPaintingController::sendSinglePoint(float x, float y)
{
    ofxOscMessage m;
	m.setAddress( "/addSinglePoint" );
	m.addFloatArg( x  / ofGetWindowWidth() );
	m.addFloatArg( y  / ofGetWindowHeight() );
    if(cleanMode)
        m.setAddress( "/removePoint" );
    // if(touchBridge)
    //cout << "send single point 1 " << endl;
        senderToTouch.sendMessage( m );
    if(directDraw)
        sender.sendMessage( m );
}


void AudioPaintingController::draw()
{
    ofBackground(0, 0, 0);
    if(!emitterMode)
        path->draw(0, 0);
	gui.draw();
    if(cleanMode)
    {
        ofSetColor(255, 0, 0);
        ofCircle(cleanPosition, 100);
    }
}