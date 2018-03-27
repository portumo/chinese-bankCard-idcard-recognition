#include <cv.h>
#include <highgui.h>
#include <algorithm>
#include "IntImage.h"

#include <iostream>

using namespace std;



IntImage::IntImage():height(0),width(0),data(NULL),buf(NULL),variance(0.0),label(-1)
{
}

IntImage::~IntImage()
{
	Clear();
}

void IntImage::Clear(void)
{
	if(data == NULL)
		assert(buf == NULL);
	else
	{
		assert(buf != NULL);
		for(int i=0;i<height;i++)	data[i] = NULL;
		delete[] data;	data = NULL;
		delete[] buf;  	buf = NULL;
		height = width = 0;
		variance = 0.0;
		label = -1;
	}
}

void IntImage::Copy(const IntImage& source)
// the ONLY way to make a copy of 'source' to this image
{
	assert(source.height > 0);
	assert(source.width > 0);
	if(&source == this)	return;
	SetSize(cvSize(source.width,source.height));
	memcpy(buf,source.buf,sizeof(REAL)*height*width);
}

void IntImage::Load(const char *& filename)
{
	IplImage* imgSrc;

	imgSrc = cvLoadImage(filename,0);
	printf("%s\n",filename);
	if(imgSrc == NULL) return;

	IplImage * img;
	if( (imgSrc->width>640)||(imgSrc->height>400) ){
		img = cvCreateImage(cvSize(imgSrc->width/2,imgSrc->height/2),8,1);
		cvResize(imgSrc,img,1);
	}else{
		img = cvCreateImage(cvSize(imgSrc->width,imgSrc->height),8,1);
		cvCopy(imgSrc,img);
	}

	SetSize(cvSize(img->width,img->height));
	for(int i=0,ih=img->height,iw=img->width;i<ih;i++)
	{
		REAL* pdata = data[i];
	//	unsigned char* pimg = reinterpret_cast<unsigned char*>(img->imageData+img->widthStep*i);
		for(int j=0;j<iw;j++) {
		//	pdata[j] = pimg[j];
		//	printf("%d %d..",i,j);
			pdata[j]=cvGet2D(img,i,j).val[0];
		}
	}
	cvReleaseImage(&imgSrc);
	cvReleaseImage(&img);
}

void IntImage::Save(const char *& filename) const
{
	IplImage* img;

	img = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	for(int i=0,ih=img->height,iw=img->width;i<ih;i++)
	{
		REAL* pdata = data[i];
		unsigned char* pimg = reinterpret_cast<unsigned char*>(img->imageData+img->widthStep*i);
		for(int j=0;j<iw;j++) pimg[j] = (unsigned char)pdata[j];
	}
	cvSaveImage(filename,img);
	cvReleaseImage(&img);	
}

void IntImage::SetSize(const CvSize size)
// 'size' is the new size of the image, if necessary, memory is reallocated
// size.cx is the new height and size.cy is the new width
{
	if((size.height == height) && (size.width == width) && (buf != NULL) && (data != NULL) ) return; 
	assert(size.height >= 0); assert(size.width >= 0);
	Clear();

	height = size.height;	
	width = size.width;

//	cout<<width<<" "<<height<<endl;

	buf = new REAL[size.height*size.width]; /*assert(buf != NULL);*/
	data = new REAL*[size.height];	/*assert(data != NULL);*/
	for(int i=0;i<size.height;i++)	data[i] = &buf[i*size.width];
}

IntImage& IntImage::operator=(const IntImage& source)
{
	SetSize(cvSize(source.width,source.height));
	memcpy(buf,source.buf,sizeof(*buf)*height*width);
	label = source.label;
	variance = source.variance;

	return *this;
}

void IntImage::Resize(IntImage &result, REAL ratio) const
{
	result.SetSize(cvSize(int(width*ratio),int(height*ratio)));
	ratio = 1/ratio;
	for(int i=0,rh=result.height,rw=result.width;i<rh;i++)
		for(int j=0;j<rw;j++) {
			int x0,y0;
			REAL x,y,fx0,fx1;
			x = j*ratio; y = i*ratio;
			x0 = (int)(x);
			y0 = (int)(y);

			//by Jianxin Wu  
			//1. The conversion of float to int in C is towards to 0 point, i.e. the floor function for positive numbers, and ceiling function for negative numbers.
			//2. We only make use of ratio<1 in this applicaiton, and all numbers involved are positive.
			//Using these, we have 0<=x<=height-1 and 0<=y<=width-1. Thus, boundary conditions check is not necessary.
			//In languages other than C/C++ or ratio>=1, take care. 
			if (x0 == width-1) x0--;
			if (y0 == height-1) y0--;

			x = x - x0; y = y - y0;

			fx0 = data[y0][x0] + x*(data[y0][x0+1]-data[y0][x0]);
			fx1 = data[y0+1][x0] + x*(data[y0+1][x0+1]-data[y0+1][x0]);

			result.data[i][j] = fx0 + y*(fx1-fx0);
		}
}

void IntImage::CalculateVarianceAndIntegralImageInPlace(void)
// We pad a zero column and a zero row, so 24*24 image will be 25*25 in size
{
	REAL partialsum;
	REAL ex,ex2;
	REAL size,var;

	ex = 0;
	ex2 = 0;
	for(int i=0,size=height*width;i<size;i++)
	{
		ex += buf[i];
		ex2 += (buf[i]*buf[i]);
	}
	size = REAL(height*width);
	var = ex2/size - (ex/size)*(ex/size);
	assert(var >= 0);
	if(var>0)
		variance = sqrt(var);
	else
		variance = 1.0;


	for(int i=1;i<width;i++)
	{
		data[0][i] = data[0][i-1] + data[0][i];
	}
	for(int i=1;i<height;i++)
	{
		partialsum = 0;
		for(int j=0;j<width;j++)
		{
			partialsum += data[i][j];
			data[i][j] = data[i-1][j] + partialsum;
		}
	}
}

void IntImage::CalcSquareAndIntegral(IntImage& square, IntImage& image) const
{
	REAL partialsum,partialsum2;

	square.SetSize(cvSize(width+1,height+1));
	image.SetSize(cvSize(width+1,height+1));

	for(int i=0;i<=width+1;i++) square.buf[i]=image.buf[i]=0;
	for(int i=1;i<=height;i++)
	{
		partialsum = partialsum2 = 0;
		square.data[i][0] = 0;
		image.data[i][0] = 0;
		for(int j=1;j<=width;j++)
		{
			partialsum += (data[i-1][j-1]*data[i-1][j-1]);
			partialsum2 += data[i-1][j-1];
			square.data[i][j] = square.data[i-1][j] + partialsum;
			image.data[i][j] = image.data[i-1][j] + partialsum2;
		}
	}
}

void SwapIntImage(IntImage& i1,IntImage& i2)
{
	std::swap(i1.height,i2.height);
	std::swap(i1.width,i2.width);
	std::swap(i1.buf,i2.buf);
	std::swap(i1.data,i2.data);
	std::swap(i1.variance,i2.variance);
	std::swap(i1.label,i2.label);
}