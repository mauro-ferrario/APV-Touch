//
//  graphicPoint.h
//  outputLycra
//
//  Created by Mauro on 06/04/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//


#include "ofMain.h"

#ifndef outputLycra_graphicPoint_h
#define outputLycra_graphicPoint_h


class graphicPoint
{
public:
    void            setup();
    void            draw();
    ofPoint         segmentPoint_1;
    ofPoint         segmentPoint_2;
    ofColor         color;
    ofPoint         position;
    float           size;
    int             id;
};


#endif
