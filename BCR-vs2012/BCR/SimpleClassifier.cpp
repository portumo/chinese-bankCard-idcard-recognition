
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;
#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"


#include <assert.h>




void SimpleClassifier::ReadFromBin(ifstream& fout)
{
	fout.read((char*)&thresh, sizeof(REAL));
	fout.read((char*)&parity, sizeof(int));
	fout.read((char*)&type, sizeof(int));
	fout.read((char*)&x1, sizeof(int));
	fout.read((char*)&x2, sizeof(int));
	fout.read((char*)&x3, sizeof(int));
	fout.read((char*)&x4, sizeof(int));
	fout.read((char*)&y1, sizeof(int));
	fout.read((char*)&y2, sizeof(int));
	fout.read((char*)&y3, sizeof(int));
	fout.read((char*)&y4, sizeof(int));

	assert(parity == 0 || parity == 1);
	assert(type>=0 && type<=4);
}



