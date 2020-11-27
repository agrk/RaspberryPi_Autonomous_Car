#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <wiringPi.h>

using namespace std;
using namespace cv;
using namespace raspicam;

#define MotorPin1 24
#define MotorPin2 23
#define MotorEnableDC1 25

#define MotorPin3 22
#define MotorPin4 27
#define MotorEnableDC2 17

int a,b,c,d,data;




Mat frame, Matrix, framePers, frameGray, frameThresh, frameEdge, frameFinal, frameFinalDuplicate;
Mat ROILane;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result;

RaspiCam_Cv Camera;

stringstream ss;


vector<int> histrogramLane;

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
void Capture()
{
	Camera.grab();
    Camera.retrieve( frame);
    cvtColor(frame, frame, COLOR_BGR2RGB);
}

void Perspective()
{
	line(frame,Source[0], Source[1], Scalar(0,0,255), 2);
	line(frame,Source[1], Source[3], Scalar(0,0,255), 2);
	line(frame,Source[3], Source[2], Scalar(0,0,255), 2);
	line(frame,Source[2], Source[0], Scalar(0,0,255), 2);
	
	
	Matrix = getPerspectiveTransform(Source, Destination);
	warpPerspective(frame, framePers, Matrix, Size(400,240));
}

void Threshold()
{
	cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
	inRange(frameGray, 200, 255, frameThresh);
	Canny(frameGray,frameEdge, 900, 900, 3, false);
	add(frameThresh, frameEdge, frameFinal);
	cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
	cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2BGR);   //used in histrogram function only
	
}

void Histrogram()
{
    histrogramLane.resize(360);
    histrogramLane.clear();
    
    for(int i=0; i<360; i++)       //frame.size().width = 400
    {
	ROILane = frameFinalDuplicate(Rect(i,140,1,100));
	divide(255, ROILane, ROILane);
	histrogramLane.push_back((int)(sum(ROILane)[0])); 
    }
}

void LaneFinder()
{
    vector<int>:: iterator LeftPtr;
    LeftPtr = max_element(histrogramLane.begin(), histrogramLane.begin() + 150);
    LeftLanePos = distance(histrogramLane.begin(), LeftPtr); 
    
    vector<int>:: iterator RightPtr;
    RightPtr = max_element(histrogramLane.begin() +250, histrogramLane.end());
    RightLanePos = distance(histrogramLane.begin(), RightPtr);
    
    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, 240), Scalar(0, 255,0), 2);
    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, 240), Scalar(0,255,0), 2); 
}

void LaneCenter()
{
    laneCenter = (RightLanePos-LeftLanePos)/2 +LeftLanePos;
    frameCenter = 179;
    
    line(frameFinal, Point2f(laneCenter,0), Point2f(laneCenter,240), Scalar(0,255,0), 3);
    line(frameFinal, Point2f(frameCenter,0), Point2f(frameCenter,240), Scalar(255,0,0), 3);

    Result = laneCenter-frameCenter;
}


//~ void Forward(){
	//~ digitalWrite(MotorPin1,HIGH);
	//~ digitalWrite(MotorPin2,LOW);
	//~ digitalWrite(MotorEnableDC1,HIGH);
	
	//~ digitalWrite(MotorPin3,HIGH);
	//~ digitalWrite(MotorPin4,LOW);
	//~ digitalWrite(MotorEnableDC2,HIGH);
//~ }


void Data()
{
   a = digitalRead(MotorPin1);
   b = digitalRead(MotorPin2);
   c = digitalRead(MotorPin2);
   d = digitalRead(MotorPin3);

   data = 8*d+4*c+2*b+a;
}

