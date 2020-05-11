/*	EN.601.654 Augmented Reality
 *	Final Project Marker Detection Code
 *	Helper functions for edge refinement
 *	Alan Lai, alai13@jhu.edu
 *	2020/05/02
 */


/* OpenCV includes */
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


/*  Finds the parameters of the stripes used for line refinement
 *	
 *  @param stripeLength: The length of each stripe (perpendicular to edge)
 *  @param stripeVecX: The vector direction along the line to be refined
 *  @param stripeVecY: The vector direction perpendicualr to the line to be refined
 *	@param dx: The x coordinate separation between each stripe along this edge
 *	@param dy: The y coordinate separation between each stripe along this edge
 *  
 *  @return stripeSize: The width and height of the stripe
 */
cv::Size setStripes(int &stripeLength, cv::Point2f &stripeVecX, cv::Point2f &stripeVecY, double dx, double dy) {

	// Find the stripe length as the 80% of the length between each segment
	stripeLength = (int)(0.8 * sqrt(dx * dx + dy * dy));

	// Modify that length to be at least 5 pixels long, and odd so it's easier to center
	if (stripeLength < 5) {
		stripeLength = 5;
	}
	stripeLength |= 1;

	// Define the stripe size, with default width of 3 pixels
	cv::Size stripeSize;
	stripeSize.width = 3;
	stripeSize.height = stripeLength;

	// Normalize the vectors
	stripeVecX.x = dx / sqrt(dx * dx + dy * dy);
	stripeVecX.y = dy / sqrt(dx * dx + dy * dy);
	stripeVecY.x = stripeVecX.y;
	stripeVecY.y = -stripeVecX.x;

	return stripeSize;
}


/*  Obtain the pixel value in a non-integer location, using relative contributions
 *	Referenced from Mostajab GitHub
 *	
 *	@param gray_im: The original image to obtain the pixel values
 *	@param p: The non-integer position of the pixel to be sampled
 *
 *	@return pixel_value: The pixel value at that specified location
 */
int subpixSampleSafe2(const cv::Mat &gray_im, const cv::Point2f &p)
{
	// Find coordinates to the pixel on the top left corner
	int x = int(floorf(p.x));
	int y = int(floorf(p.y));

	// If the pixel is off the image, we return intermediate value of 127
	if (x < 0 || x >= gray_im.cols - 1 ||
		y < 0 || y >= gray_im.rows - 1)
		return 127;

	// Finds the additional overlap onto neighbouring pixels 
	int dx = int(256 * (p.x - floorf(p.x)));
	int dy = int(256 * (p.y - floorf(p.y)));
	
	// Finds the pointer to the top left corner pixel
	unsigned char* i = (unsigned char*)((gray_im.data + y * gray_im.step) + x);

	int a = i[0] + ((dx * (i[1] - i[0])) >> 8);	// Find the addition contribution in the x direction
	i += gray_im.step;
	int b = i[0] + ((dx * (i[1] - i[0])) >> 8);	// Find the addition contribution in the y direction
	return a + ((dy * (b - a)) >> 8);
}


/*  Finds the pixel location with the maximum value in the stripe using quadratic fitting
 *	
 *	@param sobelValues: The pixel values of the stripe after passing through sobel filter
 *	@param stripeLength: The length of the stripe 
 *	@param maxIndex: The pixel index of the pixel with the maximum value
 *
 *	@return max_pos: The pixel position with the maximum value (edge)
 */
double findMaxInStripe(std::vector<double> sobelValues, const int stripeLength, int &maxIndex) {

	// Find the maximum value in the stripe
	double maxVal = -1;
	for (int n = 0; n < stripeLength - 2; n++) {
		if (sobelValues[n] > maxVal) {
			maxIndex = n;
			maxVal = sobelValues[n];
		}
	}

	// Find the three values around the maximum value
	double p0 = 0;
	if (maxIndex > 0) {
		p0 = sobelValues[maxIndex - 1];
	}
	double p1 = sobelValues[maxIndex];
	double p2 = 0;
	if (maxIndex < stripeLength - 3) {
		p2 = sobelValues[maxIndex + 1];
	}

	// Find the maximum value of parabola given three points
	// Derived by solving for the equation y = ax^2 + bx + c
	// We have three values of y equidistant, so we assume that
	// the corresponding x's are -1, 0, and 1. Then we can solve
	// for a, b, c. The parabola is situated when f'(x) = 0
	// That is, x where 2ax + b = 0
	double max_pos = (p2 - p0) / (4 * p1 - 2 * p0 - 2 * p2);

	return max_pos;
}


