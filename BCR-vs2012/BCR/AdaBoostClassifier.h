enum GOAL_METHODS {GOAL_METHOD_MIN_ERROR = 1, GOAL_METHOD_VALID_DR = 2, GOAL_METHOD_FIX_FP = 3};

struct AdaBoostClassifier
{
	int count;
	SimpleClassifier* scs;
	REAL* alphas;
	REAL thresh;

	AdaBoostClassifier();
	~AdaBoostClassifier();
	void Clear(void);	

	void ReadFromBin(ifstream& fout);
};


