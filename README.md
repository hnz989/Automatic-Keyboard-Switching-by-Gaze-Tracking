# Automatic-Keyboard-Switching-by-Gaze-Tracking
#Brief
The program is aimed at using image recognition to switch the buletooth keyboard between several monitor and based on a existing classifer.

The hardware includes a raspbeery pi 3b +,  camera and Logitech Bluetooth Multi-Device Keyboard K480. The output of raspberry is up and down signal to control the keyboard. Some change of the keyboard is needed.

#command
The common command type is shown as following:

./Eyedetector 1 11

The first integer 1 can be any integer.

The second integer  include two parts. The tenths digit means the working mode. Mode 1 is normal mode to control bluetooth keyboard. Mode 2 is used for static evaluation and Mode 3 is used for static evaluation.  The unit digit works for accuracy control. If you don't want to consider that, 1 is OK.