/*  Refine edges to get a better estimate.
 *	
 *	@param linParamsMat: Matrix to save the line parameters
 *	@param corners: The coordinates of the corners of the marker
 *	@param gray_frame: The grayscaled image
 *
 *	@return void
 */
void refineEdges(cv::Mat lineParamsMat, const cv::Point* corners, cv::Mat &gray_frame) {

	// Refines edges one edge at a time
	for (int i = 0; i < 4; i++) {

		// Find size and directions of the stripes, with 6 stripes in total
		int stripeLength = 0;
		double dx = (corners[(i + 1) % 4].x - corners[i].x) / 7.0;
		double dy = (corners[(i + 1) % 4].y - corners[i].y) / 7.0;
		cv::Point2f stripeVecX, stripeVecY;
		cv::Size stripeSize = setStripes(stripeLength, stripeVecX, stripeVecY, dx, dy);

		// Find the start and stop position 
		int nStop = stripeLength >> 1;
		int nStart = -nStop;

		// Contains to hold the pixel values in each stripe
		cv::Mat edgeStripe(stripeSize, CV_8UC1);
		cv::Point2f true_edge[6];

		// Goes through each stripe in the edge
		for (int j = 1; j < 7; ++j) {

			// Find the location of each stripe
			double px = (double)corners[i].x + (double)j * dx;
			double py = (double)corners[i].y + (double)j * dy;

			cv::Point p;
			p.x = (int)px;
			p.y = (int)py;

			// Get all the pixel values corresponding to this stripe
			for (int m = -1; m <= 1; ++m) {
				for (int n = nStart; n <= nStop; ++n) {

					// Finds all of the pixels within this stripe
					cv::Point2f subPixel;
					subPixel.x = px + ((double)m * stripeVecX.x) + ((double)n * stripeVecY.x);
					subPixel.y = py + ((double)m * stripeVecX.x) + ((double)n * stripeVecY.y);

					// Get the pixel value from the grayscale image
					int pixel = subpixSampleSafe2(gray_frame, subPixel);
					int w = m + 1;
					int h = n + (stripeLength >> 1);

					edgeStripe.at<uchar>(h, w) = (uchar)pixel;

				}
			}

			// Perform sobel operator on all inner cells in the stripe
			std::vector<double> sobelValues(stripeLength - 2);
			for (int n = 1; n < stripeLength - 1; n++) {
				unsigned char* stripePointer = &(edgeStripe.at<uchar>(n - 1, 0));
				double row_top = -stripePointer[0] - 2 * stripePointer[1] - stripePointer[2];
				stripePointer += 2 * edgeStripe.step;
				double row_bot = stripePointer[0] + 2 * stripePointer[1] + stripePointer[2];
				sobelValues[n - 1] = row_top + row_bot;
			}

			// Find the maximum value of parabola given three points
			int maxIndex = 0;
			double max_pos = findMaxInStripe(sobelValues, stripeLength, maxIndex);

			// Check max value for validity
			if (std::isinf(max_pos)) {
				continue;
			}

			// Find the center of the edge given the maximum position in this stripe
			cv::Point2f edgeCenter;
			int maxIndexShift = maxIndex - (stripeLength >> 1);
			edgeCenter.x = (double)p.x + (((double)maxIndexShift + max_pos) * stripeVecY.x);
			edgeCenter.y = (double)p.y + (((double)maxIndexShift + max_pos) * stripeVecY.y);

			// Save this in the list of true edge positions
			true_edge[j - 1].x = edgeCenter.x;
			true_edge[j - 1].y = edgeCenter.y;
		}

		// Use the OpenCV fitLine function to find the line given the position of the true edges
		cv::Mat mat(cv::Size(1, 6), CV_32FC2, true_edge);
		cv::fitLine(mat, lineParamsMat.col(i), cv::DIST_L2, 0.0, 0.01, 0.01);

	}
}