#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

struct _CascadeClassifier
{
	int count;
	AdaBoostClassifier* ac;

	_CascadeClassifier();
	~_CascadeClassifier();
	void Clear(void);
	_CascadeClassifier& operator=(const _CascadeClassifier& source);

	void ReadFromBin(ifstream& fout);
	void LoadBinCascade( char * binfile );

	virtual void ApplyOriginalSize(IntImage& original,const char * filename) ;
	
};

void PostProcess(vector<CvRect>& result, const int combine_min);
