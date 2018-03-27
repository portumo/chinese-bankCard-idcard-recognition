
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <fstream>
#include "ln_convnet.hpp"
//#include <android/log.h>
#include "BankCard.h"
//#define LARGENUM
#define showSteps 1
#define saveFlag 0

using namespace std;

struct  recCharAndP
{
	float recP;//识别的概率
	char recChar;
};

LightNet::ConvNet cnnDetect;



enum ConvolutionType {   
/* Return the full convolution, including border */
  CONVOLUTION_FULL, 
  
/* Return only the part that corresponds to the original image */
  CONVOLUTION_SAME,
  
/* Return only the submatrix containing elements that were not influenced by the border */
  CONVOLUTION_VALID
};
 
static void conv2(const Mat &img, const Mat& kernel, ConvolutionType type, Mat& dest) {
  Mat source = img;
  if(CONVOLUTION_FULL == type) {
    source = Mat();
    const int additionalRows = kernel.rows-1, additionalCols = kernel.cols-1;
    copyMakeBorder(img, source, (additionalRows+1)/2, additionalRows/2, (additionalCols+1)/2, additionalCols/2, BORDER_CONSTANT, Scalar(0));
  }
 
  Point anchor(kernel.cols - kernel.cols/2 - 1, kernel.rows - kernel.rows/2 - 1);
  int borderMode = BORDER_CONSTANT;
  filter2D(source, dest, img.depth(), kernel, anchor, 0, borderMode);
 
  if(CONVOLUTION_VALID == type) {
    dest = dest.colRange((kernel.cols-1)/2, dest.cols - kernel.cols/2)
               .rowRange((kernel.rows-1)/2, dest.rows - kernel.rows/2);
  }
}

void InitMat(Mat& m,float* num)
{
 for(int i=0;i<m.rows;i++)
  for(int j=0;j<m.cols;j++)
   m.at<float>(i,j)=*(num+i*m.rows+j);
}

