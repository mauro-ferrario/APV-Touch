#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxUI.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "ofxCvBlobFinder.h"
#include "ofxCvContourFinderDepth.h"

struct personalPoint {
    ofPoint point;
    int id;
};

class testApp : public ofBaseApp{
    
public:
	void                                setup();
	void                                update();
	void                                draw();
	void                                keyPressed(int key);
    void                                exit();
    void                                guiEvent(ofxUIEventArgs &e);
	void                                setupRecording(string _filename = "");
    void                                initGui();
    void                                saveOriginalDepthPixels();
    void                                drawGrid(int row, int col);
    void                                deletePoint(int id, vector<personalPoint>& tempPoints);
    void                                getDepthInfo();                     // Prende tutte le informazioni per la profonditˆ e le elabora
    
    bool                                showGrid;
    string                              CONTROLLER_IP;
    int                                 CONTROLLER_PORT;
    string                              VISUAL_IP;
    int                                 VISUAL_PORT;
    string                              LOCALHOST;
    int                                 OUTPUT_LYCRA_PORT;
    bool                                originalDepthSaved;
    float                                 minRange;                           // Differenza di profonditˆ da cui devo iniziare a prendere in considerazione
    int                                 originalDepth[640*480];             // Profonditˆ "base" di tutti i pixel
    int                                 frameMedia;
    float                               posX;                               // Posizione x del rettangolo di mapping
    float                               posY;                               // Posizione y del rettangolo di mapping
    float                               widthArea;                          // Larghezza del rettangolo di mapping
    float                               heightArea;                         // Altezza del rettangolo di mapping
    float                               minBlobSize;
    float                               maxBlobSize;
    
    float                               distanceToPrevPoint;                // Decidere se usarlo per vedere quando sto disegnando un nuovo punto
    long int                            totPointFromBorn;                   // Contatore dei numeri dalla nascita, utile per assegnare un id a ogni punto
    long int                            timerResetDepth;                    // Contatore di frame passati per sapere quando resettare i pixel originali della profonditˆ
    ofImage                             finalImagePixelChange;              // Immagine che visualizza i pixel con profonditˆ cambiata
    //  ofPoint                             *oldPosition;                       
    ofPoint                             leftPointForEditor;
    ofPoint                             rightPointForEditor;
    vector<personalPoint>               points;
    vector<ofPoint>                     media;
    ofxUICanvas                         *gui;
    ofxOscSender                        sender;
    ofxCvContourFinderDepth             contourFinder;
	ofxOpenNIContext                    recordContext;
	ofxDepthGenerator                   recordDepth;
    ofxCvGrayscaleImage                 grayScaleImage;
    
#ifdef USE_IR
	ofxIRGenerator                      recordImage;
#else
	ofxImageGenerator                   recordImage;
#endif
    
#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	ofxHardwareDriver                   hardware;
#endif
    
    bool                                isDown;
    bool                                isDownPrev;
    
};

#endif
