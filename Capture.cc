/* Copyright (C) 2014 David Fries <David@Fries.net>  All right reserved */

#include "Capture.h"

#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace cv;

Capture::Capture(int argc, char **argv) :
	CapWin("Unprocessed Image")
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
	// appears to not update until waitKey or if it has a thread
	//startWindowThread();
	Mat capture, image;
	struct timespec req = {0, 200*1000*1000};
	int msleep = 0;
	for(;;)
	{
		Input >> capture;
		if(capture.empty())
			break;
		imshow(CapWin, capture);
		//capture.copyTo(image);

		// stupid no give me what you have without waiting, don't use
		// OpenCV GUI code in anything but developing tests
		char c = waitKey(1);
		if(c != -1)
		{
			cout << c << flush;
			if(isdigit(c))
			{
				int d = c - '0';
				if(msleep)
					msleep*=10;
				msleep += d;
				cout << "msleep " << msleep << endl;
			}
			else
			{
				cout << "using msleep " << msleep << endl;
				req.tv_sec = msleep / 1000;
				req.tv_nsec = (msleep%1000)*1000*1000;
				msleep = 0;

				cout << Input.get(CV_CAP_PROP_FPS) << "fps"
					<< endl;
				cout << Input.get(CV_CAP_PROP_FOURCC) << "4c"
					<< endl;
				cout << "gain " << Input.get(CV_CAP_PROP_GAIN)
					<< endl;
				cout << "exposure "
					<< Input.get(CV_CAP_PROP_EXPOSURE)
					<< endl;
			}
			if(c == 'f')
			{
				cout << "set fps "
					<< Input.set(CV_CAP_PROP_FPS, 5)
					<< endl;
			}
		}
		nanosleep(&req, NULL);
	}
}
