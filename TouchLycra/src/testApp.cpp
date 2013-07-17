#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    /* Load From XML */
    
    CONTROLLER_IP = "192.168.1.2";
    CONTROLLER_PORT = 12346;
    
    VISUAL_IP = "192.168.1.2";
    VISUAL_PORT = 12345;
    
    LOCALHOST = "127.0.0.1";
    OUTPUT_LYCRA_PORT = 12345;
    
    
    showGrid = true;
    
    posX = 100;
    posY = 100;
    widthArea = 100;
    heightArea = 100;
    
    originalDepthSaved = false;
    
    minBlobSize = 10;
    maxBlobSize = 27200;
    
    
    minRange = 10;
    
	setupRecording();
    
    timerResetDepth = 0;
    
    grayScaleImage.allocate(640, 480);
    finalImagePixelChange.allocate(640, 480, OF_IMAGE_COLOR);
    
    contourFinder.setupDepthPixel(640, 480);
    
    
    initGui();
    
    ofBackground(0, 0, 0);
    //ofSetFrameRate(60);
    
    isDown = false;
    isDownPrev = false;
} 

void testApp::initGui()
{
    gui = new ofxUICanvas(100, 10, 500, 1000);  
    gui->setFont("GUI/MEMORIA_.ttf");
    gui->setFontSize(OFX_UI_FONT_SMALL, 4);
    gui->addWidgetDown(new ofxUISlider(0, 0, 200, 10, 0, 640, 100, "Pos x"));
    gui->addWidgetDown(new ofxUISlider(210, 0, 200, 10, 0, 480, 100, "Pos y"));
    gui->addWidgetDown(new ofxUISlider(420, 0, 200, 10, 0, 640, 100, "Area width"));
    gui->addWidgetDown(new ofxUISlider(660, 0, 200, 10, 0, 480, 100, "Area height"));
    gui->addWidgetDown(new ofxUISpacer(0, 0, 200, 1));
    gui->addWidgetDown(new ofxUISlider(0, 50, 200, 10, 0, 600, 10, "Min Blob Size"));
    gui->addWidgetDown(new ofxUISlider(0, 100, 200, 10, 300, 100000, 27200, "Max Blox Size"));
    gui->addWidgetDown(new ofxUISpacer(0, 0, 200, 1));
    gui->addWidgetDown(new ofxUISlider(210, 50, 200, 10, 0, 400, 50, "Min Depth Range"));
    gui->addWidgetDown(new ofxUISpacer(0, 0, 200, 1));
    gui->addWidgetDown(new ofxUISlider(660, 50, 200, 10, 0, 1, .5, "Perc is hand"));
    gui->addWidgetDown(new ofxUISlider(660, 100, 200, 10, 1000, 20000, 8000, "Area is hand"));
    gui->addWidgetDown(new ofxUISpacer(0, 0, 200, 1));
    gui->addWidgetDown(new ofxUISlider(420, 50, 200, 10, 0, 200.0, 100, "Distance to prev point"));
    gui->addWidgetDown(new ofxUISpacer(0, 0, 200, 1));
    gui->addWidgetDown(new ofxUIToggle(10, 10, true, "Show grid" , OFX_UI_FONT_SMALL));
    gui->addWidgetDown(new ofxUISpacer(0, 0, 200, 1));
    gui->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_SMALL));
    
    ofColor fillColor;
    
    fillColor.r = 227;
    fillColor.g = 59;
    fillColor.b = 2;
    
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL, fillColor);
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_FILL_HIGHLIGHT, fillColor);
    
    fillColor.r = 50;
    fillColor.g = 50;
    fillColor.b = 50;
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_BACK, fillColor);
    gui->setWidgetColor(OFX_UI_WIDGET_COLOR_OUTLINE, fillColor);
    
    fillColor.r = 255;
    fillColor.g = 255;
    fillColor.b = 255;
    fillColor.a = 0;
    gui->setColorBack(fillColor);
    
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml");
}

void testApp::setupRecording(string _filename) 
{    
#if defined (TARGET_OSX)            //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	hardware.setup();				// libusb direct control of motor, LED and accelerometers
	hardware.setLedOption(LED_OFF); // turn off the led just for yacks (or for live installation/performances ;-)
#endif
	recordContext.setup();          // all nodes created by code -> NOT using the xml config file at all
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);
    recordContext.toggleRegisterViewport();
	recordContext.toggleMirror();
}

