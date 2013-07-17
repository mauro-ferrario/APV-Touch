
#include "ofxSimpleGuiVar.h"


ofxSimpleGuiVar::ofxSimpleGuiVar(string name, string &value) : ofxSimpleGuiValueControl<string>(name, value) {
	beToggle	= false;
	beenPressed = false;
	controlType = "Val";
	setup();
}

void ofxSimpleGuiVar::setup() {
	setSize(config->gridSize.x - config->padding.x, config->buttonHeight);
}

void ofxSimpleGuiVar::loadFromXML(ofxXmlSettings &XML) {
	setValue(ofToString((XML.getValue(controlType + "_" + key + ":value", "cacca"))));
}

void ofxSimpleGuiVar::saveToXML(ofxXmlSettings &XML) {
	XML.addTag(controlType + "_" + key);
	XML.pushTag(controlType + "_" + key);
	XML.addValue("name", name);
	XML.addValue("value", getValue());
	XML.popTag();
}

void ofxSimpleGuiVar::keyPressed( int key ) {
	if(key==keyboardShortcut) toggle();
}

string ofxSimpleGuiVar::getValue() {
	return (*value);
}

void ofxSimpleGuiVar::setValue(string b) {
	(*value) = b;
}

void ofxSimpleGuiVar::toggle() {
	//(*value) = !(*value); 
}

void ofxSimpleGuiVar::setToggleMode(bool b) {
	beToggle = b;
}

void ofxSimpleGuiVar::onPress(int x, int y, int button) {
	/*beenPressed = true;	
	if(beToggle) (*value) = !(*value); 
	else (*value) = true;
     */
}

void ofxSimpleGuiVar::onRelease(int x, int y, int button) {
//	if(!beToggle) (*value) = false;
}

void ofxSimpleGuiVar::draw(float x, float y) {
	setPos(x, y);
	
	glPushMatrix();
	glTranslatef(x, y, 0);
	
	ofEnableAlphaBlending();
	ofFill();
	setTextBGColor();
	ofRect(0, 0, width, height);
	
	// if a toggle
    /*
	if((*value) && beToggle) {
		setTextColor();
		//ofLine(0, 0, box.width, box.height);
		//ofLine(box.width, 0, 0, box.height);
	}
     */
	
	setTextColor();
	ofDrawBitmapString(name, 3, 15);
	ofDrawBitmapString(ofToString(*value), 23, 15);
	
	ofDisableAlphaBlending();
	
	glPopMatrix();
}
