#ifndef __LN_VOL_HPP__
#define __LN_VOL_HPP__

#include <fstream>

using namespace std; 



namespace LightNet
{
	class Vol
	{
	private:
		int H, W, C, size;
		float *data;

	public:

		Vol(int H, int W, int C);
		~Vol();

		void Read(ifstream &f);

		int GetH();
		int GetW();
		int GetC();
		int GetSize();

		float& operator() (int h, int w, int c);
		float& operator[] (int i);

		void Add(float v);
		void AddFrom(Vol &v);
		void AddFromScaled(Vol &v, float s);
		void SetConst(float v);

		//friend class Vol;
	};
};

#endif
