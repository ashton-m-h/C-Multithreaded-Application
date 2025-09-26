/**
 * Struct containing all information needed
 * for the openCV library when processing images
 *
 * faceCascade: cascade for face
 * eyesCascade: cascade for eyes (only needed for non-replacement requests)
 * frame: image to be detected and processed
 * replace: image to replace faces, should be NUULL if non-replacement request
 *
 * Also holds a bunch of arbitrary constants defined in the spec
 */
#ifndef CASC_H
#define CASC_H

#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/objdetect/objdetect_c.h>

typedef struct {
    CvHaarClassifierCascade* faceCascade;
    CvHaarClassifierCascade* eyesCascade;
    IplImage* frame;
    IplImage* replace;
    // Constants given in spec
    float haarScaleFactor;
    int haarMinNeighbours;
    int haarFlags;
    int haarMinSize;
    int haarMaxSize;
    int ellipseStartAngle;
    int ellipseEndAngle;
    int lineThickness;
    int lineType;
    int shift;
    int bgraChannels;
    int alphaIndex;
    // Constants above
} CvCascs;

#endif
