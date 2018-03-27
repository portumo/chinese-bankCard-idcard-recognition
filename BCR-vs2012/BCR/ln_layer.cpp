//#include "stdafx.h"

#include <opencv/highgui.h>
#include <opencv/cv.h>

#include "ln_layer.hpp"
#include <cassert>
#include <cmath>



//#include <opencv/highgui/highgui.hpp >
using namespace cv;

using namespace LightNet;


InputLayer::InputLayer(int H, int W, int C)
{
	this->H = H;
	this->W = W;
	this->C = C;

	m_src = new Vol(H, W, C);
}

InputLayer::~InputLayer()
{
	delete m_src;
}

void InputLayer::Read(ifstream &f)
{
	m_src->Read(f);
}

void InputLayer::Forward(Vol &input, Vol &output)
{

}

void InputLayer::Forward(unsigned char *pImage, int width, int height, int channel, Vol &output)
{
	assert(width == W && height == H &&  channel == C);

	assert(output.GetW() == W);
	assert(output.GetH() == H);
	assert(output.GetC() == C);
	
	for (int i = 0, size = output.GetSize(); i < size; i++){
		output[i] = pImage[i] - (*m_src)[i];
		
		
	}
/*IplImage * input_r= cvCreateImage(cvSize(width,height),8,1);
	input_r->imageData = (char*)pImage;
	cvNamedWindow("input_r",0);
	cvShowImage("input_r",input_r);

	IplImage * input_g= cvCreateImage(cvSize(width,height),8,1);
	input_g->imageData = (char*)(pImage+width*height);
	cvNamedWindow("input_g",0);
	cvShowImage("input_g",input_g);

	IplImage * input_b= cvCreateImage(cvSize(width,height),8,1);
	input_b->imageData = (char*)(pImage+width*height*2);
	cvNamedWindow("input_b",0);
	cvShowImage("input_b",input_b);
	
	cvWaitKey(0);*/
	

}

OutputLayer::OutputLayer()
{

}

OutputLayer::~OutputLayer()
{

}

void OutputLayer::Read(ifstream &f)
{

}

void OutputLayer::Forward(Vol &input, Vol &output)
{

}

void OutputLayer::Forward(Vol &input, vector<float> &output)
{
	int size = input.GetSize();
	
	output = vector<float>(size);
	for (int i = 0; i < size; i++)
		output[i] = input[i];
}

ConvLayer::ConvLayer(int k_size, int filterNum, int stride, int input_C)
{
	this->k_size = k_size;
	this->filterNum = filterNum;
	this->stride = stride;
	
	for (int i = 0; i < filterNum; i++)
		filters.push_back(new Vol(k_size, k_size, input_C));
	biases = vector<float>(filterNum);
}

ConvLayer::~ConvLayer()
{
	for (int i = 0; i < filterNum; i++)
		delete filters[i];
}

void ConvLayer::Read(ifstream &f)
{
	f.read((char *)&biases[0], filterNum * sizeof(float));
	
	for (int i = 0; i < filterNum; i++){
		filters[i]->Read(f);	
	}
}

bool convolve2DOriginal(float* in, float* out, int dataSizeX, int dataSizeY,
	float* kernel, int kernelSizeX, int kernelSizeY);

bool convolve2D(float* in, float* out, int data_size_X, int data_size_Y,
	float* kernel,int kernelSizeX, int kernelSizeY)
{
	if( kernelSizeX == 3){
		
		convolve2DOriginal( in,  out,  data_size_X,  data_size_Y, kernel,3,3);
	}
	if( kernelSizeX == 5){
	
		convolve2DOriginal( in,  out,  data_size_X,  data_size_Y, kernel,5,5);
	}
	//else{
	//	#define KERNX 7 //this is the x-size of the kernel. It will always be odd.
	//	#define KERNY 7 //this is the y-size of the kernel. It will always be odd.
	//	conv2Dfor( in,  out,  data_size_X,  data_size_Y, kernel);
	//}


	return true;
}


