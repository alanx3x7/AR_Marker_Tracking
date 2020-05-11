/*  EN.601.654 Augmented Reality 
 *	Final Project Marker Detection Code
 *	Main Function
 *	Alan Lai, alai13@jhu.edu
 *	2020/05/02
 */

/* OpenCV includes */
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

/* File I/O (for debugging) includes */
#include <iostream>
#include <iomanip>

/* Helper function includes */
#include "PoseEstimation.h"
#include "UnityStructs.h"
#include "MarkerHelpers.h"
#include "EdgeRefinement.h"


/* Namespaces */
using namespace cv;
using namespace std;


/*  Main function to find and locate the AR Markers located in the image.
 *	Extern C enables this function to be callable as a library function when linked to its .dll.
 *
 *	@param outMarks: A list of Marker2 for each marker detected in the image
 *	@param raw: The raw colour image that we want to locate markers in
 *	@param width: The width of the input image
 *	@param height: The height of the input image
 *	@param maxOutMarkerCount: The maximum number of markers to be found in the image
 *	@param outMarkerDetected: The number of markers detected in the image
 *
 *	@return void
 */
extern "C" void __declspec(dllexport) __stdcall FindMarkers2(Marker2** outMarks, Color32** raw, int width, int height, int maxOutMarkerCount, int& outMarkerDetected) {

	// Convert the input raw image to cv::Mat format
	Mat old_frame(height, width, CV_8UC4, *raw);

	// If there is nothing provided in the input image, we return
	if (old_frame.empty())
	{
		return;
	}

	// We find the grayscale image, and then binarize it via thresholding
	Mat frame, binary_im, gray_frame;
	cv::cvtColor(old_frame, frame, cv::COLOR_RGBA2BGR);
	cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
	cv::threshold(gray_frame, binary_im, 105, 255, cv::THRESH_BINARY);

	// We then find the contours from the binary image
	vector<vector<Point>> contours;
	cv::findContours(binary_im, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	// We then process each contour individually
	vector<Point> polygon;
	for (size_t i = 0; i < contours.size(); i++) {

		// Approximate contour to polygon with accuracy proportional to contour perimeter
		approxPolyDP(contours[i], polygon, arcLength(contours[i], true)*0.02, true);

		// We ignore the polygon if it is too small, is nonconvex, or does not have 4 sides
		if (polygon.size() != 4 || fabs(contourArea(polygon)) < 1000 || !isContourConvex(polygon)) {
			continue;
		}

		int n = polygon.size();					// Size of the approximated marker square
		const Point* rect = &polygon[0];			// Holds the corners to this marker
		float lineParameters[16];					// Container to hold edge line equation parameters
		cv::Mat lineParamsMat(cv::Size(4, 4), CV_32F, lineParameters); 

		// Refines line position
		refineEdges(lineParamsMat, rect, gray_frame);

		// Finds the refined corners given the refined lines
		cv::Point2f corners[4];
		findCorners(corners, lineParameters);

		// Now perform a homography of the marker 
		cv::Point2f squareCorners[4];
		squareCorners[0].x = -0.5;
		squareCorners[0].y = -0.5;
		squareCorners[1].x = 5.5;
		squareCorners[1].y = -0.5;
		squareCorners[2].x = 5.5;
		squareCorners[2].y = 5.5;
		squareCorners[3].x = -0.5;
		squareCorners[3].y = 5.5;

		// Create and calculate the matrix of perspective transform
		cv::Mat projectionMatrix(cv::Size(3, 3), CV_32FC1);
		projectionMatrix = cv::getPerspectiveTransform(corners, squareCorners);

		// Create image for the marker by warping marker in image to an orthogonal projection
		cv::Mat planarMarker(cv::Size(6, 6), CV_8UC1);
		cv::warpPerspective(gray_frame, planarMarker, projectionMatrix, cv::Size(6, 6));
		cv::threshold(planarMarker, planarMarker, 100, 255, THRESH_BINARY);

		// Check if the border is black for a valid marker. If not, we skip this polygon.
		if (!checkBorderIsBlack(planarMarker)) {
			continue;
		}

		// Find the marker ID given the projected marker
		int codes[4];
		getMarkerIDs(planarMarker, codes);

		// If they're all black or white then it is an invalid marker, so we move to the next polygon
		if ((codes[0] == 0) || (codes[0] == 0xffff)) {
			continue;
		}

		// We draw the edges of the marker on the image for display when returned
		cv::line(frame, corners[0], corners[1], CV_RGB(0, 255, 0), 2, 8, 0);
		cv::line(frame, corners[1], corners[2], CV_RGB(0, 255, 0), 2, 8, 0);
		cv::line(frame, corners[2], corners[3], CV_RGB(0, 255, 0), 2, 8, 0);
		cv::line(frame, corners[3], corners[0], CV_RGB(0, 255, 0), 2, 8, 0);

		// Account for symmetry in the codes to find the representative code of the marker
		int code = correctCornerOrder(codes, corners);

		// Obtain the center of the marker
		float center_x, center_y;
		findMarkerCenter(corners, center_x, center_y);

		// Transfer screen coordinates to camera coordinates
		for (int i = 0; i < 4; i++) {
			corners[i].x -= frame.cols * 0.5;
			corners[i].y = -corners[i].y + frame.rows * 0.5;
		}

		// Estimate the transformation matrix from optical center to marker center
		float transformMatrix[16];
		estimateSquarePose(transformMatrix, (cv::Point2f*)corners, 4.5);

		// Find the distance from the marker to the optical center
		float x = transformMatrix[3];
		float y = transformMatrix[7];
		float z = transformMatrix[11];
		float distance_to_mark = sqrt(x * x + y * y + z * z);

		// Create the Marker2 object to be sent to Unity
		*outMarks[0] = {
						code, distance_to_mark,
						center_x, center_y,
						transformMatrix[3], transformMatrix[7], transformMatrix[11],
						transformMatrix[0], transformMatrix[1], transformMatrix[2],
						transformMatrix[4], transformMatrix[5], transformMatrix[6],
						transformMatrix[8], transformMatrix[9], transformMatrix[10]
					   };

		// Increment the number of markers detected and pointer to Marker2 array
		outMarkerDetected++;
		*outMarks[0]++;	

		// If we've detected the maximum number of markers, we exit
		if (outMarkerDetected == maxOutMarkerCount) {
			break;
		}

	}

	// We move the pointer back to the first Marker2 object in the array
	for (int i = 0; i < outMarkerDetected; i++) {
		*outMarks[0]--;
	}

	// Convert the image with drawn lines back to Color32 format
	cvtColor(frame, old_frame, COLOR_BGR2RGBA);
	return;
}