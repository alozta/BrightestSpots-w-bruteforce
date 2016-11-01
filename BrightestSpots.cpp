#include "opencv2/opencv.hpp"
#include <time.h>							//for timer
#include <cmath>							//abs

#define N_BRIGHTEST_SPOT 3

using namespace cv;

namespace alozta{
	class Point{							//Point structure to hold top N points
		public:
			int x;
			int y;
			uchar value;
	};
}

alozta::Point points[N_BRIGHTEST_SPOT];		//brightest spots

void topPoints(int x, int y, uchar c);
void sortTopPoints();
double getFPS(double seconds);
bool isSeperateSpot(int x, int y, int threshold=10);


int main(int, char**)
{
    VideoCapture cap(0); 					// open the default camera
    if(!cap.isOpened())
        return -1;

    Mat tmp;
    namedWindow("BrightestSpots",1);		//create a window

    int counter=0;
    for(int frames=0;;++frames)
    {
    	clock_t start = clock();						//frame start time
        Mat frame;
        cap >> frame; 									//get a new frame from camera

        cvtColor(frame, tmp, COLOR_BGR2GRAY);			//convert frame to grayscale, new frame in tmp

    	uchar* p;
    	for(int i = 0; i < tmp.rows; ++i){				//iterate in frame matrix
        	p = tmp.ptr(i);
        	for (int j = 0; j < tmp.cols; ++j, *p++){
            	topPoints(i,j,*p);						//test each pixel if it is brightest
        	}
    	}

    	for(int i=0; i<N_BRIGHTEST_SPOT; ++i){			//for each saved brightest point
    		circle(tmp, cv::Point(points[i].x,points[i].y),50,cv::Scalar(0,255,0),10);		//draw a circle around that point
    		/*if(frames%10==0){									//test brightest spots values
    			std::cout<<points[i].x<<","<<points[i].y<<","<<points[i].value<<std::endl;
    		}*/
    	}

        imshow("BrightestSpots", tmp);								//display the frame
        if(waitKey(30) >= 0) break;									//press button to quit
        for(int i=0; i<N_BRIGHTEST_SPOT; ++i){points[i].x=0;points[i].y=0;points[i].value=0;}	//reset
        clock_t end = clock();										//frame end time
    	std::cout << "FPS:" << getFPS((double)(end-start)/CLOCKS_PER_SEC) << std::endl;			//get fps
    }
    return 0;
}

//add a point if it's brighter than smallest valued one
void topPoints(int x, int y, uchar c){
	if(!isSeperateSpot(x,y)){
		return;
	}
	sortTopPoints();
	if(points[N_BRIGHTEST_SPOT-1].value < c){			//is it bigger than the smallest
		//std::cout<<x<<","<<y<<","<<std::endl;
		alozta::Point p;
		p.x=x;
		p.y=y;
		p.value=c;
		points[N_BRIGHTEST_SPOT-1]=p;
	}
	return;
}

//check the coordinates are different from top brightest point. they should be at least <threshold> pixels away.
bool isSeperateSpot(int x, int y, int threshold){
	for(int i=0; i<N_BRIGHTEST_SPOT; ++i){
		if((abs(points[i].x - x) > threshold) || (abs(points[i].y - y) > threshold)){
			return true;
		}
	}
	return false;
}

//sort points by their brightness
void sortTopPoints(){
	alozta::Point temp;
    int i, j;
 
    for (i=1; i<N_BRIGHTEST_SPOT; i++){
        for (j=0; j<N_BRIGHTEST_SPOT-i; j++){
            if(points[j].value < points[j+1].value){
                temp = points[j];
                points[j] = points[j+1];
                points[j+1] = temp;
            }
        }
    }
    return;
}

//FPS calculator based on render time of one frame
double getFPS(double seconds){
	return 1.0/seconds;
}
