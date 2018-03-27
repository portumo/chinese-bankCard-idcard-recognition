#include "opencv2/opencv.hpp"

class CV_EXPORTS TickMeter  
{  
public:  
	TickMeter();  
	void start();  
	void stop();  

	int64 getTimeTicks() const;  
	double getTimeMicro() const;  
	double getTimeMilli() const;  
	double getTimeSec()   const;  
	int64 getCounter() const;  

	void reset();  
private:  
	int64 counter;  
	int64 sumTime;  
	int64 startTime;  
};  
CV_EXPORTS std::ostream& operator<<(std::ostream& out, const TickMeter& tm);  

//Spinimages.cpp      
cv::TickMeter::TickMeter() { reset(); }  
int64 cv::TickMeter::getTimeTicks() const { return sumTime; }  
double cv::TickMeter::getTimeMicro() const { return (double)getTimeTicks()/cvGetTickFrequency(); }  
double cv::TickMeter::getTimeMilli() const { return getTimeMicro()*1e-3; }  
double cv::TickMeter::getTimeSec()   const { return getTimeMilli()*1e-3; }      
int64 cv::TickMeter::getCounter() const { return counter; }  
void  cv::TickMeter::reset() {startTime = 0; sumTime = 0; counter = 0; }  

void cv::TickMeter::start(){ startTime = cvGetTickCount(); }  
void cv::TickMeter::stop()  
{  
	int64 time = cvGetTickCount();  
	if ( startTime == 0 )  
		return;  

	++counter;  

	sumTime += ( time - startTime );  
	startTime = 0;  
}  

std::ostream& cv::operator<<(std::ostream& out, const TickMeter& tm){ return out << tm.getTimeSec() << "sec"; }  