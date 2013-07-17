
//
//  Particle.cèèpp.cpp
//  emptyExample
//
//  Created by Mauro on 15/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Particle.h"
#include "testApp.h"

Particle::Particle(ofPoint newPosition, float newAngle, float newSpeed, ofColor newColor, float newSize, float newForce, float newFriction, float newSpring, bool visible, long int _idParticle)
{
    idParticle = _idParticle;
	originalSize = newSize;
	size = newSize;
	color = newColor;
	position = newPosition;
	angle = newAngle;
	speed = newSpeed;
	vector.x = cos(angle) * speed;
	vector.y = sin(angle) * speed;
	
    force = newForce;
    friction = newFriction;
    spring = newSpring;
	visible = visible;
    audioCoeff = 1;
}

void Particle::draw()
{	
	if(size == 0 || !visible)
        return;
	ofFill();
    ofSetColor(color.r, color.g, color.b,  ofMap(ofClamp(size * audioCoeff, 0, 1), 0, 1, 0, 255) * ((testApp*)ofGetAppPtr())->audioPaintingVisual.globalAlphaCoefficent);  
	ofCircle(position.x, position.y, ofClamp(size * audioCoeff, 0, 3));
}

void Particle::setTarget(ofPoint newTarget)
{
	target = newTarget;
}

void Particle::removeTarget()
{
    this->target = NULL;
}

void Particle::update()
{
	if(target != NULL)
		followTarget();
	else
		applyForce(vector, force);
	position += vector;
}


void Particle::applyRepulsion(float force, bool repulsionFromTarget)
{
    ofPoint repulsionPoint;
    
    if(repulsionFromTarget)
        repulsionPoint = target;
    else
    {
        repulsionPoint.x = ofGetWindowWidth() * .5;
        repulsionPoint.y = ofGetWindowHeight() * .5;
        
    }    
    
    float radius = 600;
    ofPoint distCoord = this->position -  repulsionPoint;
    
    float dist = ofDist(repulsionPoint.x, repulsionPoint.y, this->position.x, this->position.y);
    
    float angleDirection = atan2(distCoord.y, distCoord.x);
    
    ofPoint rejectForce;
    dist = 1; // Annullo la distanza 
    rejectForce.x = force * abs(dist) * cos(angleDirection);
    rejectForce.y = force * abs(dist) * sin(angleDirection);
    rejectForce *= .1;
    this->force = rejectForce;
}

void Particle::applyForce(ofPoint &vector,ofPoint &force)
{
	if(force == 0) 
        return;
	vector += force;
	force  *= .9;
}

void Particle::followTarget()
{
	ofPoint distance;
	ofPoint	acceleration = 0;
	distance = target - position;
	float angleDirection = atan2( distance.y, distance.x);
	ofPoint tempVector;
	
	tempVector.x = cos(angleDirection) * speed;
	tempVector.y = sin(angleDirection) * speed;
	
	applyForce(tempVector, force);
	acceleration = tempVector * spring;
	
	vector += acceleration;
	vector *= friction;
}