/**
*	WEART - Raw Sensor Data thimble TD
*	https://www.weart.it/
*/
#pragma once

#include "WeArtCommon.h"
#include "WeArtMessageListener.h"
#include "WeArtMessages.h"
#include <queue>

//! @brief Object used to track the raw sensors data for a single thimble
class WeArtRawSensorsData : public WeArtMessageListener {
public:
	WeArtRawSensorsData(HandSide handSide, ActuationPoint actuationPoint);

	//! @brief Get the last sample received
	//! @return the last sample received
	SensorData GetLastSample();

	//! @copydoc WeArtMessageListener::OnMessageReceived
	void OnMessageReceived(WeArtMessage* msg) override;
protected:
	HandSide handSide;
	ActuationPoint actuationPoint;

private:
	const int K_NUM_SAMPLES = 3;
	std::queue<SensorData> samples;
};

