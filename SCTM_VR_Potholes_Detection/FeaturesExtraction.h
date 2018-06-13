//
// Created by Matteo Gabellini on 02/06/2018.
//

#ifndef POTHOLEDETENCTIONSYSTEM_FEATURESEXTRACTION_H
#define POTHOLEDETENCTIONSYSTEM_FEATURESEXTRACTION_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "HistogramElaboration.h"
#include "MathUtils.h"
#include "DataStructures.h"

using namespace cv;
using namespace std;

vector<Features> extractFeatures(Mat sourceImage, vector<Point> centroids, Size candidate_size);

#endif //POTHOLEDETENCTIONSYSTEM_FEATURESEXTRACTION_H
