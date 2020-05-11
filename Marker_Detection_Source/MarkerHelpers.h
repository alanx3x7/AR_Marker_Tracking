/*	EN.601.654 Augmented Reality
 *	Final Project Marker Detection Code
 *	Header file for helper functions for marker detection
 *	Alan Lai, alai13@jhu.edu
 *	2020/05/02
 */

#pragma once

/* OpenCV includes */
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


/*  Finds the location of the corners given the refined edges */
void findCorners(cv::Point2f* corners, float* lineParameters);

/*  Checks that the pixels on the marker border are black to be valid */
bool checkBorderIsBlack(cv::Mat &planarMarker);

/*  Get the Marker ID of the marker given its projection */
void getMarkerIDs(cv::Mat &planarMarker, int* codes);

/*  Finds the ID and orientation of the minimum valued code */
int correctCornerOrder(int* codes, cv::Point2f* corners);

/*  Locate the center position of the marker given our corners for Unity */
void findMarkerCenter(cv::Point2f* corners, float &center_x, float &center_y);