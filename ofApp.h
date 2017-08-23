/************************************************************
description
	This is a modfied version of "gpuParticleSystemExample".

éQçlURL
	my study of shader
		https://github.com/SJ-magic-study-oF/study__shader/tree/master/8.%20geometry
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"

/************************************************************
************************************************************/

/**************************************************
// Struct for doing PingPong quickly and easy
//
// Because on GPU you can´t write over the texture that you are reading we are
// using to pair of ofFbo attached together on what we call pingPongBuffer 
// Learn more about Ping-Pong at:
//
// http://www.comp.nus.edu/~ashwinna/docs/PingPong_FBO.pdf
// http://www.seas.upenn.edu/~cis565/fbo.htm#setupgl4
**************************************************/
struct pingPongBuffer {
public:
	void allocate( int _width, int _height, int _internalformat = GL_RGBA){
		for(int i = 0; i < 2; i++){
			FBOs[i].allocate(_width,_height, _internalformat );
			FBOs[i].getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
		}
		
		clear();
	}
	
	void swap(){
		std::swap(src,dst);
	}
	
	void clear(){
		for(int i = 0; i < 2; i++){
			FBOs[i].begin();
			ofClear(0,255);
			FBOs[i].end();
		}
	}
	
	ofFbo& operator[]( int n ){ return FBOs[n];}
	ofFbo   *src = &FBOs[0];	   // Source	   ->  Ping
	ofFbo   *dst = &FBOs[1];	   // Destination  ->  Pong

private:
	ofFbo   FBOs[2];
};


/**************************************************
**************************************************/
class ofApp : public ofBaseApp{
private:
	enum{
		WIDTH = 1280,
		HEIGHT = 720,
	};
	
	bool b_fullScreen;
	
public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofShader	updatePos;
	ofShader	updateVel;
	ofShader	updateRender;
		
	pingPongBuffer posPingPong;
	pingPongBuffer velPingPong;
	
	ofFbo   renderFBO;
	
	ofImage sparkImg;
	
	float   timeStep;
	float   particleSize;
	
	int	 width, height;
	int	 imgWidth, imgHeight;
	int	 numParticles;
	int	 textureRes;
	
	
	ofVboMesh mesh;
};
