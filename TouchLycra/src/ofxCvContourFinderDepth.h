//
//  ofxCvContourFinderDepth.h
//  checkLycraTouch
//
//  Created by Mauro on 24/02/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef checkLycraTouch_ofxCvContourFinderDepth_h
#define checkLycraTouch_ofxCvContourFinderDepth_h

#include "ofMain.h"
#include "ofxCvContourFinder.h"

struct depthPixelPosition {
    int arrayPos;
    int depthValue;
};

class ofxCvContourFinderDepth : public ofxCvContourFinder
{
public:
    vector<int>                                 diffDepthPixels;
    vector< vector<depthPixelPosition> >        blobDepthPixels;        // Vettore contenente gli array della profondità dei pixel dei blob
    vector<float>                               maxBlobDepth;           // Vettore contenente il valore massimo di profondità di ogni singolo pixel
    vector<ofPoint>                             maxPointPixelDepth;     // Vettore  contente il punto del pixel con profondità maggiore per ogni blob
    vector<float>                               depthMiddle;            // Vettore contente le medie di profondità di ogni blob
    vector<bool>                                isHand;                 // Vettore che tiene conto se ogni blob è considerato una mano
    vector<bool>                                prevIsHand;             // Vettore che tiene conto se nel momento precedente il blob era stato considerato mano
    ofImage                                     imageBlobDepth;  
    bool                                        editMode;
    float                                       percIsHand;
    float                                       areaIsHand;
    
    void  setupDepthPixel(int width, int height)
    {
        percIsHand = 0.5;
        areaIsHand = 8000;
        editMode = false;
        imageBlobDepth.allocate(width, height, OF_IMAGE_COLOR);
        for(int a = 0; a < width * height; a++)
            diffDepthPixels.push_back(0);
    }
    
    void setDepth()
    {
        // Svuota l'array di profondità di pgni pixel
        for(int a = 0; a < blobDepthPixels.size(); a++)
            blobDepthPixels[a].clear();
        
        blobDepthPixels.clear();
        depthMiddle.clear();
        maxBlobDepth.clear();
        isHand.clear();
        maxPointPixelDepth.clear();
        
        unsigned char *finalPixels = imageBlobDepth.getPixels();
        for(int a = 0; a < 640*480 * 3; a++)
            finalPixels[a] = 0;
        
        // Con queste if/else if, si fa in modo che quando si entra in editMode, se ne esce solamente staccando tutte e 2 le dita
        
        if(blobs.size() == 0)
        {
            editMode = false;
            prevIsHand.clear();
        }
        else if(blobs.size() == 2)
        {
          //  editMode = true;
        }
        
        for(int a = 0; a < this->blobs.size(); a++)
        {
            if(prevIsHand.size() < blobs.size())   
                prevIsHand.push_back(0);
            
            float maxDepthTempValue = 0;
            
            vector<depthPixelPosition> newVector;
            blobDepthPixels.push_back(newVector);
            
            ofPoint fakePoint;
            maxPointPixelDepth.push_back(fakePoint);
            
            depthMiddle.push_back(0);
            isHand.push_back(false);
            
            
            for(int cols = blobs[a].boundingRect.x; cols < blobs[a].boundingRect.x + blobs[a].boundingRect.width; cols++)
            {
                for(int rows = blobs[a].boundingRect.y; rows < blobs[a].boundingRect.y + blobs[a].boundingRect.height; rows++)
                {
                    depthPixelPosition newDepthPixelPosition;
                    newDepthPixelPosition.arrayPos = cols + rows * imageBlobDepth.width;
                    newDepthPixelPosition.depthValue = diffDepthPixels[newDepthPixelPosition.arrayPos];
                    if (newDepthPixelPosition.depthValue > maxDepthTempValue) {
                        maxDepthTempValue = newDepthPixelPosition.depthValue;
                        maxPointPixelDepth[a].x = cols;
                        maxPointPixelDepth[a].y = rows;
                        
                    }
                    depthMiddle[a] += newDepthPixelPosition.depthValue;
                    blobDepthPixels[a].push_back(newDepthPixelPosition);
                }
            }
            
            depthMiddle[a] /= blobDepthPixels[a].size();
            maxBlobDepth.push_back(maxDepthTempValue);
            
            // Evidenzia in rosso le zone più profonde
            
            for(int i = 0; i < blobDepthPixels[a].size(); i++)
            {
                if(blobDepthPixels[a][i].depthValue > 0)
                {
                    finalPixels[(blobDepthPixels[a][i].arrayPos * 3)] = ofMap(blobDepthPixels[a][i].depthValue, 0, maxBlobDepth[a], 0, 255);
                    finalPixels[(blobDepthPixels[a][i].arrayPos * 3) + 1] = 0;
                    finalPixels[(blobDepthPixels[a][i].arrayPos * 3) + 2] = 255 - ofMap(blobDepthPixels[a][i].depthValue, 0, maxBlobDepth[a], 0, 255);
                }
            }
            
            float middleMapping = ofMap(depthMiddle[a], 0, maxBlobDepth[a], 0, 1);
            
            if(middleMapping > percIsHand && blobs[a].area > areaIsHand )
                isHand[a] = true;
            else
                isHand[a] = false;
            
            // Se nel momento precedente il blob era una mano.. allora rimane mano fin quando non stacca dal touch
            
            if(prevIsHand[a])
                isHand[a] = true;
            
            prevIsHand[a] =  isHand[a];
        }
        imageBlobDepth.update();   
    }
    
    void drawBlobMapDepth(int x, int y)
    {
        imageBlobDepth.draw(x, y);
    }
    
};


#endif
