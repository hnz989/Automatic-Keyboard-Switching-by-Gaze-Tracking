#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PIN0 RPI_GPIO_P1_11 
#define PIN1 RPI_GPIO_P1_12
#define PIN2 RPI_GPIO_P1_13
//The type for decesion result
enum RESULT{NODECISION = 0, TURNLEFT = 1, TURNRIGHT = 2, TURNMIDDLE = 3};
// Debugging
const bool kPlotVectorField = false;

// Size constants
const int kEyePercentTop = 25;
const int kEyePercentSide = 13;
const int kEyePercentHeight = 30;
const int kEyePercentWidth = 35;

// Preprocessing
const bool kSmoothFaceImage = false;
const float kSmoothFaceFactor = 0.005;

// Algorithm Parameters
const int kFastEyeWidth = 50;
const int kWeightBlurSize = 5;
const bool kEnableWeight = true;
const float kWeightDivisor = 1.0;
const double kGradientThreshold = 50.0;

// Postprocessing
const bool kEnablePostProcess = true;
const float kPostProcessThreshold = 0.97;

// Eye Corner
const bool kEnableEyeCorner = false;

// Decesion constant
const float KEYEyesDisplacement = 10;
double TIMECONST = 0.1;
//const result EyeResult = NODECISION;
//const result PupilResult = NODECISION;
#endif
