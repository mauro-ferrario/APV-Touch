//
//  graphicPoint.cpp
//  outputLycra
//
//  Created by Mauro on 06/04/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "graphicPoint.h"



void graphicPoint::draw()
{
    ofSetColor(color);
    ofCircle(position, size);
}