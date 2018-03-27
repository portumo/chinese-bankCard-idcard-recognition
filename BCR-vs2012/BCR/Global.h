	enum TRAIN_METHODS {TRAIN_ADA = 1, TRAIN_FFS= 2};
	enum LINEAR_CLASSIFIER {LC_ORIGINAL = 1, LC_LAC = 2, LC_FDA = 3};

struct _CascadeClassifier;

extern vector<CvRect> resultROIs; //保存识别结果

extern char * 	option_filename;

extern int total_fp;

extern char  trainset_filename[256];
extern char  validset_filename[256];
extern char  classifier_filename[256];
extern char  ada_log_filename[256];
extern char  cascade_filename[256];
extern char  FFS_WeakClassifiers_filename[256];
extern char  FFS_log_filename[256];
extern char  FileUsage_log_filename[256];
extern char  Bootstrap_database_filename[256];
extern char  Backup_directory_name[256];
extern char  TestSet_filename[256];

extern int sx;
extern int sy;
extern int train_method;
extern int linear_classifier;
extern int bootstrap_level;
extern int max_bootstrap_level;
extern vector<REAL> bootstrap_resizeratio;
extern vector<int> bootstrap_increment;
extern int totalfeatures;
extern int max_files;
extern int goal_method;
extern REAL node_det_goal;
extern REAL node_fp_goal;
extern int first_feature;
extern REAL asym_ratio;
extern int max_nodes;
extern vector<int> nof;
extern int starting_node;
extern int facecount, validfacecount;

extern IntImage* trainset;
extern IntImage* validset;
extern int totalcount;
extern int validcount;	

extern _CascadeClassifier* cascade;
extern REAL* weights;
extern int** table;
extern SimpleClassifier* classifiers;

extern REAL* features;
extern int* labels;

extern int* fileused;
extern int bootstrap_size;
extern char ** bootstrap_filenames;

extern REAL mean_min,mean_max,sq_min,sq_max,var_min,var_max;


void InitTestGlobalData();

void ReadTextRangeFile(void);
void LoadTestOptions();
