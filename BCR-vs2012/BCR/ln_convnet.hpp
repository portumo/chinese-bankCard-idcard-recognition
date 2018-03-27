#ifndef __LN_CONVNET_HPP__
#define __LN_CONVNET_HPP__

#include "ln_layer.hpp"

#include <fstream>
#include <vector>

using namespace std;

namespace LightNet
{
	enum LayerType
	{
		LT_Input = 0, LT_Conv, LT_Local, LT_Pool, LT_LCN, LT_FC, LT_SoftMax,
		LT_Pool_Avg = 30,
		LT_Relu = 0x100, LT_Sigmod,
		LT_Unknown = - 0xFFFF
	};


	class ConvNet
	{
	private:
		vector<BaseLayer *> layers;
		InputLayer *L_input;
		OutputLayer *L_output;
		vector<Vol *> vols;
		vector<LayerType> LTs;

		void clear();
		
	public:
	

		ConvNet();
		~ConvNet();

		int GetInputHeight();
		int GetInputWidth();
		int GetInputChannel();

		void GetMapInfo(vector<LayerType> &LTs, vector<int> &mapNums);
		void CloneFeatureMap( unsigned char *&pmap, int &H, int &W, int L_idx, int M_idx);

		void Load(const char *f_name);
	

		void ReadBinWriteTxt(ifstream &f);
		void Read(ifstream &f);
		void Forward( unsigned char *pImage, int width, int height, int channel, vector<float> &output);
	};
};

#endif