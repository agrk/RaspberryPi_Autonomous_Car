#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "raspicam_cv.h"
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;
using namespace raspicam;

Mat frame , Matrix , framePers, frameGray;
RaspiCam_Cv Camera;

Point2f Source[]={Point2f(20,200),Point2f(340,200),Point2f(0,230),Point2f(360,230)};
// point degerleri olusturulacak track e duzenleme yapilacak
//ekranda olusacak seklin yan taraflari track uzerindeki line a paralel olmali
Point2f Destination[]={Point2f(80,0),Point2f(280,0),Point2f(80,240),Point2f(280,240)};


 void Setup ( int argc,char **argv, RaspiCam_Cv &Camera )
  {
    Camera.set ( CAP_PROP_FRAME_WIDTH,  ( "-w",argc,argv,360) );
    Camera.set ( CAP_PROP_FRAME_HEIGHT,  ( "-h",argc,argv,240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br",argc,argv,50 ) );
    Camera.set ( CAP_PROP_CONTRAST ,( "-co",argc,argv,50 ) );
    Camera.set ( CAP_PROP_SATURATION,  ( "-sa",argc,argv,50 ) );
    Camera.set ( CAP_PROP_GAIN,  ( "-g",argc,argv ,50 ) );
    Camera.set ( CAP_PROP_FPS,  ( "-fps",argc,argv,0));

}
void Capture(){
	
	Camera.grab();
	Camera.retrieve( frame);
	cvtColor(frame, frame,COLOR_BGR2RGB);
}

void Perspective(){
	line(frame,Source[0],Source[1], Scalar(0,0,255),2);
	line(frame,Source[1],Source[3], Scalar(0,0,255),2);
	line(frame,Source[3],Source[2], Scalar(0,0,255),2);
	line(frame,Source[2],Source[0], Scalar(0,0,255),2);
	
	//line(frame,Destination[0],Destination[1], Scalar(0,255,0),2);
	//ine(frame,Destination[1],Destination[3], Scalar(0,255,0),2);
	//line(frame,Destination[3],Destination[2], Scalar(0,255,0),2);
	//line(frame,Destination[2],Destination[0], Scalar(0,255,0),2);
	
	Matrix = getPerspectiveTransform(Source,Destination);
	warpPerspective(frame , framePers, Matrix, Size(350,240));
}

void Threshold(){
	
	cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
	inRange(frameGray, 240, 255, frameGray);
	
}

int main(int argc, char **argv){
	
	Setup(argc,argv, Camera);
	cout<<"Connecting to Camera"<<endl;
	if(!Camera.open()){
		cout<<"Failed to Connect"<<endl;
		return -1;
	}
	
	cout<<"Camera ID = "<<Camera.getId()<<endl;
	
	while(1){
		
	
		auto start = std::chrono::system_clock::now();
		
		Capture();
		Perspective();
		Threshold();
		
		namedWindow("original", WINDOW_KEEPRATIO);
		moveWindow("original",0,100);
		resizeWindow("original",640,480);					
		imshow("original", frame);
		
		namedWindow("Perspective", WINDOW_KEEPRATIO);
		moveWindow("Perspective",640,100);
		resizeWindow("Perspective",640,480);					
		imshow("Perspective", framePers);
		
		namedWindow("GRAY", WINDOW_KEEPRATIO);
		moveWindow("GRAY",1280,100);
		resizeWindow("GRAY",640,480);					
		imshow("GRAY", frameGray);
		
		
		
		waitKey(1);
		
		auto end = std::chrono::system_clock::now();
	  

		std::chrono::duration<double> elapsed_seconds = end-start;
		
		float t = elapsed_seconds.count();
		int FPS = 1/t;
		cout<<"FPS = "<<FPS<<endl;
		
	}
	return 0;
}

