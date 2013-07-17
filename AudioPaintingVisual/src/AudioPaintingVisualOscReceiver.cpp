//
//  AudioPaintingVisualOscReceiver.cpp
//  emptyExample
//
//  Created by Mauro on 16/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AudioPaintingVisualOscReceiver.h"
#include "testApp.h"


bool isFullscreen = false;

AudioPaintingVisualOscReceiver::AudioPaintingVisualOscReceiver(int port)
{
   // //cout << pAudioPaintingVisual << endl;
    receiver.setup( port );
}


void AudioPaintingVisualOscReceiver::receiveMessagges()
{
    bool setSameFriction = false;
    
    bool setSameSpring = false;
    
	while( receiver.hasWaitingMessages() )
	{
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
       // cout <<  m.getAddress() << endl;
        /*
		if ( m.getAddress() == "/changeDistancePointToPoint" )
		{
            cout << "changeDistancePointToPoint" << endl;
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.distancePointToPoint = ofMap(m.getArgAsFloat( 0 ), 0, 1, ((testApp*)ofGetAppPtr())->audioPaintingVisual.minDistancePointToPoint, ((testApp*)ofGetAppPtr())->audioPaintingVisual.maxDistancePointToPoint);
        }
        if ( m.getAddress() == "/changeTriangleCoefficent" )
		{
             ((testApp*)ofGetAppPtr())->audioPaintingVisual.triangleCoefficent = ofMap(m.getArgAsFloat( 0 ), 0, 1, ((testApp*)ofGetAppPtr())->audioPaintingVisual.minTriangleCoefficent, ((testApp*)ofGetAppPtr())->audioPaintingVisual.maxTriangleCoefficent);
        }*/
        if ( m.getAddress() == "/output" )
		{
            
            
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.bDrawPoint = m.getArgAsInt32( 0 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.bDrawTriangle = m.getArgAsInt32( 1 );
            
            
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.bSameColorTriangle = m.getArgAsInt32( 2 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.bTimeAlphaTriangle = m.getArgAsInt32( 3 );
            
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.bDrawLinePointToPoint = m.getArgAsInt32( 4 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.bConnectPointToPrev = m.getArgAsInt32( 5 );
            
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.repulsionFromTarget = m.getArgAsInt32( 6 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.distancePointToPoint = ofMap(m.getArgAsFloat( 7 ), 0, 1, ((testApp*)ofGetAppPtr())->audioPaintingVisual.minDistancePointToPoint, ((testApp*)ofGetAppPtr())->audioPaintingVisual.maxDistancePointToPoint);
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.triangleCoefficent = ofMap(m.getArgAsFloat( 8 ), 0, 1, ((testApp*)ofGetAppPtr())->audioPaintingVisual.minTriangleCoefficent, ((testApp*)ofGetAppPtr())->audioPaintingVisual.maxTriangleCoefficent);
            
            
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.maxDistancePointToPoint = m.getArgAsFloat( 9 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.maxTriangleCoefficent = m.getArgAsFloat( 10 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.volumeLimitToInvert = m.getArgAsFloat( 11 );
		}
        else if ( m.getAddress() == "/triangle/sameColor" )
        {
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.sameColorTriangle.r =  m.getArgAsInt32( 0 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.sameColorTriangle.g =  m.getArgAsInt32( 1 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.sameColorTriangle.b =  m.getArgAsInt32( 2 );
        }  
        else if ( m.getAddress() == "/emitterMode" )
        {
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.setNewEmitterMode(m.getArgAsInt32( 0 ), true);
        }
        else if ( m.getAddress() == "/fastEmitterMode" )
        {
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.setNewEmitterMode(true, false);
        }
        else if ( m.getAddress() == "/addSinglePoint" )
        {
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.addSinglePoint(m.getArgAsFloat( 0 ) * ofGetWindowWidth(), m.getArgAsFloat( 1 ) * ofGetWindowHeight(), false);
        }
        else if ( m.getAddress() == "/removePoint" )
        {
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.removePoint(m.getArgAsFloat( 0 ) * ofGetWindowWidth(), m.getArgAsFloat( 1 ) * ofGetWindowHeight());
        }
        else if ( m.getAddress() == "/clear" )
        {
            cout << "CLEAR" << endl;
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.clear();
        }
        else if ( m.getAddress() == "/setFullscreen" )
        {
            cout << "CAmbio full" << endl;
            if(isFullscreen) return;
            //cout << "Change " << endl;
            if( m.getArgAsInt32( 0 ) == 0)
                cout << "Non esce mai dal fullscreen una volta entrato" << endl;
                //      ofSetFullscreen(false);
            else
            {
                cout << "Vado full" << endl;
                isFullscreen = true;
                ofSetFullscreen(true);
            }
        }
        else if ( m.getAddress() == "/setWindowSize" )
        {
          //  ofSetWindowShape(m.getArgAsInt32( 0 ), m.getArgAsInt32( 1 ));
        }
        else if ( m.getAddress() == "/loadShape" )
        {
            
            int totPointBefore = ((testApp*)ofGetAppPtr())->audioPaintingVisual.points.size();
            cout << "**************************" << endl;
            cout << "TOT PREV POINT " <<  totPointBefore << endl;
            cout << "TOT NEW POINT " <<  m.getArgAsInt32( 0 )<< endl;
            
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.totNewPointToDraw = m.getArgAsInt32( 0 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.totPointAlreadyDraw = 0;
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.totPrevPoint = ((testApp*)ofGetAppPtr())->audioPaintingVisual.points.size();
            
            
//   if( ((testApp*)ofGetAppPtr())->audioPaintingVisual.totNewPointToDraw >  ((testApp*)ofGetAppPtr())->audioPaintingVisual.totPrevPoint)
            
            if(((testApp*)ofGetAppPtr())->audioPaintingVisual.totPrevPoint - m.getArgAsInt32( 0 ) > 0)
            {
                ((testApp*)ofGetAppPtr())->audioPaintingVisual.clearLastPoints(((testApp*)ofGetAppPtr())->audioPaintingVisual.totPrevPoint - m.getArgAsInt32( 0 ));
            }
            
        }
        else if ( m.getAddress() == "/loadDirect" )
        {
            int totPointBefore = ((testApp*)ofGetAppPtr())->audioPaintingVisual.points.size();
            for(int a = 0; a < m.getArgAsInt32( 0 ); a++)
            {
                ((testApp*)ofGetAppPtr())->audioPaintingVisual.addPointToNewForm(m.getArgAsFloat( 1 + (a * 2) ) * ofGetWindowWidth(), m.getArgAsFloat( (1 + (a * 2) + 1) ) * ofGetWindowHeight(), a);
            }
            int diff = totPointBefore - m.getArgAsInt32( 0 );
            
            if(diff > 0)
                ((testApp*)ofGetAppPtr())->audioPaintingVisual.clearLastPoints(diff);
        }
        else if ( m.getAddress() == "/addPoint" )
        {
            //cout << "TOT PUNTI = " << m.getArgAsInt32( 0 ) << endl;
            
          //  int totPointToDraw = m.getArgAsInt32( 0 );
           // int totPointFree = m.getArgAsInt32( 0 ) - m.getArgAsInt32( 1 );
           
            cout << "PUNTI DA DISEGNARE = " <<  m.getArgAsFloat( 0 ) << endl;
            
            float pointToDrawNow = m.getArgAsFloat( 0 );
            
            // ((testApp*)ofGetAppPtr())->audioPaintingVisual.points.size()
            
            int totPointToDraw = ((testApp*)ofGetAppPtr())->audioPaintingVisual.totNewPointToDraw;
            int totPointAlreadyDraw = ((testApp*)ofGetAppPtr())->audioPaintingVisual.totPointAlreadyDraw;
            int totPrevPoint = ((testApp*)ofGetAppPtr())->audioPaintingVisual.totPrevPoint;
            
            if(totPrevPoint > 0 && totPrevPoint > totPointAlreadyDraw)
            {
                cout << "CAMBIO TARGET " << endl;
                int cont = 0;
              //  errore qui sotto
                for(int a = totPointAlreadyDraw; a < ((testApp*)ofGetAppPtr())->audioPaintingVisual.totNewPointToDraw; a++)
                {
                    if(cont <= pointToDrawNow)
                    {
                        if(a < totPrevPoint)
                        {
                            ofPoint tempPoint;
                            tempPoint.x = m.getArgAsFloat( 1 + (cont * 2) ) * ofGetWindowWidth();
                            tempPoint.y = m.getArgAsFloat( (1 + (cont * 2) + 1) ) * ofGetWindowHeight();
                            ((testApp*)ofGetAppPtr())->audioPaintingVisual.points[a]->setTarget(tempPoint);
                            ((testApp*)ofGetAppPtr())->audioPaintingVisual.totPointAlreadyDraw++;
                            
                        }
                        else
                        {
                            cout << "DENTRO QUI" << endl;
                            ((testApp*)ofGetAppPtr())->audioPaintingVisual.totPointAlreadyDraw++;
                            ((testApp*)ofGetAppPtr())->audioPaintingVisual.addSinglePoint(m.getArgAsFloat( 1 + (cont * 2) ) * ofGetWindowWidth(), m.getArgAsFloat( (1 + (cont * 2) + 1) ) * ofGetWindowHeight(), true); 
                        }
                        cont++;
                    }
                }
            }
            else
            {
                for(int a = 0; a < m.getArgAsFloat( 0 ); a++)
                {
                    ((testApp*)ofGetAppPtr())->audioPaintingVisual.totPointAlreadyDraw++;
                    ((testApp*)ofGetAppPtr())->audioPaintingVisual.addSinglePoint(m.getArgAsFloat( 1 + (a * 2) ) * ofGetWindowWidth(), m.getArgAsFloat( (1 + (a * 2) + 1) ) * ofGetWindowHeight(), true);
                }
            }
            
            
            /*
            if(totPointFree > 0  )
            {
                if(totPointFree
            }
             */
            /*
            
            cout << "DISEGNO PUNTI RIMANENTI " <<  m.getArgAsInt32( 1 ) << endl;
            for(int a = 0; a < m.getArgAsInt32( 0 ); a++)
                ((testApp*)ofGetAppPtr())->audioPaintingVisual.addSinglePoint(m.getArgAsFloat( 1 + (a * 2) ) * ofGetWindowWidth(), m.getArgAsFloat( (1 + (a * 2) + 1) ) * ofGetWindowHeight());
            */
        }
        else if ( m.getAddress() == "/particle/timeSpring" )
        {
            setSameSpring = m.getArgAsInt32( 0 );
            
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.bTimeSpring = setSameSpring;
        }
        else if ( m.getAddress() == "/particle/newSpring" )
        {
            //cout << "TIME STRING = " << setSameSpring << endl;
            if(!setSameSpring)
            {
                //cout << "SET SAME SPRING " << m.getArgAsFloat( 0 ) << endl;
                ((testApp*)ofGetAppPtr())->audioPaintingVisual.setSpring(m.getArgAsFloat( 0 ));
            }
        }
        else if ( m.getAddress() == "/particle/timeFriction" )
        {
            setSameFriction = m.getArgAsInt32( 0 );
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.bTimeFriction = setSameFriction;
        }
        else if ( m.getAddress() == "/particle/newFriction" )
        {
            if(!setSameFriction)
                ((testApp*)ofGetAppPtr())->audioPaintingVisual.setFriction(m.getArgAsFloat( 0 ));
        }
        else if ( m.getAddress() == "/particle/newSpeed" )
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.setSpeed(ofMap(m.getArgAsFloat( 0 ) / 100, 0, 1, ((testApp*)ofGetAppPtr())->audioPaintingVisual.minSameSpeed, ((testApp*)ofGetAppPtr())->audioPaintingVisual.maxSameSpeed)); 
        else if ( m.getAddress() == "/particle/newRepulsionForce" )
        {  //cout << m.getArgAsFloat( 0 ) << endl;
            ((testApp*)ofGetAppPtr())->audioPaintingVisual.setRepulsionForce(ofMap(m.getArgAsFloat( 0 ), 0, 1, ((testApp*)ofGetAppPtr())->audioPaintingVisual.minRepulsionForce, ((testApp*)ofGetAppPtr())->audioPaintingVisual.maxRepulsionForcce)); 
        }
        else
        {
         //   //cout << m.getAddress() << endl;
        //    if( m.getAddress() == "/midi/cc7/1")
         //       audioPaintingVisual->maxDistance = (m.getArgAsFloat( 0 ) * 200) + 1;
        }
         
	}
    
}
