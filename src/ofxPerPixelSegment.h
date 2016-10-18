#pragma once

#include "ofLog.h"

#include "ofxCv.h"

#include "FeatureComputer.hpp"
#include "Classifier.h"
#include "LcBasic.h"
#include "HandDetector.hpp"

class ofxPerPixelSegment {
public:
    ofxPerPixelSegment()
    :modelsLoaded(false)
    ,detectReady(false)
    ,targetWidth(320)
    ,numModelsTest(10)
    ,stepSize(3)
    ,featureSet("rvl") {
    }
    
    // set targetWidth to resize the input (smaller is faster)
    void setTargetWidth(int targetWidth) {
        this->targetWidth = targetWidth;
    }
    
    // number of models used to compute a single pixel response
    // must be less than the number of training models
    // only used at test time
    void setNumModelsTest(int numModelsTest) {
        this->numModelsTest = numModelsTest;
    }
    
    // runs detector on every 'step_size' pixels
    // only used at test time
    // bigger means faster but you lose resolution
    // you need post-processing to get contours
    void setStepSize(int stepSize) {
        this->stepSize = stepSize;
    }
    
    // types of features to use (you will over-fit if you do not have enough data)
    // r: RGB (5x5 patch)
    // v: HSV
    // l: LAB
    // b: BRIEF descriptor
    // o: ORB descriptor
    // s: SIFT descriptor
    // u: SURF descriptor
    // h: HOG descriptor
    void setFeatureSet(string featureSet) {
        this->featureSet = featureSet;
    }
    
    void setPath(string path = "handtracking") {
        modelPrefix = ofToDataPath(path + "/models");
        globfeatPrefix = ofToDataPath(path + "/globfeat");
    }
    
    void loadModels() {
        detector.testInitialize(modelPrefix, globfeatPrefix, featureSet, numModelsTest, targetWidth);
        modelsLoaded = true;
    }
    
    template <class T>
    void detect(T& img) { // should be detect(const T&..) but ofxCv needs updates
        if(!modelsLoaded) {
            ofLog() << "Call loadModels() before calling detect()";
            return;
        }
        
        // convert the input image from RGB (OF) to BGR (CV)
        cv::Mat imgBgr;
        ofxCv::convertColor(img, imgBgr, CV_RGB2BGR);
        
        // runt the detector
        detector.test(imgBgr, numModelsTest, stepSize);
        
        detectReady = true;
    }
    
    void getForegroundMask(ofImage& foregroundMask) {
        if(!detectReady) {
            ofLog() << "Call detect() before calling getForegroundMask()";
            return;
        }
        
        // get the binary classification image / foreground mask
        cv::Mat foregroundMaskMat = detector.postprocess(detector._response_img);
        
        // copy it into the ofImage so it's easier to work with
        ofxCv::copyGray(foregroundMaskMat, foregroundMask);
    }
    
private:
    bool modelsLoaded;
    bool detectReady;
    
    int targetWidth;
    int numModelsTest;
    int stepSize;
    
    string featureSet;
    string modelPrefix, globfeatPrefix;
    
    HandDetector detector;
};