bool convolve2DOriginal(float* in, float* out, int dataSizeX, int dataSizeY,
	float* kernel, int kernelSizeX, int kernelSizeY)
{
	int i, j, m, n, mm, nn;
	int kCenterX, kCenterY;                         // center index of kernel
	float sum;                                      // temp accumulation buffer
	int rowIndex, colIndex;

	// check validity of params
	if(!in || !out || !kernel) return false;
	if(dataSizeX <= 0 || kernelSizeX <= 0) return false;

	// find center position of kernel (half of kernel size)
	kCenterX = kernelSizeX / 2;
	kCenterY = kernelSizeY / 2;

	for(i=0; i < dataSizeY; ++i)                // rows
	{
		for(j=0; j < dataSizeX; ++j)            // columns
		{
			sum = 0;                            // init to 0 before sum
			for(m=0; m < kernelSizeY; ++m)      // kernel rows
			{
				mm = kernelSizeY - 1 - m;       // row index of flipped kernel

				for(n=0; n < kernelSizeX; ++n)  // kernel columns
				{
					nn = kernelSizeX - 1 - n;   // column index of flipped kernel

					// index of input signal, used for checking boundary
					rowIndex = i + m - kCenterY;
					colIndex = j + n - kCenterX;

					// ignore input samples which are out of bound
					if(rowIndex >= 0 && rowIndex < dataSizeY && colIndex >= 0 && colIndex < dataSizeX){
					//	printf("%f ",kernel[kernelSizeX * m + n]);
						sum += in[dataSizeX * rowIndex + colIndex] * kernel[kernelSizeX * m + n];
					}
				}
			}
			out[dataSizeX * i + j] = sum;
		}
	}

 	return true;
}


bool convolve2DFast(float* in, float* out, int dataSizeX, int dataSizeY, 
	float* kernel, int kernelSizeX, int kernelSizeY)
{
	int i, j, m, n;
	float *inPtr, *inPtr2, *outPtr, *kPtr;
	int kCenterX, kCenterY;
	int rowMin, rowMax;                             // to check boundary of input array
	int colMin, colMax;                             //

	// check validity of params
	if(!in || !out || !kernel) return false;
	if(dataSizeX <= 0 || kernelSizeX <= 0) return false;

	// find center position of kernel (half of kernel size)
	kCenterX = kernelSizeX >> 1;
	kCenterY = kernelSizeY >> 1;

	//invert kernel
	for(int ii=0; ii< (kernelSizeX*kernelSizeY); ii++){
		kernel[ii] = kernel[kernelSizeX*kernelSizeY - ii -1];
	}

	// init working  pointers
	inPtr = inPtr2 = &in[dataSizeX * kCenterY + kCenterX];  // note that  it is shifted (kCenterX, kCenterY),
	outPtr = out;
	kPtr = kernel;

	// start convolution
	for(i= 0; i < dataSizeY; ++i)                   // number of rows
	{
		// compute the range of convolution, the current row of kernel should be between these
		rowMax = i + kCenterY;
		rowMin = i - dataSizeY + kCenterY;

		for(j = 0; j < dataSizeX; ++j)              // number of columns
		{
			// compute the range of convolution, the current column of kernel should be between these
			colMax = j + kCenterX;
			colMin = j - dataSizeX + kCenterX;

			*outPtr = 0;                            // set to 0 before accumulate

			// flip the kernel and traverse all the kernel values
			// multiply each kernel value with underlying input data
			for(m = 0; m < kernelSizeY; ++m)        // kernel rows
			{
				// check if the index is out of bound of input array
				if(m <= rowMax && m > rowMin)
				{
					for(n = 0; n < kernelSizeX; ++n)
					{
						// check the boundary of array
						if(n <= colMax && n > colMin)
							*outPtr += *(inPtr - n) * *kPtr;
						++kPtr;                     // next kernel
					}
				}
				else
					kPtr += kernelSizeX;            // out of bound, move to next row of kernel

				inPtr -= dataSizeX;                 // move input data 1 raw up
			}

			kPtr = kernel;                          // reset kernel to (0,0)
			inPtr = ++inPtr2;                       // next input
			++outPtr;                               // next output
		}
	}

	return true;
}