void testApp::update()
{
    bool addPoint = false;
    getDepthInfo();
    
    ofxOscMessage m;
    
    isDown = false;
    
    // Se ho 2 tocchi e sono in editMode
    
    if(contourFinder.nBlobs == 2 || contourFinder.editMode)
    {
        if(contourFinder.nBlobs != 2) 
        {
            leftPointForEditor = NULL;
            rightPointForEditor = NULL;
            return;
        }
        
        if(contourFinder.maxPointPixelDepth[0].x < contourFinder.maxPointPixelDepth[1].x)
        {
            leftPointForEditor = contourFinder.maxPointPixelDepth[0];
            rightPointForEditor = contourFinder.maxPointPixelDepth[1];
        }
        else
        {
            leftPointForEditor = contourFinder.maxPointPixelDepth[1];
            rightPointForEditor = contourFinder.maxPointPixelDepth[0];
        }
        
        /*
        m.setAddress( "/changeDistancePointToPoint" );
        m.addFloatArg(ofMap(abs(leftPointForEditor.y - posY), 0, heightArea, 0, 1));
        sender.setup(CONTROLLER_IP, CONTROLLER_PORT);
        sender.sendMessage(m);
        
        m.clear();
        
        m.setAddress( "/changeTriangleCoefficent" );  // changeDistancePointToPoint
        m.addFloatArg(ofMap(abs(rightPointForEditor.y - posY), 0, heightArea, 0, 1));
        sender.setup(CONTROLLER_IP, CONTROLLER_PORT);
        sender.sendMessage(m);
         */
    }
    else
    {
        leftPointForEditor = NULL;
        rightPointForEditor = NULL;
        
        for(int a = 0; a < contourFinder.nBlobs; a++)
        {         
            isDown = true;
            
            ofPoint tempoPoint = contourFinder.maxPointPixelDepth[a];
      //      tempoPoint.x += 0;
       //     tempoPoint.y += 0;
            
            personalPoint tempPoint;
            
            tempPoint.point = contourFinder.maxPointPixelDepth[a];
            tempPoint.id = totPointFromBorn++;
            
            if(contourFinder.isHand[a])
            {
                // Cancello tutti i punti all'interno di una certa distanza
                
                for(int k = points.size() - 1; k >= 0 ; k--)
                {
                    float distance = points[k].point.distance(tempPoint.point);
                    if( distance < 500)
                        deletePoint(points[k].id, points);                            
                }
                // Se non ho pi punti posso resettare la variabile degli id dei punti
                
                if(points.size() == 0)
                    totPointFromBorn = 0;
            }
            else
            {
                if(isDownPrev)
                {
                    if(points.size() > 1)
                    {
                        if(tempPoint.point.distance(points[points.size() - 1].point) > distanceToPrevPoint)
                        {
                            points.push_back(tempPoint);
                            addPoint = true;
                        }
                    }
                    else
                    {
                        points.push_back(tempPoint);
                        addPoint = true;
                    }
                    
                }
            }
            
            m.clear();
            
            if(isDownPrev)
            {
                if(contourFinder.isHand[a])
                    m.setAddress( "/removePoint" );                
                else
                    m.setAddress( "/addSinglePoint" );
            }           
            
            
            m.addFloatArg((contourFinder.maxPointPixelDepth[a].x - posX)/widthArea);
            m.addFloatArg((contourFinder.maxPointPixelDepth[a].y - posY)/ heightArea);
            
            sender.setup(LOCALHOST, OUTPUT_LYCRA_PORT);
            sender.sendMessage(m);
            sender.setup(VISUAL_IP, VISUAL_PORT);
             
            
            sender.setup(CONTROLLER_IP, CONTROLLER_PORT);
            
            if(isDownPrev)
            {
                if(contourFinder.isHand[a])
                    sender.sendMessage(m);
                else
                {
                    if(addPoint)
                    {
                        sender.sendMessage(m);
                    }
                }
            }
        }
    }
    
    if(contourFinder.nBlobs == 0)
        timerResetDepth++;
    else
        timerResetDepth = 0;
    
    if(timerResetDepth >= 60)
        saveOriginalDepthPixels();
    
    isDownPrev = isDown;
}

