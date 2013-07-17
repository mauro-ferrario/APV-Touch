#pragma once

#include "ofxSimpleGuiValueControl.h"


class ofxSimpleGuiVar : public ofxSimpleGuiValueControl<string> {
public:
	bool			beToggle;
	bool			beenPressed;
	
	ofxSimpleGuiVar(string name, string &value);	
	void setup();
	
	void loadFromXML(ofxXmlSettings &XML);	
	void saveToXML(ofxXmlSettings &XML);
	void keyPressed( int key );
    
	string getValue();
	void setValue(string b);
	void toggle();
	
	void setToggleMode(bool b);
	
	void onPress(int x, int y, int button);
	void onRelease(int x, int y, int button);
	void draw(float x, float y);
};
