#pragma once

#include "ofMain.h"
#include "ofxOculusRift.h"
#include "ofxKinect.h"

class testApp : public ofBaseApp
{
public:

    void setup();
    void update();
    void draw();
    void exit();
    
    void drawSceneGeometry();
    void drawPointCloud();
    void positionPointCloud();
    void drawUFO();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxKinect           kinect;
    ofxOculusRift		oculusRift;

    float				lastUpdateTime;

    ofLight				light;
    ofEasyCam			cam;
    
    float               pointCloudZ;
    float               fov;

    ofTrueTypeFont		fontWorld;
    
    bool                bDebug;
    
    int step;
    float zoom;
    
    ofSpherePrimitive saucer, cockpit;
    
    ofLight pointLight;
    
    bool bDrawUFO;
    
    float alpha;
    
    int colorMode, vMode;
    
	
};