void testApp::getDepthInfo()
{
#ifdef TARGET_OSX                   // only working on Mac at the moment
	hardware.update();
#endif    
    recordContext.update();
    recordDepth.update();
    recordImage.update();
    
    unsigned char *pixels = recordImage.getPixels();
    unsigned char *finalPixels = finalImagePixelChange.getPixels();
    
    grayScaleImage.clear();
    grayScaleImage.allocate(640,480);
    
    unsigned char *grayScalePixels = grayScaleImage.getPixels();
    
    for(int a = 0; a < (640*480); a++)
    {
        ofPoint actualPoint;
        
        actualPoint.y = (int) a/640;
        actualPoint.x = a - (actualPoint.y * 640);
        
        // Effettua i controlli solamente sui pixel all'interno del rettangolo di mapping
        
        if(originalDepthSaved&&(actualPoint.x > posX && actualPoint.x < posX + widthArea && actualPoint.y > posY && actualPoint.y < posY + widthArea))
        {         
            int actualDepth = recordDepth.getPixelDepth(actualPoint.x, actualPoint.y);
            // Calcolo la differenza di profonditˆ
            int diff = originalDepth[a] - actualDepth;
            
            // Ogni volta assegna la differenza dei pixel all'array in countourFinder
            contourFinder.diffDepthPixels[a] = diff;
            
            if(originalDepth[a]!= actualDepth && diff > minRange)
            {
                finalPixels[(a * 3)] = 255;
                finalPixels[(a * 3) + 1] = 255;
                finalPixels[(a * 3) + 2] = 0;
                
                grayScalePixels[a] = 255;
            }
            else
            {
                finalPixels[(a * 3)] = pixels[(a * 3)];
                finalPixels[(a * 3) + 1] = pixels[(a * 3) + 1];
                finalPixels[(a * 3) + 2] = pixels[(a * 3) + 2]; 
                
                grayScalePixels[a] = 0;
            }
        }
        else
        {
            finalPixels[(a * 3)] = pixels[(a * 3)];
            finalPixels[(a * 3) + 1] = pixels[(a * 3) + 1];
            finalPixels[(a * 3) + 2] = pixels[(a * 3) + 2]; 
            
            grayScalePixels[a]  = 0;
        }
        
        // Per tutti i punti esterni al rettangolo di mapping
        
        if(!(actualPoint.x > posX && actualPoint.x < posX + widthArea && actualPoint.y > posY && actualPoint.y < posY + heightArea))
        {
            finalPixels[(a * 3)] = pixels[(a * 3)];
            finalPixels[(a * 3) + 1] = pixels[(a * 3) + 1];
            finalPixels[(a * 3) + 2] = pixels[(a * 3) + 2]; 
            
            grayScalePixels[a]  = 0;
        }
    }
    
    grayScaleImage.updateTexture();
    finalImagePixelChange.update();
    
    contourFinder.findContours(grayScaleImage, minBlobSize, maxBlobSize, 100, false);
    contourFinder.setDepth();
}

