//
//  AudioPaintingVisual.cpp
//  emptyExample
//
//  Created by Mauro on 15/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AudioPaintingVisual.h"


AudioPaintingVisual::AudioPaintingVisual()
{
    //cout << "*** Init AudioPaintingVisual" << endl;
    timeInverted = 0;
    isInverted = false;
    backgroundColor.r = 0;
    backgroundColor.g = 0;
    backgroundColor.b = 0;
    globalAlphaCoefficent = 0;
    
    
    ofSetCircleResolution(60);
    ofEnableAlphaBlending();
    
    bDrawPoint = true;
    bDrawLinePointToPoint = true;
    bDrawTriangle = true;
    bConnectPointToPrev = true;
    
    minDistancePointToPoint = 0;
    maxDistancePointToPoint = 1000;
    distancePointToPoint = 100;
    
    minTriangleCoefficent = .000001;
    maxTriangleCoefficent = 3;
    triangleCoefficent = .5;
    
    
    bSameColorTriangle = true;
    bTimeAlphaTriangle = true;
    
    int bufferSize = 256; 
    left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
    
    smoothedVol     = 0.0;
	scaledVol		= 0.0;
	bufferCounter	= 0; 
    
    totPoints = 0;
    
    maxParticleSize = 1;
    
    sameColorTriangle.r = 255;
    sameColorTriangle.g = 255;
    sameColorTriangle.b = 255;
    
    emitterMode = false;
    fastEmitterMode = false;
    
    sameSpeed = 1;
    sameFriction = .3;
    sameSpring = .2;
    
    minSameSpeed = 1;
    maxSameSpeed = 500;
    
    minRepulsionForce = 50;
    maxRepulsionForcce = 2200;
    
    repulsionForce = 1;
    
    bTimeSpring = true;
    bTimeFriction = true;
    
    pAudioPaintingVisualOscReceiver = new AudioPaintingVisualOscReceiver(12345);
    pAudioPaintingVisualOscReceiver->pAudioPaintingVisual = this;
    countParticle = 0;
    
    showFrameRate = true;
    
    totNewPointToDraw = 0;
    totPointAlreadyDraw = 0;
    totPrevPoint = 0;
    volumeLimitToInvert = .5;
   // overlay.loadImage("/C++/of_preRelease_v007_osx/apps/Kernel/AudioPaintingVisual/bin/data/overlay.png");
}


void AudioPaintingVisual::audioIn(float * input, int bufferSize, int nChannels)
{
    float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	
    
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[ i * 2 ] * 0.5;
		right[i]	= input[ i * 2 +1 ] * 0.5;
        
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
        
	}
	
	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :) 
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
    volume = smoothedVol;
    
    if(volume < .05)
        globalAlphaCoefficent = ofMap(volume, 0, .2, 0, 1);
    else
        globalAlphaCoefficent = 1;
    
 //   globalAlphaCoefficent = 1;
    
	bufferCounter++;
}

