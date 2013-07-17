#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	receiver.setup( PORT );

    ofSetWindowPosition(1280,0);
   // ofSetFullscreen(true);

    color.r = 255;
    color.g = 255;
    color.b = 0;
    
	ofBackground( 30, 30, 130 );

    ofSetFrameRate(200);
    countPoint = 0;
    showOriginalLine = true;
    showGrid = TRUE;
    drawingPoint = false;
    removePoint = false;
    lineWeight = .5;
    ofSetLineWidth(lineWeight);
    
    windowWidth = ofGetWindowWidth();
    windowHeight = ofGetWindowHeight();
    
    posX = 1280;
    posY = 0;
}

//--------------------------------------------------------------
void testApp::update(){
    
    line.clear();
    reversePoint.clear();
    
    drawingPoint = false;
    removePoint = false;
    
	while( receiver.hasWaitingMessages() )
	{
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
        //cout << m.getAddress() << endl;
		if ( m.getAddress() == "/addSinglePoint" )
		{
            graphicPoint newPoint;
            newPoint.position.x = m.getArgAsFloat( 0 ) * ofGetWindowWidth();
            newPoint.position.y = m.getArgAsFloat( 1 ) * ofGetWindowHeight();
            newPoint.id = countPoint++;
            points.push_back(newPoint);
            color.r = 255;
            color.g = 255;
            color.b = 0;
            drawingPoint = true;
        }
        else if(m.getAddress() == "/clear")
        {
            points.clear();
        }
        else if(m.getAddress() == "/removePoint")
        {
            graphicPoint newPoint;
            newPoint.position.x = m.getArgAsFloat( 0 ) * ofGetWindowWidth();
            newPoint.position.y = m.getArgAsFloat( 1 ) * ofGetWindowHeight();
            for(int k = points.size() - 1; k >= 0 ; k--)
            {
                float distance = points[k].position.distance(newPoint.position);
                if( distance < 100)
                    Delete(points[k].id, points);
            }
            mainPositionPoint.position.x = newPoint.position.x;
            mainPositionPoint.position.y = newPoint.position.y;
            removePoint = true;
        }
	}
    
    for(int a = 0; a < points.size(); a++)
        reversePoint.push_back(points[points.size() - 1 - a].position);
    
    if(points.size() > 0)
    {
        for(int a = 0; a < points.size(); a++)
            line.lineTo(points[a].position);
        
        
        for(int a = 0; a < reversePoint.size(); a++)
            line.lineTo(reversePoint[a]);
        
        line.close();
    }
    
    
    }

void testApp::draw(){
    
    ofEnableAlphaBlending();
    ofSeedRandom(10);
    
    if(showGrid)
        ofBackground(255, 0,0);
    else
        ofBackground(0, 0, 0);
    
    if(showGrid)
    {
        for(int a = 0; a < 5; a++)
        {
            ofSetColor(0, 255, 255);
            ofLine(0, a * ofGetWindowHeight()/5, ofGetWindowWidth(), a * ofGetWindowHeight()/5);
        }
        
        for(int a = 0; a < 5; a++)
        {
            ofSetColor(0, 255, 255);
            ofLine(a * ofGetWindowWidth()/5, 0, a * ofGetWindowWidth()/5, ofGetWindowHeight());
        }
    }
    
    
    if(showOriginalLine)
    {
        for(int a = 1; a < points.size(); a++)
        {
            ofSetColor(0, 0, 0);
            ofLine(points[a - 1].position.x, points[a - 1].position.y, points[a].position.x, points[a].position.y);
            ofSetColor(0,0,255);
            ofCircle(points[points.size() - 1].position.x, points[points.size() - 1].position.y, 10);
        }   
    }
    
    ofSetColor(255, 255, 255);
    
    float smoothShape = ofMap(mouseY, 0, ofGetHeight(), 0, 1, true);  
    int smoothSize = ofMap(mouseX, 0, ofGetWidth(), 0, 32, true);  
    
    
    
        
    if(points.size() > 0)
    {
        ofPolyline p;
        
        if(points.size() > 0)
            p = line.getSmoothed(3, 1);  
        ofSetColor(255, 255, 255);
        p.draw();
        
        vector<ofPoint> smoothedPoints = p.getVertices();
        
        for(int a = 0; a < smoothedPoints.size(); a++)
        {
            if(globalSmoothedPoint.size() < a + 1)
            {
                cout << "creo nuovo" << endl;
                graphicPoint tempPoint;
                tempPoint.position = smoothedPoints[a];
                tempPoint.color = 255;
                tempPoint.size = 10;
                globalSmoothedPoint.push_back(tempPoint);
            }
            
            for(int i = 0; i < a; i++)
            {
                float distance = smoothedPoints[a].distance(smoothedPoints[i]);
                
                ofSetColor(255, 255, 255, 10);
                if(distance > 80 && distance < 100)
                    ofLine(smoothedPoints[a], smoothedPoints[i]);
            }
            
          //  ofSetColor(255, 0, 255);
        //    ofCircle(globalSmoothedPoint[a].position, 10);
            //ofSetColor(255, 255, 255);
        
           // ofLine(smoothedPoints[a].x + ofRandom(0, 100), smoothedPoints[a].y + ofRandom(0, 100), smoothedPoints[a].x - ofRandom(0, 100), smoothedPoints[a].y + ofRandom(0, 100));
        }
        
    }
   
   if(removePoint)
   {
       ofFill();
       ofSetColor(255, 255, 255);
       ofCircle(mainPositionPoint.position, 100);
   }
    
    ofSetColor(255, 255, 255);
    if(points.size() > 0&&drawingPoint)
        ofCircle(points[points.size() -1].position, 10);
    

}

void testApp::keyPressed  (int key){
    switch (key) {
        case OF_KEY_RIGHT:
            windowWidth += 1;
            ofSetWindowShape(windowWidth, windowHeight);
            break;            
        case OF_KEY_LEFT:
            windowWidth -= 1;
            ofSetWindowShape(windowWidth, windowHeight);
            break;        
        case OF_KEY_UP:
            windowHeight += 1;
            ofSetWindowShape(windowWidth, windowHeight);
            break;            
        case OF_KEY_DOWN:
            windowHeight -= 1;
            ofSetWindowShape(windowWidth, windowHeight);
            cout << "Change Shape" << endl;
            break;  
            
        case 'd':
            posX += 1;
            ofSetWindowPosition(posX, posY);
            break;            
        case 'a':
            posX -= 1;
            ofSetWindowPosition(posX, posY);
            break;        
        case 'w':
            posY += 1;
            ofSetWindowPosition(posX, posY);
            break;            
        case 's':
            posY -= 1;
            ofSetWindowPosition(posX, posY);
            cout << "Change Shape" << endl;
            break;  
        case 'c':
            points.clear();
             break;
            break;
        case 'g':
            showGrid = !showGrid;
             break;
        case 'o':
            showOriginalLine = !showOriginalLine;
             break;
        case 'f':
            ofToggleFullscreen();
            break;
        default:
            break;
    }
}

void testApp::Delete(int id, vector<graphicPoint>& tempPoints)
{
    vector<graphicPoint>::iterator vItr = tempPoints.begin();
    while ( vItr != tempPoints.end() )
    {
        if ( vItr->id == id )
        {
            vItr = tempPoints.erase( vItr );
            break;
        }
        else
            vItr++;
    }
}