void testApp::draw(){
    
    float scale = .5;
    
    ofPushMatrix();
    ofPushStyle();
    ofScale(scale, scale);
    
    ofSetColor(255, 255, 255);
    ofFill();
    
    recordImage.draw(0, 0, 640, 480);
    grayScaleImage.draw(640,  480, 640, 480);
    contourFinder.drawBlobMapDepth(0, 480);
    contourFinder.draw(0, 480);
    contourFinder.drawBlobMapDepth(640, 0);
    
   // finalImagePixelChange.draw(640, 0,640,480);
    
    ofNoFill();
    ofRect(0, 0, 640, 480);
    ofRect(640, 0, 640, 480);
    ofRect(0, 480, 640, 480);
    ofRect(640, 480, 640, 480);
    
    if(showGrid)
        drawGrid(5, 5);
    
    ofPopStyle();
    ofPopMatrix();
    
    ofFill();
    
    gui->getRect()->x = ((640 * 2) * scale) + 10;
    gui->getRect()->y = 0;
        
    if(contourFinder.nBlobs == 2 || contourFinder.editMode)
    {     
        // cout << "Edit mode - To do" << endl;   
        /*
        ofSetColor(255, 0, 255);
        ofLine(leftPointForEditor, rightPointForEditor);
        ofSetColor(255, 255, 0);
        ofRect(0, ofGetWindowHeight() - 200, ofGetWindowWidth(), 200);
        ofDrawBitmapString("Edit mode", ofGetWindowWidth() - 98, ofGetWindowHeight() - 5);
        */
    }
    else
    {
        for(int a = 0; a < contourFinder.nBlobs; a++)
        {
            if(contourFinder.isHand[a])
                ofDrawBitmapString("Is hand!", 10, (480 * 2 * scale) + 10);     
            else
                ofDrawBitmapString("is fingerr!", 10, (480 * 2 * scale) + 10);            
            
            ofPushMatrix();
            ofScale(scale, scale);
            
            ofFill();
            ofCircle(contourFinder.maxPointPixelDepth[a].x, contourFinder.maxPointPixelDepth[a].y, 50); 
            
            ofSetColor(227, 59, 2);
            ofLine(contourFinder.maxPointPixelDepth[a].x, 0, contourFinder.maxPointPixelDepth[a].x, ofGetWindowHeight());
            ofLine(0, contourFinder.maxPointPixelDepth[a].y, ofGetWindowWidth(), contourFinder.maxPointPixelDepth[a].y);
            
            ofPopMatrix();
        }
    }
    
    // Disegna tutti i punti
    
    ofPushStyle();
    ofSetColor(255, 0, 0);
    for(int i = 1; i < points.size(); i++)
        ofLine((points[i - 1].point.x) * 2, (points[i - 1].point.y) * 2,(points[i].point.x) * 2, (points[i].point.y) * 2);
	
    ofPopStyle();
    
    gui->draw();
}

void testApp::drawGrid(int row, int col)
{
    ofNoFill();
    ofSetColor(255,255,0);
    ofRect(posX, posY, widthArea, heightArea);
    for(int a = 0; a < col; a++)
        ofLine(posX + (a * widthArea/5), posY, posX + (a * widthArea/5), posY + heightArea);
    
    for(int a = 0; a < row; a++)
        ofLine(posX, posY + (a * heightArea/5), posX + widthArea, posY + (a * heightArea/5));
}

void testApp::deletePoint(int id, vector<personalPoint>& tempPoints)
{
    vector<personalPoint>::iterator vItr = tempPoints.begin();
    while ( vItr != tempPoints.end() )
    {
        if ( vItr->id == id )
        {
            vItr = tempPoints.erase( vItr );
            break;
        }
        else
            vItr++;        
    }
}

void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    gui = 0;
    cout << "*** Kill CheckLycraTouch ***" << endl;
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    //cout << name << "." << endl;
    if(name == "Pos x")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        posX = slider->getScaledValue();        
    }
    else if(name == "Pos y")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        posY = slider->getScaledValue();        
    }
    else if(name == "Area width")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        widthArea = slider->getScaledValue();        
    }
    else if(name == "Area height")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        heightArea = slider->getScaledValue();        
    }
    else if(name == "Min Depth Range")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        minRange = slider->getScaledValue(); 
    }
    else if(name == "Min Blob Size")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        minBlobSize = slider->getScaledValue();        
    }
    else if(name == "Max Blox Size")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        maxBlobSize = slider->getScaledValue(); 
    }
    else if(name == "Distance to prev point")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        distanceToPrevPoint = slider->getScaledValue();
    }
    else if(name == "Perc is hand")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        contourFinder.percIsHand = slider->getScaledValue();
    }
    else if(name == "Area is hand")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        contourFinder.areaIsHand = slider->getScaledValue();
    }
    else if(name == "Show grid")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        showGrid = toggle->getValue();
    }
}

void testApp::saveOriginalDepthPixels()
{
    for(int a = 0; a < (640*480); a++)
        originalDepth[a] = recordDepth.getPixelDepth(a -(((int) a/640) * 640),(int) a/640);
    originalDepthSaved = true;
    timerResetDepth = 0;
}

void testApp::keyPressed(int key){
    ofxOscMessage m;
    switch (key) {
        case 's':
            saveOriginalDepthPixels();
            break;
        case 'c':
            points.clear();
            totPointFromBorn = 0;
            m.setAddress( "/clear" );            
            sender.setup(LOCALHOST, OUTPUT_LYCRA_PORT);
            sender.sendMessage(m);
            sender.setup(CONTROLLER_IP, CONTROLLER_PORT);
            sender.sendMessage(m);
            break;
        default:
            break;
    }
}