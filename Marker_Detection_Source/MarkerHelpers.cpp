/*	EN.601.654 Augmented Reality
 *	Final Project Marker Detection Code
 *	Helper functions for marker detection
 *	Alan Lai, alai13@jhu.edu
 *	2020/05/02
 */


/* OpenCV includes */
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

/* IO includes for debugging */
#include <iostream>


/*  Finds the location of the corners given the refined edges
 *
 *	@param corners: Container to hold location of refined corners
 *	@param lineParameters: Parameters for refined edge locations
 *
 *	@return void
 */
void findCorners(cv::Point2f* corners, float* lineParameters) {

	// Goes through each corner
	for (int i = 0; i < 4; ++i) {
		int j = (i + 1) % 4;

		// Find line parameters of this current line
		double x0 = lineParameters[8 + i];
		double y0 = lineParameters[12 + i];
		double u0 = lineParameters[i];
		double v0 = lineParameters[4 + i];

		// Find line parameters of the next line
		double x1 = lineParameters[8 + j];
		double y1 = lineParameters[12 + j];
		double u1 = lineParameters[j];
		double v1 = lineParameters[4 + j];

		// Find the intersection between the two lines (cross product)
		double a = x1 * u0*v1 - y1 * u0*u1 - x0 * u1*v0 + y0 * u0*u1;
		double b = -x0 * v0*v1 + y0 * u0*v1 + x1 * v0*v1 - y1 * v0*u1;
		double c = v1 * u0 - v0 * u1;

		// Check if lines are parallel
		if (fabs(c) < 0.001) {
			std::cout << "Lines are parallel" << std::endl;
			continue;
		}

		// Find the exact corner location
		corners[i].x = a / c;
		corners[i].y = b / c;

	}
}


/*  Checks that the pixels on the marker border are black to be valid
 *
 *	@param planarMarker: The 6x6 marker pixels
 *
 *	@return valid: True if it is a valid marker (black border), false otherwise
 */
bool checkBorderIsBlack(cv::Mat &planarMarker) {

	// Check if the border is black for a valid marker
	for (int i = 0; i < 6; ++i) {

		// Loads the values at each edge to check if black (black pixels have value 0)
		// If any edge has a nonzero value, then we exit and return false
		if (planarMarker.at<uchar>(0, i) > 0) {
			return false;
		}

		if (planarMarker.at<uchar>(5, i) > 0) {
			return false;
		}

		if (planarMarker.at<uchar>(i, 0) > 0) {
			return false;
		}

		if (planarMarker.at<uchar>(i, 5) > 0) {
			return false;
		}

	}

	return true;
}


/*  Get the Marker ID of the marker given its projection
 *	Since each marker has 4 orientations, there are 4 corresponding IDs
 *
 *	@param planarMarker: The 6x6 marker
 *	@param codes: The list of marker IDs that this marker has
 *
 *	@return void
 */
void getMarkerIDs(cv::Mat &planarMarker, int* codes) {

	// Obtain the pixels in the inner 4x4 portion of the marker that holds the ID
	int marker_sign[4][4];
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			marker_sign[i][3 - j] = planarMarker.at<uchar>(i + 1, j + 1);
			marker_sign[i][3 - j] = (marker_sign[i][3 - j]) ? 0 : 1;
		}
	}

	// Loads the marker values for the orientations
	codes[0] = codes[1] = codes[2] = codes[3] = 0;
	for (int i = 0; i < 16; i++) {
		int row = i >> 2;
		int col = i % 4;
		int factor = int(pow(2, i));

		codes[0] = codes[0] + factor * marker_sign[row][col];
		codes[1] = codes[1] + factor * marker_sign[3 - col][row];
		codes[2] = codes[2] + factor * marker_sign[3 - row][3 - col];
		codes[3] = codes[3] + factor * marker_sign[col][3 - row];
	}
}


/*  Finds the ID and orientation of the minimum valued code
 *	Note that we arbitrarily label the marker with the minimum code value
 *
 *	@param codes: The list of codes for the marker
 *	@param corners: The locations of the corners of the marker
 *
 *	@return code: The minimum code value that identifies this marker
 */
int correctCornerOrder(int* codes, cv::Point2f* corners) {
	
	// Find the minimum code value given all the orientations
	int code = codes[0];
	int angle = 0;
	for (int i = 1; i < 4; ++i) {
		if (codes[i] < code) {
			code = codes[i];
			angle = i;
		}
	}

	// If that is not the default angle, we reorder the codes so that the minimum code is first
	// This is for the pose estimation code later on so we get the correct orientation
	if (angle != 0) {
		cv::Point2f corrected_corners[4];
		for (int i = 0; i < 4; i++) corrected_corners[(i + angle) % 4] = corners[i];
		for (int i = 0; i < 4; i++) corners[i] = corrected_corners[i];
	}

	return code;

}


/*  Locate the center position of the marker given our corners for Unity
 *	Center position is assumed to be the average of the x, y positions of all corners
 *
 *	@param corners: The coordinates of the corners of the marker
 *	@param center_x: The x coordinate of the center of the marker
 *	@param center_y: The y coordinate of the center of the marker
 *
 *	@return void
 */
void findMarkerCenter(cv::Point2f* corners, float &center_x, float &center_y) {
	
	float sum_x = 0.0;
	float sum_y = 0.0;

	// Sum up all the x, and y
	for (int i = 0; i < 4; i++) {
		sum_x += corners[i].x;
		sum_y += corners[i].y;
	}

	// Find the center as the average
	center_x = sum_x / 4.0;
	center_y = sum_y / 4.0;
}