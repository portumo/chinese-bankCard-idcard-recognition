
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>
//#include <android/log.h>
using namespace std;
#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"


typedef long LONG;


vector<CvRect> resultROIs; //保存识别结果

_CascadeClassifier::_CascadeClassifier():count(0),ac(NULL)
{
}

_CascadeClassifier::~_CascadeClassifier()
{
	Clear();	
}

void _CascadeClassifier::Clear()
{
	count = 0;
	delete[] ac; ac=NULL;
}

_CascadeClassifier& _CascadeClassifier::operator=(const _CascadeClassifier& source)
{
	Clear();
	count = source.count;
	ac = new AdaBoostClassifier[max_nodes]; assert(ac!=NULL);
	for(int i=0;i<count;i++) ac[i] = source.ac[i];
	return *this;
}



//=======================读取bin文件==================//
void _CascadeClassifier::ReadFromBin(ifstream& fout)
{
	Clear();	
	fout.read((char*)&count, sizeof(int));
//	cout<<count<<endl;
//	printf("%d,\n",count);

	ac = new AdaBoostClassifier[max_nodes]; assert(ac!=NULL);
	for(int i=0;i<count;i++) {
	//	printf("%d-%d\n",i,count);
		ac[i].ReadFromBin(fout);
	}


}

void _CascadeClassifier::LoadBinCascade(char *binfile )
{	
	ifstream fout(binfile ,ios::binary);
//	__android_log_print(ANDROID_LOG_DEBUG, "bcnr_debug", "after load cascade.bin." );
	ReadFromBin(fout);

	fout.close();
}


void _CascadeClassifier::ApplyOriginalSize(IntImage& original,const char * filename) 
{
	IntImage procface;
	IntImage image,square;
	REAL sq,ex,value;
	int result;
	CvRect rect;
	REAL ratio;
	vector<CvRect> results;

	procface.Copy(original);
	ratio = 1.0;
	results.clear();
	REAL paddedsize = REAL(1)/REAL((sx+1)*(sy+1));
	while((procface.height>sx+1) && (procface.width>sy+1))
	{
		procface.CalcSquareAndIntegral(square,image);
		for(int i=0,size_x=image.height-sx;i<size_x;i+=1)
			for(int j=0,size_y=image.width-sy;j<size_y;j+=1)
			{
				ex = image.data[i+sx][j+sy]+image.data[i][j]-image.data[i+sx][j]-image.data[i][j+sy];
				if(ex<mean_min || ex>mean_max) continue;
				sq = square.data[i+sx][j+sy]+square.data[i][j]-square.data[i+sx][j]-square.data[i][j+sy];
				if(sq<sq_min) continue;
				ex *= paddedsize;
				ex = ex * ex;
				sq *= paddedsize;
				sq = sq - ex;
				assert(sq>=0);
				if(sq>0) sq = sqrt(sq); else sq = 1.0;
				if(sq<var_min) continue;
				result = 1;
				for(int k=0;k<count;k++)
				{
					value = 0.0;
					for(int t=0,size=ac[k].count;t<size;t++)
					{
						REAL f1 = 0;
						REAL** p = image.data + i;
						SimpleClassifier& s = ac[k].scs[t];

						switch(s.type)
						{
							case 0:
								f1 =   p[s.x1][j+s.y3] - p[s.x1][j+s.y1] + p[s.x3][j+s.y3] - p[s.x3][j+s.y1]
									+ REAL(2)*(p[s.x2][j+s.y1] - p[s.x2][j+s.y3]);
								break;
							case 1:
								f1 =   p[s.x3][j+s.y1] + p[s.x3][j+s.y3] - p[s.x1][j+s.y1] - p[s.x1][j+s.y3]
									+ REAL(2)*(p[s.x1][j+s.y2] - p[s.x3][j+s.y2]);
								break;
							case 2:
								f1 =   p[s.x1][j+s.y1] - p[s.x1][j+s.y3] + p[s.x4][j+s.y3] - p[s.x4][j+s.y1]
									+ REAL(3)*(p[s.x2][j+s.y3] - p[s.x2][j+s.y1] + p[s.x3][j+s.y1]  - p[s.x3][j+s.y3]);
								break;
							case 3:
								f1 =   p[s.x1][j+s.y1] - p[s.x1][j+s.y4] + p[s.x3][j+s.y4] - p[s.x3][j+s.y1]
									+ REAL(3)*(p[s.x3][j+s.y2] - p[s.x3][j+s.y3] + p[s.x1][j+s.y3] - p[s.x1][j+s.y2]);
								break;
							case 4:
								f1 =   p[s.x1][j+s.y1] + p[s.x1][j+s.y3] + p[s.x3][j+s.y1] + p[s.x3][j+s.y3]
									- REAL(2)*(p[s.x2][j+s.y1] + p[s.x2][j+s.y3] + p[s.x1][j+s.y2] + p[s.x3][j+s.y2])
									+ REAL(4)*p[s.x2][j+s.y2];
								break;
							default:break;

						}
						if(s.parity!=0)
							if(f1<sq*s.thresh)
								value += ac[k].alphas[t];
							else ;
						else
							if(f1>=sq*s.thresh)
								value += ac[k].alphas[t];
							else ;
					}
					if(value<ac[k].thresh) 
					{
						result = 0;
						break;
					}
				}
				if(result!=0) 
				{
					const REAL r = 1.0/ratio;
					rect.x = (LONG)(j*r);rect.y = (LONG)(i*r);
					rect.width = (LONG)((j+sy)*r) - rect.x;
					rect.height = (LONG)((i+sx)*r) - rect.y;
					results.push_back(rect);
				}
			}
	/*	ratio = ratio * REAL(0.8);
		procface.Resize(image,REAL(0.8));*/
			ratio = ratio * REAL(0.8);
			procface.Resize(image,REAL(0.8));
		SwapIntImage(procface,image);
	}

	total_fp += results.size();

	PostProcess(results,2);
	PostProcess(results,0);

	resultROIs = results;
//	DrawResults(original,results);
//	original.Save(filename+"_result.JPG");
}

