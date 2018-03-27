//#include "stdafx.h"

#include "ln_vol.hpp"
#include <cassert>

using namespace LightNet;


Vol::Vol(int H, int W, int C)
{
	this->H = H;
	this->W = W;
	this->C = C;
	
	size = H * W * C;
	data = new float[size];
}

Vol::~Vol()
{
	delete[] data;
}

void Vol::Read(ifstream &f)
{
	f.read((char *)data, size * sizeof(float));
}

int Vol::GetH()
{
	return H;
}

int Vol::GetW()
{
	return W;
}

int Vol::GetC()
{
	return C;
}

int Vol::GetSize()
{
	return size;
}

float& Vol::operator() (int h, int w, int c)
{
	assert(0 <= h && h < H);
	assert(0 <= w && w < W);
	assert(0 <= c && c < C);

	return data[(h * W + w) * C + c];
}

float& Vol::operator[] (int i)
{
	assert(0 <= i && i < size);
	
	return data[i];
}

void Vol::Add(float v)
{
	for (int i = 0; i < size; i++)
		data[i] += v;
}

void Vol::AddFrom(Vol &v)
{
	assert(this->size == v.size);

	for (int i = 0; i < size; i++)
		data[i] += v.data[i];
}

void Vol::AddFromScaled(Vol &v, float s)
{
	assert(this->size == v.size);

	for (int i = 0; i < size; i++)
		data[i] += s * v.data[i];
}

void Vol::SetConst(float v)
{
	for (int i = 0; i < size; i++)
		data[i] = v;
}