void BankCard::readModel(string model10Path,string modelPath)
{
	cnnDetect.Load(model10Path.c_str());

	FILE *fp;
	if(fp=fopen(modelPath.c_str(),"rb"))
	{
		int height,width;
		fread(&height,sizeof(int),1,fp);
		fread(&width,sizeof(int),1,fp);
		W1 = Mat::zeros(height,width,CV_32FC1);
		for(int i = 0;i<height;i++)
		{
			float* pLine = (float*)W1.ptr(i);
			fread(pLine,sizeof(float),width,fp);
		}
		//cout<<W1.row(0)<<endl;
		numFilters = W1.rows;
		filterWidth=(int)sqrt((double)W1.cols-1);
		for(int i=0;i<numFilters;i++)
		{
			Mat filter = W1.row(i).colRange(1,W1.cols);
			filter = filter.reshape(0,filterWidth);
			filter = filter.t();
			//if(i==numFilters-1) cout<<filter<<endl;
			//flip(filter,filter,-1);
			filters.push_back(filter);
		}
		b1 = W1.col(0);
		if(numFilters>1)
		{
			fread(&height,sizeof(int),1,fp);
			fread(&width,sizeof(int),1,fp);
			W2 = Mat::zeros(height,width,CV_32FC1);
			for(int i = 0;i<height;i++)
			{
				float* pLine = (float*)W2.ptr(i);
				fread(pLine,sizeof(float),width,fp);
			}
		}

		fread(&numPatterns,sizeof(int),1,fp);
		for(int i=0;i<numPatterns;i++)
		{		
			fread(&width,sizeof(int),1,fp);
			Mat pattern = Mat::zeros(1,width,CV_32FC1);
			float* pLine = (float*)pattern.ptr(0);
			fread(pLine,sizeof(float),width,fp);
			if( ( i != 4)&&(i != 5)&&(i != 6)&&(i != 7)) //去掉%6 10  会干扰6-13
				patterns.push_back(pattern);		
		}
 		numPatterns = numPatterns - 4;
		
	//	numPatterns = 0;

		/*  可以在这里加入种类。
			pattern{1} = [0,24,54,80,105,131,156,206,232,258,283,334,358,383,408,432,456,486];%7 4 7
			pattern{2} = [0,26,53,78,129,155,181,206,258,282,306,330,386,412,437,463];%4 4 4 4
			pattern{3} = [0,25,51,76,101,128,178,203,228,254,279,330,354,380,406,430,457,482,507];%6 5 8
			pattern{4} = [0,25,51,78,104,129,155,180,206,232,258,283,309,335,361,387,411,437,462];%19
		//	pattern{5} = [0,25,50,74,99,124,188,213,237,262,287,311,336,361,385,410];%6 10  会干扰6-13
		//	pattern{6} = [0,25,48,73,121,145,169,218,243,267,292,316,340,365,389,437];%4 3 8 1
		//	pattern{7} = [0,24,48,71,93,116,163,186,210,232,256,301,324,347,419,443];%6 5 3 2
		//	pattern{8} = [0,25,49,73,96,121,167,191,215,239,263,286,311,334,357,381,404,429];%6 12
			pattern{9} = [0,22,47,70,94,116,162,185,210,233,279,302,326,348,394,417,441,465];%6 4 4 4
			pattern{10} = [0,23,50,75,100,125,174,200,225,250,298,322,346,376,426,450,476,500];%7 4 7 间距更大。。
			pattern{11} = [0,19,39,61,101,123,143,162,200,222,244,265,304,324,344,365];%4 4 4 4这个比较多，就多弄点，这个比pattern{2}小好几号。
			%pattern{12} = [0,23,49,75,104,128,154,181,214,237,262,286,317,345,372,395,429];%4 4 4 1锦州商业银行，好像就一家是这个。。
			%pattern{12} = [0,23,42,68, 92,114,138,160,183,226,249,275,297,320,342,386,409,432,456,477,500];%6 6 6
			pattern{12} = [0,25,49,73,96,121,167,191,215,239,263,286,334,357,381,404,429,454];%6 6 6  */
			// pattern{13} = [];

			float m[19]={ 15, 41, 67, 92, 141, 167, 191, 215, 265, 289, 310, 332, 388, 415, 438, 462, 487, 512, 537};	 // 4 4 4 7
			//	float m[19]={ 0,26,53,78,129,155,181,206,258,282,306,330,386,412,437,463,490,517,543};	 // 4 4 4 7	              
			Mat pattern = Mat::zeros(1,19,CV_32FC1);
			InitMat(pattern,m);

			patterns.push_back(pattern);
			numPatterns = numPatterns + 1;

		//	float m2[19]={ 24,46,71,97,122,147,200,225,250,275,300,326,352,378,405,431,457,484,509};	 // 6 13	
			float m2[19]={ 42,67,91,116,140,164,214,242,265,289,313,338,364,391,415,440,465,490,515};
			Mat pattern2 = Mat::zeros(1,19,CV_32FC1);
			InitMat(pattern2,m2);
			patterns.push_back(pattern2);
			numPatterns += 1;

			//float m3[16]={ 0,26,53,78,129,155,181,206,258,282,306,330,386,410,436,462};	 // 4 4 4 4	              
			//Mat pattern3 = Mat::zeros(1,16,CV_32FC1);
			//InitMat(pattern3,m3);
			//patterns.push_back(pattern3);
			//numPatterns += 1;
	}

	numHeightScale = 11.5;
	numWidthScale = 0.75;
	fclose(fp);

}

BankCard::BankCard(void)
{
}

