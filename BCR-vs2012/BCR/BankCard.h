#pragma once



using namespace cv;
using namespace std;



class BankCard
{
public:
    Mat nImg;
   
    Mat W1,W2,b1,b2;
    vector<Mat> filters;
    int numFilters;
    int filterWidth;

	Mat W1s,W2s,b1s,b2s;
    vector<Mat> filters_s;
    int numFilters_s;

    vector<Mat> patterns;
    int numPatterns;
    float numHeightScale,numWidthScale;

    BankCard();

	void readModel(string model10Path,string modelPath);
    vector<Mat> findNum(Mat &image, Mat &imageRGB);
  
    ~BankCard(void);
};

