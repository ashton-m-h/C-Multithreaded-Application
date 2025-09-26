#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/objdetect/objdetect_c.h>
#include "cascade_struct.h"

#define SCALE 1.1
#define MIN_NEIGHBOURS 4
#define HAAR_FLAGS 0
#define HAAR_MIN_SIZE 0
#define HAAR_MAX_SIZE 1000
#define ELL_START_ANGLE 0
#define ELL_END_ANGLE 360
#define LINE_THICKNESS 4
#define LINE_TYPE 8
#define SHIFT 0
#define BGRA_CHANNELS 4
#define ALPHA_INDEX 3

/**
 * Function that loades the default cascades and
 * then loads them into the pointers
 * returns 1 on failure (and frees), 0 on sucess
 *
 * faceCascade: address of pointer to the faceCascade
 * eyesCascade: address of pointer to the eyesCascade
 */
int load_cascades(CvHaarClassifierCascade** faceCascade,
        CvHaarClassifierCascade** eyesCascade)
{
    const char* const faceCascadeFilename
            = "/local/courses/csse2310/resources/a4/"
              "haarcascade_frontalface_alt2.xml";
    const char* const eyesCascadeFilename
            = "/local/courses/csse2310/resources/a4/"
              "haarcascade_eye_tree_eyeglasses.xml";
    *faceCascade = (CvHaarClassifierCascade*)cvLoad(
            faceCascadeFilename, NULL, NULL, NULL);
    *eyesCascade = (CvHaarClassifierCascade*)cvLoad(
            eyesCascadeFilename, NULL, NULL, NULL);
    // Check that there was no error, free the cascades if so
    if (!(*faceCascade) && !(*eyesCascade)) {
        return 1;
    }
    if (!(*faceCascade)) {
        cvReleaseHaarClassifierCascade(eyesCascade);
        return 1;
    }
    if (!(*eyesCascade)) {
        cvReleaseHaarClassifierCascade(faceCascade);
        return 1;
    }
    return 0;
}

/**
 * Initialise the cvCascs with the correct constants and face/eyes cascade
 * cvCascs: initialises this struct
 * loads replace as NULL. If replacement request, must use the function
 * load_replace below to load something into replace, otherwise
 * program will think it's a detect request
 *
 * return 1 on success, 0 on failure (when fails to load the tmpFile)
 */
int init_cascades(CvCascs* cvCascs)
{
    const char* tmpFile = "/tmp/imagefile.jpg";
    load_cascades(&(cvCascs->faceCascade), &(cvCascs->eyesCascade));
    cvCascs->haarScaleFactor = SCALE;
    cvCascs->haarMinNeighbours = MIN_NEIGHBOURS;
    cvCascs->haarFlags = HAAR_FLAGS;
    cvCascs->haarMinSize = HAAR_MIN_SIZE;
    cvCascs->haarMaxSize = HAAR_MAX_SIZE;
    cvCascs->ellipseStartAngle = ELL_START_ANGLE;
    cvCascs->ellipseEndAngle = ELL_END_ANGLE;
    cvCascs->lineThickness = LINE_THICKNESS;
    cvCascs->lineType = LINE_TYPE;
    cvCascs->shift = SHIFT;
    cvCascs->bgraChannels = BGRA_CHANNELS;
    cvCascs->alphaIndex = ALPHA_INDEX;
    cvCascs->frame = cvLoadImage(tmpFile, CV_LOAD_IMAGE_COLOR);
    cvCascs->replace = NULL;

    return (cvCascs->frame) ? 1 : 0;
}

/**
 * loads a replacement file into cvCascs, returns 0 on failure, and 1 on
 * success
 * Takes replacement file from /tmp/imagefile.jpg
 */
int load_replace(CvCascs* cvCascs)
{
    const char* tmpFile = "/tmp/imagefile.jpg";
    cvCascs->replace = cvLoadImage(tmpFile, CV_LOAD_IMAGE_UNCHANGED);

    return (cvCascs->replace) ? 1 : 0;
}

/**
 * Releases the face and eyes cascades conatained in cvCascs
 */
void release_cvcascs(CvCascs* cvCascs)
{
    cvReleaseHaarClassifierCascade(&(cvCascs->eyesCascade));
    cvReleaseHaarClassifierCascade(&(cvCascs->faceCascade));
}
