/**************************************************************************/
/*!
@file     ML8511.h
@author   lnquy065
@license  GNU GPLv3
@version  1.0
First version of an Arduino Library for the ML8511 uv sensor
Ref: https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide
*/
/**************************************************************************/

class ML8511 {
  private:
  int pinAO;
  int pinAref;
  float uvMax = 15;
  float uvMin = 0;
  public:
  ML8511(int, int);
  int averageAnalogRead(int, int);
  int readUVIntensity();
  void setUVRange(float, float);
};