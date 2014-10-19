/* Copyright (C) 2014 David Fries <David@Fries.net>  All right reserved */

#include "opencv2/highgui/highgui.hpp"

#include <string>

class Capture
{
public:
	Capture(int argc, char **argv);
private:
	cv::VideoCapture Input;
	const std::string CapWin, OutWin;
};
