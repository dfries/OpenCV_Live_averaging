/* Copyright (C) 2014 David Fries <David@Fries.net>  All right reserved */

#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	VideoCapture cap;
	// take a number for a device name or a file, but it seems like
	// the file won't accept a video device filename
	if(argc == 1)
	{
		cap.open(0);
	}
	else if(argc == 2)
	{
		if(isdigit(argv[1][0]))
			cap.open(atoi(argv[1]));
		else
			cap.open(argv[1]);
	}
	if(!cap.isOpened())
	{
		cerr << "Failed to open device\n";
		return 1;
	}
}
