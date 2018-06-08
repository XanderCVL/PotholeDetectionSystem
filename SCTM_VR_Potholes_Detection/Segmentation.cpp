#include "Segmentation.h"
#include "MathUtils.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ximgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;
using namespace cv::ximgproc;

const int RESIZING_WIDTH = 640;
const int RESIZING_HEIGHT = 480;

void printThresholds(ExtractionThresholds thresholds) {
    cout << "THRESHOLDS" << endl;
    cout << "Density_Threshold: " << thresholds.Density_Threshold << endl;
    cout << "Variance_Threshold: " << thresholds.Variance_Threshold << endl;
    cout << "Gauss_RoadThreshold: " << thresholds.Gauss_RoadThreshold << endl;
}

void printOffsets(Offsets of) {
    cout << "OFFSETS" << endl;
    cout << "Horizon_Offset: " << of.Horizon_Offset << endl;
    cout << "SLine_X_Offset: " << of.SLine_X_Offset << endl;
    cout << "SLine_Y_Offset: " << of.SLine_Y_Offset << endl;
}

void preprocessing(Mat &src, Mat &resizedImage, const double Horizon_Offset) {

    cout << "Preprocessing... ";

    Size scale(RESIZING_WIDTH, RESIZING_HEIGHT);
//    Point2d translation_((double) -W / 2.0, (double) -H / 1.0), shrink_(3.0 / (double) W, 5.0 / (double) H);

    // Apply gaussian blur in order to smooth edges and gaining cleaner superpixels
    GaussianBlur(src, src, Size(5, 5), 0.0);

    resize(src, resizedImage, scale);

    resizedImage = resizedImage(
            Rect(Point2d(0, RESIZING_HEIGHT * Horizon_Offset), Point2d(RESIZING_WIDTH - 1, RESIZING_HEIGHT - 1)));

    cout << "Finished." << endl;
}

bool evalutateThresholds(Mat &src, Offsets offsets, Point2d center) {
    // How to evaluate thresholds in order to separate road super pixels?
    // Or directly identify RoI (Region of interest) where a pothole will be more likely detected?
    // ...
    // Possibilities
    // => Gaussian 3D function
    // => Evaluates the pixels through an Analytic Rect Function : F(x) > 0 is over the rect, F(x) < 0 is under, F(x) = 0 it lies on.

    bool isRoad = AnalyticRect2D(cv::Point2d(src.cols * offsets.SLine_X_Offset, src.rows * offsets.SLine_Y_Offset),
                                 cv::Point2d(src.cols * 0.4, 0.0), center) >= 0 &&
                  AnalyticRect2D(
                          cv::Point2d(src.cols * (1.0 - offsets.SLine_X_Offset), src.rows * offsets.SLine_Y_Offset),
                          cv::Point2d(src.cols * 0.6, 0.0), center) >= 0;

    return isRoad;
}

Point2d calculateVariance(vector<cv::Point> PixelsInLabel, Point2d center) {

    cv::Point2d variance(0.0, 0.0);

    for (Point2d p : PixelsInLabel) {
        variance = p - (cv::Point2d) center;
        variance = Point2d(variance.x * variance.x, variance.y * variance.y);
    }

    variance /= (double) PixelsInLabel.size();

    return variance;
}

