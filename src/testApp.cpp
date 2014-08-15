#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetLogLevel( OF_LOG_VERBOSE );	
	
	ofSetFrameRate(999);
	ofSetVerticalSync( false );
	
	//ofSetFrameRate( 60 );
	//ofSetVerticalSync( true );
		
	fontWorld.loadFont( "Fonts/DIN.otf", 18, true, false, true );

	oculusRift.init( 1280, 800, 4 );
	oculusRift.setPosition( 0, 0, 0 );
    oculusRift.setDoWarping(false);
    
    fov = 90;
    step = 1;
    zoom = 1.2;
    
    
    cam.setPosition(0, 1000, 0);
    cam.setOrientation(ofVec3f(0,-1,0));
	
	lastUpdateTime = ofGetElapsedTimef();
    
    kinect.init();
    kinect.open();
    kinect.setRegistration(true);
    
    bDebug = false;
    
    pointLight.setPosition(0, 640, 0);
    pointLight.setSpecularColor(ofFloatColor(1.0,1.0,1.0));
    
    ofEnableDepthTest();
    
    bDrawUFO = false;
    alpha = 255;
    
    colorMode = 0;
    vMode = 0;
    

}


//--------------------------------------------------------------
void testApp::update()
{
    
    kinect.update();
    
	float currTime = ofGetElapsedTimef();
	float frameDeltaTime = currTime - lastUpdateTime;
	lastUpdateTime = currTime;
	
	if(	ofGetKeyPressed('i') ) { oculusRift.setInterOcularDistance( oculusRift.getInterOcularDistance() + 0.001f ); }
	else if( ofGetKeyPressed('o') ) { oculusRift.setInterOcularDistance( oculusRift.getInterOcularDistance() - 0.001f ); }
	else if( ofGetKeyPressed('k') ) { oculusRift.setInterOcularDistance( oculusRift.getInterOcularDistance() + 1.0f ); }
	else if( ofGetKeyPressed('l') ) { oculusRift.setInterOcularDistance( oculusRift.getInterOcularDistance() - 1.0f ); }
	
	if(	ofGetKeyPressed(OF_KEY_UP) )    { oculusRift.dolly(  30.0f * frameDeltaTime ); }
	if( ofGetKeyPressed(OF_KEY_DOWN) )  { oculusRift.dolly( -30.0f * frameDeltaTime ); }
	if( ofGetKeyPressed(OF_KEY_LEFT) )  { oculusRift.truck(  30.0f * frameDeltaTime ); }
	if( ofGetKeyPressed(OF_KEY_RIGHT) ) { oculusRift.truck( -30.0f * frameDeltaTime ); }
    
//    alpha = ((sin(ofGetElapsedTimef()*0.21) + 1)*0.5) * ((sin(ofGetElapsedTimef()*0.21) + 1)*0.5) * ((sin(ofGetElapsedTimef()*0.21) + 1)*0.5) * 255;
	
}


//--------------------------------------------------------------
void testApp::draw()
{
    glEnable( GL_DEPTH_TEST );
    if (bDebug) {
        oculusRift.setNeedSensorReadingThisFrame( true );
        
        cam.begin();
        {
            ofPushMatrix();
            {
                ofPushMatrix();
                {
                    positionPointCloud();
                    drawUFO();

                    ofPushMatrix();
                    {
                        ofMultMatrix(oculusRift.getHeadsetOrientationMat());
                        ofSetColor( 255, 0, 0 );
                        ofBox( 200 );
                        
                        ofSetColor( 0, 0, 255 );
                        ofPushMatrix();
                        {
                            ofTranslate(0, 0, -100);
//                            ofScale(1,1,20);
                            ofBox( 100 );
                        }
                        ofPopMatrix();
                    }
                    ofPopMatrix();
                }
                ofPopMatrix();
            }
            ofPopMatrix();
        }
        cam.end();
        
        glDisable( GL_DEPTH_TEST );
    }
    else
    {
        oculusRift.beginRenderSceneLeftEye();
        {
            positionPointCloud();
            if (bDrawUFO) drawUFO();
        }
        oculusRift.endRenderSceneLeftEye();
        
        oculusRift.beginRenderSceneRightEye();
        {
            positionPointCloud();
            if (bDrawUFO) drawUFO();
        }
        oculusRift.endRenderSceneRightEye();
        
        
        ofSetColor( 255 );
        oculusRift.draw( ofVec2f(0,0), ofVec2f( ofGetWidth(), ofGetHeight() ) );
    }
}


