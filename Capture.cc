/* Copyright (C) 2014 David Fries <David@Fries.net>  All right reserved */

#include "Capture.h"

#include <iostream>

#include <ctype.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

Capture::Capture(int argc, char **argv) :
	CapWin("Unprocessed Image"),
	OutWin("Output Image"),
	Count(0),
	Brightness(1),
	FramePos(0)
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
	Mat out;
	Input >> capture;

	Accum = Mat::zeros(capture.rows, capture.cols, CV_32SC3);
	ConvertIn = Accum.clone();
	cv::Mat convert_out = Accum.clone();

	capture.copyTo(out);
	for(;;)
	{
		Input >> capture;
		if(capture.empty())
			break;
		imshow(CapWin, capture);
		capture.convertTo(ConvertIn, ConvertIn.type());

		// process while ConvertIn is still the last frame read
		ProcessKey();

		if(Frames.empty())
		{
			add(ConvertIn, Accum, Accum);
			++Count;
		}
		else
		{
			// out with the old, in with the new
			subtract(Accum, Frames[FramePos], Accum);
			add(ConvertIn, Accum, Accum);

			// store off the frame
			Mat t = Frames[FramePos];
			Frames[FramePos] = ConvertIn;
			ConvertIn = t;
			FramePos = (FramePos + 1) % Frames.size();
		}

		Accum.convertTo(convert_out, convert_out.type(),
			Brightness/Count);
		convert_out.convertTo(out, out.type());
		imshow(OutWin, out);
	}
}

void Capture::ProcessKey()
{
	// stupid no give me what you have without waiting, don't use
	// OpenCV GUI code in anything but developing tests
	char c = waitKey(1);
	if(c == -1)
		return;

	/*
	cout << Input.get(CV_CAP_PROP_FPS) << "fps" << endl;
	cout << Input.get(CV_CAP_PROP_FOURCC) << "4c" << endl;
	cout << "gain " << Input.get(CV_CAP_PROP_GAIN) << endl;
	cout << "exposure " << Input.get(CV_CAP_PROP_EXPOSURE)
		<< endl;
	*/
	if(isdigit(c))
	{
		Brightness = c - '0';
		if(Brightness == 0)
			Brightness = .5;
		cout << "Brightness " << Brightness << endl;
	}
	else if(c == '+' || c == '=')
	{
		if(Frames.empty())
			Accum.setTo(cv::Scalar(0, 0, 0));

		// add in the current frame a second time
		// so the division works out
		Frames.push_back(ConvertIn.clone());
		add(ConvertIn, Accum, Accum);
		Count = Frames.size();
	}
	else if(c == '-')
	{
		if(!Frames.empty())
		{
			// subtract out the frame being dropped
			subtract(Accum, Frames[FramePos], Accum);
			Frames.erase(Frames.begin()+FramePos);
		}
		Count = Frames.size();
		if(Frames.empty())
			FramePos = 0;
		else
			FramePos = FramePos % Frames.size();
	}
	else if(c == 'f')
	{
		cout << "set fps "
			<< Input.set(CV_CAP_PROP_FPS, 5)
			<< endl;
	}
	else if(c == 'c')
	{
		cout << "clear Accumulation matrix Count was "
			<< Count << endl;
		// clear the matrix
		cv::Scalar zero(0, 0, 0);
		Accum.setTo(zero);
		for(size_t i = 0; i < Frames.size(); ++i)
		{
			ConvertIn.copyTo(Frames[i]);
			Accum += ConvertIn;
		}
		if(Frames.empty())
			Count = 0;
	}
	else if(c == '?')
	{
		cout << "0-9	brightness Brightness\n"
			"c	clear frame\n"
			"f	set fps to 5 (not working)\n"
			"+ or =	increment rolling average\n"
			"-	decrement rolling average\n";
	}
	cout << (Frames.empty() ? "Accum" : "frame") <<
		" count " << Count << endl;
}
