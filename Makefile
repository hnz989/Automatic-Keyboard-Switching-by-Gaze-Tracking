CPP_FLAGS=-std=c++11
OPENCV_LIBS:= -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_imgcodecs -lopencv_videoio -lopencv_face -lbcm2835
LD_FLAGS=$(OPENCV_LIBS)

default: EyeDetector
helpers: helpers.cpp
	g++ $(CPP_FLAGS) $@ -o $^ -g
findEyeCenter:	findEyeCenter.cpp
	g++ $(CPP_FLAGS) $@ -o $^ $(LD_FLAGS) -g

EyeDetector: eye_detector.cpp helpers.o findEyeCenter.o  
	g++ $(CPP_FLAGS) $^ -o $@ $(LD_FLAGS) -g
#EyeDetector: eye_detector.cpp
#	g++ $(CPP_FLAGS) $^ -o $@ $(LD_FLAGS)
clean:
	rm -f EyeDetector