//BankCard::BankCard(string model10Path,string modelPath)
//{
//    FILE *fp;
//    if(fp=fopen(modelPath.c_str(),"rb"))
//    {
//        int height,width;
//        fread(&height,sizeof(int),1,fp);
//        fread(&width,sizeof(int),1,fp);
//        W1 = Mat::zeros(height,width,CV_32FC1);
//        for(int i = 0;i<height;i++)
//        {
//            float* pLine = (float*)W1.ptr(i);
//            fread(pLine,sizeof(float),width,fp);
//        }
//        //cout<<W1.row(0)<<endl;
//        numFilters = W1.rows;
//        filterWidth=(int)sqrt((double)W1.cols-1);
//        for(int i=0;i<numFilters;i++)
//        {
//            Mat filter = W1.row(i).colRange(1,W1.cols);
//            filter = filter.reshape(0,filterWidth);
//            filter = filter.t();
//			//if(i==numFilters-1) cout<<filter<<endl;
//            //flip(filter,filter,-1);
//            filters.push_back(filter);
//        }
//        b1 = W1.col(0);
//        if(numFilters>1)
//        {
//            fread(&height,sizeof(int),1,fp);
//            fread(&width,sizeof(int),1,fp);
//            W2 = Mat::zeros(height,width,CV_32FC1);
//            for(int i = 0;i<height;i++)
//            {
//                float* pLine = (float*)W2.ptr(i);
//                fread(pLine,sizeof(float),width,fp);
//            }
//        }
//        
//        fread(&numPatterns,sizeof(int),1,fp);
//        for(int i=0;i<numPatterns;i++)
//        {
//            fread(&width,sizeof(int),1,fp);
//            Mat pattern = Mat::zeros(1,width,CV_32FC1);
//            float* pLine = (float*)pattern.ptr(0);
//            fread(pLine,sizeof(float),width,fp);
//            patterns.push_back(pattern);
//            //cout<<pattern<<endl;
//        }
//    }
//	if(fp=fopen(model10Path.c_str(),"rb"))
//    {
//        int height,width;
//        fread(&height,sizeof(int),1,fp);
//        fread(&width,sizeof(int),1,fp);
//        W1s = Mat::zeros(height,width,CV_32FC1);
//        for(int i = 0;i<height;i++)
//        {
//            float* pLine = (float*)W1s.ptr(i);
//            fread(pLine,sizeof(float),width,fp);
//        }
//        //cout<<W1.row(0)<<endl;
//        numFilters_s = W1s.rows;
//        //filterWidth=(int)sqrt((double)W1.cols-1);
//        for(int i=0;i<numFilters_s;i++)
//        {
//            Mat filter = W1s.row(i).colRange(1,W1s.cols);
//            filter = filter.reshape(0,filterWidth);
//            filter = filter.t();
//			//if(i==numFilters_s-1) cout<<filter<<endl;
//            //flip(filter,filter,-1);
//            filters_s.push_back(filter);
//        }
//        b1s = W1s.col(0);
//        if(numFilters_s>1)
//        {
//            fread(&height,sizeof(int),1,fp);
//            fread(&width,sizeof(int),1,fp);
//            W2s = Mat::zeros(height,width,CV_32FC1);
//            for(int i = 0;i<height;i++)
//            {
//                float* pLine = (float*)W2s.ptr(i);
//                fread(pLine,sizeof(float),width,fp);
//            }
//        }
//    }
//  
//    numHeightScale = 11.5;
//    numWidthScale = 0.75;
//}

const int imgWidth = 16;
const int imgSize = imgWidth * imgWidth;

recCharAndP rec(IplImage *image){
	/*char * name = "9.bmp";
	IplImage * image = cvLoadImage(name,1);
*/
	/*cvNamedWindow("1",0);
	cvShowImage("1",image);*/

	IplImage *pSrc=cvCreateImage(cvSize(imgWidth,imgWidth),8,3);
	cvResize(image,pSrc);


	IplImage* pRGB = cvCreateImage(cvSize(imgWidth,imgWidth),8,3);
	cvCvtColor(pSrc,pRGB,CV_BGR2RGB);

	uchar * imgData;
	imgData = (uchar *)pRGB->imageData;
	/*for (int i=0;i<3072;i++){
	cout<<(int)imgData[i]<<" ";
	}
	*/

	uchar rdata[imgSize];	uchar gdata[imgSize];	uchar bdata[imgSize];
	int m = 0; int n = 0; int l = 0;
	for (int i=0;i<imgSize*3;i++){
		if(i%3 == 0){
			bdata[m++] = imgData[i];
		}

		if(i%3 == 1){
			gdata[n++] = imgData[i];
		}

		if(i%3 == 2){
			rdata[l++] = imgData[i];
		}
	}

	uchar *data;
	data = (uchar *)malloc(sizeof(uchar)*imgSize*3);
	m = 0; n = 0; l = 0;
	for(int i=0;i<imgSize;i++){
		data[i] = bdata[m++] ;	
	}
	for(int i=imgSize;i<imgSize*2;i++){
		data[i] = gdata[n++] ;	
	}
	for(int i=imgSize*2;i<imgSize*3;i++){
		data[i] = rdata[l++] ;	
	}

	/*for (int i=0;i<3072;i++){
		if(i % 1024 == 0)
		cout<<(int)data[i]<<" ";
	}*/

		char *Label[10];
		Label[0] = "0";
		Label[1] = "1";
		Label[2] = "2";
		Label[3] = "3";
		Label[4] = "4";

		Label[5] = "5";
		Label[6] = "6";
		Label[7] = "7";
		Label[8] = "8";
		Label[9] = "9";	
		
		vector<float> output;
	//	getStartTime();
		cnnDetect.Forward( (unsigned char *)data, imgWidth, imgWidth, 3, output);
	//	getEndTime();
	// 	cout<<"消耗时间:"<<dfTim<<"ms"<<endl;
				
		int idx = 0;
	//	float singleCharSumP = 0.0;
		for (int ii = 0; ii < 10; ii++){
		//	singleCharSumP += output[ii];
		//	cout<<"单个字符的概率： "<<output[ii]<<endl;
			if (output[idx] < output[ii])
				idx = ii;
		}
	//	cout<<"单个字符总的概率： "<<singleCharSumP<<endl;

	//	cout<<Label[idx]<<endl;
	//	cvWaitKey(0);

		//	cout<<Label[idx]<<" 概率:("<<output[idx]<<")"<<" 消耗时间:"<<dfTim<<"ms"<<endl;
		if(Label[idx] == "10")
			Label[idx] = " ";

		recCharAndP result;
		result.recP = output[idx];
		result.recChar = Label[idx][0];

		cvReleaseImage(&pSrc);
		cvReleaseImage(&pRGB);
		free(data);
		return result;
}


