/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	ofSetVerticalSync(false);
	ofSetFrameRate(60);
	
	particleSize = 30.0f;
	timeStep = 0.005f;
	numParticles = 5000;
	
	ofSetWindowShape(WIDTH, HEIGHT);
	b_fullScreen = false;
	
	/********************
	********************/
	width = ofGetWindowWidth();
	height = ofGetWindowHeight();
	
	string shadersFolder;
	/*
	if(ofIsGLProgrammableRenderer())	shadersFolder="shaders_gl3";
	else								shadersFolder="shaders";
	*/
	shadersFolder="shaders";

	/*
	if(ofIsGLProgrammableRenderer()){
		updatePos.load(shadersFolder+"/passthru.vert", shadersFolder+"/posUpdate.frag");// shader for updating the texture that store the particles position on RG channels
		updateVel.load(shadersFolder+"/passthru.vert", shadersFolder+"/velUpdate.frag");// shader for updating the texture that store the particles velocity on RG channels
	}else{
		updatePos.load("",shadersFolder+"/posUpdate.frag");// shader for updating the texture that store the particles position on RG channels
		updateVel.load("",shadersFolder+"/velUpdate.frag");// shader for updating the texture that store the particles velocity on RG channels
	}
	*/
	updatePos.load("",shadersFolder+"/posUpdate.frag");// shader for updating the texture that store the particles position on RG channels
	updateVel.load("",shadersFolder+"/velUpdate.frag");// shader for updating the texture that store the particles velocity on RG channels
	
	/********************
	cf.	https://github.com/SJ-magic-study-oF/study__shader/tree/master/8.%20geometry
	********************/
	updateRender.setGeometryInputType(GL_POINTS); // num inputs = 1
	updateRender.setGeometryOutputType(GL_TRIANGLE_STRIP); // 
	updateRender.setGeometryOutputCount(6);
	updateRender.load(shadersFolder+"/render.vert",shadersFolder+"/render.frag",shadersFolder+"/render.geom");
	
	/********************
	********************/
	textureRes = (int)sqrt((float)numParticles);
	numParticles = textureRes * textureRes;
	printf("(textureRes, numParticles) = (%d, %d)\n", textureRes, numParticles);
	
	// 1. Making arrays of float pixels with position information
	vector<float> pos(numParticles*3);
	for (int y = 0; y < textureRes; y++){
		for (int x = 0; x < textureRes; x++){
			int i = textureRes * y + x;
			
			pos[i*3 + 0] = ofRandom(1.0); //x*offset;
			pos[i*3 + 1] = ofRandom(1.0); //y*offset;
			pos[i*3 + 2] = 0.0;
		}
	}
	// Load this information in to the FBO's texture
	posPingPong.allocate(textureRes, textureRes, GL_RGB32F);
	posPingPong.src->getTexture().loadData(pos.data(), textureRes, textureRes, GL_RGB);
	posPingPong.dst->getTexture().loadData(pos.data(), textureRes, textureRes, GL_RGB);
	
	
	// 2. Making arrays of float pixels with velocity information and the load it to a texture
	vector<float> vel(numParticles*3);
	for (int i = 0; i < numParticles; i++){
		vel[i*3 + 0] = ofRandom(-1.0,1.0);
		vel[i*3 + 1] = ofRandom(-1.0,1.0);
		vel[i*3 + 2] = 1.0;
	}
	// Load this information in to the FBO's texture
	velPingPong.allocate(textureRes, textureRes, GL_RGB32F);
	velPingPong.src->getTexture().loadData(vel.data(), textureRes, textureRes, GL_RGB);
	velPingPong.dst->getTexture().loadData(vel.data(), textureRes, textureRes, GL_RGB);
	
	// Loading and setings of the variables of the textures of the particles
	sparkImg.load("spark.png");
	imgWidth = sparkImg.getWidth();
	imgHeight = sparkImg.getHeight();
	
	// Allocate the final 
	renderFBO.allocate(width, height, GL_RGB32F);
	// renderFBO.allocate(width, height, GL_RGBA); // αがないとblendできないかと思ったけど、特に問題なくblendできたので.
	renderFBO.begin();
	ofClear(0, 0, 0, 255);
	renderFBO.end();


	mesh.setMode(OF_PRIMITIVE_POINTS);
	for(int y = 0; y < textureRes; y++){
		for(int x = 0; x < textureRes; x++){
			mesh.addVertex(ofVec3f(x,y)); // 未使用. mesh.addVertex(ofVec3f(0, 0));でもok. see:shader of "render".
			mesh.addTexCoord(ofVec2f(x, y));
		}
	}

}

//--------------------------------------------------------------
void ofApp::update(){
	/********************
	今のposとvelのまま行った時のnext posを算出.
	もしrangeの外に出てしまう場合は、velを変更して跳ね返す.
	同時に速度も落としている.
	********************/
	velPingPong.dst->begin();
		ofClear(0);
		updateVel.begin();
		updateVel.setUniformTexture("backbuffer", velPingPong.src->getTexture(), 0);
		updateVel.setUniformTexture("posData", posPingPong.src->getTexture(), 1);
		updateVel.setUniform1i("resolution", (int)textureRes); 
		updateVel.setUniform2f("screen", (float)width, (float)height);
		updateVel.setUniform1f("timestep", (float)timeStep);
	
		velPingPong.src->draw(0, 0);
		
		updateVel.end();
	velPingPong.dst->end();
	
	velPingPong.swap();
	
	/********************
	********************/
	posPingPong.dst->begin();
		ofClear(0);
		updatePos.begin();
		updatePos.setUniformTexture("prevPosData", posPingPong.src->getTexture(), 0);
		updatePos.setUniformTexture("velData", velPingPong.src->getTexture(), 1);
		updatePos.setUniform1f("timestep",(float) timeStep );
		
		posPingPong.src->draw(0, 0);
		
		updatePos.end();
	posPingPong.dst->end();
	
	posPingPong.swap();
	

	/********************
	********************/
	renderFBO.begin();
		ofClear(0,0,0,0);
		
		updateRender.begin();
		updateRender.setUniformTexture("posTex", posPingPong.src->getTexture(), 0);
		updateRender.setUniformTexture("sparkTex", sparkImg.getTexture() , 1);
		
		updateRender.setUniform1i("resolution", (float)textureRes); 
		updateRender.setUniform2f("screen", (float)width, (float)height);
		updateRender.setUniform1f("size", (float)particleSize); 
		updateRender.setUniform1f("imgWidth", (float)sparkImg.getWidth());
		updateRender.setUniform1f("imgHeight", (float)sparkImg.getHeight());
		
		ofPushStyle();
		ofEnableBlendMode( OF_BLENDMODE_ADD );
		ofSetColor(255);
		
		mesh.draw();
		
		ofDisableBlendMode();
		glEnd();
		
		updateRender.end();
		ofPopStyle();
	renderFBO.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	
	ofSetColor(255,255,255, 255);
	renderFBO.draw(0,0, ofGetWidth(), ofGetHeight());
	
	ofSetColor(255);
	ofDrawBitmapString("Fps: " + ofToString( ofGetFrameRate()), 15,15);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 'f':
			ofToggleFullscreen();
			
			b_fullScreen = !b_fullScreen;
			if(!b_fullScreen) ofSetWindowShape(WIDTH, HEIGHT);
			break;
			
		case ' ':
			ofSaveScreen("image.png");
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
