//
// Created by Matteo Gabellini on 25/05/2018.
//
#ifndef POTHOLEDETECTIONSYSTEM_DATASTRUCTURES_H
#define POTHOLEDETECTIONSYSTEM_DATASTRUCTURES_H

#include <opencv2/core.hpp>
#include <set>

using namespace cv;
using namespace std;

typedef struct RoadOffsets {
    double Horizon_Offset;
    double SLine_X_Right_Offset;
    double SLine_X_Left_Offset;
    double SLine_Y_Right_Offset;
    double SLine_Y_Left_Offset;
    double SLine_Right_Escape_Offset;
    double SLine_Left_Escape_Offset;
} RoadOffsets;
const RoadOffsets defaultOffsets = {0.60, 0.0, 0.4, 0.8, 0.8, 0.5, 0.5};

typedef struct ExtractionThresholds {
    double Density_Threshold;
    double Variance_Threshold;
    double Gauss_RoadThreshold;
    double colourRatioThresholdMin;
    double colourRatioThresholdMax;
} ExtractionThresholds;

const ExtractionThresholds defaultThresholds = {0.80, 0.35, 0.60, 1.25, 4.0};

typedef struct Features {
    int label;
    Mat candidate;
    Mat histogram;
    float averageGreyValue;
    float contrast;
    float entropy;
    float skewness;
    float energy;
    Mat1f hogDescriptors;
} Features;

typedef struct SuperPixel {
    int label;
    vector<cv::Point> points;
    Point2d center;
    Mat selection;
    Mat1b mask;
    Mat contour;
    Scalar meanColourValue;
    std::set<int> neighbors;
} SuperPixel;

void printThresholds(ExtractionThresholds thresholds);


void printOffsets(RoadOffsets of);


#endif //POTHOLEDETECTIONSYSTEM_DATASTRUCTURES_H