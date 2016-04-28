/* Copyright (C) 2014 David Fries <David@Fries.net>  All right reserved */

#include "Capture.h"

#include <iostream>
#include <deque>

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
	Mat convert_in(accum.rows, accum.cols, accum.type());
	Mat convert_out(accum.rows, accum.cols, accum.type());
	Mat out;

	// keep track of past frames to subtract from the accumulated image
	deque<Mat> frames;
	size_t pos = 0;
	
	capture.copyTo(out);
	int count = 0;
	double multiplier = 1;
	for(;;)
	{
		Input >> capture;
		if(capture.empty())
			break;
		imshow(CapWin, capture);
		capture.convertTo(convert_in, convert_in.type());

		if(frames.empty())
		{
			add(convert_in, accum, accum);
			++count;
		}
		else
		{
			// out with the old, in with the new
			subtract(accum, frames[pos], accum);
			add(convert_in, accum, accum);

			// store off the frame
			Mat t = frames[pos];
			frames[pos] = convert_in;
			convert_in = t;
			pos = (pos + 1) % frames.size();
		}

		accum.convertTo(convert_out, convert_out.type(),
			multiplier/count);
		convert_out.convertTo(out, out.type());
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
			if(isdigit(c))
			{
				multiplier = c - '0';
				if(multiplier == 0)
					multiplier = .5;
				cout << "Brightness " << multiplier << endl;
			}
			else if(c == '+')
			{
				if(frames.empty())
					accum.setTo(cv::Scalar(0, 0, 0));

				// add in the current frame a second time
				// so the division works out
				frames.push_back(convert_in.clone());
				add(convert_in, accum, accum);
				count = frames.size();
			}
			else if(c == '-')
			{
				if(!frames.empty())
				{
					// subtract out the frame being dropped
					subtract(accum, frames[pos], accum);
					frames.erase(frames.begin()+pos);
				}
				count = frames.size();
				if(frames.empty())
					pos = 0;
				else
					pos = pos % frames.size();
			}
			else if(c == 'f')
			{
				cout << "set fps "
					<< Input.set(CV_CAP_PROP_FPS, 5)
					<< endl;
			}
			else if(c == 'c')
			{
				cout << "clear accumulation matrix count was "
					<< count << endl;
				// clear the matrix
				cv::Scalar zero(0, 0, 0);
				accum.setTo(zero);
				for(size_t i = 0; i < frames.size(); ++i)
				{
					convert_in.copyTo(frames[i]);
					accum += convert_in;
				}
				if(frames.empty())
					count = 0;
			}
			else if(c == '?')
			{
				cout << "0-9	brightness multiplier\n"
					"c	clear frame\n"
					"f	set fps to 5 (not working)\n"
					"+	increment rolling average\n"
					"-	decrement rolling average\n";
			}
			cout << (frames.empty() ? "accum" : "frame") <<
				" count " << count << endl;
		}
	}
}