void AudioPaintingVisual::update()
{
    if(points.size() == 0)
        countParticle = 0;
    
    totMax = 0;
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    //if(bTimeSpring)
    //    sameFriction = scaledVol;    
   // //cout << "Same Friction = " << sameFriction << endl;
    pAudioPaintingVisualOscReceiver->receiveMessagges();
    for(int a = 0; a < points.size(); a++)
    {
        
      //  if(!emitterMode)
      //  {
            points[a]->friction = sameFriction;
            points[a]->speed = sameSpeed;
            points[a]->spring = sameSpring;
      //  }
        
    //    points[a]->size = maxParticleSize;
        
        if(a < points.size() * .5)
        {
            if(left[(int)ofMap(a, 0, points.size()*.5, 0, 256)] > 0)
            {
                if(a < 10)
                    totMax += left[(int)ofMap(a, 0, points.size()*.5, 0, 256)];
                points[a]->audioCoeff = left[(int)ofMap(a, 0, points.size()*.5, 0, 256)] * 100;
                if(!emitterMode)
                    points[a]->applyRepulsion(repulsionForce * points[a]->audioCoeff , repulsionFromTarget);
                 //   points[a]->applyRepulsion(20000 * left[(int)ofMap(a, 0, points.size()*.5, 0, 256)], repulsionFromTarget);
            }
        }
        else
        {
            if(right[(int)ofMap(a, points.size() * .5, points.size() - 1, 0, 256)] > 0)
            {
                
                
                
                points[a]->audioCoeff = right[(int)ofMap(a, points.size() * .5, points.size() - 1, 0, 256)] * 100;
                if(!emitterMode)
                 //   points[a]->applyRepulsion(20000 * right[(int)ofMap(a, 0, points.size()*.5, 0, 256)], repulsionFromTarget);
                    points[a]->applyRepulsion(repulsionForce * points[a]->audioCoeff, repulsionFromTarget);
            }
        }
        
        if(points[a]->target == NULL)
        {
            if(points[a]->position.x > ofGetWindowWidth() + LIMIT_OUTISDE || points[a]->position.x < - LIMIT_OUTISDE || points[a]->position.y < -LIMIT_OUTISDE || points[a]->position.y > ofGetWindowHeight() + LIMIT_OUTISDE)
            {
                Particle* pPoint = points[a];
                //   points.begin() + a) = 0; // Devo mettere a 0 l'indirizzo a cui punta
                points.erase(points.begin() + a);
                delete pPoint;  // Cancello l'oggetto
                //pPoint = NULL;
            } 
        }
    
      if(bTimeSpring)
            points[a]->spring = points[a]->audioCoeff / 10 ;
      if(bTimeFriction)
            points[a]->friction = points[a]->audioCoeff / 10 ;
      points[a]->update();
    }
    
    
    
    //if(totMax > 1 && volume > .01)
    if(volume > volumeLimitToInvert)
    {
        if(!isInverted)
        {
            isInverted = true;
            timeInverted = 0;
            backgroundColor.invert();
        }
       // isInverted = !isInverted;
       // backgroundColor.invert();
    }
     
    
    if(isInverted)
        timeInverted++;
    
    if(timeInverted > 8)
    {
        timeInverted = 0;
        isInverted = false;
        backgroundColor.invert();
    }
    
    //overlay.width = ofGetWindowWidth();
    //overlay.height = ofGetWindowHeight();
}

void AudioPaintingVisual::draw()
{
    
    
    ofBackground(backgroundColor);
    
    
    
 //   ofPushMatrix();
  //  ofPushMatrix();
  //  if(OF_FULLSCREEN)
  //      ofTranslate(ofGetScreenWidth() * .5, ofGetScreenHeight() * .5);
  //  else
       // ofTranslate(ofGetWidth() * .5, ofGetHeight() * .5);
    //  ofScale(.2, .2);
  //  ofRotate(ofGetFrameNum());
    
  //  if(OF_FULLSCREEN)
  //      ofTranslate(-ofGetScreenWidth() * .5, -ofGetScreenHeight() * .5);
  //  else
    //    ofTranslate(-ofGetWidth() * .5, -ofGetHeight() * .5);
    
    Particle *oldPoint;
    
    for(int a = 0; a < points.size(); a++)
    {
        if(a > 0 && bConnectPointToPrev)
        {
            oldPoint = points[a - 1];
            ofSetColor(255, 255, 255, 100 * globalAlphaCoefficent);
            ofLine(oldPoint->position.x, oldPoint->position.y, points[a]->position.x, points[a]->position.y);
        }
        
        points[a]->color = backgroundColor;
        points[a]->color.invert();
        
        if(bDrawPoint)
            points[a]->visible = true;
        else
            points[a]->visible = false;
        if(bDrawLinePointToPoint || bDrawTriangle)
        {
            for(int i = 0; i < a; i++)
            {
                if(bDrawLinePointToPoint)
                    drawLinePointToPoint(*points[i], *points[a]);
                if(bDrawTriangle && i > 2)
                {
                    if(bTimeAlphaTriangle)
                        drawTriangle(*points[i], *points[i - 1], *points[a], points[i]->audioCoeff * globalAlphaCoefficent);
                    else
                        drawTriangle(*points[i], *points[i - 1], *points[a], globalAlphaCoefficent);
                }
            }
        }
        points[a]->draw();
    }
    
    ofSetColor(255, 255, 255);
    //overlay.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  //  ofPopStyle();
    
    
   // ofPopMatrix();
  //  ofPopMatrix();
    
    ofSetColor(255, 255, 255);
    if(showFrameRate)
    {
        ofDrawBitmapString(ofToString(ofGetFrameRate()), 0, 10);
        ofDrawBitmapString(ofToString(points.size()), 0, 30);
    }
    
}