void ConvLayer::Forward(Vol &input, Vol &output)
{
	/* ---- input为输入data：32*32*3 = 3072 ---- */
	/* ---- output为输出的featureMap ：32 * 32*32 32个32*32的图 ---- */
	/* ---- 权值为 32行的 1024列的数据 ---- */
	assert(input.GetC() == filters[0]->GetC());
	assert(filterNum == output.GetC());

	assert(input.GetH() / stride == output.GetH());
	assert(input.GetW() / stride == output.GetW());
	
	int hk_size = k_size / 2;
	const int iH = input.GetH();
	const int iW = input.GetW();
	const int iC = input.GetC();
	const int inputDataSize = iH * iW;
	//输入data
	float *i_data = &input[0];

	//测试input
	/*for(int i=0;i<input.GetSize();i++){
		printf("%f ",i_data[i]);
	}*/
	int numOutput = 0;

//#pragma omp parallel num_threads(2)
	{
//#pragma omp for
	
	for (int oc = 0; oc < filterNum; oc++)
	{
		// 权值data
		float *f_data = &(*filters[oc])[0];	
		//feature Map输出
		float *fout = new float[inputDataSize];
		for(int i=0;i<inputDataSize;i++){
			fout[i] = (float)0;
		}
		//输入的feature map 个数
		for(int numInPutFeatureMap = 0; numInPutFeatureMap<iC; numInPutFeatureMap++){
			float *out = new float[inputDataSize]; 

			//卷积运算	
			convolve2DOriginal(i_data + numInPutFeatureMap*iH*iW , out,  iW,  iH,  f_data + k_size*k_size*numInPutFeatureMap ,  k_size,  k_size);

			//对输入feature Map的卷积进行相应位置叠加
			for(int i=0;i<inputDataSize;i++){
				fout[i] += out[i];
			}

			delete out;
		}

		//输出featureMap 加上biases
		for(int i=0;i<inputDataSize;i++){
			fout[i] += biases[oc] ;	
		}

		for (int oh = 0;  oh < output.GetH(); oh++ )
		{
			for (int ow = 0; ow < output.GetW(); ow++){
				output[numOutput] = fout[oh*output.GetW() + ow];

				numOutput ++;
			}
		}

		delete [] fout;
	}

	}
}

LocalLayer::LocalLayer(int k_size, int filterNum, int stride, int input_H, int input_W, int input_C)
{
	this->k_size = k_size;
	this->filterNum = filterNum;
	this->stride = stride;

	output_H = input_H / stride;
	output_W = input_W / stride;
	output_C = filterNum;
	output_S = output_H * output_W * filterNum;

	this->input_H = input_H;
	this->input_W = input_W;
	this->input_C = input_C;
	this->input_S = input_H * input_W * input_C;

	for (int i = 0; i < filterNum; i++)
		filters.push_back(new Vol(k_size*k_size, output_H*output_W , input_C));
	biases = vector<float>(output_S);
}

LocalLayer::~LocalLayer()
{
	for (int i = 0; i < output_S; i++)
		delete filters[i];
}

void LocalLayer::Read(ifstream &f)
{
	f.read((char *)&biases[0], output_S * sizeof(float));

	for (int i = 0; i < output_C; i++){
		filters[i]->Read(f);
	}	
}


