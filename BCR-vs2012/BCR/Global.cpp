
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

#include <stdio.h>
#include <time.h>
#include <iostream>  
#include <fstream>  
#include <string>  
using namespace std; 

#include <cv.h>
#include <highgui.h>
#include <cvaux.h>

using namespace std; 

int total_fp;

char  trainset_filename[256];
char  validset_filename[256];
char  classifier_filename[256];
char  ada_log_filename[256];
char  cascade_filename[256];
char  FFS_WeakClassifiers_filename[256];
char  FFS_log_filename[256];
char  FileUsage_log_filename[256];
char  Bootstrap_database_filename[256];
char  Backup_directory_name[256];
char  TestSet_filename[256];
int sx; //height
int sy; //width
int train_method;
int linear_classifier;
int bootstrap_level;
int max_bootstrap_level;
vector<REAL> bootstrap_resizeratio;
vector<int> bootstrap_increment;
int totalfeatures;
int max_files;
int goal_method;
REAL node_det_goal;
REAL node_fp_goal;
int first_feature;
REAL asym_ratio;
int max_nodes;
vector<int> nof;
int starting_node;
int facecount,validfacecount;

IntImage* trainset=NULL;
IntImage* validset=NULL;
int totalcount;
int validcount;	

_CascadeClassifier* cascade=NULL;
REAL* weights=NULL;
int** table=NULL;
SimpleClassifier* classifiers=NULL;

REAL* features=NULL;
int* labels=NULL;

int* fileused=NULL;
int bootstrap_size;
char ** bootstrap_filenames;

REAL mean_min, mean_max, sq_min, sq_max, var_min, var_max;


void ReadTextRangeFile(void)
{		
	mean_min = 6543.9;
	mean_max = 71819;
	sq_min = 164206;
	sq_max = 1.6259e+007;
	var_min = 9.71844;
	var_max = 117.453;

}



void LoadTestOptions()
{	
	sx = 23;
	sy = 15;

	max_nodes = 80;
	
}



void InitTestGlobalData()
{

	srand((unsigned)time(NULL));
	
	cascade = new _CascadeClassifier;
	assert(cascade != NULL);
	LoadTestOptions();
	cascade->LoadBinCascade("");
	ReadTextRangeFile();
}

