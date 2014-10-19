/* Copyright (C) 2014 David Fries <David@Fries.net>  All right reserved */

#include "Capture.h"

#include <iostream>
#include <ctype.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

Capture::Capture(int argc, char **argv) :
	CapWin("Unprocessed Image"),
	OutWin("Output Image")
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
	namedWindow(CapWin);
	namedWindow(OutWin);
	// appears to not update until waitKey or if it has a thread
	//startWindowThread();
	Mat capture;
	Input >> capture;
	// the sum of all captured images
	Mat accum = Mat::zeros(capture.rows, capture.cols, CV_32SC3);
	// used to type convert the captured image before processing
	Mat convert(accum.rows, accum.cols, accum.type());
	Mat out;
	capture.copyTo(out);
	int count = 0;
	double multiplier = 1;
	for(;;)
	{
		Input >> capture;
		if(capture.empty())
			break;
		imshow(CapWin, capture);

		capture.convertTo(convert, convert.type());
		add(convert, accum, accum);
		++count;

		accum.convertTo(out, out.type(), multiplier/count);
		imshow(OutWin, out);

		// stupid no give me what you have without waiting, don't use
		// OpenCV GUI code in anything but developing tests
		char c = waitKey(1);
		if(c != -1)
		{
			/*
			cout << Input.get(CV_CAP_PROP_FPS) << "fps" << endl;
			cout << Input.get(CV_CAP_PROP_FOURCC) << "4c" << endl;
			cout << "gain " << Input.get(CV_CAP_PROP_GAIN) << endl;
			cout << "exposure " << Input.get(CV_CAP_PROP_EXPOSURE)
				<< endl;
			*/
			cout << "accum count " << count << endl;
			if(isdigit(c))
			{
				multiplier = c - '0';
			}
			if(c == 'f')
			{
				cout << "set fps "
					<< Input.set(CV_CAP_PROP_FPS, 5)
					<< endl;
			}
			if(c == 'c')
			{
				cout << "clear accumulation matrix\n";
				// Seems like there should be a better way to
				// clear a matrix.
				subtract(accum, accum, accum);
				count = 0;
			}
		}
	}
}