bool convolve2DLocal(float* in, float* out, int dataSizeX, int dataSizeY,int dataSizeC,
	float* kernel, int kernelSizeX, int kernelSizeY)
{
	int i_data, j_data, m, n;
	int kCenterX, kCenterY;                         // center index of kernel
	int rowIndex, colIndex;

	// check validity of params
	if(!in || !out || !kernel) return false;
	if(dataSizeX <= 0 || kernelSizeX <= 0) return false;

	// find center position of kernel (half of kernel size)
	kCenterX = kernelSizeX / 2;
	kCenterY = kernelSizeY / 2;

//#pragma omp parallel num_threads(2)
	{
//#pragma omp for

	//对64个输入分别从左到右，从上到下进行扫描，由于输入是8*8，padding是2，stride是1，所以要扫,8*8次，即dataSizeY*dataSizeX次
	for(i_data=0; i_data < dataSizeY; ++i_data)                // rows
	{
		for(j_data=0; j_data < dataSizeX; ++j_data)            // columns
		{
			//取（输入channels）64个输入的左上角分别与64个3*3进行卷积得到64个值，然后将64个值相加 然后加上biases 得到输出的左上角的该值
			float sum = 0; //sum为64个值相加得到的值

			//取前输入图像个数64个3*3的卷积核
			for(int i_kernel=0; i_kernel < dataSizeC; ++i_kernel)    //循环iC次
			{
						//第一个输入的3*3（左上角）data与第一个3*3的卷积核卷积得到一个值sum_single
						//第二个输入的3*3（左上角）data与第二个3*3的卷积核卷积得到一个值sum_single
						float sum_single = 0; 
						//取3*3的卷积核
						for(m=0; m < kernelSizeY; ++m)      // kernel rows
						{
							for(n=0; n < kernelSizeX; ++n)  // kernel columns
							{
								// index of input signal, used for checking boundary
								rowIndex = i_data + m - kCenterY;
								colIndex = j_data + n - kCenterX;

								// ignore input samples which are out of bound
								if(rowIndex >= 0 && rowIndex < dataSizeY && colIndex >= 0 && colIndex < dataSizeX){
								//	printf("%f ",in[dataSizeX * rowIndex + colIndex + (i_kernel*dataSizeX+j_kernel)*dataSizeX*dataSizeY] );
								//	printf("%f ",kernel[kernelSizeX * m + n + (i_kernel*dataSizeY+j_kernel)*kernelSizeX*kernelSizeY  +(i_data*dataSizeX + j_data)*dataSizeX*dataSizeY*kernelSizeX*kernelSizeY]);

									sum_single += in[dataSizeX * rowIndex + colIndex + (i_kernel)*dataSizeX*dataSizeY] 
										* kernel[kernelSizeX * m + n + (i_kernel)*kernelSizeX*kernelSizeY  +(i_data*dataSizeX + j_data)*dataSizeC*kernelSizeX*kernelSizeY];
									/*----------data取值说明：-----------（这里输入的data是将64张data全部输入了）
									dataSizeX * rowIndex + colIndex ---- （3*3）kernelSizeY*kernelSizeX 的卷积输入data
									(i_kernel)*dataSizeX*dataSizeY   ------  间隔（8*8）dataSizeX*dataSizeY 再取另一个data，这时候由于输入data是（8*8）dataSizeX*dataSizeY大小的，相当于取下一张输入图,
																			因为总共有iC个输入，所以for(int i_kernel=0; i_kernel < dataSizeC; ++i_kernel) 
									-----------------------------------------*/

									/*----------kernel取值说明：-----------（这里的输入kernel是输入一列的所有权值）
									kernelSizeX * m + n ---- （3*3）kernelSizeY*kernelSizeX 的一个卷积核大小
									(i_kernel*dataSizeY+j_kernel)*kernelSizeX*kernelSizeY   ------  间隔（3*3）kernelSizeX*kernelSizeY 再取另一个卷积核
									(i_data*dataSizeX + j_data)*dataSizeC*kernelSizeX*kernelSizeY  ------ 间隔（64个3*3）dataSizeX*dataSizeY*kernelSizeX*kernelSizeY再取另一批卷积核，
																										其中第一批64个3*3是对应64个输入data的第一个左上角；第二批64个3*3是对应64个输入的左上第二个3*3.
																										这里的64代表的是输入的个数，iC
									-----------------------------------------*/
								}
							}
						}
						//这里得到一张输出的一个pixel，
						sum += sum_single ;
					
			}
			//这里得到输出的完整一张pixel.  
			out[dataSizeX * i_data + j_data] = sum ;
		
		}
	}
	
	
	} //omp

 	return true;
}