void AudioPaintingVisual::setNewEmitterMode(bool changeEmitter, bool saveMode = true)
{
    if(saveMode)
        emitterMode = changeEmitter;
    if(changeEmitter)
        for(int a = 0; a < points.size(); a++)
            points[a]->removeTarget();
}

void AudioPaintingVisual::addPointToNewForm(float x, float y, int index)
{
    if(index < points.size())
    {
        ofPoint newTarget;
        newTarget.x = x;
        newTarget.y = y ;
        points[index]->setTarget(newTarget);
    }
    else
        points.push_back(this->initSinglePoint(x, y, !emitterMode, true));
}

void AudioPaintingVisual::DeleteParticle(int id)
{
   vector<Particle*>::iterator vItr = points.begin();
    
    while ( vItr != points.end() )
    {
        if ((*vItr)->idParticle == id )
        {
            vItr = points.erase( vItr ); // Will return next valid iterator
            break;
        }
        else
            vItr++;
    } 
}

void AudioPaintingVisual::addSinglePoint(float x, float y, bool outside)
{
    points.push_back(this->initSinglePoint(x, y, !emitterMode, outside));
}

void AudioPaintingVisual::removePoint(float x, float y)
{
    ofPoint tempPoint;
    tempPoint.x = x;
    tempPoint.y = y;
    
    for(int k = points.size() - 1; k >= 0 ; k--)
    {
        float distance = points[k]->target.distance(tempPoint);
        if( distance < 200)
            DeleteParticle(points[k]->idParticle);
        
    }
    points.push_back(this->initSinglePoint(x, y, !emitterMode, false));
}

Particle* AudioPaintingVisual::initSinglePoint(int x, int y, bool setTarget, bool outside)
{
   // ofSeedRandom();
    ofPoint newPoint;
    newPoint.x = x;
    newPoint.y = y;
    if(outside)
    {
        if(ofRandom(5) > 2.5)
            newPoint.x = ofGetWindowWidth() + ofRandom(0, LIMIT_OUTISDE * .5);
        else
            newPoint.x = -ofRandom(0, LIMIT_OUTISDE * .5);
        if(ofRandom(5) > 2.5)
            newPoint.y =  ofGetWindowHeight() + ofRandom(0, LIMIT_OUTISDE * .5);
        else
            newPoint.y =  -ofRandom(0, LIMIT_OUTISDE * .5);
    }
    
    
    
    ofColor color;
    color = 255;
    
    Particle* pParticle = new Particle(newPoint, ofRandom(0, 2 * PI), 10, color, 1, 0,  .3, .2, true, countParticle++);
    if(setTarget)
    {
        newPoint.x = x;
        newPoint.y = y;
        pParticle->setTarget(newPoint);
    }
    return pParticle;
}

