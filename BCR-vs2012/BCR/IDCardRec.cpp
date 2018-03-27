
#include "IDCardRec.h"
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
//#include <android/log.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG


/*
string processingTotal(){
	initTrainImage();

	int imgNum = imgNames.size();
	cout<<imgNum<<endl;
	for(int iNum=0;iNum<imgNum;iNum++){
		__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "after %s", imgNames[iNum].c_str() );
		cout<<endl<<iNum<<endl;
		cout<<imgNames[iNum].c_str()<<endl;
		IplImage * src=cvLoadImage(imgNames[iNum].c_str(),1);
		if(!src) continue;
			__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "start to proc %s", imgNames[iNum].c_str() );
//		if(showSteps){
//			cvNamedWindow("image",1);
//			cvShowImage("image",src);
//		}


		string result = processingOne(src);
		__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "after %s", result.c_str() );
		cout<< result<<endl;
		return result;
	}
}

void init(){
//	cnn.Load("ConvexCard20.model");
	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "before load testWeight24.bin." );
	cnn.Load("/storage/sdcard0/bcnr/testWeight24.bin");
	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "after load testWeight24.bin." );
	//====CNN识别初始化====//
	cnnPlane.Load("/storage/sdcard0/bcnr/testWeight16p.bin");
	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "after load testWeight16p.bin." );
	//====检测初始化====//
	BC.readModel("/storage/sdcard0/bcnr/testWeight16.bin","/storage/sdcard0/bcnr/npclass.bin");
	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "after load testWeight16.bin." );

	InitTestGlobalData();
}
*/
static void OpenClose(IplImage* src,IplImage* dst,int pos)
{
	int an =1;
	IplConvKernel* element = 0;
	int element_shape = CV_SHAPE_RECT;
	element = cvCreateStructuringElementEx( an*2+1, an*2+1, an, an, element_shape, 0 );
	if( pos < 0 )
	{
		cvErode(src,dst,element,1);
		cvDilate(dst,dst,element,1);
	}
	else
	{
		cvDilate(src,dst,element,1);
		cvErode(dst,dst,element,1);
	}
	cvReleaseStructuringElement(&element);

}

void InsertSort(int a[],int count)
{
	int i,j,temp;
	for(i=1;i<count;i++)   
	{
		temp=a[i];
		j=i-1;
		while(a[j]>temp && j>=0)
		{
			a[j+1]=a[j];
			j--;
		}
		if(j!=(i-1))     
			a[j+1]=temp;
	}
}

//resultFinal  sortAndPrint_without_kongge(vector<resultPos> recResults){
//	resultFinal temp;
//
//	const int num = recResults.size();
//	if(num<=0) {
//		temp.recPFinal = 0;
//		temp.recString = "0";
//		return temp;
//	}
//
//	char *resultChar;
//	resultChar = (char *)malloc(num*sizeof(char));
//
//	int *rec;
//	rec = (int *)malloc(num*sizeof(int));
//	for(int i=0;i<num;i++){
//		rec[i] = recResults[i].recPosition;
//	}
//
//	InsertSort(rec,num);
//
//	for(int i=0;i<num;i++){
//		for(int j=0;j<num;j++){
//			if(rec[i] == recResults[j].recPosition){
//				resultChar[i] = recResults[j].recCharP.recChar;
//				break;
//			}
//		}
//
//	}
//
//
//	float sumP = 0;
//	for(int i=0;i<num;i++){
//		sumP += recResults[i].recCharP.recP;
//		cout<<i<<" "<<recResults[i].recCharP.recP<<endl;
//
//	}
//	cout<<sumP<<endl;
//
//	temp.recPFinal = sumP;
//
//
//	//for(int i=0;i<num;i++) { cout<<resultChar[i];}
//	//cout<<endl;
//
//	std::string str(resultChar, resultChar + num);
//	cout<<str <<endl;
//	temp.recString = str;
//
//	return temp;
//
//}


resultFinal  sortAndPrint(vector<resultPos> recResults){
	resultFinal temp;

	const int num = recResults.size();
	if(num<=0) {
		temp.recPFinal = 0;
		temp.recString = "0";

	//	cout<<"************进入另一种类型卡识别**************"<<endl;
		return temp;
	}

	char *resultChar;
	resultChar = (char *)malloc(num*sizeof(char));

	int *rec;
	rec = (int *)malloc(num*sizeof(int));
	for(int i=0;i<num;i++){
		rec[i] = recResults[i].recPosition;
	}

	InsertSort(rec,num);

	for(int i=0;i<num;i++){
		for(int j=0;j<num;j++){
			if(rec[i] == recResults[j].recPosition){
				resultChar[i] = recResults[j].recCharP.recChar;
				break;
			}
		}

	}

	int tempFlag = 0;
	char result[30] = {'a'};
	//	char *result ;
	int resultNum = 0;
	for(int i=0;i<num-1 ;i++){
		result[resultNum++] = resultChar[i];

		if( (rec[i+1] - rec[i]) > 50){
			++tempFlag;
		//	cout<<"=============="<<i<<" "<<++tempFlag<<"  "<<(rec[i+1] - rec[i])<<endl;
			result[resultNum++] = ' ';
		}
	}

	/*for(int i=0;i<30;i++){
	if(result[i] != 'a' )
	cout<<result[i];
	}
	cout<<endl;*/
	char *resultCharKG;
	resultCharKG = (char *)malloc((num+tempFlag)*sizeof(char));
	for(int i=0;i<(num+tempFlag) ;i++){
		resultCharKG[i] = result[i];
	}

	float sumP = 0;
	for(int i=0;i<num;i++){
		sumP += recResults[i].recCharP.recP;
//		cout<<"第"<<i<<"个的概率为："<<recResults[i].recCharP.recP<<"  结果是："<<resultCharKG[i]<<endl;

	}
//	cout<<"识别的银行卡总的概率为："<<sumP<<endl;

	temp.recPFinal = sumP;


	//for(int i=0;i<num;i++) { cout<<resultChar[i];}
	//cout<<endl;	
	std::string str(resultCharKG, resultCharKG + num + tempFlag);


//	cout<<"识别结果是："<<str.c_str() <<endl;
	temp.recString = str;

	delete rec;
	delete resultCharKG;
	delete resultChar;
	return temp;

}

void recChar(vector<imageIpl> imageChar){
	int imgNum = imageChar.size();
	for(int iNum=0;iNum<imgNum;iNum++){
		char name[200];
		sprintf(name,"%s%d%s%d%s","E:\\CreditCardNum\\",iNum,"_",1,".bmp");
		cvSaveImage(name,imageChar[iNum].roiImage);
	}	
	vector<imageIpl>(imageChar).swap(imageChar);
}


bool vifResult(const char* str, bool bCheck = true )
{
	char* lpszCard;

	/*char a[] = "this is a dog.";
	char *lpszCards = " ";
	char * lpszCard = strtok( a, lpszCards );
	while( lpszCard )
	{
	if( lpszCard == a )
	strcpy( a, lpszCard );
	else
	strcat( a, lpszCard );
	lpszCard = strtok( NULL, lpszCards );
	}

	cout<<"3333333333333333333333333333:"<<endl;
	cout<<lpszCards<<endl;
	cout<<lpszCard<<endl;*/


	int j = 0;	//j表示不为空格的位数。
	for(int i=0;i<strlen(str);i++){
		if(str[i] != ' '){
			j++;
		}
	}

//	cout<<"不为0的个数："<<j<<endl;
	int jNum = 0;
	lpszCard = (char *)malloc(j*sizeof(char));
	for(int i=0;i<strlen(str);i++){
		if(str[i] != ' '){
			lpszCard[jNum++] = str[i];
		}
	}
//	cout<<"不为0的个数："<<jNum<<endl;
	/*for(int i=0;i<jNum;i++){
		cout<<"** "<<lpszCard[i]<<endl;
	}*/

	int nLength = jNum;
//	cout<<"不为0的个数："<<nLength<<endl;
	if( nLength != 16 && nLength != 19 )
		return false;

	if( !bCheck )
		return true;

//	int nCheck = ( lpszCard[ nLength - 1 ] - '0' );
	int sum = 0;
//	nLength -= 2;
	for( int i = 0; i < nLength; ++ i )
	{
		int j = nLength-1 - i;
		int w = lpszCard[ j ] - '0';
	//	cout<<"识别的数组为："<<w<<endl;;
		if( i % 2 == 0 )
		{
			int n = w * 2;
			if( n > 9 )
			{
				sum += ( n % 10 );
				sum += ( n / 10 );
			}
			else
				sum += n;
		}
		else
		{
			sum += w;
		}
	}

	free(lpszCard) ; 
	lpszCard = NULL;

	if(( sum  ) % 10 == 0 )
		return true;
	else
		return false;
}

