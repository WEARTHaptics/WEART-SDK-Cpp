/**
*	WEART - Raw Sensor Data thimble TD
*	https://www.weart.it/
*/
#pragma once

#include "WeArtCommon.h"
#include "WeArtMessages.h"

class WeArtRawSensorData
{
public:

	WeArtRawSensorData(HandSide handSide, ActuationPoint actuationPoint);

	void OnMessageReceived(WeArtMessage* msg);

	float AccX;
	float AccY;
	float AccZ;
	float GyroX;
	float GyroY;
	float GyroZ;
	int TOF;

protected:

	HandSide handSide;

	ActuationPoint actuationPoint;
};