//--------------------------------------------------------------
void testApp::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
    
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
                ofColor meshCol;
				switch (colorMode) {
                    case 0:
                        meshCol = kinect.getColorAt(x,y);
                        break;
                        
                    case 1:
                        meshCol.set((int)ofMap(kinect.getDistanceAt(x,y), 0, 10000, 255, 0));
                        break;
                        
                    case 2:
                        meshCol.set(255,255,0);
                        meshCol.setHue((int)ofMap(kinect.getDistanceAt(x,y), 0, 10000, 0, 255));
                        break;
                        
                    case 3:
                        meshCol = kinect.getColorAt(x,y);
                        meshCol.setHue((int)ofMap(kinect.getDistanceAt(x,y), 0, 10000, 0, 255));
                        break;
                }

                ofVec3f v = kinect.getWorldCoordinateAt(x, y);
                switch (vMode) {
                    case 0:
                        break;
                        
                    case 1:
                        v.z += ofSignedNoise(ofGetElapsedTimef(), x * 0.002, y * 0.002) * 500;
                        break;
                        
                    case 2:
                        v.z += ofSignedNoise(ofGetElapsedTimef() * 0.7, v.z * 0.002) * 500;
                        break;
                }
                
                mesh.addColor(meshCol);
				mesh.addVertex(v);
			}
		}
	}
    
	glPointSize(3);
	ofPushMatrix();
    ofScale(zoom,zoom,1);
	ofScale(1, -1, -1); // the projected points are 'upside down' and 'backwards'
	mesh.drawVertices();
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::positionPointCloud(){
    
    
    ofPushMatrix();
    {
        ofMultMatrix( oculusRift.getHeadsetOrientationMat() );
        drawPointCloud();
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::drawUFO(){
    float radius = 300 + ofNoise(ofGetElapsedTimef() * 0.3) * 700;
    float x = sin(ofGetElapsedTimef() * 0.4) * radius;
    float z = cos(ofGetElapsedTimef() * 0.4) * radius;
    float y = ofSignedNoise(ofGetElapsedTimef() * 0.008, x*0.0001, z*0.0001) * 420 - 100;
    
    
    ofEnableLighting();
    pointLight.enable();
    glEnable( GL_DEPTH_TEST );
    ofPushMatrix();
    {
        ofSetColor(100, alpha);
        ofTranslate(x,y,z);
        cockpit.draw();
        
        ofPushMatrix();
        {
            ofSetColor(150,alpha);
            ofScale(2, 0.5, 2);
            saucer.draw();
        }
        ofPopMatrix();
        
        
        float red = sin(ofGetElapsedTimef()*4) * sin(ofGetElapsedTimef()*4) * 255;
        ofSetColor(red,0,0, alpha);
        
        for (float r = 0; r < 2*PI; r+= 2*PI/5.0){
            
            float x = sin(r) * 40;
            float z = cos(r) * 40;
            
            ofPushMatrix();
            {
                ofTranslate(x, 0, z);
                ofScale(0.2, 0.2, 0.2);
                cockpit.draw();
            }
            ofPopMatrix();
        }
    }
    ofPopMatrix();
    
    
    ofDisableLighting();
}

//--------------------------------------------------------------
void testApp::exit() {
//	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	if( key == 'f' )
	{
		ofToggleFullscreen();
	}
	if( key == 'w' )
	{
		oculusRift.setDoWarping( !oculusRift.getDoWarping() );
	}
    if ( key == 'd' ) {
        bDebug ^= true;
    }
    
    if ( key == OF_KEY_UP) {
        fov++;
        oculusRift.setFov(fov);
//        cout << "fov set -> " << fov << endl;
    }
    if ( key == OF_KEY_DOWN ) {
        fov--;
        oculusRift.setFov(fov);
//        cout << "fov set -> " << fov << endl;
    }
    
    if ( key == 's') {
        step++;
    }
    
    if (key == 'S') {
        step--;
        if (step < 1) step = 1;
    }
    
    if (key == OF_KEY_RIGHT) {
        zoom+=0.1;
    }
    
    if (key == OF_KEY_LEFT) {
        zoom-=0.1;
    }
    
    if (key == 'u') {
        bDrawUFO ^= true;
    }
    
    if (key == 'c') {
        colorMode++;
        colorMode%=4;
    }
    
    if (key == 'v') {
        vMode++;
        vMode%=3;
    }
    
    if (key == 'r') {
        fov = 90;
        step = 1;
        zoom = 1.2;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}