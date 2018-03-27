# chinese-bankCard-idcard-recognition
haar + adboost, cnn,识别国内20多家银行的20多种银行卡，支持凸面和平面印刷字体

一、银行卡号识别系统

随着智能终端（智能手机及平板电脑）及移动通信（3G）的发展，原来运行在PC上的信息系统（如邮件系统、即时通信、网页浏览、协同办公、网络购物、社交网站、博客等）逐渐转移到智能终端设备上。可以预见未来几年60%以上的业务将会逐渐转移到智能终端系统上来。在这种背景下，杭州图铭科技有限公司推出基于Android 平台的银行卡号识别系统。

功能介绍
•	  通过拍照界面，指导用户拍出合格证件图像。
•	  采用文字识别（OCR）技术，自动识别银行卡信息（如卡号,卡所属银行等）.
•	  通过调用 识别功能Activity，实现其他应用程序接口调用。
•	  识别银行卡种类，主要是国内外20多家银行的印刷字体（平面黑色字体卡类）和凸面字体银行卡(包括字符间距类型为6-13、4-4-4-4-3、4-4-4-4等类型)。

系统功能
•	  对原图像进行倾斜矫正、抠图银行卡区域。
•	  通过形态学和目前检测思路。对字符进行区域定位和单个字符分割。
•	  对单个字符进行识别

