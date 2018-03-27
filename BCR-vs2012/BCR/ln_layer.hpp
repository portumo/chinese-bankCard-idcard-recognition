#ifndef __LN_LAYER_HPP__
#define __LN_LAYER_HPP__

#include "ln_vol.hpp"
#include <fstream>
#include <vector>

using namespace std;

namespace LightNet
{
	class BaseLayer
	{
	public:
		virtual void Read(ifstream &f) = 0;
		virtual void Forward(Vol &input, Vol &output) = 0;
	};

	class InputLayer : public BaseLayer
	{
	private:
		int H, W, C;
		Vol *m_src;

	public:
		InputLayer(int H, int W, int C);
		virtual ~InputLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);

		void Forward( unsigned char *pImage, int width, int height, int channel, Vol &output);
	};

	class OutputLayer : public BaseLayer
	{
	public:
		OutputLayer();
		virtual ~OutputLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);

		void Forward(Vol &input, vector<float> &output);
	};

	class ConvLayer : public BaseLayer
	{
	private:
		int k_size, filterNum, stride;
		vector<Vol *> filters;
		vector<float> biases;

	public:
		ConvLayer(int k_size, int filterNum, int stride, int input_C);
		virtual ~ConvLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};

	class LocalLayer : public BaseLayer
	{
	private:
		int k_size, filterNum, stride;
		vector<Vol *> filters;
		vector<float> biases;

		int input_H, input_W, input_C, input_S;
		int output_H, output_W, output_C, output_S;

	public:
		LocalLayer(int k_size, int filterNum, int stride, int input_H, int input_W, int input_C);
		virtual ~LocalLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};

	class PoolLayer : public BaseLayer
	{
	private:
		int k_size, stride;

	public:
		PoolLayer(int k_size, int stride);
		virtual ~PoolLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};

	class PoolAvgLayer : public BaseLayer
	{
	private:
		int k_size, stride;

	public:
		PoolAvgLayer(int k_size, int stride);
		virtual ~PoolAvgLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};

	class LCNLayer : public BaseLayer
	{
	private:
		float alpha, beta;
		int K, N;

	public:
		LCNLayer(int K, int N, float alpha, float beta);
		virtual ~LCNLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};

	class FCLayer : public BaseLayer
	{
	private:
		vector<Vol *> filters;
		vector<float> biases;

		int input_C, output_C;
	public:
		FCLayer(int input_C, int output_C);
		virtual ~FCLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};

	class SoftMaxLayer : public BaseLayer
	{
	public:
		SoftMaxLayer();
		virtual ~SoftMaxLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};

	class ReluLayer : public BaseLayer
	{
	public:
		ReluLayer();
		virtual ~ReluLayer();
		
		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};

	class SigmodLayer : public BaseLayer
	{
	public:
		SigmodLayer();
		virtual ~SigmodLayer();

		virtual void Read(ifstream &f);
		virtual void Forward(Vol &input, Vol &output);
	};
};

#endif