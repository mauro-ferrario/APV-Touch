//
//  Particle.h
//  emptyExample
//
//  Created by Mauro on 15/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofMain.h"

#pragma once

class Particle
{
public:
                        Particle(ofPoint newPosition, float newAngle, float newSpeed, ofColor newColor, float newSize, float newForce, float newFriction, float newSpring, bool visible, long int _idParticle);
	void				update();
	void				draw();
	void				followTarget();
	void				setTarget(ofPoint newTarget);
	void				applyForce(ofPoint &vector,ofPoint &force);
    void                applyRepulsion(float force, bool repulsionFromTarget);
    void                removeTarget();
	
	ofPoint				position;
	ofPoint				prevPosition;
	ofPoint				force;
	ofPoint				vector;
	ofPoint				target;
	ofColor				color;
	float				friction;
	float				spring;
	float				size;
	float				originalSize;
	float				angle;
	float				speed;
	float               audioCoeff;
    bool				visible;
    int                 idParticle;
};
