#ifndef _IDCARDREC_H
#define _IDCARDREC_H

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> /*用到了time函数，所以要有这个头文件*/
#include <fstream>
#include <sstream>
#include <exception>
#include <vector>
//#include <sys/io.h>
#include <direct.h>

#include <opencv/highgui.h>
#include <opencv/cv.h>

#include "jiaoZheng.h"
#include "ln_convnet.hpp"

#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"

#include "BankCard.h"

#define showSteps 0
#define saveFlag 0



//typedef unsigned char uchar;

int charWidth = 36;
int charHeight = 54;

const int ROIHEIGHT = 46;

struct  recCharAndP
{
	float recP;//识别的概率
	char recChar;
};

struct imageIpl //二值化保存的图像区域，存位置和roi图像区域
{
	IplImage * roiImage;
	int positionX;
};

struct resultPos //存识别结果和roi图像区域在原图中的位置
{
	recCharAndP recCharP;
	int recPosition;
};

struct resultFinal //存识别结果和roi图像区域在原图中的位置
{
	string recString;
	float recPFinal;//识别的概率
};

//====================初始化部分====================//
LightNet::ConvNet *cnn;
LightNet::ConvNet *cnnPlane;
BankCard *BC;

extern "C"
{
void init();
string processingOne(IplImage * src);
resultFinal processingOneT(IplImage *src);
resultFinal processingOneP(IplImage *src);
}
//char *vifLine(vector<cv::Vec4i> lineT,vector<cv::Vec4i> lineB,vector<cv::Vec4i> lineL,vector<cv::Vec4i> lineR);
/*
LARGE_INTEGER m_liPerfFreq;
LARGE_INTEGER m_liPerfStart;
LARGE_INTEGER liPerfNow;
double dfTim;
void getStartTime()
{
	QueryPerformanceFrequency(&m_liPerfFreq);
	QueryPerformanceCounter(&m_liPerfStart);
}

void getEndTime()
{
	QueryPerformanceCounter(&liPerfNow);
	dfTim=( ((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000.0f)/m_liPerfFreq.QuadPart);
}
*/



//char * configFile = "/storage/sdcard0/bcnr/config.txt";
//char* trainSetPosPath = (char *)malloc(200*sizeof(char));


#endif
