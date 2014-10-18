/* Copyright (C) 2014 David Fries <David@Fries.net>  All right reserved */

#include "Capture.h"

#include <iostream>
#include <ctype.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

Capture::Capture(int argc, char **argv)
{
	// take a number for a device name or a file, but it seems like
	// the file won't accept a video device filename
	if(argc == 1)
	{
		Input.open(0);
	}
	else if(argc == 2)
	{
		if(isdigit(argv[1][0]))
			Input.open(atoi(argv[1]));
		else
			Input.open(argv[1]);
	}
	if(!Input.isOpened())
	{
		cerr << "Failed to open device\n";
		exit(1);
	}
}
