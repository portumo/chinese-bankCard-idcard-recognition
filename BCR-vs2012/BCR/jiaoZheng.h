#ifndef _JIAOZHENG_H
#define _JIAOZHENG_H

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cmath>

#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <math.h>

#define showSteps 0

using namespace std;



using namespace cv;

//===========两点，以及两点与x轴的角度，两点之间的距离
struct linePtAngle{
	CvPoint startPt;
	CvPoint endPt;
	double angle;
	float lineLength;
};


struct pPoint  
{  
	double x;  
	double y;  
};  

struct Line  
{  
	pPoint p1,p2;  
	double a,b,c;  
}; 

//================定义角度和点================/
struct ptAndAngle 
{
	double Angle;
	Point2f ptf;
};





bool InRectYesOrNo(CvPoint pt,CvRect rect);
float lenghtOf2P(CvPoint pt1,CvPoint pt2);
vector<cv::Vec4i> chooseLine(IplImage* src,vector<linePtAngle> lineTop);
vector<cv::Vec4i> chooseLineLR(IplImage* src,vector<linePtAngle> lineTop);

void GetLinePara(Line &l) ;
pPoint getCrossPoint(Line &l1,Line &l2) ;
double angle(pPoint o,pPoint s,pPoint e) ;
ptAndAngle computeIntersect(cv::Vec4i a,cv::Vec4i b);
void GetMaxAndMin(double *arr , int n , double &max , double &min);
cv::Vec4i chooseLongest(vector<cv::Vec4i> lineT );
void sortCorners(std::vector<cv::Point2f>& corners,cv::Point2f center);

void vifLine(IplImage * m_im,char * str);
IplImage *jiaozheng2(IplImage *res_im);

IplImage * jiaoZheng(IplImage * m_im);

#endif