void LocalLayer::Forward(Vol &input, Vol &output)
{
	assert(input.GetH() == input_H && input.GetW() == input_W && input.GetC() == input_C);
	assert(output.GetH() == output_H && output.GetW() == output_W && output.GetC() == output_C);

	const int iH = input.GetH();
	const int iW = input.GetW();
	const int iC = input.GetC();
	const int inputDataSize = iH * iW;
	int hk_size = k_size / 2;	

	//输入data
	float *i_data = &input[0];

	//=====================================================//
	//ofstream o_file;
	//o_file.open("local.txt");
	//=====================================================//

	int numOutput = 0;
	for (int oc = 0; oc < filterNum; oc++)
	{
		// 取第一列的权值data 36864 64*3*3 * 8*8
		float *f_data = &(*filters[oc])[0];	

		//feature Map输出
		float *out = new float[inputDataSize]; 

		//卷积运算
		//每次输入的权值为iW*iH*output.GetW()*output.GetH()*k_size*k_size个,即local层权值的完整的一列，对应全部输入，一个输出
		convolve2DLocal(i_data , out,  iW,  iH,  iC, f_data /*+ iW*iH*output.GetW()*output.GetH()*k_size*k_size*oc*/ ,  k_size,  k_size);

		for (int oh = 0;  oh < output.GetH(); oh++ )
		{
			for (int ow = 0; ow < output.GetW(); ow++){
				// 这里的output必须还要加上biases。
				output[numOutput] = out[oh*output.GetW() + ow] + biases[numOutput];

				//=====================================================//
			//	o_file<<output[numOutput]<<" ";	//输出的featureMap写txt
				//=====================================================//

				numOutput ++;
			}
		}

		delete [] out;
	}

	//=====================================================//
//	o_file.close();
	//=====================================================//

}

PoolLayer::PoolLayer(int k_size, int stride)
{
	this->k_size = k_size;
	this->stride = stride;
}

PoolLayer::~PoolLayer()
{
	
}

void PoolLayer::Read(ifstream &f)
{

}

void PoolLayer::Forward(Vol &input, Vol &output)
{
	assert(input.GetH() / stride == output.GetH());
	assert(input.GetW() / stride == output.GetW());
	assert(input.GetC() == output.GetC());

	int iH = input.GetH();
	int iW = input.GetW();

	int oC = output.GetC();
	int oH = output.GetH();
	int oW = output.GetW();

	int hk_size = 0;

	int xk_size;int yk_size;
	xk_size = yk_size = k_size;


	int numOutput = 0;
	for (int oc = 0; oc < oC; oc++){
		for(int i=0; i <  iH  ; i+= stride)                // rows
		{
			for(int j=0; j <   iW ; j+= stride)            // columns
			{ 
				float val = -1e38f;

				//30-32列
				if( (j == ( iW - (xk_size-1) ))&&((i != ( iH - (yk_size-1) ))) )
				{
				//	xk_size = xk_size -1;

					for(int m=i; m < i+3; ++m)      // kernel rows
					{			
						for(int n=j; n < j+2; ++n)  // kernel columns
						{	
						//	printf("%f ",input[ (m * iW + n) + oc*iH*iW ]);
						
							float cas = input[ (m * iW + n) + oc*iH*iW ] ;
							if (cas > val) val = cas;
						}
					}
				}
				//30-32列
				else if( (j != ( iW - (xk_size-1) ))&&((i == ( iH - (yk_size-1) ))) )
				{
				//	yk_size = yk_size -1;

					for(int m=i; m < i+2; ++m)      // kernel rows
					{			
						for(int n=j; n < j+3; ++n)  // kernel columns
						{	
						//	printf("%f ",input[ (m * iW + n) + oc*iH*iW ]);
						
							float cas = input[ (m * iW + n) + oc*iH*iW ] ;
							if (cas > val) val = cas;
						}
					}
				}
				//30-32行与 30-32列
				else if( (j == ( iW - (xk_size-1) ))&&((i == ( iH - (yk_size-1) ))) )
				{
				//	yk_size = yk_size -1;

					for(int m=i; m < i+2; ++m)      // kernel rows
					{			
						for(int n=j; n < j+2; ++n)  // kernel columns
						{	
						//	printf("%f ",input[ (m * iW + n) + oc*iH*iW ]);
						
							float cas = input[ (m * iW + n) + oc*iH*iW ] ;
							if (cas > val) val = cas;
						}
					}
				}
				else
				{
					for(int m=i; m < i+k_size; ++m)      // kernel rows
					{			
						for(int n=j; n < j+k_size; ++n)  // kernel columns
						{	
						//	printf("%f ",input[ (m * iW + n) + oc*iH*iW ]);
						
							float cas = input[ (m * iW + n) + oc*iH*iW ] ;
							if (cas > val) val = cas;
						}
					}
				}
		
				output[numOutput ] = val;
			
				numOutput++ ;
			}
		}
	}
}

