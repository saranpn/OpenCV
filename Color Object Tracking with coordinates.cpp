#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	// Capture the video from Integrated Webcam
	VideoCapture cap(0);

	// If not success, exit program
	if (!cap.isOpened())
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	// Create a Control Window and Trackbars
	// namedWindow("Control", CV_WINDOW_AUTOSIZE);		// Control Window
	namedWindow("Control");		// Control Window

	int iLowH = 169;
	int iHighH = 179;

	int iLowS = 182;
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;

	// Create Trackbars in Control Window
	createTrackbar("LowH", "Control", &iLowH, 179);		//Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255);		//Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);		//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	while (true)
	{
		Mat imgOriginal;
		cap.read(imgOriginal);

		// Convert the captured frame from BGR to HSV
		Mat imgHSV;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

		// Create the Thresholded Image
		Mat imgThresholded;
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

		// Noise Reduction using Mathematical Morphology
		// Morphological Opening (Removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		// Morphological Closing (Removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		// Calculate the Moments of the Thresholded Image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// If the area <= 10000, consider that it's because of the noise
		if (dArea > 10000)
		{
			// Calculate the Centroid of the Object
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;

			// Draw a Red Circle tracking the Object
			int R = sqrt((dArea / 255) / 3.14);
			if (posX >= 0 && posY >= 0)
			{
				circle(imgOriginal, Point(posX, posY), R, Scalar(0, 0, 255), 2);
			}

			// Calculate the Distance between the Object and the camera
			int realR = 4;
			int f = -10;
			int fpx = 750;
			int d = (realR*fpx) / R + f;
			cout << "Distance = " << d << endl;
			cout << "X Position = " << posX << endl;
			cout << "y Position = " << posY << endl;
			cout << endl;
		}

		// Show the Thresholded Image
		imshow("Thresholded Image", imgThresholded);

		// Show the Tracked Image
		// imgOriginal = imgOriginal + imgLines;
		imshow("Original", imgOriginal);

		// Wait for key is pressed then break loop
		if (waitKey(5) == 27)			//ESC 27, ENTER 13, SPACE 32
		{
			break;
		}
	}

	return 0;
}
