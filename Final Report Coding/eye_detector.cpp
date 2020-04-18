#include <iostream>
#include <cstdio>
#include <fstream>
#include <queue>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>
extern "C"{
#include <bcm2835.h>
}
	
	//some modlues used in most opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

	//some modules used for facemark
#include <opencv2/face.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/face/facemark.hpp>
#include <opencv2/face/facemarkLBF.hpp>
	
	//the modules for pupil detection
#include "constants.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"

using namespace std;
using namespace cv;
using namespace cv::face;


FILE *OutPutFile;

//#define TEST
//#define EYEREGION
#define COMPARE
#define DELETE
//#define FACESIZE_GRA
//#define FACESIZE_LAN
//#define EYEREGION_LAN 
//#define EYEREGION_GRA 


int control(RESULT Decesion){
	if(!bcm2835_init())
		return 1;	
	
		//define output mode
	bcm2835_gpio_fsel(PIN0, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN2, BCM2835_GPIO_FSEL_OUTP);

		//Close all the GPIO 
	for(;;){
		bcm2835_gpio_write(PIN0,LOW);
		bcm2835_gpio_write(PIN1,LOW);
		bcm2835_gpio_write(PIN2,LOW);
	}
	for(;;){
		if(Decesion == TURNMIDDLE){
			bcm2835_gpio_write(PIN0,HIGH);
		}
		else if(Decesion == TURNLEFT){
		bcm2835_gpio_write(PIN1,HIGH);
		}
		else{
		bcm2835_gpio_write(PIN2,HIGH);
		}
	}
	return 0;
}

/*	brief: Control the logic of raspberry GPIO
 * */
// void GPIOChange(RESUTL Decesion){}


/* brief:	This function draw the face, eye and iris in the frame
 * faceCascade & eyeCascade:	The exist classifier in the lib, no training is required
 * frame:	The input image
 * Others:	
 * 			faces: the rectangle for face, it's a set of rectangle
 * 			face:  the new frame for the faces
 * 			eyes:  the rectangle for eye, it's a set of rectangle
 * 			eye:   the new frame for eye
 */

	//New version of face detector
