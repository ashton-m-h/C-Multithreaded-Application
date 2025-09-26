#include <semaphore.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/objdetect/objdetect_c.h>
#include "cascade_struct.h"

#define SCALAR_CONST 255

void release_cvcascs(CvCascs* cvCascs);
void detection_loop(CvSeq* faces, CvCascs* cvCascs, IplImage* frameGray);
void replacement_loop(CvSeq* faces, CvCascs* cvCascs);

/**
 * Using the openCV library, processes the image
 * Assumes that the cvCascs struct has been properly
 * initialised
 * loads the final image into temp file and then
 * releases memory
 * Requires tmpFile to be protected by a semaphore
 * done with tmpFileSem
 * cvCascs: struct with images and information
 * assumes that had been loaded in the load_cascades.c source file
 *
 * returns 0 on success and 1 if NO FACES FOUND
 *
 */
int process_image(CvCascs* cvCascs, sem_t* tmpFileSem)
{
    IplImage* frameGray
            = cvCreateImage(cvGetSize(cvCascs->frame), IPL_DEPTH_8U, 1);
    cvCvtColor(cvCascs->frame, frameGray, CV_BGR2GRAY);
    cvEqualizeHist(frameGray, frameGray);

    CvMemStorage* storage = 0;
    storage = cvCreateMemStorage(0);
    cvClearMemStorage(storage);

    CvSeq* faces = cvHaarDetectObjects(frameGray, cvCascs->faceCascade, storage,
            cvCascs->haarScaleFactor, cvCascs->haarMinNeighbours,
            cvCascs->haarFlags,
            cvSize(cvCascs->haarMinSize, cvCascs->haarMinSize),
            cvSize(cvCascs->haarMaxSize, cvCascs->haarMaxSize));
    if (faces->total == 0) { // No detected faces -> error
        return 1;
    }
    if (!cvCascs->replace) { // Draw ellipses
        detection_loop(faces, cvCascs, frameGray);
    } else { // Replace faces with replace image
        replacement_loop(faces, cvCascs);
    }
    sem_wait(tmpFileSem); // POSTED IN CLIENT THREAD FUNC
    const char* tmpFile = "/tmp/imagefile.jpg";
    cvSaveImage(tmpFile, cvCascs->frame, 0);

    cvReleaseImage(&(cvCascs->frame));
    release_cvcascs(cvCascs); // Release eyes and face
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&frameGray);
    return 0;
}

/**
 * Helper function for the main loop, processing imgage
 * for detect requests.
 * faces: CvSeq* object made in the process_image function
 * cvCascs: struct containing images
 */
void detection_loop(CvSeq* faces, CvCascs* cvCascs, IplImage* frameGray)
{
    for (int i = 0; i < faces->total; i++) {
        CvRect* face = (CvRect*)cvGetSeqElem(faces, i);
        CvPoint center
                = {face->x + face->width / 2, face->y + face->height / 2};
        const CvScalar magenta = cvScalar(SCALAR_CONST, 0, SCALAR_CONST, 0);
        const CvScalar blue = cvScalar(SCALAR_CONST, 0, 0, 0);
        cvEllipse(cvCascs->frame, center,
                cvSize(face->width / 2, face->height / 2), 0,
                cvCascs->ellipseStartAngle, cvCascs->ellipseEndAngle, magenta,
                cvCascs->lineThickness, cvCascs->lineType, cvCascs->shift);
        IplImage* faceROI
                = cvCreateImage(cvGetSize(frameGray), IPL_DEPTH_8U, 1);
        cvCopy(frameGray, faceROI, NULL);
        cvSetImageROI(faceROI, *face);
        CvMemStorage* eyeStorage = 0;
        eyeStorage = cvCreateMemStorage(0);
        cvClearMemStorage(eyeStorage);
        CvSeq* eyes = cvHaarDetectObjects(faceROI, cvCascs->eyesCascade,
                eyeStorage, cvCascs->haarScaleFactor,
                cvCascs->haarMinNeighbours, cvCascs->haarFlags,
                cvSize(cvCascs->haarMinSize, cvCascs->haarMinSize),
                cvSize(cvCascs->haarMaxSize, cvCascs->haarMaxSize));
        if (eyes->total == 2) {
            for (int j = 0; j < eyes->total; j++) {
                CvRect* eye = (CvRect*)cvGetSeqElem(eyes, j);
                CvPoint eyeCenter = {face->x + eye->x + eye->width / 2,
                        face->y + eye->y + eye->height / 2};
                int radius = cvRound((eye->width / 2 + eye->height / 2) / 2);
                cvCircle(cvCascs->frame, eyeCenter, radius, blue,
                        cvCascs->lineThickness, cvCascs->lineType,
                        cvCascs->shift);
            }
        }
        cvReleaseImage(&faceROI);
        cvReleaseMemStorage(&eyeStorage);
    }
}

/**
 * Helper function for the main loop, processing imgage
 * for replace requests.
 * faces: CvSeq* object made in the process_image function
 * cvCascs: struct containing images
 */
void replacement_loop(CvSeq* faces, CvCascs* cvCascs)
{
    for (int i = 0; i < faces->total; i++) {
        CvRect* face = (CvRect*)cvGetSeqElem(faces, i);
        IplImage* resized = cvCreateImage(cvSize(face->width, face->height),
                IPL_DEPTH_8U, (cvCascs->replace)->nChannels);
        cvResize(cvCascs->replace, resized, CV_INTER_AREA);
        char* frameData = (cvCascs->frame)->imageData;
        char* faceData = resized->imageData;
        for (int y = 0; y < face->height; y++) {
            for (int x = 0; x < face->width; x++) {
                int faceIndex
                        = (resized->widthStep * y) + (x * resized->nChannels);
                if ((resized->nChannels == cvCascs->bgraChannels)
                        && (faceData[faceIndex + cvCascs->alphaIndex] == 0)) {
                    continue;
                }
                int frameIndex = ((cvCascs->frame)->widthStep * (face->y + y))
                        + ((face->x + x) * (cvCascs->frame)->nChannels);
                frameData[frameIndex + 0] = faceData[faceIndex + 0];
                frameData[frameIndex + 1] = faceData[faceIndex + 1];
                frameData[frameIndex + 2] = faceData[faceIndex + 2];
            }
        }
        cvReleaseImage(&resized);
    }
    cvReleaseImage(&(cvCascs->replace));
}
