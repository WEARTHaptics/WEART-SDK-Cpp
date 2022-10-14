/**
*	WEART - Raw Sensor Data thimble TD
*	https://www.weart.it/
*/

#include "pch.h"
#include "WeArtRawSensorsData.h"
#include "WeArtController.h"

WeArtRawSensorsData::WeArtRawSensorsData(HandSide side, ActuationPoint actuation) {
	handSide = side;
	actuationPoint = actuation;
}

WeArtRawSensorsData::Sample* WeArtRawSensorsData::GetLastSample() {
	if (this->samples.size() > 0) {
		Sample *lastSample = this->samples.back();
		return lastSample;
	}
	else {
		Sample *sample = new Sample();
		return sample;

	}
	
}

void WeArtRawSensorsData::OnMessageReceived(WeArtMessage* msg)
{
	if (msg->getID() == "SensorsData") {
		RawSensorsData* rawSensorsData = static_cast<RawSensorsData*>(msg);
		if (rawSensorsData->GetHandSide() == handSide && rawSensorsData->GetActuationPoint() == actuationPoint) {
			
			Sample* dataSample = new Sample();

			dataSample->AccX = rawSensorsData->accX;
			dataSample->AccY = rawSensorsData->accY;
			dataSample->AccZ = rawSensorsData->accZ;

			dataSample->GyroX = rawSensorsData->gyroX;
			dataSample->GyroY = rawSensorsData->gyroY;
			dataSample->GyroZ = rawSensorsData->gyroZ;

			dataSample->TOF = rawSensorsData->TOF;

			if (this->samples.size() <= K_NUM_SAMPLES) {
				samples.push_back(dataSample);
			}
			else {
				samples.clear();
				samples.push_back(dataSample);
			}
		}

	}
}