RESULT detectEyes(cv::Mat &frame, cv::CascadeClassifier &faceCascade, cv::Ptr<cv::face::FacemarkLBF> &facemark, int Mode){
	int EvalMode = Mode / 10;
	int AccMode = Mode % 10;
	RESULT EyeResult = NODECISION;
	RESULT PupilResult = NODECISION;
	RESULT NewDecision = NODECISION;
	std::vector<Rect> faces;
	cv::Mat grayscale;
	cv::cvtColor(frame, grayscale, CV_BGR2GRAY); // convert image to grayscale

	equalizeHist( grayscale, grayscale);
	faceCascade.detectMultiScale(grayscale, faces, 1.1, 3,0, Size(150, 150));
	if (faces.size() == 0) return NewDecision; // none face was detected

	cv::Mat grayEye;
	grayscale.copyTo(grayEye);
	cv::Mat face = grayEye(faces[0]); // crop the face
	if(faces.size() > 0){
		vector< vector<Point2f> > shapes;
		if (facemark->fit(frame,faces,shapes))
		{
			 //Get the boundary of the eyes
			float Left_Left = 999999, Left_Top = 999999, Left_Bot = -999999, Left_Right = -999999;
			for(int i = 36; i <= 41;i++){
				if(shapes[0][i].x <= Left_Left ){
					Left_Left = shapes[0][i].x;
				}
				if(shapes[0][i].x >= Left_Right ){
					Left_Right = shapes[0][i].x;
				}
				if(shapes[0][i].y >= Left_Bot ){
					Left_Bot = shapes[0][i].y;
				}	
				if(shapes[0][i].y <= Left_Top ){
					Left_Top = shapes[0][i].y;
				}
			}

				//Let the rectangle be a little bigger
			Left_Left -= 15;
			Left_Right += 15;
			Left_Bot += 15;
			Left_Top -= 15;

				//The same thing to the right eye		
			float Right_Left = 999999,Right_Top = 999999, Right_Bot = -999999, Right_Right = -999999;
			for(int i = 42; i <= 47;i++){
				if(shapes[0][i].x <= Right_Left ){
					Right_Left = shapes[0][i].x;
				}
				if(shapes[0][i].x >= Right_Right ){
					Right_Right = shapes[0][i].x;
				}
				if(shapes[0][i].y >= Right_Bot ){
					Right_Bot = shapes[0][i].y;
				}	
				if(shapes[0][i].y <= Right_Top ){
					Right_Top = shapes[0][i].y;
				}
			}
	
				//Let the rectangle a little bigger
			Right_Top -= 15;
			Right_Bot += 15;
			Right_Left -= 15;
			Right_Right += 15;
	
			if(AccMode == 1 || AccMode == 2){
				float NosePosition = 0;
				for(int i = 27; i <= 30; i++){
					NosePosition += shapes[0][i].x;
				}
				NosePosition = NosePosition / 4;
				float NoseToLeft = 0;
				float NoseToRight = 0;

				//Another way to have the location of the eyes
				//	NoseToLeft = Left_Left - faces[0].x;
				//	NoseToRight = faces[0].x + faces[0].width - Right_Right;
		
				NoseToLeft =  NosePosition - Left_Right;
				NoseToRight = Right_Left - NosePosition;
				float EyesDisplacement = NoseToLeft - NoseToRight;
						//
				cout<<"EyesDisplacement is "<<EyesDisplacement<<endl;
				if(EyesDisplacement > KEYEyesDisplacement){
					EyeResult = TURNRIGHT;
					cout<<"The eye is looking at Right"<<endl;
				}	
				else if(EyesDisplacement < - KEYEyesDisplacement){
					EyeResult = TURNLEFT;
					cout<<"The eye is looking at left"<<endl;
				}
				else{
					EyeResult = TURNMIDDLE;
					cout<<"The eye is looking at middle"<<endl;
				}
	
			}
			
			//Store the boudary as the Rect
			cv::Rect leftEyeRegion(Left_Left- faces[0].x,Left_Top - faces[0].y ,Left_Right - Left_Left, Left_Bot - Left_Top);
			//Store the boudary as the Rect
			cv::Rect rightEyeRegion(Right_Left - faces[0].x, Right_Top - faces[0].y,Right_Right - Right_Left, Right_Bot - Right_Top);		
			
			if(AccMode == 1 || AccMode == 3){
					//now it's time to find pupil	
				cv::Mat faceROI = grayscale(faces[0]);
					//some preprocessing of the face frame
				if(kSmoothFaceImage){
					double sigma = kSmoothFaceFactor * faces[0].width;
					GaussianBlur(faceROI, faceROI, cv::Size(0,0), sigma);
				}
			
					//find the pupil by the eye region
				cv::Point leftPupil = findEyeCenter(faceROI,leftEyeRegion,"Left Eye");
				cv::Point rightPupil = findEyeCenter(faceROI,rightEyeRegion,"Right Eye");
				
				float LeftPupilRatio = float(leftPupil.x) / float(leftEyeRegion.width);
				float RightPupilRatio = float(rightPupil.x) / float(rightEyeRegion.width);

				  //draw the location of the pupil
				rightPupil.x += rightEyeRegion.x + faces[0].x;
				rightPupil.y += rightEyeRegion.y + faces[0].y;
				leftPupil.x += leftEyeRegion.x + faces[0].x;
				leftPupil.y += leftEyeRegion.y + faces[0].y;
			

		/*		cout <<"LeftPupil.x "<<leftPupil.x<<endl;	
				cout <<"LeftPupil.y "<<leftPupil.y<<endl;	
				cout <<"RightPupil.x "<<rightPupil.x<<endl;	
				cout <<"RightPupil.y "<<rightPupil.y<<endl;	

				cout<<"faces[0].x"<<faces[0].x<<endl;
				cout<<"faces[0].y"<<faces[0].x<<endl;
				cout<<"faces[0].width"<<faces[0].width<<endl;
				cout<<"faces[0].height"<<faces[0].height<<endl;
		*/	
				float PupilRatio = sqrt(pow(LeftPupilRatio,2) + pow(RightPupilRatio,2)) / 2;	
				//
				cout<<"The PupilRation is "<<PupilRatio<<endl;
				if(PupilRatio < 0.33){
					PupilResult = TURNLEFT;
				}	
				else if(PupilRatio > 0.66){
					PupilResult = TURNRIGHT;
				}
				else{
					PupilResult = TURNMIDDLE;
				}

				circle(grayscale,leftPupil,3,1234);
				circle(grayscale,rightPupil,3,1234);
				circle(frame,leftPupil,3,1234);
				circle(frame,rightPupil,3,1234);
			}
				
			if(AccMode == 1 || AccMode == 2){
				rightEyeRegion.x = Right_Left;
				rightEyeRegion.y = Right_Top;
				leftEyeRegion.x = Left_Left;
				leftEyeRegion.y = Left_Top;
				rectangle(frame,faces[0],Scalar(0,255,0));
				rectangle(frame,leftEyeRegion,Scalar(255,0,0));
				rectangle(frame,rightEyeRegion,Scalar(255,0,0));
			}
		}
	}	
	
	if(AccMode == 2){
		NewDecision = EyeResult;
	}
	else if(AccMode == 3){
		NewDecision = PupilResult;
	}
	else{
		if(EyeResult != TURNMIDDLE){
			NewDecision = EyeResult;
		}	
		else{
			NewDecision = PupilResult;
		}
	}

	imshow("grayscale",grayscale);
	return NewDecision;
}