void AudioPaintingVisual::drawTriangle(const Particle& firstPoint, const Particle& secondPoint, const Particle& thirdPoint, float alpha)
{
    float perimeter = abs(ofDist(firstPoint.position.x, firstPoint.position.y, thirdPoint.position.x, thirdPoint.position.y)) + abs(ofDist(firstPoint.position.x, firstPoint.position.y, secondPoint.position.x, secondPoint.position.y)) + abs(ofDist(secondPoint.position.x, secondPoint.position.y, thirdPoint.position.x, thirdPoint.position.y));
   // ofSetColor(0, 0, 0, 0);
    
    
    
    
    /*
     if(perimeter < ofGetWindowWidth() / 3 )
         ofSetColor(255, 0, 0, alpha);
     else if(perimeter >= ofGetWindowWidth() / 3 && perimeter < ofGetWindowWidth() / 2 )
         ofSetColor(0, 255, 0, alpha);
     else if(perimeter >= ofGetWindowWidth() / 2  && perimeter <= ofGetWindowWidth() )
         ofSetColor(0, 0, 255, alpha);
     */
    
    
    if(perimeter < 700 / triangleCoefficent)
        ofSetColor(255, 0, 0, alpha);
    else if(perimeter >= 700 / triangleCoefficent && perimeter < 900 / triangleCoefficent)
        ofSetColor(0, 255, 0, alpha);
    else if(perimeter >= 900 / triangleCoefficent && perimeter <= 1300 / triangleCoefficent)
        ofSetColor(0, 0, 255, alpha);


    
    /*
    if(perimeter < (ofGetWindowWidth() * triangleCoefficent / 30))
        ofSetColor(255, 0, 0, triangleCoefficent);
    else if(perimeter >= (ofGetWindowWidth() * triangleCoefficent / 30) && perimeter < (ofGetWindowWidth() * triangleCoefficent / 30) * 2)
        ofSetColor(0, 255, 0, triangleCoefficent);
    else if(perimeter >= (ofGetWindowWidth() * triangleCoefficent / 30) * 2 && perimeter < ofGetWindowWidth() * triangleCoefficent)
        ofSetColor(0, 0, 255, triangleCoefficent);
   */
    if(bSameColorTriangle)
    {
        if(isInverted)
            ofSetColor(sameColorTriangle.invert(), alpha);
        else
            ofSetColor(sameColorTriangle, alpha);
    }
    
    if(perimeter < 1300 / triangleCoefficent)
    {
        ofFill();
        ofTriangle(firstPoint.position.x, firstPoint.position.y, secondPoint.position.x, secondPoint.position.y, thirdPoint.position.x, thirdPoint.position.y);
    }
}

void AudioPaintingVisual::clear()
{
    clearLastPoints(points.size());
    // Quando cancello le particelle (tutte assieme) devo farle uscire verso l'esterno... se le cancello con la mano devo cancellare subito
   // return;
  /*  int size = points.size();
    for(int a = 0; a < size; a++)
    {
        Particle* pParticle = points[points.size() - 1];
        points.pop_back();
        delete pParticle;
    }
   */
}

void AudioPaintingVisual::clearLastPoints(int totToRemove)
{
   // points.clear();
    
   // for(int a = points.size() - 1; a >= 0; a--)
     //   points[points.size() - 1 - z]->removeTarget();
    
    for(int z = 0; z < totToRemove; z++)
    {
        points[points.size() - 1 - z]->removeTarget();
        /*
         Particle* pParticle = points[points.size() - 1];
         points.pop_back();
         delete pParticle;
       */  
    }
     
}

void AudioPaintingVisual::drawLinePointToPoint(const Particle& firstPoint, const Particle& secondPoint)
{
    float distance = abs(ofDist(firstPoint.position.x, firstPoint.position.y, secondPoint.position.x, secondPoint.position.y));
    if(distance > distancePointToPoint || distancePointToPoint < 100)  return;
    ofNoFill();
    ofSetLineWidth(1);
    
    if(isInverted)
        ofSetColor(0, 0, 0, 10 * globalAlphaCoefficent);
    else
        ofSetColor(255, 255, 255, 10 * globalAlphaCoefficent);
    glBegin(GL_LINES);
    glVertex3f( firstPoint.position.x, firstPoint.position.y, 0.0f);
    glVertex3f(secondPoint.position.x, secondPoint.position.y, 0.0f);
    glEnd( );
}

void AudioPaintingVisual::setFriction(float newFriction)
{
    sameFriction = ofClamp(newFriction, .1, 1);
}

void AudioPaintingVisual::setSpeed(float newSpeed)
{
    sameSpeed = newSpeed;
}

void AudioPaintingVisual::setRepulsionForce(float newRepulsionForce)
{
    //cout << "NEW REPULSION = "<< newRepulsionForce << endl;
    repulsionForce = newRepulsionForce;
}

void AudioPaintingVisual::setSpring(float newSpring)
{
    sameSpring = ofClamp(newSpring, .1, 1);
}

void AudioPaintingVisual::exit()
{
    //cout << "See you in hell! \\m/_ .... by AudioPaintingVisual" << endl;
}