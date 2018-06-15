#ifndef POTHOLEDETENCTIONSYSTEM_SEGMENTATION_H
#define POTHOLEDETENCTIONSYSTEM_SEGMENTATION_H

#include <opencv2/core.hpp>
#include "DataStructures.h"
#include "SuperPixelingUtils.h"

using namespace cv;
using namespace std;

void preprocessing(Mat &src, Mat &processedImage, const double Horizon_Offset);

int extractRegionsOfInterest(const Mat &src, vector<SuperPixel> &candidateSuperpixels,
                             const int superPixelEdge = 32,
                             const ExtractionThresholds thresholds = defaultThresholds,
                             const RoadOffsets offsets = defaultOffsets);

SuperPixel extractPotholeRegionFromCandidate(Mat &src, string candidateName);

#endif //POTHOLEDETENCTIONSYSTEM_SEGMENTATION_H
