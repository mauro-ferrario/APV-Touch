//
//  AudioPaintingVisualOscReceiver.h
//  emptyExample
//
//  Created by Mauro on 16/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class AudioPaintingVisual;

class AudioPaintingVisualOscReceiver
{
    
public:
                                        AudioPaintingVisualOscReceiver(int port);
    void                                receiveMessagges();
    ofxOscReceiver                      receiver;
    AudioPaintingVisual*                pAudioPaintingVisual;
    
};