/*  EN.601.654 Augmented Reality
 *	Final Project Marker Detection Code
 *	Header file for helper functions for edge refinement
 *	Alan Lai, alai13@jhu.edu
 *	2020/05/02
 */

#pragma once

/* OpenCV includes */
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


/*  Finds the parameters of the stripes used for line refinement */
cv::Size setStripes(int &stripeLength, cv::Point2f &stripeVecX, cv::Point2f &stripeVecY, double dx, double dy);

/*  Obtain the pixel value in a non-integer location, using relative contributions */
int subpixSampleSafe2(const cv::Mat &gray_im, const cv::Point2f &p);

/*  Finds the pixel location with the maximum value in the stripe using quadratic fitting */
double findMaxInStripe(std::vector<double> sobelValues, const int stripeLength, int &maxIndex);

/*  Refine edges to get a better estimate */
void refineEdges(cv::Mat lineParamsMat, const cv::Point* corners, cv::Mat &gray_frame);