void extract_candidates(Mat &src,
                        Mat &labels,
                        int nSuperPixels,
                        vector<Point> &candidates,
                        ExtractionThresholds thresolds,
                        Mat &out,
                        Mat &mask,
                        Offsets offsets) {

    for (int l = 0; l < nSuperPixels; ++l) {

        Mat1b LabelMask = (labels == l);

        vector<cv::Point> PixelsInLabel;
        findNonZero(LabelMask, PixelsInLabel);

        cv::Point2d center(0.0, 0.0);
        for (Point2d p : PixelsInLabel) center += p;
        center /= (double) PixelsInLabel.size();

//        Point translated_((Center.x + translation_.x), (Center.y + translation_.y));
//        Point shrinked_(translated_.x*shrink_.x, translated_.x*shrink_.y);

        bool isRoad = evalutateThresholds(src, offsets, center);

        Scalar mean_color_value = mean(src, LabelMask);
        Scalar color_mask_value = Scalar(0, 0, 0);

        if (isRoad) {

            cv::Point2d Variance(0.0, 0.0);
            Variance = calculateVariance(PixelsInLabel, center);
            
            Point2f vertex[4];
            minAreaRect(PixelsInLabel).points(vertex);
            // Shoelace Area Formula
            double area =
                    ((vertex[0].x * vertex[1].y - vertex[1].x * vertex[0].y) +
                     (vertex[1].x * vertex[2].y - vertex[2].x * vertex[1].y) +
                     (vertex[2].x * vertex[3].y - vertex[3].x * vertex[2].y) +
                     (vertex[3].x * vertex[0].y - vertex[0].x * vertex[3].y)) * 0.5;


            double density = (double) PixelsInLabel.size() / area;

            if (density < thresolds.Density_Threshold &&
                (Variance.x > thresolds.Variance_Threshold || Variance.y > thresolds.Variance_Threshold)) {

//                cout << l
//                     << ", "
//                     << PixelsInLabel.size()
//                     << ", "
//                     << Area
//                     << ", \""
//                     << Variance
//                     << "\", "
//                     << density
//                     << endl;

                color_mask_value = Scalar(255, 255, 255);

                // Add the center of the superpixel to the candidates.
                candidates.push_back(center);
            }
        }

        out.setTo(mean_color_value, LabelMask);
        mask.setTo(color_mask_value, LabelMask);
    }
}

void superPixeling(Mat &src,
                   Mat &out,
                   Mat &mask,
                   Mat &contour,
                   vector<Point> &candidates,
                   ExtractionThresholds thresholds,
                   Offsets offsets,
                   int superPixelEdge) {


    Mat imgCIELab;
    // Switch color space from RGB to CieLAB
    cvtColor(src, imgCIELab, COLOR_BGR2Lab);
//	imshow("CieLab color space", imgCIELab);

    // Linear Spectral Clustering
    Ptr<SuperpixelLSC> superpixels = cv::ximgproc::createSuperpixelLSC(imgCIELab, superPixelEdge);
//  Ptr<SuperpixelSLIC> superpixels = cv::ximgproc::createSuperpixelSLIC(imgCIELab, SLIC::MSLIC, 32, 50.0);

    superpixels->iterate(10);
    superpixels->getLabelContourMask(contour);

    src.copyTo(out);
    src.copyTo(mask);
    mask.setTo(Scalar(255, 255, 255));

//    cout << "SP, Size, Area, Variance, Density" << endl;

    Mat labels;
    superpixels->getLabels(labels);


    extract_candidates(src, labels, superpixels->getNumberOfSuperpixels(), candidates, thresholds, out, mask, offsets);

//    imshow("src", src);
//    imshow("out", out);
//    imshow("contour", contour);
//    imshow("mask", mask);
//    imshow("labels", labels);
}

int PotholeSegmentation(Mat &src,
                        vector<Point> &candidates,
                        const int superPixelEdge,
                        const ExtractionThresholds thresholds,
                        const Offsets offsets) {

    printThresholds(thresholds);
    printOffsets(offsets);

    preprocessing(src, src, offsets.Horizon_Offset);
//    imshow("Preprocessed Image (Resized & Cropped)", src);

    Mat contour;
    Mat out;
    Mat mask;
    Mat res;
    superPixeling(src, out, mask, contour, candidates, thresholds, offsets, superPixelEdge);

    out.setTo(Scalar(0, 0, 255), contour);


    //imshow("Segmentation", out);

    // Dilate to clean possible small black dots into the image "center"
    auto dilateElem = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    // Remove small white dots outside the image "center"
    auto erodeElem = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

//    dilate(mask, mask, dilateElem);
    erode(mask, mask, erodeElem);

    //imshow("Mask", mask);

    src.copyTo(res, mask);

//    imshow("Result", res);

    // Cut the image in order to resize it to the smalled square/rectangle possible
    // To Do ...
//    waitKey();

    return 1;
}