/**
*	WEART - Raw Sensor Data thimble TD
*	https://www.weart.it/
*/
#pragma once

#include "WeArtCommon.h"
#include "WeArtMessageListener.h"
#include "WeArtMessages.h"

//! @brief Object used to track the raw sensors data for a single thimble
class WeArtRawSensorsData : public WeArtMessageListener {
public:
	WeArtRawSensorsData(HandSide handSide, ActuationPoint actuationPoint);

	//! @copydoc WeArtMessageListener::OnMessageReceived
	void OnMessageReceived(WeArtMessage* msg) override;

	//! @brief Data received from sensors
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

	//! @brief Get the last sample received
	//! @return the last sample received
	Sample* GetLastSample();

protected:

	HandSide handSide;

	ActuationPoint actuationPoint;

private:

	const int K_NUM_SAMPLES = 3;
};

