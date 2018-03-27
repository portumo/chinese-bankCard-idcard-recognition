
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <float.h>
using namespace std;
#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"

#include <assert.h>


AdaBoostClassifier::AdaBoostClassifier():count(0),thresh(0.0),alphas(NULL),scs(NULL)
{
}

AdaBoostClassifier::~AdaBoostClassifier()
{
	Clear();
}

void AdaBoostClassifier::Clear()
{
	delete[] scs; scs=NULL;
	delete[] alphas; alphas=NULL;
	count = 0;
	thresh = 0.0;
}




void AdaBoostClassifier::ReadFromBin(ifstream& fout)
{
//	Clear();
	fout.read((char*)&count, sizeof(int));
//	printf("%d,\n",count);
	int countNum = count;
//	cout<<countNum<<endl;

	scs = new SimpleClassifier[countNum]; assert(scs!=NULL);
	alphas = new REAL[countNum]; assert(alphas!=NULL);

	fout.read((char*)&thresh, sizeof(REAL));
	for(int i=0;i<countNum;i++) {
		fout.read((char*)&alphas[i], sizeof(REAL));
	}
	for(int i=0;i<countNum;i++) {
		scs[i].ReadFromBin(fout);
	} 

}