int main(int argc, char **argv){	
	if (argc <= 2 ){
		std::cerr << "Usage: EyeDetector <WEBCAM_INDEX>" << std::endl;
		return -1;
	} 
	if(argc != 3){
		std::cerr <<"Usage: choose a mode, three mode can be choose"<<std::endl;
	}

	int Mode = atoi(argv[2]);
	int EvalMode = Mode / 10;
	int AccMode = Mode % 10;

	if(EvalMode > 3){
		std::cerr <<"Usage: Choose mode from one to three. Mode1 is normal mode for work"<<
			"Model2 is static evaluation and Model3 is dynamic evaluation"<<std::endl;
		return -1;
	}

	cv::CascadeClassifier faceCascade;
	if (!faceCascade.load("./haarcascade_frontalface_alt.xml")){
		std::cerr << "Could not load face detector." << std::endl;
		return -1;
	} 

	cv::CascadeClassifier faceCascadeLowAccuracy;
	if (!faceCascadeLowAccuracy.load("./haarcascade_frontalface_alt.xml")){
		std::cerr << "Could not load face detector." << std::endl;
		return -1;
	} 

	cv::VideoCapture cap(0); // the fist webcam connected to your PC
	if (!cap.isOpened()){
		std::cerr << "Webcam not detected." << std::endl;
		return -1;
	} 
	
	FacemarkLBF::Params params;
	cv::Ptr<cv::face::FacemarkLBF> facemark = FacemarkLBF::create(params);
	facemark->loadModel("./lbfmodel.yaml");
	RESULT OldDecesion = TURNMIDDLE;
	RESULT Decesion = NODECISION;

		//The normal mode to control buletooth keyboard
	if(EvalMode == 1 || EvalMode == 3){
	//	if(EvalMode == 3){
			ofstream OutPutFile;
			string OutName = "resultdynamic.txt";
			OutPutFile.open(OutName);
			if( !OutPutFile.is_open()){
				std::cout<<"ERROR: No output file can be created"<<std::endl;
				return -1;
			}
	//	}
		//The flag to show the decesion
		bool TimeChangeFlag = false;
		clock_t StartTime;
		clock_t StopTime = clock();
		cv::Mat frame;
		string ImgName;
		int WorkCount = 1; //The number of image
		while (1){
			cap.read(frame);
			if (!frame.data) break;
				//Refresh the start time, time is used to
				//reduce the noise
			if(TimeChangeFlag == true){
				StartTime = clock();
			}
			else{
				StartTime = StopTime;
			}

			Decesion = detectEyes(frame, faceCascade, facemark, Mode);
			StopTime = clock(); //recording the stop time
			double	TimeInterval = double(StopTime - StartTime) / double(CLOCKS_PER_SEC);	
			
				//Make the decesion according to the old decesion
			if(EvalMode == 3){
				//Make sure each time get a result rather than the value from old decesion
				OldDecesion = NODECISION;
			}
			if(OldDecesion == Decesion && Decesion != NODECISION){
				//if same make no decesion
				TimeChangeFlag = true;
			}
			else{
				if(Decesion != NODECISION){
					//else according to the time interval to  make a decesion
					if(TimeInterval  <= TIMECONST){
						TimeChangeFlag = true;
						Decesion = OldDecesion; //This is the noise, use the old decesion
					}
					else{
						//Only in this situation, change the output
						OldDecesion = Decesion;
						TimeChangeFlag = true;
						control(Decesion); 
					}	
				}	
				else{ //No decesion make
					TimeChangeFlag = false;
					Decesion = OldDecesion;
				}
			}
			
			cv::imshow("Webcam", frame); // displays the Mat
				//store the image
			if(EvalMode == 3 && Decesion != NODECISION){
				if(Decesion == TURNLEFT){
					OutPutFile <<"The result of "<< WorkCount<<"th image, is TURNLEFT. The time is "<<TimeInterval<<endl;
					ImgName = "./DynamicResult/The_" + std::to_string(WorkCount) + "_left.jpg";
					cv::imwrite(ImgName,frame);
					WorkCount++;
				}
				else if(Decesion == TURNRIGHT){
					OutPutFile <<"The result of "<< WorkCount<<"th image, is TURNRIGHT. The time is "<<TimeInterval<<endl;
					ImgName = "./DynamicResult/The_" + std::to_string(WorkCount) + "_right.jpg";
					cv::imwrite(ImgName,frame);
					WorkCount++;
				}
				else if(Decesion == TURNMIDDLE){
					OutPutFile <<"The result of "<< WorkCount<<"th image,  is TURNMIDDLE. The time is "<<TimeInterval<<endl;
					ImgName = "./DynamicResult/The_"+ std::to_string(WorkCount) + "_middle.jpg";
					cv::imwrite(ImgName,frame);
					WorkCount++;
				}
			}
			if (cv::waitKey(30) >= 0) break;  // takes 30 frames per second. if the user presses any button, it stops from showing the webcam
		}
		OutPutFile.close();
	}
		//The mode for static evaluation
	else if(EvalMode == 2){
		ofstream OutPutFile;
		string OutName = "result" + std::to_string(AccMode) + ".txt";
		OutPutFile.open(OutName);
		if( !OutPutFile.is_open()){
			std::cout<<"ERROR: No output file can be created"<<std::endl;
			return -1;
		}


			//Load picture in the folder
		string DirPath = "./ImageStatic/"; //the directoy path
		std::vector<cv::String> FileNames;
		glob(DirPath + "*.jpg",FileNames, false);
		RESULT Decesion = NODECISION;
		size_t count = FileNames.size();
		for(size_t i = 0; i < FileNames.size();i++){
				//get image name
			string FileName = FileNames[i];
			cout<<"Processing "<<FileName<<endl;
			cv:Mat frame = imread(FileName);
			clock_t StartTime = clock();
			Decesion = detectEyes(frame, faceCascade, facemark, Mode);
			clock_t	StopTime = clock(); //recording the stop time
			string ImgName = FileName.erase(0,13);
			ImgName = "./ImageResult" + FileName;
			cout<<ImgName<<endl;
			imwrite(ImgName, frame);
			double	TimeInterval = double(StopTime - StartTime)/double(CLOCKS_PER_SEC);	
				//store the result in a file
			if(Decesion == NODECISION){
				OutPutFile <<"The result of "<< i<<"th image, "<<FileName<<" is NODECISION"<<" The time is "<<TimeInterval<<endl;
			}
			else if(Decesion == TURNLEFT){
				OutPutFile <<"The result of "<< i<<"th image, "<<FileName<<" is TURNLEFT"<<" The time is "<<TimeInterval<<endl;
			}
			else if(Decesion == TURNRIGHT){
				OutPutFile <<"The result of "<< i<<"th image, "<<FileName<<" is TURNRIGHT"<<" The time is "<<TimeInterval<<endl;
			}
			else if(Decesion == TURNMIDDLE){
				OutPutFile <<"The result of "<< i<<"th image, "<<FileName<<" is TURNMIDDLE"<<" The time is "<<TimeInterval<<endl;
			}
		}
		OutPutFile.close();
	}
	return 0;
}

