/* Copyright (C) 2014 David Fries <David@Fries.net>  All right reserved */

#include "opencv2/highgui/highgui.hpp"

#include <string>
#include <deque>

class Capture
{
public:
	Capture(int argc, char **argv);
private:
	void ProcessKey();

	cv::VideoCapture Input;
	const std::string CapWin, OutWin;

	// the sum of all captured images
	cv::Mat Accum;
	// used to type convert the captured image before processing
	cv::Mat ConvertIn;

	// number of frames in Accum
	int Count;
	double Brightness;

	// keep track of past frames to subtract from the Accumulated image
	std::deque<cv::Mat> Frames;
	size_t FramePos;
};
