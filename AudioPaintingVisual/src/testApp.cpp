#include "testApp.h"

bool savePdf = false;

//--------------------------------------------------------------
void testApp::setup(){    
    ofSetVerticalSync(false);
    ofSetFrameRate(30);
    
    int bufferSize = 256;    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    senderFrameRate.setup("127.0.0.1", 12346);
   // ofSetWindowPosition(2560, 0);
  //  ofSetFullscreen(true);
}

//--------------------------------------------------------------
void testApp::update(){
    audioPaintingVisual.update();
    
    
    ofxOscMessage m;
    m.setAddress("/frameRate");
    m.addFloatArg(ofGetFrameRate());
    senderFrameRate.sendMessage(m);
    
}

//--------------------------------------------------------------
void testApp::draw(){
    if(savePdf)
        ofBeginSaveScreenAsPDF(ofToString(ofGetFrameNum()) + ".pdf");
    audioPaintingVisual.draw();
    if(savePdf)
        ofEndSaveScreenAsPDF();
    savePdf = false;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch(key)
    {
            case 'f':
            cout << "DENTRO " << endl;
                audioPaintingVisual.showFrameRate = !audioPaintingVisual.showFrameRate;
                break;
    }
    if(key == 's')
    {
        screenImage.allocate(ofGetWindowWidth(), ofGetWindowHeight(), OF_IMAGE_COLOR);
        screenImage.grabScreen(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
        
        screenImage.saveImage(ofToString(ofGetFrameNum())+".jpg");
        savePdf = true;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::exit()
{
    cout << "See you in hell! \\m/_ .... by testApp" << endl;
    audioPaintingVisual.exit();
}


void testApp::audioIn(float * input, int bufferSize, int nChannels){	
    audioPaintingVisual.audioIn(input, bufferSize, nChannels);
}