extern IplImage* res_im;
string processingOne(IplImage * src){
	
	//IplImage * dst = jiaoZheng(src);
	char vif[5]; 
	/*char *vifAllLine; 
	vifAllLine = (char *)malloc(5*sizeof(char));*/
	for(int i=0;i<4;i++){
		vif[i] = '1';
	}

	vifLine(src,vif);
	vif[4] = '\0';

	if(showSteps)
		if( (vif[0] == '1')&&(vif[1] == '1')&&(vif[2] == '1')&&(vif[3] == '1') )
			cout<<"right"<<endl;

	/*for(int i = 0;i<4;i++){
		if(showSteps)
			printf("%c\t",vif[i]);
		if(vif[i] != '1' )
			return "";
	}*/
	if(showSteps)
		cout<<endl;


	IplImage * dst = jiaozheng2(res_im);
	if(showSteps){
		cvNamedWindow("dst",1);
		cvShowImage("dst",dst);
	//	cvWaitKey();
	}

	IplImage* dst1 = cvCreateImage(cvGetSize(dst),8,dst->nChannels);
	cvCopy(dst,dst1);


	/*--进一步筛选模糊的图片-保存dst-*/
	////保存测试图片
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	char pVideoName[256];
	sprintf(pVideoName, "%d_%d_%d_%d_%d_%d_%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds); 
	char showWindow[500];
	_mkdir("img");
	sprintf(showWindow,"%s%s%s","img\\",pVideoName,".bmp");
	cvSaveImage(showWindow,dst);
	


//	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr before processingOneT" );
	//resultFinal a = processingOneT(dst1);
//	return a.recString;
//	resultFinal b = processingOneP(dst);
//	cvDestroyAllWindows();
//	cvReleaseImage(&dst);
//	cvReleaseImage(&dst1);
//
//	//	if(a.recPFinal > b.recPFinal)
//	//return a.recString;
//	//	else 
//	return b.recString;

	resultFinal a = processingOneT(dst1);

//	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr after processingOneT" );
	const char *_rec = a.recString.c_str(); 
	bool vifRec = vifResult(_rec,true);

	cout<<"# total T "<<a.recPFinal<<endl;;

	if (a.recPFinal >= 16.0)
	{
		char str[10];
		sprintf (str, "%8f", a.recPFinal);
		//gcvt(a.recPFinal,8,str);

		cvReleaseImage(&dst);
		cvReleaseImage(&dst1);

		return a.recString + "|" + str;
	}
	else
	{
		resultFinal b = processingOneP(dst);
		cvReleaseImage(&dst);
		cvReleaseImage(&dst1);

		if( ( a.recPFinal < 12.0 )&&( b.recPFinal < 12.0 ) ){ //加入如果TP识别概率都很低，则直接返回空白
			return "";
		}

		if(a.recPFinal > b.recPFinal)
		{
			char str[100];
			sprintf (str, "%8f", a.recPFinal);
			gcvt(a.recPFinal,8,str);

			return a.recString + "|" + str;
		}
		else
		{
			char str[100];
			sprintf (str, "%8f", a.recPFinal);
			gcvt(b.recPFinal,8,str);

			return b.recString + "|" + str;
		}
	}
}

resultFinal processingOneT(IplImage *src){
	resultFinal result;

	vector<imageIpl> imageChar;

	IplImage * srcGray = cvCreateImage(cvGetSize(src),8,1);
	cvCvtColor(src,srcGray,CV_BGR2GRAY);

	Mat imgGray = (Mat)srcGray;
	Mat imgRGB = (Mat)src;

	vector<Mat> imageAll = BC->findNum(imgGray,imgRGB);

	if(imageAll.size() == 0){
		result.recString = "";
		result.recPFinal = 0.0;
		return result;
	}

	//for(int iiNum=0;iiNum<imageAll.size();iiNum++){
	////	imshow("src",imageAll[iiNum]);

	//	IplImage pImg = imageAll[iiNum];
	////	cvShowImage("pImg",&pImg);

	//	IplImage *pRGB=cvCreateImage(cvSize(20,20),8,3);
	//	cvResize(&pImg,pRGB);
	//	cvCvtColor(pRGB,pRGB,CV_BGR2RGB);

	////	waitKey(0);
	//}

	
	vector<resultPos> imageCharRecPos;
	int imgNum1 = imageAll.size();

	//char *Label[11];
	//Label[0] = "1";
	//Label[1] = "0";
	//Label[2] = "8";
	//Label[3] = "3";
	//Label[4] = "4";

	//Label[5] = "6";
	//Label[6] = "9";
	//Label[7] = "5";
	//Label[8] = "2";
	//Label[9] = "10";
	//Label[10] = "7";

	char *Label[11];
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
	Label[10] = "10";	

	//char *Label[10]; //32*32  10类
	//Label[0] = "9";
	//Label[1] = "6";
	//Label[2] = "5";
	//Label[3] = "2";
	//Label[4] = "3";

	//Label[5] = "0";
	//Label[6] = "4";
	//Label[7] = "1";
	//Label[8] = "8";
	//Label[9] = "7";
	
	const int imgWidth = 24;
	const int imgSize = imgWidth * imgWidth;

	

	if(imgNum1 >= 16){
		for(int iiNum=0;iiNum<imgNum1;iiNum++){
		//	imshow("src",imageAll[iiNum]);

			IplImage pImg = imageAll[iiNum];
		//	cvShowImage("pImg",&pImg);

			IplImage *pRGB=cvCreateImage(cvSize(imgWidth,imgWidth),8,3);
			cvResize(&pImg,pRGB);
			cvCvtColor(pRGB,pRGB,CV_BGR2RGB);

			/**/
			uchar * imgData;
			imgData = (uchar *)pRGB->imageData;
			/*for (int i=0;i<3072;i++){
			cout<<(int)imgData[i]<<" ";
			}*/
	

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
					 

			vector<float> output;
	
		//	getStartTime();
			cnn->Forward((unsigned char *)data, imgWidth, imgWidth, 3, output);
		//	getEndTime();
		//	printf("time %fms\n",dfTim);
	

		
			//cout<<"消耗时间:"<<t/(cvGetTickFrequency()*1000000)<<"秒";
			//cout<<t/(cvGetTickFrequency()*1.e^6.)<<endl;
			int idx = 0;
			for (int i = 0; i < 11; i++)
				if (output[idx] < output[i])
					idx = i;

			if(Label[idx][0] == '10' ) 
				Label[idx][0] == ' ';

			//	cout<<Label[idx]<<" 概率:("<<output[idx]<<")"<<" 消耗时间:"<<t/(cvGetTickFrequency()*1000000)<<"秒"<<endl;
	
			//产生样本并保存
			/*SYSTEMTIME stTime;
			GetLocalTime(&stTime);
			char pVideoName[256];
			sprintf(pVideoName, "%d_%d_%d_%d_%d_%d_%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds); 


			char showWindow[500];
			sprintf(showWindow,"%s%s%s%s%s","D:\\CreditCardRec\\imgNet\\data_batch_3\\",Label[idx],"\\", pVideoName  ,".bmp");
			cvCvtColor(pRGB,pRGB,CV_RGB2BGR);

			if(Label[idx] == "1")
			cvSaveImage(showWindow,pRGB);*/


			recCharAndP tempLO;
			tempLO.recChar = Label[idx][0];
			tempLO.recP = output[idx];

			resultPos temp;
			temp.recCharP = tempLO;
			temp.recPosition = iiNum;

			imageCharRecPos.push_back(temp);

			
			cvReleaseImage(&pRGB);
			delete []data;
			
			vector<float>(output).swap(output);
		}


		//对结果出来并显示
		//resultFinal result;
		result = sortAndPrint(imageCharRecPos);
	}else{
		result.recString = "0";
		result.recPFinal = 0;
	}
	
	//=========根据总的识别概率判断，如果概率小于一个值，那么就采用密集CNN的思路，找第一个字符========//
//	cout<<"总的识别概率为："<<result.recPFinal<<endl;
	//==================================== end ============================================//


	vector<imageIpl>(imageChar).swap(imageChar);
	vector<resultPos>(imageCharRecPos).swap(imageCharRecPos);
	
//	cvWaitKey(0);

	cvReleaseImage(&srcGray);


	return result;
}


resultFinal processingOneTo(IplImage *src){

	//processing-------------
	int _width = (int)((float)(540.0/src->height)*src->width);;
	int _height = 540;
	//		cout<<_width<<" "<<_height<<endl;

	IplImage *srcResize = cvCreateImage(cvSize(_width,_height),8,src->nChannels);
	cvResize(src,srcResize,CV_INTER_LINEAR);

	//	cvNormalize(srcResize,srcResize,255,0,CV_MINMAX);


	//cvShowImage("cvNorm",srcResize);
	////	if(showSteps){
	if(showSteps){
		cvNamedWindow("image",1);
		cvShowImage("image",srcResize);
	}
	//	cvWaitKey(0);

	//ROI区域
	CvRect roi;
	roi.x = 55;
	roi.y = 300;
	roi.width = 769;
	if( (roi.width + roi.x)>srcResize->width )
		roi.width = srcResize->width - roi.x;
	roi.height = 80;
	IplImage *dst = cvCreateImage(cvSize(roi.width,roi.height),8,src->nChannels);
	cvSetImageROI(srcResize,roi);
	cvCopy(srcResize,dst);
	cvResetImageROI(srcResize);
	if(showSteps){
		cvNamedWindow("dst0",1);
		cvShowImage("dst0",dst);
	}

	if(saveFlag){

		char pVideoName[256];
		//sprintf(pVideoName, ".\\%d_%d_%d_%d_%d_%d_%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);


		char name[500];
		//sprintf(name,"%s%s%s%d%s","E:\\CreditCardNum\\",pVideoName,"_",1,".bmp");
		sprintf(name,"%s%d%s%d%s","E:\\CreditCardNum\\",1,"_",1,".bmp");
		//	cout<<name<<endl;
		//cvSaveImage(name,dst);
	}

	IplImage* dstTemp = cvCreateImage(cvSize(roi.width,roi.height),8,src->nChannels);
	cvCopy(dst,dstTemp);
	cvSmooth(dstTemp,dstTemp,CV_GAUSSIAN );
	/*if(showSteps){
	cvNamedWindow("dstTemp",1);
	cvShowImage("dstTemp",dstTemp);
	}*/
	//===================retinex================
	/*	IplImage* imgRetinex = cvCreateImage(cvGetSize(dst),8,3);
	cvCopy(dst,imgRetinex);
	Retinex(imgRetinex, 200, 128, 128);
	*/
	//double weights[3] = {1.05, 1.2, 1.3};
	//double sigmas[3] = {200, 100, 300};
	//MultiScaleRetinex(imgRetinex, 3 , weights , sigmas , 50 ,128);
	/*if(showSteps){
	cvNamedWindow("imgRetinex",1);
	cvShowImage("imgRetinex",imgRetinex);
	}*/

	//==================彩色图像Sobel变换=====================
	IplImage * pImagePlanes[3]={NULL,NULL,NULL};
	IplImage * pImage16uColorSobel=NULL;
	IplImage * pImage8uColorSobelShow_x=NULL;
	IplImage * pImage8uColorSobelShow_y=NULL;
	int i;
	for (i=0;i<3;i++){
		pImagePlanes[i]=cvCreateImage(cvGetSize(dst),IPL_DEPTH_8U,1);
	}
	pImage16uColorSobel=cvCreateImage(cvGetSize(dst),IPL_DEPTH_16S,1);
	pImage8uColorSobelShow_x=cvCreateImage(cvGetSize(dst),IPL_DEPTH_8U,3);
	//分成3个单通道
	cvSplit(dstTemp,pImagePlanes[0],pImagePlanes[1],pImagePlanes[2],NULL);
	for (i=0;i<3;i++){
		cvSobel(pImagePlanes[i],pImage16uColorSobel,0,1,3 );
		cvConvertScaleAbs(pImage16uColorSobel,pImagePlanes[i],1,0);
	}
	cvMerge(pImagePlanes[0],pImagePlanes[1],pImagePlanes[2],NULL,pImage8uColorSobelShow_x);
	//IplImage* imgRetinex1 = cvCreateImage(cvGetSize(pImage8uColorSobelShow),8,3);
	//cvCopy(pImage8uColorSobelShow,imgRetinex1);
	//Retinex(imgRetinex1, 200, 128, 128);

	////double weights[3] = {1.05, 1.2, 1.3};
	////double sigmas[3] = {200, 100, 300};
	////MultiScaleRetinex(imgRetinex, 3 , weights , sigmas , 50 ,128);
	//if(showSteps){
	//	cvNamedWindow("imgRetinex1",1);
	//	cvShowImage("imgRetinex1",imgRetinex1);
	//}
	/*	if(showSteps){
	cvNamedWindow(" pImage8uColorSobelShow_x",CV_WINDOW_AUTOSIZE);
	cvShowImage(" pImage8uColorSobelShow_x",pImage8uColorSobelShow_x);
	}*/
	//y
	pImage8uColorSobelShow_y=cvCreateImage(cvGetSize(dst),IPL_DEPTH_8U,3);
	cvSplit(dstTemp,pImagePlanes[0],pImagePlanes[1],pImagePlanes[2],NULL);
	for (i=0;i<3;i++){
		cvSobel(pImagePlanes[i],pImage16uColorSobel,1,0,3 );
		cvConvertScaleAbs(pImage16uColorSobel,pImagePlanes[i],1,0);
	}
	cvMerge(pImagePlanes[0],pImagePlanes[1],pImagePlanes[2],NULL,pImage8uColorSobelShow_y);
	/*if(showSteps){
	cvNamedWindow("  pImage8uColorSobelShow_y",CV_WINDOW_AUTOSIZE);
	cvShowImage("  pImage8uColorSobelShow_y",pImage8uColorSobelShow_y);
	}*/

	IplImage * imgsum =cvCreateImage(cvGetSize(dst),IPL_DEPTH_32F,3);  
	cvZero(imgsum);  
	cvAcc(pImage8uColorSobelShow_y,imgsum);  
	cvAcc(pImage8uColorSobelShow_x,imgsum);  
	/*if(showSteps){
	cvNamedWindow("imgsum",1);
	cvShowImage("imgsum",imgsum);
	}*/

	IplImage * imgavg = cvCreateImage(cvGetSize(dst),IPL_DEPTH_8U,3);  
	cvConvertScale(imgsum,imgavg,1.0/2.0);  
	if(showSteps){
		cvNamedWindow("imgavg",1);
		cvShowImage("imgavg",imgavg);
	}

	//==================threshold=========================
	IplImage * imgBin = cvCreateImage(cvGetSize(imgavg),IPL_DEPTH_8U,1);
	cvCvtColor(imgavg,imgBin,CV_BGR2GRAY);
	cvThreshold(imgBin,imgBin,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
	/*	int blockSize = 19;
	int constValue = 9;
	cvAdaptiveThreshold(imgPostSegEqual, imgPostSegEqualBinary, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);*/
	if(showSteps){
		cvNamedWindow("imgBin",1);
		cvShowImage("imgBin",imgBin);
	}
	
	//================== imgBin 和 imgavg are used of input for the stroke width algorithm


	//Y轴投影 确定字符具体区域
	IplImage* painty=cvCreateImage( cvGetSize(imgBin),IPL_DEPTH_8U, 1 );  		
	cvZero(painty);  
	int* h=new int[imgBin->height];  		
	memset(h,0,imgBin->height*4);  

	int x,y;  
	CvScalar s,t;  
	for(y=0;y<imgBin->height;y++)  
	{  
		for(x=0;x<imgBin->width;x++)  
		{  
			s=cvGet2D(imgBin,y,x);           
			if(s.val[0]==0)  
				h[y]++;       
		}     
	} 

	//将y投影后，值小于50的赋值为0
	for(y=0;y<imgBin->height;y++)   {  
		if((imgBin->width-h[y]) <= 80)
			h[y] = imgBin->width;
		//	printf("%d ",imgBin->width-h[y]); 
	}

	//将Y轴上 很窄的线段，即横着的很长的细线直接抹掉
	for(x=0;x<painty->height;x++)  {
		for(y=x;y<painty->height;y++)  {
			if( (h[x] == h[y])&&(h[y] == painty->width)&&(y-x <= 6) ){
				for(int i=x;i<=y;i++){
					h[i] = painty->width;
				}
			}
			if( (h[x] != painty->width)&&(h[y] == painty->width)&&(abs(y-x) <= 6)&&((x == 0)||(y == 0)) ){
				for(int i=x;i<=y;i++){
					h[i] = painty->width;
				}
			} 

		}
	}

	for(x=0;x<painty->height;x++)  {
		for(y=x;y<painty->height;y++)  {
			if( (h[x] == h[y])&&(h[y] == painty->width)&&(y-x <= 15) ){
				for(int i=x;i<=y;i++){
					h[i] = painty->width;
				}
			}
			if( (h[x] != painty->width)&&(h[y] == painty->width)&&(abs(y-x) <= 6)&&((x == 0)||(y == 0)) ){
				for(int i=x;i<=y;i++){
					h[i] = painty->width;
				}
			} 

		}
	}


	for(y=0;y<imgBin->height;y++)  
	{  
		for(x=0;x<h[y];x++)  
		{             
			t.val[0]=255;  
			cvSet2D(painty,y,x,t);            
		}         
	} 
	//确定Y轴字符的收尾，确定字符区域的高度
	//查找paintx首尾两端的x轴坐标
	int xLeft = 0;
	for(x=0;x<painty->height-2;x++){
		if ( cvGet2D(painty,x,painty->width - 1).val[0]== 0 ){
			xLeft = x;
			break;	
		}
		if( (cvGet2D(painty,x,painty->width - 1).val[0] == 255)&&(cvGet2D(painty,x+1,painty->width - 1).val[0] == 0) ){
			xLeft = x;
			break;	
		}
	}
	//	if(showSteps)
	//	cout<<"列字符区域上边字符起始点是："<<xLeft<<endl;

	int xRight = 0;
	for(x=painty->height-1; x>0 ;x--){	
		if ( cvGet2D(painty,x,painty->width - 1).val[0]== 0 ){
			xRight = x;
			break;	
		}
		if( (cvGet2D(painty,x,painty->width - 1).val[0]== 255)&&(cvGet2D(painty,x-1,painty->width - 1).val[0] == 0) ){
			xRight = x;
			break;	
		}
	}
	if(xRight == 0)  xRight = painty->height;
	//	if(showSteps)
	//	cout<<"列字符区域下边字符起始点是："<<xRight<<endl;


	if(showSteps){  
		cvNamedWindow("水平积分投影",1);  		 
		cvShowImage("水平积分投影",painty);  
	}


	//根据y轴投影确定的字符高度 roi字符区域
	CvRect roiDst;
	roiDst.x = 0;
	roiDst.y = xLeft - 3;
	if(roiDst.y < 0) roiDst.y = xLeft;
	roiDst.width = dst->width;
	roiDst.height = xRight - xLeft + 6;
	//===========================================================================================//
	roiDst.height = charHeight;
	if(roiDst.height >= dst->height - xLeft - 6) roiDst.height = xRight - xLeft  ;

//	cout<<roiDst.x<<" "<<roiDst.y<<" "<<roiDst.width<<" "<<roiDst.height<<endl;

	IplImage *dstY = cvCreateImage(cvSize(roiDst.width,roiDst.height),8,imgBin->nChannels);
	cvSetImageROI(imgBin,roiDst);
	cvCopy(imgBin,dstY);
	cvResetImageROI(imgBin);
	if(showSteps){
		cvNamedWindow("dstY",1);
		cvShowImage("dstY",dstY);
	}

	//=========================x轴投影=====================
	IplImage* paintx=cvCreateImage( cvGetSize(dstY),IPL_DEPTH_8U, 1 );  		
	cvZero(paintx);  		  
	int* v=new int[dstY->width];  
	memset(v,0,dstY->width*4);  

	/*	int x,y;  
	CvScalar s,t;  */
	for(x=0;x<dstY->width;x++)  
	{  
		for(y=0;y<dstY->height;y++)  
		{  
			s=cvGet2D(dstY,y,x);           
			if(s.val[0]==0)  
				v[x]++;                   
		}         
	}  

	//将x投影后，值小于2的赋值为0.接近于dstY->height的只为0
	for(x=0;x<dstY->width;x++)  {  
		if((dstY->height-v[x]) <= 5)
			v[x] = dstY->height;
		/*	if( ((dstY->height-v[x]) >= (dstY->height-15))&&(dstY->height>15) )
		v[x] = dstY->height;*/
		//	printf("%d ",dstY->height-v[x]); 
	}

	//若投影后出现很细的竖直黑线，将这条黑线去掉
	for(x=0;x<paintx->width;x++)  {
		for(y=x;y<paintx->width;y++)  {
			if( (v[x] == v[y])&&(v[y] == paintx->height)&&(y-x < 3) ){
				for(int i=x;i<=y;i++){
					v[i] = paintx->height;
				}
			}
			if( (v[x] != paintx->width)&&(v[y] == paintx->width)&&(y-x <= 6) ){
				for(int i=x;i<=y;i++){
					v[i] = paintx->height;
				}
			} 
		}
	}

	//根据paintx投影值 分析出具体的分割字体
	int xZuo[145] = {0};
	int xYou[145] = {0};
	int xD[145] = {0};

	int xZNum = 0;
	int xYNum = 0;
	for(x=0;x<paintx->width-1;x++)  {
		if( (v[x]  < paintx->height)&&(  x== 0) ){
			xZuo[0] = 0;
			xZNum ++;
		}
		if( (v[x] == paintx->height)&&(v[x+1] < paintx->height) ){
			xZuo[xZNum++] = x;
		}

		if( (v[x] < paintx->height)&&(v[x+1] == paintx->height) ){
			xYou[xYNum++] = x+1;
		}
		if( (v[x]  < paintx->height)&&(  x== paintx->width-2)&&(v[x+1]  < paintx->height) ){
			xYou[xYNum++] = x;
		}
	}
	/*	cout<<endl;
	for(x=0;x<xZNum;x++){
	printf("%3d ",xZuo[x]);
	}
	cout<<endl;
	for(x=0;x<xYNum;x++){
	printf("%3d ",xYou[x]);
	}
	cout<<endl;*/

	for(x=0;x<xYNum-1;x++){
		//	xD[x] = xYou[x]-xZuo[x];
		xD[x] = xZuo[x+1]-xYou[x];
		//	printf("%3d ",xD[x]);
		//若投影后两条竖直黑线之间的间距小于21，将这两条黑线中间的区域变为黑
		if( xD[x] <= 25){
			for(y=xYou[x];y<=xZuo[x+1];y++)  {
				v[y] = paintx->height/4;
			}
		}
	}
	//	cout<<endl;


	//对处理得到的paintX继续进行细筛选和纠错，==================重复上面的过程================
	int xZuo1[15] = {0};
	int xYou1[15] = {0};
	int xD1[15] = {0};
	int xStepD[15] = {0};

	int xZNum1 = 0;
	int xYNum1 = 0;
	for(x=0;x<paintx->width-1;x++)  {
		if( (v[x]  < paintx->height)&&(  x== 0) ){
			xZuo1[0] = 0;
			xZNum1 ++;
		}
		if( (v[x] == paintx->height)&&(v[x+1] < paintx->height) ){
			xZuo1[xZNum1++] = x;
		}

		if( (v[x] < paintx->height)&&(v[x+1] == paintx->height) ){
			xYou1[xYNum1++] = x+1;
		}
		if( (v[x]  < paintx->height)&&(  x== paintx->width-2)&&(v[x+1]  < paintx->height) ){
			xYou1[xYNum1++] = x;
		}
	}
//	cout<<endl;
	//for(x=0;x<xZNum1;x++){
	//	printf("%3d ",xZuo1[x]);
	//}
//	cout<<endl;
	//for(x=0;x<xYNum1;x++){
	//	printf("%3d ",xYou1[x]);
	//}
	//cout<<endl;

	vector<imageIpl> imageChar;
	for(x=0;x<xYNum1-1;x++){
		xD1[x] = xZuo1[x+1]-xYou1[x];
	//	printf(" xD1:%3d  ", xD1[x]);
	}
	int flagNum = 0;
	for(x=0;x<xYNum1;x++){
		xStepD[x] = xYou1[x]-xZuo1[x];
		if(xStepD[x]>0) flagNum ++;
	//	printf(" xStepD:%3d ",xStepD[x] );
	}

	for(x=0;x<dstY->width;x++)  
	{  
		for(y=0;y<v[x];y++)  
		{         
			t.val[0]=255;  
			cvSet2D(paintx,y,x,t);        
		}         
	} 

	//根据x轴投影的直方图，找到第一个字符
	//查找paintx首尾两端的x轴坐标
	int paintxLeft = 0;
	for(x=0;x<paintx->width;x++){	
		if( (cvGet2D(paintx,paintx->height - 3,x).val[0] == 255)&&(cvGet2D(paintx,paintx->height - 3,x+1).val[0] == 0) ){
			paintxLeft = x;

			if(paintxLeft>100)
				paintxLeft = 0;
			break;	
		}
	}
	//if(showSteps)
	//	cout<<"paintx行字符区域左边字符起始点是："<<paintxLeft<<endl;
	int paintxRight = 0;
	for(x=paintx->width-1; x>0 ;x--){	
		if( (cvGet2D(paintx,paintx->height - 1,x).val[0]== 255)&&(cvGet2D(paintx,paintx->height - 1,x-1).val[0] == 0) ){
			paintxRight = x;
			break;	
		}
	}
	//if(showSteps)
	//	cout<<"paintx行字符区域右边字符起始点是："<<paintxRight<<endl;

	if(showSteps){
		cvNamedWindow("垂直积分投影",1);  
		cvShowImage("垂直积分投影",paintx);  		 
	}
	//	cvWaitKey(0);


	




	for(x=0;x<xYNum1;x++){
		//===================第一种情况：四个区域差不多大，信用卡标准形式，直接切割===========
		//	if( (( xStepD[x]>115 )&&(xStepD[x]<175))&&(flagNum >= 4) ){
		if(( xStepD[x]>=110 )&&(xStepD[x]<175)){
			//	cout<<"1"<<endl;
			//直接均分
			for(int ii = 0;ii<4;ii ++){
				CvRect roiChar;
				roiChar.x = xZuo1[x] + ii*charWidth - 1;
				if(roiChar.x<0) roiChar.x = 0;
				roiChar.y = roiDst.y;
				roiChar.width = charWidth+2;
				if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
				roiChar.height = roiDst.height;
				if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 

				if(roiChar.x < 0) roiChar.x = 0;
				if(roiChar.y < 0) roiChar.y = 0;
				if(roiChar.width < 0)	{roiChar.width = 1;		break;}
				if(roiChar.height < 0)	{roiChar.height = 1;		break;}

				IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
				cvSetImageROI(dst,roiChar);
				cvCopy(dst,dstChar);
				cvResetImageROI(dst);

				imageIpl roiImageChar;
				roiImageChar.roiImage = dstChar;
				roiImageChar.positionX = roiChar.x;
				imageChar.push_back(roiImageChar);

				//	cvReleaseImage(&dstChar);
			}
		}
		//====================4个字符，但是有部分背景粘连了，根据首或者尾部开始分割
		if( (( xStepD[x]>165 )&&(xStepD[x]<230))&&((( xStepD[x+1]>115 )&&(xStepD[x+1]<175))||(( xStepD[x-1]>115 )&&(xStepD[x-1]<175))) ){
			if(xD1[x-1] > 28){
				//直接均分
				for(int ii = 0;ii<4;ii ++){
					CvRect roiChar;
					roiChar.x = xZuo1[x] + ii*charWidth;
					roiChar.y = roiDst.y;
					roiChar.width = charWidth+4;
					if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
					roiChar.height = roiDst.height;
					if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 

					if(roiChar.x < 0) roiChar.x = 0;
					if(roiChar.y < 0) roiChar.y = 0;
					if(roiChar.width < 0)	{roiChar.width = 1;		break;}
					if(roiChar.height < 0)	{roiChar.height = 1;		break;}

					IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
					cvSetImageROI(dst,roiChar);
					cvCopy(dst,dstChar);
					cvResetImageROI(dst);

					imageIpl roiImageChar;
					roiImageChar.roiImage = dstChar;
					roiImageChar.positionX = roiChar.x;
					imageChar.push_back(roiImageChar);

					//	cvReleaseImage(&dstChar);
				}

			}
		}

		if( (( xStepD[x]>165 )&&(xStepD[x]<230))&&((( xStepD[x+1]>115 )&&(xStepD[x+1]<175))||(( xStepD[x-1]>115 )&&(xStepD[x-1]<175))) ){
			if(xD1[x] > 28){
				//直接均分
				for(int ii = 1;ii<=4;ii ++){
					CvRect roiChar;
					roiChar.x = xYou1[x] - ii*charWidth;
					roiChar.y = roiDst.y;
					roiChar.width = charWidth+4;
					if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
					roiChar.height = roiDst.height;
					if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 

					if(roiChar.x < 0) roiChar.x = 0;
					if(roiChar.y < 0) roiChar.y = 0;
					if(roiChar.width < 0)	{roiChar.width = 1;		break;}
					if(roiChar.height < 0)	{roiChar.height = 1;		break;}

					IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
					cvSetImageROI(dst,roiChar);
					cvCopy(dst,dstChar);
					cvResetImageROI(dst);

					imageIpl roiImageChar;
					roiImageChar.roiImage = dstChar;
					roiImageChar.positionX = roiChar.x;
					imageChar.push_back(roiImageChar);

					//	cvReleaseImage(&dstChar);
				}

			}
		}

		//===================第二种情况：8个字符连在一起===========
		if(( xStepD[x]>300 )&&(xStepD[x]<370)){
			//	cout<<"2"<<endl;
			//直接均分
			if( (x==0)&&(xD1[x]>30) ){//第一个字符区域分割出现8且与前部背景相连接
				//从后面往前面分割
				for(int ii = 0;ii<8;ii ++){
					CvRect roiChar;
					if(ii>=4)
						roiChar.x = xYou1[x] - (8-ii)*charWidth;
					else 
						roiChar.x = xYou1[x] - (8-ii)*charWidth -36;
					roiChar.y = roiDst.y;
					roiChar.width = charWidth+2;
					if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
					roiChar.height = roiDst.height;;
					if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 

					if(roiChar.x < 0) roiChar.x = 0;
					if(roiChar.y < 0) roiChar.y = 0;
					if(roiChar.width < 0)	{roiChar.width = 1;		break;}
					if(roiChar.height < 0)	{roiChar.height = 1;		break;}

					IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
					cvSetImageROI(dst,roiChar);
					cvCopy(dst,dstChar);
					cvResetImageROI(dst);

					imageIpl roiImageChar;
					roiImageChar.roiImage = dstChar;
					roiImageChar.positionX = roiChar.x;
					imageChar.push_back(roiImageChar);

					/*cvNamedWindow("img12",0);
					cvShowImage("img12",dstChar);
					cvWaitKey(0);*/
				}

			}else{
				for(int ii = 0;ii<8;ii ++){
					CvRect roiChar;
					if(ii<4)
						roiChar.x = xZuo1[x] + ii*charWidth;
					else 
						roiChar.x = xZuo1[x] + ii*(charWidth) + 36;
					roiChar.y = roiDst.y;
					roiChar.width = charWidth+2;
					if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
					roiChar.height = roiDst.height;;
					if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 

					if(roiChar.x < 0) roiChar.x = 0;
					if(roiChar.y < 0) roiChar.y = 0;
					if(roiChar.width < 0)	{roiChar.width = 1;		break;}
					if(roiChar.height < 0)	{roiChar.height = 1;		break;}

					IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
					cvSetImageROI(dst,roiChar);
					cvCopy(dst,dstChar);
					cvResetImageROI(dst);

					imageIpl roiImageChar;
					roiImageChar.roiImage = dstChar;
					roiImageChar.positionX = roiChar.x;
					imageChar.push_back(roiImageChar);
				}
			}

		}


		//===================第三种情况：12或者13个字符连在一起===========
		if(( xStepD[x]>480 )&&(xStepD[x]<=555)){
			//	cout<<"3"<<endl;
			//		cout<<x<<endl;
			if( (( xStepD[x-1]>130 )&&(xStepD[x-1]<175))||(( xStepD[x+1]>130 )&&(xStepD[x+1]<180)) ){
				//信用卡类型
				//直接均分
				for(int ii = 0;ii<12;ii ++){
					CvRect roiChar;
					if(ii<4)
						roiChar.x = xZuo1[x] + ii*charWidth;							
					else if(ii<8)
						roiChar.x = xZuo1[x] + ii*charWidth + 36;
					else 
						roiChar.x = xZuo1[x] + ii*charWidth + 72;
					roiChar.y = roiDst.y;
					roiChar.width = charWidth+4;
					if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
					roiChar.height = roiDst.height;
					if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 

					if(roiChar.x < 0) roiChar.x = 0;
					if(roiChar.y < 0) roiChar.y = 0;
					if(roiChar.width < 0)	{roiChar.width = 1;		break;}
					if(roiChar.height < 0)	{roiChar.height = 1;		break;}

					IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
					cvSetImageROI(dst,roiChar);
					cvCopy(dst,dstChar);
					cvResetImageROI(dst);

					imageIpl roiImageChar;
					roiImageChar.roiImage = dstChar;
					roiImageChar.positionX = roiChar.x;
					imageChar.push_back(roiImageChar);
				}
			}

		}
		//===================第四种情况：6 13开===========
		if( (( xStepD[x]>150 )&&(xStepD[x]<240))&&(( xStepD[x+1]>440 )&&(xStepD[x+1]<490)) ){
			//	cout<<"4.1"<<endl;
			for(int ii = 0;ii<6;ii ++){
				CvRect roiChar;
				roiChar.x = xZuo1[x] + ii*charWidth;										
				roiChar.y = roiDst.y;
				roiChar.width = charWidth+2;
				if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
				roiChar.height = roiDst.height;
				if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 
				if(roiChar.x < 0) roiChar.x = 0;
				if(roiChar.y < 0) roiChar.y = 0;
				if(roiChar.width < 0)	{roiChar.width = 1;		break;}
				if(roiChar.height < 0)	{roiChar.height = 1;		break;}

				IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
				cvSetImageROI(dst,roiChar);
				cvCopy(dst,dstChar);
				cvResetImageROI(dst);

				imageIpl roiImageChar;
				roiImageChar.roiImage = dstChar;
				roiImageChar.positionX = roiChar.x;
				imageChar.push_back(roiImageChar);
			}
			for(int ii = 0;ii<13;ii ++){

				CvRect roiChar;
				roiChar.x = xZuo1[x+1] + ii*(charWidth);										
				roiChar.y = roiDst.y;
				roiChar.width = charWidth+4;
				if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
				roiChar.height = roiDst.height;
				if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 
				if(roiChar.x < 0) roiChar.x = 0;
				if(roiChar.y < 0) roiChar.y = 0;
				if(roiChar.width < 0)	{roiChar.width = 1;		break;}
				if(roiChar.height < 0)	{roiChar.height = 1;		break;}

				IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
				cvSetImageROI(dst,roiChar);
				cvCopy(dst,dstChar);
				cvResetImageROI(dst);

				imageIpl roiImageChar;
				roiImageChar.roiImage = dstChar;
				roiImageChar.positionX = roiChar.x;
				imageChar.push_back(roiImageChar);
			}

		}
		//===================第四种情况：13 6开===========
		if( (( xStepD[x]>440 )&&(xStepD[x]<490))&&(( xStepD[x+1]>150 )&&(xStepD[x+1]<240)) ){
			//	cout<<"4.2"<<endl;
			for(int ii = 0;ii<13;ii ++){
				CvRect roiChar;
				roiChar.x = xZuo1[x] + ii*charWidth;										
				roiChar.y = roiDst.y;
				roiChar.width = charWidth;
				//	if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
				if( (roiChar.width + roiChar.x) > xYou1[x] ) roiChar.width = xYou1[x] -roiChar.x -1; 
				roiChar.height = roiDst.height;
				if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 
				//	cout<<roiChar.x<<" "<<roiChar.y<<" "<<roiChar.width<<" "<<roiChar.height<<endl;
				if(roiChar.x < 0) roiChar.x = 0;
				if(roiChar.y < 0) roiChar.y = 0;
				if(roiChar.width < 0)	{roiChar.width = 1;		break;}
				if(roiChar.height < 0)	{roiChar.height = 1;		break;}

				IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
				cvSetImageROI(dst,roiChar);
				cvCopy(dst,dstChar);
				cvResetImageROI(dst);

				imageIpl roiImageChar;
				roiImageChar.roiImage = dstChar;
				roiImageChar.positionX = roiChar.x;
				if(ii!=6)
					imageChar.push_back(roiImageChar);
			}
			for(int ii = 0;ii<6;ii ++){
				CvRect roiChar;
				roiChar.x = xZuo1[x+1] + ii*(charWidth+1);	
				if(roiChar.x>dst->width)	 break;								
				roiChar.y = roiDst.y;
				roiChar.width = charWidth+4;
				if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
				roiChar.height = roiDst.height;
				if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 
				//	cout<<roiChar.x<<" "<<roiChar.y<<" "<<roiChar.width<<" "<<roiChar.height<<endl;
				if(roiChar.x < 0) roiChar.x = 0;
				if(roiChar.y < 0) roiChar.y = 0;
				if(roiChar.width < 0)	{roiChar.width = 1;		break;}
				if(roiChar.height < 0)	{roiChar.height = 1;		break;}

				IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
				cvSetImageROI(dst,roiChar);
				cvCopy(dst,dstChar);
				cvResetImageROI(dst);

				imageIpl roiImageChar;
				roiImageChar.roiImage = dstChar;
				roiImageChar.positionX = roiChar.x;
				imageChar.push_back(roiImageChar);

			}

		}
		//===================第四种情况：6 6 6开===========
		if( ( ( (( xStepD[x]>150 )&&(xStepD[x]<240))&&((( xStepD[x+1]>150 )&&(xStepD[x+1]<240))&&(( xStepD[x+2]>150 )&&(xStepD[x+2]<240))) )||
			( (( xStepD[x-1]>150 )&&(xStepD[x-1]<240))&&((( xStepD[x ]>150 )&&(xStepD[x ]<240))&&(( xStepD[x+1]>150 )&&(xStepD[x+1]<240))) )||
			( (( xStepD[x-2]>150 )&&(xStepD[x-2]<240))&&((( xStepD[x-1]>150 )&&(xStepD[x-1]<240))&&(( xStepD[x]>150 )&&(xStepD[x]<240))) )

			)&&(flagNum == 3) ){
				//		cout<<"4.3"<<endl;
				for(int ii = 0;ii<6;ii ++){
					CvRect roiChar;
					roiChar.x = xZuo1[x] + ii*(charWidth );										
					roiChar.y = roiDst.y;
					roiChar.width = charWidth+4;
					if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
					roiChar.height = roiDst.height;
					if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 

					if(roiChar.x < 0) roiChar.x = 0;
					if(roiChar.y < 0) roiChar.y = 0;
					if(roiChar.width < 0)	{roiChar.width = 1;		break;}
					if(roiChar.height < 0)	{roiChar.height = 1;		break;}

					IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
					cvSetImageROI(dst,roiChar);
					cvCopy(dst,dstChar);
					cvResetImageROI(dst);

					imageIpl roiImageChar;
					roiImageChar.roiImage = dstChar;
					roiImageChar.positionX = roiChar.x;
					imageChar.push_back(roiImageChar);

				}
		}
		//===================第五种情况：19个字符一起===========
		if( (( xStepD[x]>640 )&&(xStepD[x]<765))&&(x==0) ){
			//	cout<<"5"<<endl;
			for(int ii = 0;ii<19;ii ++){
				CvRect roiChar;
				roiChar.x = xZuo1[x] + ii*(charWidth);										
				roiChar.y = roiDst.y;
				roiChar.width = charWidth+2;
				roiChar.height = roiDst.height;
				//	if( (roiChar.width + roiChar.x) > dst->width ) roiChar.width = dst->width -roiChar.x -1; 
				
				if( (roiChar.width + roiChar.x) > xYou1[x] ) roiChar.width = xYou1[x] -roiChar.x -1; 	
				if( (roiChar.y + roiChar.height)>dst->height ) roiChar.height = dst->height -roiDst.y; 
				if(roiChar.x < 0) roiChar.x = 0;
				if(roiChar.y < 0) roiChar.y = 0;
				if(roiChar.width < 0)	{roiChar.width = 1;		break;}
				if(roiChar.height < 0)	{roiChar.height = 1;		break;}


				IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
				cvSetImageROI(dst,roiChar);
				cvCopy(dst,dstChar);
				cvResetImageROI(dst);

				imageIpl roiImageChar;
				roiImageChar.roiImage = dstChar;
				roiImageChar.positionX = roiChar.x;
				imageChar.push_back(roiImageChar);

			}
		}

	}


//	cout<<dfTim<<endl;

	//	cout<<endl;

	//=========说明：出现空格时候，空格要比字体宽一点。否则分割出错。
	//确定字符宽高并分割 36*54大小
	//对凸面字体按照20个字体进行分割
	//	cout<<dst->width<<" "<<dst->height<<endl;

	//for(int ii = 0;ii<20;ii ++){
	//	CvRect roiChar;
	//	roiChar.x = paintxLeft + ii*charWidth;
	////	roiChar.x++;
	////	roiChar.y = dst->height - ((dst->height-roiDst.y-roiDst.height/2)+charHeight/2);
	////	roiChar.y = roiDst.y+roiDst.height/2-charHeight/2;
	//	roiChar.y = roiDst.y;
	//	roiChar.width = charWidth; 
	////	roiChar.height = charHeight;
	//	roiChar.height = roiDst.height;
	//	if( (roiChar.x + roiChar.width) > dst->width  ) 
	//		break;
	////	cout<<roiChar.x<<" "<<roiChar.y<<" "<<roiChar.width<<" "<<roiChar.height<<endl;
	//	
	//	IplImage *dstChar = cvCreateImage(cvSize(roiChar.width,roiChar.height),8,3);
	//	cvSetImageROI(dst,roiChar);
	//	cvCopy(dst,dstChar);
	//	cvResetImageROI(dst);

	//	if(showSteps){
	//		char name[200];
	//		sprintf(name,"%s%d%s%d%s","E:\\CreditCardNum\\",iNum,"_",ii,".bmp");
	//		//	cvNamedWindow(name,0);
	//		//	cvShowImage(name,dst);
	//		cvSaveImage(name,dstChar);
	//	}

	//	cvReleaseImage(&dstChar);
	//}

	//recChar(imageChar);
	vector<resultPos> imageCharRecPos;
	int imgNum1 = imageChar.size();
	//	cout<<imgNum1<<endl;
	//cout<<endl;
	//cout<<"识别的卡号为："<<endl;
//	LightNet::ConvNet cnn;
//	double _t = (double)cvGetTickCount();
//	cnn.Load("card.model");
//	_t = (double)cvGetTickCount() - _t;
//	cout<<"消耗时间:"<<_t/(cvGetTickFrequency()*1000000)<<"秒";

	//convexCard20*20:1 0 8 3 4 6 9 5 2 10 7

	char *Label[11];
	Label[0] = "1";
	Label[1] = "0";
	Label[2] = "8";
	Label[3] = "3";
	Label[4] = "4";

	Label[5] = "6";
	Label[6] = "9";
	Label[7] = "5";
	Label[8] = "2";
	Label[9] = "10";
	Label[10] = "7";
	resultFinal result;
	if(imgNum1 >= 16){
		for(int iiNum=0;iiNum<imgNum1;iiNum++){
			IplImage* pRGB = cvCreateImage(cvSize(20,20),8,3);

			IplImage *pSrc=cvCreateImage(cvSize(20,20),8,3);
			cvResize(imageChar[iiNum].roiImage,pSrc);
			cvCvtColor(pSrc,pRGB,CV_BGR2RGB);


			vector<float> output;
			//	double t = (double)cvGetTickCount();
			cnn->Forward((unsigned char *)pRGB->imageData, 20, 20, 3, output);
			//	t = (double)cvGetTickCount() - t;
			//cout<<"消耗时间:"<<t/(cvGetTickFrequency()*1000000)<<"秒";
			//cout<<t/(cvGetTickFrequency()*1.e^6.)<<endl;
			int idx = 0;
			for (int i = 1; i < 11; i++)
				if (output[idx] < output[i])
					idx = i;

			//	cout<<Label[idx]<<" 概率:("<<output[idx]<<")"<<" 消耗时间:"<<t/(cvGetTickFrequency()*1000000)<<"秒"<<endl;
			if(Label[idx][0] == '10' )
				Label[idx][0] == ' ';


			recCharAndP tempLO;
			tempLO.recChar = Label[idx][0];
			tempLO.recP = output[idx];

			resultPos temp;
			temp.recCharP = tempLO;
			temp.recPosition = imageChar[iiNum].positionX;

			imageCharRecPos.push_back(temp);

			cvReleaseImage(&pSrc);
			cvReleaseImage(&pRGB);
			if(saveFlag){

				char pVideoName[256];
//				sprintf(pVideoName, ".\\%d_%d_%d_%d_%d_%d_%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);

				char name[500];		
			//	sprintf(name,"%s%s%s%s%s%d%s","result2\\",Label[idx],"\\",pVideoName,"_",iiNum,".bmp");	
				sprintf(name,"%s%s%s%d%s","result1\\",pVideoName,"_",iiNum,".bmp");	
				//	cout<<name<<endl;
				cvSaveImage(name,imageChar[iiNum].roiImage);
			}
			vector<float>(output).swap(output);
		}


		//对结果出来并显示
		//resultFinal result;
		result = sortAndPrint(imageCharRecPos);
	}else{
		

		result.recString = "0";
		result.recPFinal = 0;

	}
	
	//================================ 得到首个字符 ===============================//
	/*printf("srcResize:");
	cout<<roiDst.x<<" "<<roiDst.y + roi.y <<" "<<roiDst.width<<" "<<roiDst.height<<endl;
*/
	//if(showSteps)
	//	cout<<"paintx行字符区域左边字符起始点是："<<paintxLeft<<endl;

	CvRect tempz;
	tempz.x = roiDst.x + roi.x + paintxLeft;
	tempz.y = roiDst.y + roi.y;
	tempz.width = charWidth;
	tempz.height = roiDst.height;

	cvRectangle(srcResize, cvPoint(tempz.x , tempz.y ),
		cvPoint(tempz.x+tempz.width,tempz.y+tempz.height),CV_RGB(255,255,0), 2);

	if(showSteps){
		cvNamedWindow("image",1);
		cvShowImage("image",srcResize);
	}
	if(saveFlag){

		char pVideoName[256];
//		sprintf(pVideoName, ".\\%d_%d_%d_%d_%d_%d_%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);

		char name[500];		
		//	sprintf(name,"%s%s%s%s%s%d%s","result2\\",Label[idx],"\\",pVideoName,"_",iiNum,".bmp");	
		sprintf(name,"%s%s%s%s","result3\\",pVideoName,"_",".jpg");	
		//	cout<<name<<endl;
		cvSaveImage(name,srcResize);
	}
	//===================================end=====================================//


	//=========根据总的识别概率判断，如果概率小于一个值，那么就采用密集CNN的思路，找第一个字符========//
//	cout<<"总的识别概率为："<<result.recPFinal<<endl;
	//==================================== end ============================================//


	vector<imageIpl>(imageChar).swap(imageChar);
	vector<resultPos>(imageCharRecPos).swap(imageCharRecPos);
	
	cvWaitKey(0);


	cvReleaseImage(&srcResize);
	cvReleaseImage(&dst);
	cvReleaseImage(&dstTemp);
	cvReleaseImage(&imgBin);
	cvReleaseImage(&dstY);
	//cvReleaseImage(&imgRetinex);
	cvReleaseImage(&pImagePlanes[0]);
	cvReleaseImage(&pImagePlanes[1]);
	cvReleaseImage(&pImagePlanes[2]);
	cvReleaseImage(&pImage16uColorSobel);
	cvReleaseImage(&pImage8uColorSobelShow_x);
	cvReleaseImage(&pImage8uColorSobelShow_y);
	cvReleaseImage(&imgsum);
	cvReleaseImage(&imgavg);
	cvReleaseImage(&paintx);  
	cvReleaseImage(&painty);  

	return result;
}

resultFinal processingOneP(IplImage *src){
	 resultFinal result;

		if(!src) printf("Error!\n");

		//====resize=====//
		int _width = (int)((float)(540.0/src->height)*src->width);;
		int _height = 540;

		IplImage *srcResize = cvCreateImage(cvSize(_width,_height),8,src->nChannels);
		cvResize(src,srcResize,CV_INTER_CUBIC);

		if(showSteps){
			cvNamedWindow("srcResize",1);
			cvShowImage("srcResize",srcResize);
		}
		
		CvRect roi;//ROI区域
		roi.x = 40;
		roi.y = 200;
		roi.width = srcResize->width - 20;
		if( (roi.width + roi.x)>srcResize->width )
			roi.width = srcResize->width - roi.x;
		roi.height = 170;
		IplImage *dst = cvCreateImage(cvSize(roi.width,roi.height),8,src->nChannels);
		cvSetImageROI(srcResize,roi);
		cvCopy(srcResize,dst);
		cvResetImageROI(srcResize);
				
		IplImage * input_image = cvCreateImage(cvGetSize(dst),8,1);
		cvCvtColor(dst,input_image,CV_BGR2GRAY);

		IplImage* dstImage = cvCreateImage(cvGetSize(input_image),8,1);
		cvCopy(input_image,dstImage);

		/*cvNamedWindow("dst_image",1);
		cvShowImage("dst_image",dstImage);*/

	////////////////--    one   --/////////////////////////////////////////////
	//	dstImage = dehist(input_image,155,115);	

	////////////////--    one   --/////////////////////////////////////////////
	//	cvEqualizeHist(dstImage,dstImage);

	////////////////--    one   --/////////////////////////////////////////////
		cvNormalize(dstImage,dstImage,255,0,CV_MINMAX);
	//	Retinex(dstImage, 200, 255, 128);

	/*	double weights[3] = {1.05, 1.2, 1.3};
		double sigmas[3] = {200, 100, 300};
		MultiScaleRetinex(dstImage, 3 , weights , sigmas , 128 ,128);*/

		/*	cvNamedWindow("dst_image1",1);
		cvShowImage("dst_image1",dstImage);*/

		IntImage image;
			const CvSize csize = cvSize(dstImage->width,dstImage->height);
			image.SetSize(csize);
			for(int i=0; i<dstImage->height; i++){
				uchar* pimg = (uchar*)(dstImage->imageData+input_image->widthStep*i);
				for(int j=0;j<dstImage->width;j++)	image.data[i][j] = pimg[j];

			}

			//===iphone上时间大概500ms
			const char * fileName = "1";
			cascade->ApplyOriginalSize(image,fileName);
			//printf("平面字符检测时间为：%f毫秒\n",dfTim);


			if(saveFlag){
				char pVideoName[256] = "";
				char image_name[500] ;
				//====根据识别结果分别存到相应的文件夹里面======//
				sprintf(image_name, "%s%s", pVideoName, ".bmp");//保存的图片名
				cvSaveImage(image_name, dst);
				//	cvSaveImage("dst.bmp",dst);
			}

			if( (int)resultROIs.size() < 5 ){ /* -- 加入平面检测判断是否为正确的平面 -- */
				result.recPFinal = 0.0;
				result.recString = "";
			}


			//根据识别结果保存样本图像
			CvRect ROI;
			vector<CvRect> detectROI;
			for(int num = 0;num< (int)resultROIs.size(); num++){
				ROI.x = resultROIs.at(num).x;
				ROI.y = resultROIs.at(num).y;
				ROI.width = resultROIs.at(num).width;	
				ROI.height = resultROIs.at(num).height;
				if(ROI.x < 0) ROI.x = 0;
				if(ROI.y < 0) ROI.y = 0;
				if((ROI.x + ROI.width) > dstImage->width) ROI.width = dstImage->width - ROI.x;
				if((ROI.y + ROI.height) > dstImage->height) ROI.height = dstImage->height - ROI.y;

		//		cout<<ROI.x<<" "<<ROI.y<<" "<<ROI.width<<" "<<ROI.height<<endl;
				
			//	cvRectangle(dst,cvPoint(ROI.x,ROI.y),cvPoint(ROI.x+ROI.width,ROI.y+ROI.height),CV_RGB(0,255,255),6);
				///////////////////////////////////////////////////////////////////////////////////////
				if( (ROI.width>=20)&&(ROI.height>=35) ){
					detectROI.push_back(ROI);
				}			
			}
 
		//	cout<<detectROI.size()<<endl;

			if(detectROI.size() > 0){
				//=====先对检测结果很另类的去掉====//
				int detectROIyavg = 0;
				for(int i=0;i<detectROI.size();i++){
					detectROIyavg += detectROI[i].y;
				}
				detectROIyavg = detectROIyavg/detectROI.size();

				//======排序=====//
				int *detectROIX;
				detectROIX = (int *)malloc(detectROI.size()*sizeof(int));
				for(int i=0;i<detectROI.size();i++){
					//============将y轴偏移很大的去掉=======//
					if( (detectROI[i].y >(detectROIyavg - 20))&&(detectROI[i].y < (detectROIyavg + 20)) )
						detectROIX[i] = detectROI[i].x;
				}
				InsertSort(detectROIX,detectROI.size());

				vector<CvRect> detectResult;

				for(int j=0;j<detectROI.size();j++){
					for(int i=0;i<detectROI.size();i++){	
						if(detectROI[i].x == detectROIX[j]){
			//				cout<<detectROIX[i]<<" ";

							detectResult.push_back(detectROI[i]);

						}
					}
				}

				free(detectROIX);

				if(detectResult.size() <= 0){
					result.recPFinal = 0;
					result.recString = "0";

					cvReleaseImage(&srcResize);
					cvReleaseImage(&dst);
					cvReleaseImage(&input_image);
					cvReleaseImage(&dstImage);

					image.Clear();

					return result;
				}


				//====计算平均宽度值======//
				int ROIWIDTH = 30;
				for(int i=0;i<detectResult.size();i++){
				//	cout<<"===##==="<<detectResult[i].x<<" "<<detectResult[i].y<<" "<<detectResult[i].width<<" "<<detectResult[i].height<<endl;
					ROIWIDTH += detectResult[i].width;
				}
				if(detectResult.size() != 0)
					ROIWIDTH = ROIWIDTH/detectResult.size();

				//======补全====//
				vector<CvRect> noDetectResult;
				for(int i=0;i<detectResult.size()-1;i++){
					if(
						(detectResult[i].x + detectResult[i].width + ROIWIDTH/2) < (detectResult[i+1].x )
 						){
						int j = ( detectResult[i+1].x - detectResult[i].x ) / ROIWIDTH  - 1;

			//			cout << i << " " << j <<endl;
			//			cout << "=================" << ( detectResult[i+1].x - detectResult[i].x ) % ROIWIDTH << endl;
						if( ((detectResult[i+1].x - detectResult[i].x) % ROIWIDTH ) > ROIWIDTH/2  ){
							j++;
						}
						for(int n =0 ;n<j;n++){
							CvRect Roi;
							Roi.x = detectResult[i].x  /*+ ROIWIDTH*n*/ + (ROIWIDTH )*(n+1);
						//	Roi.y = detectROIyavg;
							Roi.y = detectResult[i].y;
							Roi.width = ROIWIDTH;
							Roi.height = ROIHEIGHT;

						//	cvRectangle(dst, cvPoint(Roi.x , Roi.y ),cvPoint(Roi.x+Roi.width,Roi.y+Roi.height),CV_RGB(255,255,255), 2);	

							noDetectResult.push_back(Roi);
						}
					}

				}
				vector<CvRect> DetectAllResult;
				for(int i=0;i<detectResult.size();i++)		DetectAllResult.push_back(detectResult[i]);

		//		cout<<"aaaaa: "<<noDetectResult.size()<<endl;
				if(noDetectResult.size()!=0){
					for(int i=0;i<noDetectResult.size();i++)		DetectAllResult.push_back(noDetectResult[i]);
				}
				

				vector<CvRect>(detectResult).swap(detectResult);
				vector<CvRect>(noDetectResult).swap(noDetectResult);
				vector<CvRect>(detectROI).swap(detectROI);

		//		cout<<DetectAllResult.size()<<endl;

				//将首尾没有识别的补齐
				if(DetectAllResult.size()<=23){
					int *detectROIXX;
					detectROIXX = (int *)malloc(DetectAllResult.size()*sizeof(int));
					for(int i=0;i<DetectAllResult.size();i++){
						////============将y轴便宜很大的去掉=======//
						//if( (DetectAllResult[i].y >(detectROIyavg - 20))&&(DetectAllResult[i].y < (detectROIyavg + 20)) )
						detectROIXX[i] = DetectAllResult[i].x;
					}
					InsertSort(detectROIXX,DetectAllResult.size());

					vector<CvRect> detectResultF;

					for(int j=0;j<DetectAllResult.size();j++){
						for(int i=0;i<DetectAllResult.size();i++){
						//	cout<<detectROIXX[i]<<endl;
							if(DetectAllResult[i].x == detectROIXX[j]){
								detectResultF.push_back(DetectAllResult[i]);

							}
						}
					}

				//	for(int i=0;i<detectResultF.size();i++){
				//		cout<<"==**=="<<detectResultF[i].x<<" "<<detectResultF[i].y<<" "<<detectResultF[i].width<<" "<<detectResultF[i].height<<endl;		
				//	}

					if(detectResultF.size()!=0){
						if(detectResultF[0].x/ROIWIDTH > 0){
							CvRect temp;
							temp.x = detectResultF[0].x -ROIWIDTH;	temp.y = detectResultF[0].y;
							temp.width = detectResultF[0].x - temp.x;    temp.height = detectResultF[0].y+detectResultF[0].height - temp.y;

							DetectAllResult.push_back(temp);
						//	cvRectangle(dst, cvPoint(detectResultF[0].x -30 , detectResultF[0].y ),
						//		cvPoint(detectResultF[0].x ,detectResultF[0].y+detectResultF[0].height),CV_RGB(255,255,255), 2);	
						}
					}
					

					int tempNum = detectResultF.size() - 1;
					int tempFlagF = ( dstImage->width - (detectResultF[tempNum].x+detectResultF[tempNum].width) )/ROIWIDTH ;
					if( tempFlagF > 0){
					//	cout<<"daw gbhsevnu bw "<< tempFlagF <<endl;

						//====在tempFlagF的基础上减去2表示不考虑最后的背景，然后重新框选tempFlagF-2个区域
						for(int iR = 0;iR<(tempFlagF-2);iR++){
							CvRect temp;
							temp.x = (detectResultF[tempNum].x+detectResultF[tempNum].width) + ROIWIDTH*(iR);	  temp.y = detectResultF[tempNum].y;
							temp.width = ROIWIDTH;    temp.height = ROIHEIGHT;

							DetectAllResult.push_back(temp);
						//	cvRectangle(dst, cvPoint(detectResultF[0].x -30 , detectResultF[0].y ),
						//		cvPoint(detectResultF[0].x ,detectResultF[0].y+detectResultF[0].height),CV_RGB(255,255,255), 2);	
						}		
					}
					free(detectROIXX);
				}


				vector<resultPos> imageCharRecPos;

				char *Label[11];
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
				Label[10] = "10";	

				for(int i=0;i<DetectAllResult.size();i++){
					DetectAllResult[i].x = DetectAllResult[i].x - 2;
					DetectAllResult[i].y = DetectAllResult[i].y - 3;
					DetectAllResult[i].width = DetectAllResult[i].width + 2;
					DetectAllResult[i].height = DetectAllResult[i].height + 6;

					//目前对检测时候字符进行微调整
					const int _charHeight = 50;
					if(DetectAllResult[i].height< _charHeight){
						
						DetectAllResult[i].y = DetectAllResult[i].y + DetectAllResult[i].height/2 - _charHeight/2;
						
						DetectAllResult[i].height = _charHeight;

					}

					if(DetectAllResult[i].x < 0) DetectAllResult[i].x = 0;
					if(DetectAllResult[i].y < 0) DetectAllResult[i].y = 0;
					if( (DetectAllResult[i].x + DetectAllResult[i].width) > dstImage->width) DetectAllResult[i].width = dstImage->width - DetectAllResult[i].x;
					if( (DetectAllResult[i].y + DetectAllResult[i].height) > dstImage->height) DetectAllResult[i].height = dstImage->height - DetectAllResult[i].y;
					
				//	cout<<"====="<<DetectAllResult[i].x<<" "<<DetectAllResult[i].y<<" "<<DetectAllResult[i].width<<" "<<DetectAllResult[i].height<<endl;
				
					

					//		cvRectangle(dst, cvPoint(DetectAllResult[i].x , DetectAllResult[i].y ),
					//cvPoint(DetectAllResult[i].x+DetectAllResult[i].width,DetectAllResult[i].y+DetectAllResult[i].height),CV_RGB(255,255,0), 2);	
					
					cvSetImageROI(dst,DetectAllResult[i]);
					IplImage*  imgTempRoi = cvCreateImage(cvSize(DetectAllResult[i].width,DetectAllResult[i].height),8,3);
					cvCopy(dst,imgTempRoi);
					cvResetImageROI(dst);

			
					//================识别部分=============//
					const int imgWidth = 16;
					const int imgSize = imgWidth * imgWidth;

					IplImage *pRGB=cvCreateImage(cvSize(imgWidth,imgWidth),8,3);
					cvResize(imgTempRoi,pRGB);
					cvCvtColor(pRGB,pRGB,CV_BGR2RGB);
			//		cvNamedWindow("pRGB",1);
			//		cvShowImage("pRGB",pRGB);

					

					/**/
					uchar * imgData;
					imgData = (uchar *)pRGB->imageData;
					/*for (int i=0;i<3072;i++){
					cout<<(int)imgData[i]<<" ";
					}*/
	

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
					 

					vector<float> output;
					double t = (double)cvGetTickCount();
					cnnPlane->Forward((unsigned char *)data, imgWidth, imgWidth, 3, output);
					t = (double)cvGetTickCount() - t;

					int idx = 0;
				//	float singleCharSumP = 0.0;
					for (int ii = 0; ii < 11; ii++){
				//		singleCharSumP += output[ii];
						if (output[idx] < output[ii])
							idx = ii;
					}
				//	cout<<"单个字符总的概率： "<<singleCharSumP<<endl;

				//	cout<<Label[idx]<<endl;
				//	cvWaitKey(0);

					
					if(Label[idx] == "10")
						Label[idx] = " ";

			//		cout<<Label[idx]<<" 概率:("<<output[idx]<<")"<<endl;

					/*cvNamedWindow("imgTempRoi Image",1);
					cvShowImage("imgTempRoi Image",imgTempRoi);
					cvWaitKey(0);*/


					recCharAndP tempLO;
					tempLO.recChar = Label[idx][0];
					tempLO.recP = output[idx];

					resultPos temp;
					temp.recCharP = tempLO;
					temp.recPosition = DetectAllResult[i].x ;

					imageCharRecPos.push_back(temp);

				//	if (Label[idx] == "A")
					{

						char pVideoName[256];
						//sprintf(pVideoName, ".\\%d_%d_%d_%d_%d_%d_%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);
						char image_name[500] ;
		
						//====根据识别结果分别存到相应的文件夹里面======//
						sprintf(image_name, "%s%s%s%s%s%d%s%d%s", "result\\",Label[idx],"\\", pVideoName, "_", 1,"_", i, ".bmp");//保存的图片名
						cvSaveImage(image_name, imgTempRoi);
					}
					
					cvReleaseImage(&imgTempRoi);
					cvReleaseImage(&pRGB);
				
					free(data);
					//cvWaitKey(0);
				}
				
				//对结果出来并显示
				result = sortAndPrint(imageCharRecPos);

				
			}else{
				
				result.recPFinal = 0;
				result.recString = "0";

				
			}
			
			//=====保存最终结果======//
			if(saveFlag){

				char pVideoName[256];
				//sprintf(pVideoName, ".\\%d_%d_%d_%d_%d_%d_%d", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds);

				char image_name1[500] ;
				sprintf(image_name1, "%s%s%s", "result\\", pVideoName, ".bmp");//保存的图片名
			//	cvSaveImage(image_name1, dst);
			}

			if(1){
				//cvNamedWindow("Source Image",1);
				//cvShowImage("Source Image",dst);
			}

			
		
		//	cvWaitKey(0);
		
	
			cvReleaseImage(&srcResize);
			cvReleaseImage(&dst);
			cvReleaseImage(&input_image);
			cvReleaseImage(&dstImage);

			image.Clear();

			return result;

}


#ifdef __cplusplus
extern "C" {
#endif

int init_all ( char * cnn_bin,
				char * cnn_plane_bin,
				char * bc_bin,
				char * mode_path,
				char * cascade_bin )
{
	cnn = new LightNet::ConvNet();
	cnnPlane = new LightNet::ConvNet();
	BC = new BankCard();
	cascade = new _CascadeClassifier;
	if ( !cnn || !cnnPlane || !BC || !cascade )
		return -1;

	cnn->Load( cnn_bin );
	//__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr %s.",cnn_bin );
	cnnPlane->Load(cnn_plane_bin);
	//__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr %s.",cnn_plane_bin );
	BC->readModel( bc_bin, mode_path );
	//__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr %s.",bc_bin );
	srand((unsigned)time(NULL));
	LoadTestOptions();
	cascade->LoadBinCascade( cascade_bin );
	//__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr %s.",cascade_bin );
	ReadTextRangeFile();
	return 0;
}

int recognise_from_pic ( char * pic_file, char *s_result )
{
	if ( pic_file == NULL && s_result == NULL  )
		return -1;
	IplImage * src=cvLoadImage(pic_file,1);
	if ( !src )
		return -2;
	//__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr %s.",pic_file );
	string result = processingOne(src);
	strcpy ( s_result , result.c_str());
	cvReleaseImage ( &src );
//	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr %s.",result.c_str() );
	return 0 ;
}


int recognise_from_IplImage ( IplImage * src, char * s_result )
{
	//__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr %s.",pic_file );
	string result = processingOne(src);
	strcpy ( s_result , result.c_str());
//	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "bcnr %s.",result.c_str() );
	return 0 ;
}


int release_all ()
{
	if ( cnn )
		delete cnn;
	if ( cnnPlane )
		delete cnnPlane;
	if ( BC )
		delete BC;
	if ( cascade )
		delete cascade;
	cnn = NULL;
	cnnPlane = NULL;
	cascade = NULL;
	BC = NULL;
	cascade = NULL;
	return 0;
}

#ifdef __cplusplus
}
#endif
