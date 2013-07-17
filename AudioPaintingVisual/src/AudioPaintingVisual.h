//
//  AudioPaintingVisual.h
//  emptyExample
//
//  Created by Mauro on 15/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofMain.h"
#include "Particle.h"
#include "AudioPaintingVisualOscReceiver.h"
#define     LIMIT_OUTISDE 1000

#pragma once


class AudioPaintingVisual
{
public:
                            AudioPaintingVisual();
    void                    exit();
    void                    audioIn(float * input, int bufferSize, int nChannels); 
    void                    update();
    void                    addSinglePoint(float x, float y, bool outside);
    void                    draw();
    
    
   
    
    Particle*               initSinglePoint(int x, int y, bool setTarget, bool outside);
    void                    drawTriangle(const Particle& firstPoint, const Particle& secondPoint, const Particle& thirdPoint, float alpha);
    void                    setFriction(float newFriction);
    void                    setSpeed(float newSpeed);
    void                    setSpring(float newSpring);
    void                    drawLinePointToPoint(const Particle& firstPoint, const Particle& secondPoint);
    void                    clearLastPoints(int totToRemove);
    void                    addPointToNewForm(float x, float y, int index);
    void                    setNewEmitterMode(bool changeEmitter, bool saveMode);
    void                    clear();
    void                    setRepulsionForce(float newRepulsionForce);
    ofSoundStream                            soundStream;
    AudioPaintingVisualOscReceiver*          pAudioPaintingVisualOscReceiver;
    void                    DeleteParticle(int id);
    void                    removePoint(float x, float y);
    
    ofColor                 backgroundColor;
    
    vector<float>           left;
    vector<float>           right;
    
    float                   smoothedVol;
    float                   scaledVol;
    int                     bufferCounter;
    
    bool                    bDrawPoint;
    bool                    bDrawLinePointToPoint;
    bool                    bDrawTriangle;
    bool                    bConnectPointToPrev;
    
    bool                    bSameColorTriangle;
    bool                    bTimeAlphaTriangle;
    bool                    bTimeSpeed;
    bool                    bTimeFriction;
    bool                    bTimeSpring;
    
    float                   minDistancePointToPoint;
    float                   maxDistancePointToPoint;
    float                   distancePointToPoint;
    
    float                   minRepulsionForce;
    float                   maxRepulsionForcce;
    float                   repulsionForce;
    
    float                   minTriangleCoefficent;
    float                   maxTriangleCoefficent;
    float                   triangleCoefficent;
    
    int                     totPoints;
    int                     maxParticleSize;
    
    bool                    emitterMode;
    bool                    fastEmitterMode;
    bool                    repulsionFromTarget;
    
    float                   sameSpeed;
    float                   sameFriction;
    float                   sameSpring;
    
    float                   minSameSpeed;
    float                   maxSameSpeed;
    
    float                   volumeLimitToInvert;
    
    ofColor                 sameColorTriangle;
    long int                countParticle;
    
    vector<Particle*>       points;
    
    
    float                   volume;
    float                   totMax;
    int                     maggiore;
    bool                    isInverted;
    int                     timeInverted;
    
    float                   globalAlphaCoefficent;
    
    ofImage                 overlay;
    
    bool                    showFrameRate;
    
    
    int                     totNewPointToDraw;      // Variabile per il conteggio dei nuovi punti da disegnare quando si cambia forma
    int                     totPointAlreadyDraw;
    int                     totPrevPoint;
};