int main(int argc,char **argv)
{
	wiringPiSetupGpio();
	pinMode(MotorPin1, OUTPUT);
	pinMode(MotorPin2, OUTPUT);
	pinMode(MotorEnableDC1, OUTPUT);
	pinMode(MotorPin3, OUTPUT);
	pinMode(MotorPin4, OUTPUT);
	pinMode(MotorEnableDC2, OUTPUT);
	
	Setup(argc, argv, Camera);
	cout<<"Connecting to camera"<<endl;
	if (!Camera.open())
	{
		
	cout<<"Failed to Connect"<<endl;
     }
     
     cout<<"Camera Id = "<<Camera.getId()<<endl;
     
     
     
    
    while(1)
    {
	auto start = std::chrono::system_clock::now();

    Capture();
    Perspective();
    Threshold();
    Histrogram();
    LaneFinder();
    LaneCenter();
    //Forward();
    
    ss.str(" ");
    ss.clear();
    ss<<"Result = "<<Result;
    putText(frame, ss.str(), Point2f(1,50), 0,1, Scalar(0,0,255), 2);
    
    namedWindow("orignal", WINDOW_KEEPRATIO);
    moveWindow("orignal", 0, 100);
    resizeWindow("orignal", 640, 480);
    imshow("orignal", frame);
    
    namedWindow("Perspective", WINDOW_KEEPRATIO);
    moveWindow("Perspective", 640, 100);
    resizeWindow("Perspective", 640, 480);
    imshow("Perspective", framePers);
    
    namedWindow("Final", WINDOW_KEEPRATIO);
    moveWindow("Final", 1280, 100);
    resizeWindow("Final", 640, 480);
    imshow("Final", frameFinal);
    
    
    //~ digitalWrite(MotorEnableDC1,HIGH);
    //~ digitalWrite(MotorPin1,HIGH);
    //~ digitalWrite(MotorPin2,HIGH);
    //~ delay(5000);
    
    //~ digitalWrite(MotorEnableDC1,HIGH);
    //~ digitalWrite(MotorPin1,HIGH);
    //~ digitalWrite(MotorPin2,HIGH);
    
    //~ cout<<"after 5 sec"<<endl;
   
    
     if (Result == 0 && data==1)
    {
	   digitalWrite(MotorPin1,LOW);
	   digitalWrite(MotorPin2,LOW);
	   digitalWrite(MotorEnableDC1,LOW);
	
	   digitalWrite(MotorPin3,LOW);
	   digitalWrite(MotorPin4,LOW);
	   digitalWrite(MotorEnableDC2,LOW);
    }
    
        
    //~ else if (Result >0 && Result <10)
    //~ {
	
    //~ }
    
        //~ else if (Result >=10 && Result <20)
    //~ {
	//~ digitalWrite(24, 0);
	//~ digitalWrite(23, 1);    //decimal = 2
	//~ digitalWrite(22, 0);
	//~ digitalWrite(27, 0);
	//~ cout<<"Right2"<<endl;
    //~ }
    
        //~ else if (Result >20)
    //~ {
	//~ digitalWrite(24, 1);
	//~ digitalWrite(23, 1);    //decimal = 3
	//~ digitalWrite(22, 0);
	//~ digitalWrite(27, 0);
	//~ cout<<"Right3"<<endl;
    //~ }
    
        //~ else if (Result <0 && Result >-10)
    //~ {
	//~ digitalWrite(24, 0);
	//~ digitalWrite(23, 0);    //decimal = 4
	//~ digitalWrite(22, 1);
	//~ digitalWrite(27, 0);
	//~ cout<<"Left1"<<endl;
    //~ }
    
        //~ else if (Result <=-10 && Result >-20)
    //~ {
	//~ digitalWrite(24, 1);
	//~ digitalWrite(23, 0);    //decimal = 5
	//~ digitalWrite(22, 1);
	//~ digitalWrite(27, 0);
	//~ cout<<"Left2"<<endl;
    //~ }
    
        //~ else if (Result <-20)
    //~ {
	//~ digitalWrite(24, 0);
	//~ digitalWrite(23, 1);    //decimal = 6
	//~ digitalWrite(22, 1);
	//~ digitalWrite(27, 0);
	//~ cout<<"Left3"<<endl;
    //~ }
    
    
    waitKey(1);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    
    float t = elapsed_seconds.count();
    int FPS = 1/t;
    cout<<"FPS = "<<FPS<<endl;
    
    }

    
    return 0;
     
}