PoolAvgLayer::PoolAvgLayer(int k_size, int stride)
{
	this->k_size = k_size;
	this->stride = stride;
}

PoolAvgLayer::~PoolAvgLayer()
{
	
}

void PoolAvgLayer::Read(ifstream &f)
{

}

void PoolAvgLayer::Forward(Vol &input, Vol &output)
{
	assert(input.GetH() / stride == output.GetH());
	assert(input.GetW() / stride == output.GetW());
	assert(input.GetC() == output.GetC());

	int iH = input.GetH();
	int iW = input.GetW();

	int oC = output.GetC();
	int oH = output.GetH();
	int oW = output.GetW();

	int hk_size = 0;

	int xk_size;int yk_size;
	xk_size = yk_size = k_size;

	
	int numOutput = 0;
	for (int oc = 0; oc < oC; oc++){
		for(int i=0; i <  iH  ; i+= stride)                // rows
		{
			for(int j=0; j <   iW ; j+= stride)            // columns
			{ 
				float val = 0.0;

				//30-32列
				if( (j == ( iW - (xk_size-1) ))&&((i != ( iH - (yk_size-1) ))) )
				{
				//	xk_size = xk_size -1;

					for(int m=i; m < i+3; ++m)      // kernel rows
					{			
						for(int n=j; n < j+2; ++n)  // kernel columns
						{	
						//	printf("%f ",input[ (m * iW + n) + oc*iH*iW ]);
						
							val += input[ (m * iW + n) + oc*iH*iW ] ;
							
						}
					}
					val = val/6;
				}
				//30-32列
				else if( (j != ( iW - (xk_size-1) ))&&((i == ( iH - (yk_size-1) ))) )
				{
				//	yk_size = yk_size -1;

					for(int m=i; m < i+2; ++m)      // kernel rows
					{			
						for(int n=j; n < j+3; ++n)  // kernel columns
						{	
					//		printf("%f ",input[ (m * iW + n) + oc*iH*iW ]);
						
							val += input[ (m * iW + n) + oc*iH*iW ] ;
							
						}
					}
					val = val/6;
				}
				//30-32行与 30-32列
				else if( (j == ( iW - (xk_size-1) ))&&((i == ( iH - (yk_size-1) ))) )
				{
				//	yk_size = yk_size -1;

					for(int m=i; m < i+2; ++m)      // kernel rows
					{			
						for(int n=j; n < j+2; ++n)  // kernel columns
						{	
					//		printf("%f ",input[ (m * iW + n) + oc*iH*iW ]);
						
							val += input[ (m * iW + n) + oc*iH*iW ] ;
							
						}
					}
					val = val/4;
				}
				else
				{
					for(int m=i; m < i+k_size; ++m)      // kernel rows
					{			
						for(int n=j; n < j+k_size; ++n)  // kernel columns
						{	
					//		printf("%f ",input[ (m * iW + n) + oc*iH*iW ]);
						
							val += input[ (m * iW + n) + oc*iH*iW ] ;						
						}
					}
					val = val/(k_size * k_size);
				}
		
				output[numOutput ] = val;
			
				numOutput++ ;
			}
		}
	}
}

