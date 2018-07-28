#include "ML8511.h"
#include <Arduino.h>
  
ML8511::ML8511(int pinAO, int pinAref) {
    this->pinAO = pinAO;
    this->pinAref = pinAref;
}

int ML8511::averageAnalogRead(int aPin, int nSample) {
    int ret = 0;
    for (int i=0; i < nSample; i++) ret+= analogRead(aPin);
    return (int) ret/nSample;
}

int ML8511::readUVIntensity() {
    float uv_VolOutput = 3.3 / averageAnalogRead(pinAref,8) * averageAnalogRead(pinAO,8);
    int uv_Intensity = (uv_VolOutput - 0.99) * (2.8 - 0.99) / (uvMax - uvMin) + uvMin;
    return uv_Intensity;
}

void ML8511::setUVRange(float min, float max) {
    this->uvMin = min;
    this->uvMax = max;
}