vector<Mat> BankCard::findNum( Mat &image,Mat &imgRGB )
{
	vector<Mat> imageChar;

	Mat nImg;
	Mat nImgRGB;//保存彩色信息的图像
 	int s = 4;
    Mat img = image.clone();
    Mat dx = Mat::zeros(img.rows, img.cols, CV_32F);
    Mat dy = Mat::zeros(img.rows, img.cols, CV_32F);
	
	Sobel(img, dx, CV_32F, 1, 0, 3, 1, 0, BORDER_REPLICATE);
    Sobel(img, dy, CV_32F, 0, 1, 3, 1, 0, BORDER_REPLICATE);
    Mat gradient;
	pow(dx,2,dx);
	pow(dy,2,dy);
	sqrt(dx+dy,gradient);
  
	/*cout<<dx(Rect(0,0,10,10))<<endl;
	cout<<dy(Rect(0,0,10,10))<<endl;
	cout<<gradient(Rect(0,0,10,10))<<endl;*/

    img = img.rowRange(img.rows/4,img.rows*4/5);
	imgRGB = imgRGB.rowRange(imgRGB.rows/4,imgRGB.rows*4/5);
    gradient = gradient.rowRange(gradient.rows/4,gradient.rows*4/5);
	
    float numh = (float)image.rows/numHeightScale;
    float numw = numh*numWidthScale;
	resize(img,nImg,Size( floor((float)img.cols*(float)filterWidth/numw), floor((float)img.rows*(float)filterWidth/numh)),0,0);
	resize(imgRGB,nImgRGB,Size( floor((float)img.cols*(float)filterWidth/numw) , floor((float)img.rows*(float)filterWidth/numh)),0,0);
	
    Mat nGrad;
	resize(gradient,nGrad,Size( floor((float)gradient.cols*(float)filterWidth/numw) , floor((float)gradient.rows*(float)filterWidth/numh)),0,0);
	
	imshow("输入原始灰度图像，resize之后的灰度图像",img);
	imshow("输入原始彩色图像，resize之后的彩色图像",imgRGB);
	imshow("sobel梯度之后的图像",gradient);
	imshow("拉伸之后的灰度图",nImg);
	imshow("拉伸之后的彩色图",nImgRGB);
	imshow("拉伸之后的梯度图",nGrad);
		waitKey(0);

	//第二步： --** 根据字符行可能出现的区域将字符行ROI出来 **--//
	IplImage srcResize = imgRGB;

	CvRect roi;
	roi.x = 0;
	roi.y = 70;
	roi.width = (&srcResize)->width ;
	if( (roi.width + roi.x)>(&srcResize)->width )
		roi.width = (&srcResize)->width - roi.x;
	roi.height = 60;

	IplImage *dst = cvCreateImage(cvSize(roi.width,roi.height),8,(&srcResize)->nChannels);
//	std::cout<<dst->width<<" "<<dst->height<<endl;
	cvSetImageROI((&srcResize),roi);
	cvCopy(&srcResize,dst);
	cvResetImageROI(&srcResize);

	if(showSteps){
		cvNamedWindow("dst",1);
		cvShowImage("dst",dst);
	}

	
	//第三步--- **根据训练的样本大小，将待卷积的全图按照 样本字符区域大小进行resize** ---//
	IplImage * dstCnn = cvCreateImage(cvSize( cvFloor(  imgWidth*( (&srcResize)->width )/20.0 ), cvFloor( imgWidth*60.0/30.0) ),8,dst->nChannels);
	cvResize(dst,dstCnn);

	if(showSteps){
		cvNamedWindow("dstCnn",1);
		cvShowImage("dstCnn",dstCnn);
		cvWaitKey(0);
	}
	
	//cvSaveImage("dstCnn.bmp",dstCnn);

	//第四步--**全图卷积，得到概率分布图**--//
	double t = (double)getTickCount();  

	int stride = 5;
	int strideWidth = 4*stride;
	IplImage * imgCnns = cvCreateImage(cvSize( (dstCnn->width - imgWidth)/strideWidth +1 , (dstCnn->height - imgWidth)/stride+1 ),8,1);
	//	IplImage * imgCnns = cvCreateImage(cvSize( 60, 24),8,1);

	//----- 对dstCnn进行imgWidth*imgWidth扫描 ----- 全图卷积 -----//
	IplImage* cnn0 ;
	float recPP = 0.0; //全图卷积的CNN概率和
	for(int i=0;i<dstCnn->height - imgWidth;i+=stride){
		for(int j=0;j<dstCnn->width -imgWidth;j+=strideWidth){
			CvRect roi;
			roi.x = j;
			roi.y = i;
			roi.width = imgWidth;
			roi.height = imgWidth;

			cnn0 = cvCreateImage(cvSize(imgWidth,imgWidth),8,3);
			cvSetImageROI(dstCnn,roi);
			cvCopy(dstCnn,cnn0);
			cvResetImageROI(dstCnn);

		//	cvShowImage("the patch of single cnn uesd ",cnn0);

			//对cnn0进行识别识别，得到概率分布图
			recCharAndP result = rec(cnn0);
			//cout<<" "<< j <<"-"<< i <<"-"<<result.recP<<"-"<<result.recChar<<endl;

			recPP = recPP + result.recP;

			((uchar*)(imgCnns->imageData + imgCnns->widthStep*i/stride))[j/strideWidth] = (uchar)cvFloor(result.recP * 255);
		}
	}
	cvReleaseImage(&cnn0);

	t = ((double)getTickCount() - t)/getTickFrequency();  
	if(showSteps){
		cout << "概率和：" << recPP << "Times passed in seconds: " << t << endl;  
	
		cvNamedWindow("imgCnns",0);
		cvShowImage("imgCnns",imgCnns);
	//	cvSaveImage("imgCNN1.bmp",imgCnns);
	}

	if(recPP < 45.0 )  /* 加入根据全图卷积概率和筛选银行卡号图片和非银行卡号识别（包括模糊银行卡号图片） */
		return imageChar;

	IplImage * imgBin = cvCreateImage(cvGetSize(imgCnns),8,1);
	cvThreshold(imgCnns,imgBin,252,255,CV_THRESH_BINARY);

	if(showSteps){
		cvNamedWindow("imgBin",0);
		cvShowImage("imgBin",imgBin);
	}

	//第五步--** Y轴投影 确定字符行具体区域 **---//
	IplImage* painty=cvCreateImage( cvGetSize(imgBin),IPL_DEPTH_8U, 1 );  		
	cvZero(painty);  
	int* h=new int[imgBin->height];  		
	memset(h,0,imgBin->height*4);  

	int x,y;  
	CvScalar sData;  
	for(y=0;y<imgBin->height;y++)  
	{  
		for(x=0;x<imgBin->width;x++)  
		{  
			sData=cvGet2D(imgBin,y,x);           
			if(sData.val[0]==0)  
				h[y]++;       
		}     
	} 

	//将y投影后，找值最大的点
	int temp = 0;
	for(y=0;y<imgBin->height;y++)   {  
		//if((imgBin->width-h[y]) <= 80)
		//	h[y] = imgBin->width;
	//	printf("%d ",imgBin->width-h[y]); 
		if(temp < (imgBin->width-h[y]))
			temp = imgBin->width-h[y];
	}
//	printf("y投影最大值是： %d\n",temp);
	int y0 = 0;
	for(y=0;y<imgBin->height;y++)   {  
		if(temp == (imgBin->width-h[y])){
			y0 = y;
		//	break; //有两个以上相等的，取第一个
		}
	}

	free(h);

	//确定原始图像中y方向的位置
	CvRect yRoi;
	yRoi.x = 0;
	yRoi.y = stride*(y0);
	yRoi.width = dstCnn->width;
	yRoi.height = imgWidth;

	IplImage *dstY = cvCreateImage(cvSize(yRoi.width,yRoi.height),8,dstCnn->nChannels);
	cvSetImageROI(dstCnn,yRoi);
	cvCopy(dstCnn,dstY);
	cvResetImageROI(dstCnn);
	if(showSteps){
		cvNamedWindow("dstY",0);
		cvShowImage("dstY",dstY);
		cvWaitKey();
	}

	
//	cnnDetect.~ConvNet();

//	cvWaitKey(0);
	//Mat RE = Mat::ones(nImg.rows-filterWidth+1,nImg.cols-filterWidth+1,CV_32F);
	//RE *= W2s.at<float>(0,0);

	//for(int i=0;i<numFilters_s;i++)
	//{
	//	Mat Fk;
	//	conv2(nGrad,filters_s[i],CONVOLUTION_VALID,Fk);    
	//	Fk = max(Fk+W1s.at<float>(i,0),0)*0.5;  
	//	RE += Fk*W2s.at<float>(0,i+1);     
	//}
	//RE = max(RE,0);
	//imshow("行检测卷积概率分布图",RE);

	//float maxMean=0;
	//int maxp;
	//for(int i=0;i<RE.rows;i++)
	//{
	//	float thisMean = mean(RE.row(i))[0];
	//	if(thisMean>maxMean)
	//	{
	//		maxMean=thisMean;
	//		maxp=i;
	//	}
	//}

	//以上不需要；我只需要在这里对maxp求值就行了，maxp就是字符行所在行的位置；
	/**--- 输入是imgRGB ---**/
	int maxp = stride*(y0)*(30.0/imgWidth) + roi.y;
	
	Mat lineImage0 = img.rowRange(maxp*numh/(float)filterWidth,(maxp+filterWidth)*numh/(float)filterWidth);
	if(showSteps)
		imshow("检测出来的原始字符行",lineImage0);
	
	//在检测出来的原始字符行的基础上，上下各偏移s*2 加宽原来的字符。这里降低s的值可以提升速度。
    //Mat lineImage = img.rowRange(maxp*numh/(float)filterWidth,(maxp+filterWidth)*numh/(float)filterWidth);
    nImg = nImg.rowRange(max(maxp-s*2,0),min(maxp+filterWidth+s*2,nImg.rows));
	nImgRGB = nImgRGB.rowRange(max(maxp-s*2,0),min(maxp+filterWidth+s*2,nImgRGB.rows));
	if(showSteps){
		imshow("检测出来的拉伸后的加宽灰度字符行",nImg);
		imshow("检测出来的拉伸后的加宽彩色字符行",nImgRGB);
	}

	nGrad = nGrad.rowRange(max(maxp-s*2,0),min(maxp+filterWidth+s*2,nGrad.rows));
	if(showSteps){
		imshow("检测出来的拉伸后的字符行加宽后所对应的梯度",nGrad);
	}

	int subImageStart = max(maxp-s*2,0); //检测的起始点
  //  imshow("numLine",lineImage);
    //waitKey(0);

	//**--- RE为卷积
	Mat RE = Mat::ones(nImg.rows-filterWidth+1,nImg.cols-filterWidth+1,CV_32F);
    //Mat REr = Mat::ones(nImg.rows-filterWidth+1,nImg.cols-filterWidth+1,CV_32F);
    RE *= W2.at<float>(0,0);

    for(int i=0;i<numFilters;i++)
    {
        Mat Fk;
		conv2(nGrad,filters[i],CONVOLUTION_VALID,Fk);
        //matchTemplate(nGrad,filters[i],Fk,CV_TM_CCORR);
		/*filter2D(nGrad,Fk,CV_32F,filters[i],Point(0,0));
        Fk = Fk(Rect(0,0,nImg.cols-filterWidth+1,nImg.rows-filterWidth+1));*/

        Fk = max(Mat(Fk+W1.at<float>(i,0)),0.0f)*0.5;
        //cout<<W2.at<float>(0,i+1)<<endl;
        RE += Fk*W2.at<float>(0,i+1);
        //cout<<RE(Rect(0,0,10,5))<<endl;
    }
    RE = max(RE,0);

	if(showSteps)
		imshow("REi",RE);

    //cout<<RE(Rect(0,0,10,5))<<endl;
    float maxMean=0;
    for(int i=0;i<RE.rows;i++)
    {
        float thisMean = mean(RE.row(i))[0];
        if(thisMean>maxMean)
        {
            maxMean=thisMean;
            maxp=i;
        }
     //   cout<<thisMean<<" ";
    }
	subImageStart += maxp;
	subImageStart *= numh/(float)filterWidth;
	Mat lineImage = nImg.rowRange(maxp,maxp+filterWidth);
   
	if(showSteps){
		imshow("lineImage",lineImage);
	//	imwrite("1.jpg",lineImage);
		cvWaitKey(0);
	}

    int rStart = max(maxp-s,0);
    int rEnd = min(maxp+s,RE.rows-1);
    Mat confidence = Mat::zeros(1,RE.cols,CV_32F);
    
    for(int j=0;j<RE.cols;j++)
    {
        for(int i=rStart;i<=rEnd;i++)
        {
            if(RE.at<float>(i,j)>confidence.at<float>(0,j)) confidence.at<float>(0,j)=RE.at<float>(i,j);
			//confidence.at<float>(0,j)+=RE.at<float>(i,j);
        }
    }
    //outfile<<confidence<<endl;
     double minV,maxV,maxMD=0;
    Point maxPoint;
    Mat mD = Mat::zeros(confidence.size(),confidence.type());
    Mat pD = Mat::zeros(confidence.size(),CV_32S);
	
    for(int j=s;j<confidence.cols-s;j++)
    {
        minMaxLoc(confidence.colRange(j-s,j+s+1),&minV,&maxV,(Point *)0,&maxPoint);
        mD.at<float>(0,j) = maxV;
		//mD.at<float>(0,j) = confidence.at<float>(0,j);
        pD.at<int>(0,j) = maxPoint.x+j-s;
		if(maxMD<maxV) maxMD=maxV;
    }
	Mat drawMD = Mat::zeros(400,mD.cols,CV_8UC1);
	for(int j=0;j<mD.cols;j++)
    {
		line(drawMD,Point(j,400-floor(confidence.at<float>(0,j)*400/maxMD)),Point(j,399),Scalar(255));
	}

	if(showSteps){
		imshow("drawMD",drawMD); 
		waitKey(0);
	}
    //outfile<<mD<<endl;
    //cout<<pD<<endl;

	//根据卷积之后的概率直方图确定字符分割点
    Mat BestPattern;
    float maxMatch=0;
	int BestPatternIndex=0;

    for(int i=0;i<numPatterns;i++)
    {
		//if(i==7)continue;
        for(float scale=0.8;scale<=1.2;scale+=0.02)
        {
            //cout<<"pattern "<<i<<" scale "<<scale<<endl;
            Mat thisPattern = patterns[i] * scale;
            //cout<<thisPattern<<endl;
            for(int b = 0;b<mD.cols-thisPattern.at<float>(0,thisPattern.cols-1)-1;b++)
            {
                float thisMatch=0;
                for(int k=0;k<thisPattern.cols;k++)
                {
                    thisMatch += mD.at<float>(0,floor(thisPattern.at<float>(0,k))+b);
                }
                thisMatch /= (float)(thisPattern.cols);
				thisMatch *= sqrt((float)(thisPattern.cols+100));
                //cout<<thisMatch<<" ";
                if(thisMatch>=maxMatch)
                {
                    maxMatch=thisMatch;
                    BestPattern = thisPattern.clone() + b;
					BestPatternIndex = i;
                }
            }
			//cout<<i<<" "<<maxMatch<<endl;
        }
		//cout<<i<<" "<<maxMatch<<endl;
    }
	cout<<maxMatch<<endl;
	if(maxMatch < 300.0 )  /* 加入根据投影匹配值，筛选直接返回的 */
		return imageChar;
    for(int k=0;k<BestPattern.cols;k++)
    {
        BestPattern.at<float>(0,k) = pD.at<int>(0,floor(BestPattern.at<float>(0,k)));
    }
	BestPattern = BestPattern - 2 ;

    //cout<<BestPattern<<endl;
    //BestPattern *= numw/(float)filterWidth;


	
	int imageNums = 0;


    Mat result;
    for(int k=0;k<BestPattern.cols;k++)
    {

		float candidate[10],maxCandi=0;
		memset(candidate,0,sizeof(candidate));
		int guess;

		#define scanSize 0
		int strideH = 1;
		//制作样本的时候，加上stride，通过偏移设置不同的点。
		for(int x=-scanSize;x<=scanSize;x+=strideH)
            {

				if(floor(BestPattern.at<float>(0,k))+x<0||floor(BestPattern.at<float>(0,k))+filterWidth+x>=nImg.cols)continue;
				if( ((floor(BestPattern.at<float>(0,k))+x ) >= nImgRGB.cols) || (( floor(BestPattern.at<float>(0,k))+filterWidth+x ) >= nImgRGB.cols) ) continue;

			//	cout<<floor(BestPattern.at<float>(0,k))+x<<" "<<floor(BestPattern.at<float>(0,k))+filterWidth+x<<" "<<nImgRGB.cols<<endl;
                for(int y=-scanSize;y<=scanSize;y+=strideH)
                {

					if(maxp+y<0||maxp+filterWidth+y>=nImg.rows) continue;
					
				//	cout<<maxp+y<<" "<<maxp+filterWidth+y<<" "<<nImg.rows<<endl;

                    Mat newImage = nImg.rowRange(maxp+y,maxp+filterWidth+y).colRange(floor(BestPattern.at<float>(0,k))+x,floor(BestPattern.at<float>(0,k))+filterWidth+x);

					Mat newImageRGB = nImgRGB.rowRange(maxp+y,maxp+filterWidth+y).colRange(floor(BestPattern.at<float>(0,k))+x,floor(BestPattern.at<float>(0,k))+filterWidth+x);


					/*Mat newImageRGB;
					cvtColor(newImage, newImageRGB, CV_GRAY2BGR);*/ 
					imageChar.push_back(newImageRGB);

					/*imshow("nImg",nImg);
					imshow("newImage",newImage);

					imshow("nImgRGB",nImgRGB);*/
					if(showSteps){
						char showWindow[200];
						imageNums ++ ;
						sprintf(showWindow,"%d%s",imageNums,".bmp");
						moveWindow(showWindow,imageNums*60,imageNums);
						imshow(showWindow,newImageRGB);


						imwrite(showWindow,newImageRGB);
					}
					
				//	waitKey(0);
                 /*   int num = ocr->recognize(newImage);*/
					///////////////////
					//////////////////////
					///////////////////////
                   // candidate[num] += ocr->matchRate * exp(-0.1*(x*x+y*y));
					//if(candidate[num]>maxCandi) maxCandi=candidate[num],guess=num;

                }
            }

		result.push_back(guess);
    }
//	int64 end_time = getTickCount();
//	cout<<result.t()<<" "<<"pattern "<<BestPatternIndex<<" time:"<<(end_time-start_time)/getTickFrequency()<<endl;

	if(showSteps){
		for(int k=0;k<BestPattern.cols;k++)
		{
			lineImage.col((int)floor(BestPattern.at<float>(0,k)))=Scalar(0);
			lineImage.col((int)floor(BestPattern.at<float>(0,k))+1) = Scalar(0);
		}

		Mat displayMat = Mat::zeros(drawMD.rows+lineImage.rows,lineImage.cols,CV_8UC1);
		drawMD.copyTo(displayMat(Rect(0,0,drawMD.cols,drawMD.rows)));
		//imshow("drawMD",displayMat);
		lineImage.copyTo(displayMat(Rect(0,drawMD.rows,lineImage.cols,lineImage.rows)));
		//imshow("lineImage",displayMat);

		imshow("result",displayMat);
	}

	cvReleaseImage( &dst );
	cvReleaseImage ( &dstCnn );
	cvReleaseImage ( &dstY );
	cvReleaseImage ( &painty );
	cvReleaseImage ( &imgCnns );
	cvReleaseImage ( &imgBin );
    return imageChar;
}

BankCard::~BankCard(void)
{
}