inline int SizeOfRect(const CvRect& rect)
{
	return rect.height*rect.width;
}

CvRect IntersectRect(CvRect result_i,CvRect resmax_j){
	CvRect rectInter;
	

	rectInter.x=max(result_i.x,resmax_j.x) ; 
	rectInter.y=max(result_i.y,resmax_j.y) ;

	int xxx = min((result_i.x + result_i.width),(resmax_j.x + resmax_j.width));
	int yyy = min((result_i.y + result_i.height),(resmax_j.y + resmax_j.height));

	rectInter.width = xxx - rectInter.x;
	rectInter.height = yyy - rectInter.y;

	return rectInter;
}

CvRect UnionRect(CvRect resmax_j,CvRect result_i){
	CvRect resmax_jj;

	resmax_jj.x = min(result_i.x,resmax_j.x) ; 
	resmax_jj.y = min(result_i.y,resmax_j.y) ;

	int xxx = max((result_i.x + result_i.width),(resmax_j.x + resmax_j.width));
	int yyy = max((result_i.y + result_i.height),(resmax_j.y + resmax_j.height));

	resmax_jj.width = xxx - resmax_j.x;
	resmax_jj.height = yyy - resmax_j.y;

	return resmax_jj;

}


void PostProcess(vector<CvRect>& result,const int combine_min)
{
	vector<CvRect> res1;
	vector<CvRect> resmax;
	vector<int> res2;
	bool yet;
	CvRect rectInter;
	
	for(unsigned int i=0,size_i=result.size();i<size_i;i++)
	{
		yet = false;
		CvRect& result_i = result[i];
		for(unsigned int j=0,size_r=res1.size();j<size_r;j++)
		{
			CvRect& resmax_j = resmax[j];
	
			rectInter = IntersectRect(result_i,resmax_j);
			if( (rectInter.height>0)&&(rectInter.width>0) )
			{
				if(
					SizeOfRect(rectInter)>0.6*SizeOfRect(result_i) 
				 && SizeOfRect(rectInter)>0.6*SizeOfRect(resmax_j)
				  )
				{
					CvRect& res1_j = res1[j];
					resmax_j = UnionRect(resmax_j,result_i);
					res1_j.height += result_i.height;
					res1_j.y += result_i.y;
					res1_j.x += result_i.x;
					res1_j.width += result_i.width;
					res2[j]++;
					yet = true;
					break;
				}
			}
		}
		if(yet==false)
		{
			res1.push_back(result_i);
			resmax.push_back(result_i);
			res2.push_back(1);
		}
	}

	for(unsigned int i=0,size=res1.size();i<size;i++)
	{
		const int count = res2[i];
		CvRect& res1_i = res1[i];
		res1_i.y /= count;
		res1_i.height /= count;
		res1_i.x /= count;
		res1_i.width /= count;
	}

	result.clear();
	for(unsigned int i=0,size=res1.size();i<size;i++) 
		if(res2[i]>combine_min)
			result.push_back(res1[i]);
}