LCNLayer::LCNLayer(int K, int N, float alpha, float beta)
{
	this->K = K;
	this->N = N;
	this->alpha = alpha;
	this->beta = beta;
}

LCNLayer::~LCNLayer()
{

}

void LCNLayer::Read(ifstream &f)
{

}

void LCNLayer::Forward(Vol &input, Vol &output)
{
	assert(input.GetH() == output.GetH());
	assert(input.GetW() == output.GetW());
	assert(input.GetC() == output.GetC());

	int H = input.GetH();
	int W = input.GetW();
	int C = input.GetC();

	int hN = N / 2;

	for (int oc = 0; oc < C; oc++)
	{
		for (int oh = 0; oh < H; oh++)
		{
			for (int ow = 0; ow < W; ow++)
			{
				float val = 0;

				for (int ic = max(0, oc - hN); ic < min(C, oc + hN + 1); ic++)
				{
					float u = input[oh*W+ow + ic*W*H];
					val += u * u;
				}

				val *= alpha;
				val += K;
				val = pow(val, beta);

				output[oh*W+ow + oc*W*H] = input[oh*W+ow + oc*W*H] / val;		
			}
		}
	}
}

FCLayer::FCLayer(int input_C, int output_C)
{
	this->input_C = input_C;
	this->output_C = output_C;

	for (int i = 0; i < output_C; i++)
		filters.push_back(new Vol(1, 1, input_C));
	biases = vector<float>(output_C);
}

FCLayer::~FCLayer()
{
	for (int i = 0; i < output_C; i++)
	{
		delete filters[i];
	}
}

void FCLayer::Read(ifstream &f)
{
	f.read((char *)&biases[0], output_C * sizeof(float));
	
	for (int i = 0; i < output_C; i++){
		filters[i]->Read(f);	
	}	
}

void FCLayer::Forward(Vol &input, Vol &output)
{
	assert(input.GetSize() == input_C && output.GetSize() == output_C);
	
	float *i_data = &input[0];

	for (int oc = 0; oc < output_C; oc++)
	{
		float *f_data = &(*filters[oc])[0];
		float val = 0;

		for (int ic = 0; ic < input_C; ic++)
		{
			val += f_data[ic] * i_data[ic];
		}
		output[oc] = val;
	}
}

SoftMaxLayer::SoftMaxLayer()
{

}

SoftMaxLayer::~SoftMaxLayer()
{

}

void SoftMaxLayer::Read(ifstream &f)
{

}

void SoftMaxLayer::Forward(Vol &input, Vol &output)
{
	assert(input.GetSize() == output.GetSize());

	int size = input.GetSize();
	float z = 0, m = -1e38f;

	for (int i = 0; i < size; i++)
	{
		if (input[i] > m)
		{
			m = input[i];
		}
	}

	for (int i = 0; i < size; i++)
	{
		output[i] = exp(input[i] - m);
		z = z + output[i];
	}

	for (int i = 0; i < size; i++)
	{
		output[i] = output[i] / z;
	}
}

ReluLayer::ReluLayer()
{

}

ReluLayer::~ReluLayer()
{

}

void ReluLayer::Read(ifstream &f)
{

}

void ReluLayer::Forward(Vol &input, Vol &output)
{
	assert(input.GetSize() == output.GetSize());
	int size = input.GetSize();

	for (int i = 0; i < size; i++)
	{
		output[i] = max(0.0f, input[i]);		
	}
}

SigmodLayer::SigmodLayer()
{

}

SigmodLayer::~SigmodLayer()
{

}

void SigmodLayer::Read(ifstream &f)
{

}

void SigmodLayer::Forward(Vol &input, Vol &output)
{
	assert(input.GetSize() == output.GetSize());
	int size = input.GetSize();

	for (int i = 0; i < size; i++)
	{
		output[i] = 1 / (1 + exp(-input[i]));
	}
}
