//#include "stdafx.h"

#include "ln_convnet.hpp"
#include <cassert>
#include <iostream>

using namespace LightNet;

ConvNet::ConvNet()
{
	L_input = NULL;
	L_output = NULL;
}

ConvNet::~ConvNet()
{
	clear();
}

void ConvNet::clear()
{

	for (int i = 0, l_size = layers.size(); i < l_size; i++)
	{
		delete layers[i];
	}

	for (int i = 0, v_size = vols.size(); i < v_size; i++)
	{
		delete vols[i];
	}

	layers.clear();
	
	vols.clear();
	
	L_input = NULL;
	L_output = NULL;
}

int ConvNet::GetInputHeight()
{
	return vols[0]->GetH();
}

int ConvNet::GetInputWidth()
{
	return vols[0]->GetW();
}

int ConvNet::GetInputChannel()
{
	return vols[0]->GetC();
}

void ConvNet::GetMapInfo(vector<LayerType> &LTs, vector<int> &mapNums)
{
	LTs = this->LTs;
	mapNums.clear();
	for (unsigned i = 0; i < vols.size(); i++)
		mapNums.push_back(vols[i]->GetC());
}

void ConvNet::CloneFeatureMap(unsigned char *&pmap, int &H, int &W, int L_idx, int M_idx)
{
	assert(0 <= L_idx && L_idx < (int) vols.size());
	assert(0 <= M_idx && M_idx < vols[L_idx]->GetC());

	H = vols[L_idx]->GetH();
	W = vols[L_idx]->GetW();

	float *i_map = new float[H * W];
	float val;
	float low = 1e38f, up = -1e38f;

	for (int h = 0; h < H; h++)
	{
		for (int w = 0; w < W; w++)
		{
			i_map[h * W + w] = val = (*vols[L_idx])(h, w, M_idx);
			
			if (val < low) low = val;
			if (val > up) up = val;
		}
	}

	pmap = new unsigned char[H * W];

	for (int i = 0, sz = H * W; i < sz; i++)
	{
		pmap[i] = (unsigned char)((i_map[i] - low) / (up - low) * 255 + 0.499f);
	}

	delete[] i_map;
}

void ConvNet::Load(const char *f_name)
{
	ifstream f(f_name, ios::binary);
	assert(f.is_open());

	ReadBinWriteTxt(f);	
	
	f.close();		
}


void ConvNet::ReadBinWriteTxt( ifstream &f )
{
	clear();

	f.seekg(0, ifstream::end); //将文件指针指向文件尾
	int nbytes = (int) f.tellg();

	f.seekg(0, ifstream::beg); //将文件指针指向文件头

	BaseLayer *layer;
	int L_type = LT_Unknown;
	int ibuf[4];
	float fbuf[4];

	int output_H = 0;
	int output_W = 0;
	int output_C = 0;

	while (f.tellg() < nbytes)
	{
		f.read((char *)&L_type, sizeof(int));

		switch (L_type)
		{
		case LT_Input :
			f.read((char *)ibuf, 3 * sizeof(int));
			

			L_input = new InputLayer(ibuf[0], ibuf[1], ibuf[2]);
			L_input->Read(f);
			layers.push_back(L_input);
			output_H = ibuf[0];
			output_W = ibuf[1];
			output_C = ibuf[2];
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_Input);
			break;

		case LT_Conv :
			f.read((char *)ibuf, 3 * sizeof(int));
			

			layer = new ConvLayer(ibuf[0], ibuf[1], ibuf[2], output_C);
			layer->Read(f);
			layers.push_back(layer);
			output_H = output_H / ibuf[2];
			output_W = output_W / ibuf[2];
			output_C = ibuf[1];
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_Conv);
			break;

		case LT_Local :
			f.read((char *)ibuf, 3 * sizeof(int));
			

			layer = new LocalLayer(ibuf[0], ibuf[1], ibuf[2], output_H, output_W, output_C);
			layer->Read(f);
			layers.push_back(layer);
			output_H = output_H / ibuf[2];
			output_W = output_W / ibuf[2];
			output_C = ibuf[1];
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_Local);
			break;

		case LT_Pool :	
			f.read((char *)ibuf, 2 * sizeof(int));
		

			layer = new PoolLayer(ibuf[0], ibuf[1]);
			layer->Read(f);
			layers.push_back(layer);
			output_H = output_H / ibuf[1];
			output_W = output_W / ibuf[1];
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_Pool);
			break;

		case LT_Pool_Avg :
			f.read((char *)ibuf, 2 * sizeof(int));
			

			layer = new PoolAvgLayer(ibuf[0], ibuf[1]);
			layer->Read(f);
			layers.push_back(layer);
			output_H = output_H / ibuf[1];
			output_W = output_W / ibuf[1];
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_Pool);
			break;
		case LT_LCN :
			f.read((char *)ibuf, 2 * sizeof(int));
			

			f.read((char *)fbuf, 2 * sizeof(int));
			

			layer = new LCNLayer(ibuf[0], ibuf[1], fbuf[0], fbuf[1]);
			layer->Read(f);
			layers.push_back(layer);
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_LCN);
			break;

		case LT_FC :
			f.read((char *)ibuf, sizeof(int));
			

			layer = new FCLayer(output_H * output_W * output_C, ibuf[0]);
			layer->Read(f);
			layers.push_back(layer);
			output_C = ibuf[0];
			output_H = 1;
			output_W = 1;
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_FC);
			break;

		case LT_SoftMax :
			layer = new SoftMaxLayer();
			layer->Read(f);
			layers.push_back(layer);
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_SoftMax);
			break;

		case LT_Relu :
			layer = new ReluLayer();
			layer->Read(f);
			layers.push_back(layer);
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_Relu);
			break;

		case LT_Sigmod :
			layer = new SigmodLayer();
			layer->Read(f);
			layers.push_back(layer);
			vols.push_back(new Vol(output_H, output_W, output_C));
			LTs.push_back(LT_Sigmod);
			break;
		}
	}
	L_output = new OutputLayer();
	layers.push_back(L_output);
}


void ConvNet::Forward( unsigned char *pImage, int width, int height, int channel, vector<float> &output)
{
	L_input->Forward(pImage, width, height, channel, *vols[0]);
	for (int i = 1, size = layers.size() - 1; i < size; i++)
	{
		layers[i]->Forward(*vols[i - 1], *vols[i]);
	}
	L_output->Forward(*vols[vols.size() - 1], output);
}