#include "bcnr_wrap.h"
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> /*用到了time函数，所以要有这个头文件*/
#include <fstream>
#include <sstream>
#include <exception>
#include <vector>
#include <io.h>

#include <highgui.h> 
#include <cv.h>

#define showSteps 1

using namespace std;

char * configFile = "config.txt";  //在样本筛选model中，config.txt里的内容为要筛选的样本库，未分类。---

char* trainSetPosPath = (char *)malloc(200*sizeof(char)); //读取config文件里的内容--
void readConfig(char* configFile, char* trainSetPosPath){
	fstream f;
	char cstring[1000];
	int readS=0;
	f.open(configFile, fstream::in);
	char param1[200]; strcpy(param1,"");
	char param2[200]; strcpy(param2,"");
	char param3[200]; strcpy(param3,"");

	
	f.getline(cstring, sizeof(cstring));//--读取第一行：--
	readS=sscanf (cstring, "%s %s %s", param1,param2, param3);
	strcpy(trainSetPosPath,param3);
}


vector<string> imgNames;
int labelTemp = 0;

void dfsFolder(string folderPath){ //遍历config.txt里的根目录下的所有的文件，包括子目录。--// 其中子目录的名字就是label，子目录里的文件为label对于的训练测试样本---
	_finddata_t FileInfo;
	string strfind = folderPath + "\\*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L)
	{
		cerr << "can not match the folder path" << endl;
		exit(-1);
	}
	do{	
		if (FileInfo.attrib & _A_SUBDIR)    	{//判断是否有子目录--
			//	cout<<FileInfo.name<<" "<<FileInfo.attrib<<endl;	
			if( (strcmp(FileInfo.name,".") != 0 ) &&(strcmp(FileInfo.name,"..") != 0))   		{//这个语句很重要--
				string newPath = folderPath + "\\" + FileInfo.name;
				cout<<FileInfo.name<<" "<<newPath<<endl;
				
				labelTemp = atoi(FileInfo.name);//根目录下下的子目录名字就是label名，如果没有子目录则其为根目录下
				//	printf("%d\n",labelTemp);
				dfsFolder(newPath);
			}
		}else  {
			string finalName = folderPath + "\\" + FileInfo.name;
			//将所有的文件名写入一个txt文件--
			//	cout << FileInfo.name << "\t";
			//	printf("%d\t",label);
			//	cout << folderPath << "\\" << FileInfo.name  << " " <<endl;
			//将文件名字和label名字（子目录名字赋值给向量）--
			imgNames.push_back(finalName);
		}
	}while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);

}

void initTrainImage(){
	readConfig(configFile, trainSetPosPath);

	string folderPath = trainSetPosPath;
	dfsFolder(folderPath);

}


void processingTotal(){
	initTrainImage();

	int imgNum = imgNames.size();
	cout<<imgNum<<endl;
	for(int iNum=0;iNum<imgNum;iNum++){

		cout<<endl<<iNum<<endl;
		cout<<imgNames[iNum].c_str()<<endl;
		IplImage * src=cvLoadImage(imgNames[iNum].c_str(),1);  
		if(!src) continue;

		if(showSteps){
			cvNamedWindow("image",1);
			cvShowImage("image",src);

			cvWaitKey(1);
		}
	
		char result[200] ;	
		recognise_from_IplImage(src,result);

		if(result[0] == '\0')  
			continue;
		cout<< result<<endl;

		cvReleaseImage(&src);
		cvWaitKey(0);
	}
}




void main()
{
	 init_all ( "testWeight24.bin",
				 "testWeight16p.bin",
				 "testWeight16.bin","npclass.bin",
				 "cascade.bin" );

	processingTotal();

	release_all ();
}