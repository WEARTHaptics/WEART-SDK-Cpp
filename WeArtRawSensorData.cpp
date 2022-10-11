/**
*	WEART - Raw Sensor Data thimble TD
*	https://www.weart.it/
*/

#include "pch.h"
#include "WeArtRawSensorData.h"
#include "WeArtController.h"

WeArtRawSensorData::WeArtRawSensorData(HandSide side, ActuationPoint actuation) {
	handSide = side;
	actuationPoint = actuation;
}

void WeArtRawSensorData::OnMessageReceived(WeArtMessage* msg)
{
	if (msg->getID() == "SensorsData") {
		RawSensorsData* rawSensorsData = static_cast<RawSensorsData*>(msg);
		if (rawSensorsData->GetHandSide() == handSide && rawSensorsData->GetActuationPoint() == actuationPoint) {
			
			this->AccX = rawSensorsData->accX;
			this->AccY = rawSensorsData->accY;
			this->AccZ = rawSensorsData->accZ;

			this->GyroX = rawSensorsData->gyroX;
			this->GyroY = rawSensorsData->gyroY;
			this->GyroZ = rawSensorsData->gyroZ;

			this->TOF = rawSensorsData->TOF;
		}

	}
}