![Alt text](https://github.com/portumo/chinese-bankCard-idcard-recognition/blob/master/result_img/2.jpg)
![Alt text](https://github.com/portumo/chinese-bankCard-idcard-recognition/blob/master/result_img/222.png)


二、Bank Card Rec 主要功能接口说明：
说明：如果需要其他接口都可以封装出来
主接口
/*调用识别之前先初始化一次*/
int init_all ();  

/*  调用识别，读取图像版本函数：
picture_file-输入图像名字，支持bmp、jpg、png、tif等等；
s_result为输出结果。
进行了很好的封装，客户只需要调用这个函数，会自动进行银行卡定位，银行卡矫正，银行卡号区域定位，银行卡单个字符分割识别，输出结果。
返回的int类型：为1表示是识别正确，为0就表示输入的是背景或者模糊的卡 或者卡不全*/
int recognise_from_pic ( char * picture_file, char *s_result ); 

/*释放权值*/ 
int release_all () ; 

/*摄像头版本识别从内存里面读取图像数据，进行识别。
camera_ID为前后相机ID，
s_result为输出结果。*/
int recognise_from_Camera( int camera_ID, char * s_result ); 

识别函数内部封装了
struct resultFinal //存识别结果和roi图像区域在原图中的位置
{
	string recString;//识别结果
	float recPFinal; //识别的概率
};
resultFinal processingOneT(IplImage *src);  //凸面字体识别，包括卡号定位和识别。
resultFinal processingOneP(IplImage *src);  //印刷字体卡片识别，包括卡号定位和识别。

/* IplImage * image为输入图像；
返回recCharAndP为分割出来的单个银行卡号字符的识别结果
struct  recCharAndP
{
	float recP;//识别的概率
	char recChar; //识别结果
}; */
recCharAndP RecSingleChar(IplImage * image);





凸面字符定位函数接口：
/* Mat & source,输入的原始图像；
返回Mat类型为定位分割出来的银行卡号区域*/
Mat findNum(Mat &source);

/* Mat &imageRGB为输入图像；
返回vector<Mat>为分割出来的单个银行卡号字符*/
vector<Mat> BankCard::findNum( Mat &imgRGB )

印刷体字符定位函数接口：
/* IplImage * source,输入的原始图像；
返回IplImage *类型为定位分割出来的银行卡号区域*/
IplImage *findNum(IplImage *source);

/* IplImage *image, IplImage * imageRGB为输入图像；
返回vector< IplImage *>为分割出来的单个银行卡号字符*/
vector< IplImage *> findChar(IplImage *imageRGB);



图像处理部分函数接口：
/*检测输入的图像是否为矩形卡片图像，
m_im为输入图像，
str如果为“1111”，表示输入图像是矩形卡片图像。*/
void vifLine(IplImage * m_im,char * str);  

IplImage *jiaozheng2(IplImage *res_im);//对倾斜图像进行矫正，并返回矫正过的图像

坐标点
/*输出原始图像的待矫正的四个顶点
res_im为输入图像，
vector<cv::Point2f>待矫正的顶点。*/
vector<cv::Point2f> getCardCornersPt(IplImage *res_im);


结果显示接口
/*将识别结果画在目标函数图像上，
m_im为输入图像，
result为识别结果*/
	IplImage * showResult (IplImage *res_im, string result);


卡类型添加接口
/*统计不到的卡类型添加到库里面
num为输入卡类型数组
length为该数据长度*/
	addCardType (char* num,  int length);

三、银行卡号识别 Q&A 技术问题：
1、提供的sdk是什么形式的？如*.so、apk？
算法是采用C、C++开发的，JNI交叉编译成*.so，然后提供接口给贵司调用。

2、提供的接口函数使用大致流程？
首先调用init_all初始化权值，之后直接调用相关封装好的函数recognise_from_Camera就可以识别输出结果，app退出时，调用release_all释放权值。

图像版本：
调用识别之前先初始化一次：
int init_all ()

之后可以调用识别，其中读取图像版本函数为：picture_file-输入图像名字，支持bmp、jpg、png、tif等等；s_result为输出结果。
int recognise_from_pic ( char * picture_file, char *s_result )

释放权值
int release_all () 

相机扫描版本：	
摄像头版本识别从内存里面读取图像数据，进行识别。camera_ID为前后相机ID，s_result为输出结果。
int recognise_from_Camera( int camera_ID, char * s_result )

3、能否函数介绍一下具体重要接口函数参数，如图像分析部分？
recognise_from_pic进行了很好的封装，客户只需要调用这个函数，会自动进行银行卡定位，银行卡矫正，银行卡号区域定位，银行卡单个字符分割识别，输出结果。
定位矫正函数：
void vifLine(IplImage * m_im,char * str);  //检测输入的图像是否为矩形卡片图像，m_im为输入图像，str如果为“1111”，表示输入图像是矩形卡片图像。
IplImage *jiaozheng2(IplImage *res_im);  //对输入的图像进行矫正，并返回矫正过的图像。

识别函数内部封装了
struct resultFinal //存识别结果和roi图像区域在原图中的位置
{
	string recString;//识别结果
	float recPFinal;//识别的概率
};
resultFinal processingOneT(IplImage *src);凸面字体识别，包括卡号定位和识别。
resultFinal processingOneP(IplImage *src);印刷字体卡片识别，包括卡号定位和识别。
以上接口客户不用关心，如果客户有需要我们也可以提供。



4、贵公司系统使用系统资源情况（具体些）？
具体的没有详细测试过，但是在iPhone4s，三星，小米，VIVO等多款手机上测试过可以正常运行。机器配置1.5GHz以上，识别一张图片1.5秒左右

5、如sdk出错，能否保证我们系统正常使用？
目前测试没有内存溢出，空指针的问题，基本上没有错误发生。

6、如果我们想在此sdk基础上做提示，能否开发更多更下层接口？
有需要接口都可以开放，包括图像预处理，卡号定位，识别结果显示在图像上，等等。

7、目前sdk有什么技术问题，如准确率、光线（不稳定部分）？
目前SDK支持国内大部分常见的卡类型，包括
7 4 7\4 4 4 4\6 5 8\19\6 10\4 3 8 1\6 5 3 2\6 12\6 4 4 4\7 4 7 \4 4 4 1 \6 6 6
其他类型暂时没有碰到，碰到可以扩展加入进去。
目前对卡的识别，光线和阴影应该都可以出来，特别是复杂背景也可以处理识别，准确率之前测试30张卡片错1张。
唯一的问题就是速度上比支付宝慢一点。
			
8、目前产品接口到什么底层（如准底层的像数处理层、图像处理层、应用接口层）：
由于SDK是采用标准C、C++开发的，方便移植到windows，linux，Android，ios等系统上，除了对图像的一些预处理矫正用到了开源的OpenCV之外，其他定位、检测，识别，都是我司独立开发的C、C++算法。算法模块接口根据客户需求提供，包括最底层的像素处理，图像处理，检测、识别、应用等。
以Android JNI为例我们会提供完整的MK示例文件，接口头文件和*.so。




