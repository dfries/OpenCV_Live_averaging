/* Copyright (C) 2016 David Fries <David@Fries.net>
 * Licensed under the terms of GNU General Public License version 3.
 */

#include "opencv2/highgui/highgui.hpp"

#include <string>
#include <deque>

class Capture
{
public:
	Capture(int argc, char **argv);
private:
	void Setup(int type);
	void ProcessKey();
	void Clear();

	cv::VideoCapture Input;
	const std::string CapWin, OutWin;

	cv::Mat RawFrame;

	// the sum of all captured images
	cv::Mat Accum;
	// used to type convert the captured image before processing
	cv::Mat ConvertIn;

	// number of frames in Accum
	int Count;
	double Brightness;
	double SmoothAlpha;

	// keep track of past frames to subtract from the Accumulated image
	std::deque<cv::Mat> Frames;
	size_t FramePos;
};
