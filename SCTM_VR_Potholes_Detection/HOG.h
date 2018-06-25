//
// Created by Xander_C on 15/06/2018.
//

#ifndef POTHOLEDETECTIONSYSTEM_HOG_H
#define POTHOLEDETECTIONSYSTEM_HOG_H

#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

typedef struct HOGConfig {
    const int binNumber;
    const Size detenctionWindowSize;
    const Size blockSize;
    const Size blockStride;
    const Size cellSize;
    const Size windowStride;
    const Size padding;
} HOGConfig;

const HOGConfig defaultConfig = {
        9,
        Size(64, 64),   //detenctionWindowSize
        Size(16, 16),     //blockSize
        Size(16, 16),     //blockStride
        Size(2, 2),     //cellSize
        Size(16, 16),     //windowStride
        Size(4, 4)      //padding
};

typedef struct HoG {
    vector<float> descriptors = vector<float>();
    vector<Point> locations = vector<Point>();;
} Hog;

Mat getHoGDescriptorVisualImage(Mat &origImg,
                                vector<float> &descriptorValues,
                                Size winSize,
                                Size cellSize,
                                int scaleFactor,
                                double viz_factor);


HoG calculateHoG(const Mat &src, const HOGConfig config = defaultConfig);


#endif //POTHOLEDETECTIONSYSTEM_HOG_H