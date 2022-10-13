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

	class Sample {
		public: 
			float AccX;
			float AccY;
			float AccZ;
			float GyroX;
			float GyroY;
			float GyroZ;
			int TOF;

			Sample() {
				AccX = 0.0f;
				AccY = 0.0f;
				AccZ = 0.0f;
				GyroX = 0.0f;
				GyroY = 0.0f;
				GyroZ = 0.0f;
				TOF = 0;
			}
	};

	std::vector<Sample*> samples;

	Sample* GetLastSample();

protected:

	HandSide handSide;

	ActuationPoint actuationPoint;

private:

	const int K_NUM_SAMPLES = 3;
};

