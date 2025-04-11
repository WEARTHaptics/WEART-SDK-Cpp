/**
*	WEART - Raw Analog Sensor Data thimble TD
*	https://www.weart.it/
*/
#pragma once

#include "WeArtCommon.h"
#include "WeArtMessageListener.h"
#include "WeArtMessages.h"
#include <queue>

//! @brief Analog sensor data
class WeArtAnalogSensorData : public WeArtMessageListener
{
public:
	//! @brief Create a WeArtAnalogSensorData tracking object
	//! @param handSide			Hand side from which to take the sensor data
	//! @param actuationPoint	Thimble from which to take the sensor data
	WeArtAnalogSensorData(HandSide handSide, ActuationPoint actuationPoint);

	//! @brief Sensor data sample
	struct Sample {
		//! @brief Timestamp when the sample was created (in milliseconds unix epoch time)
		std::uint64_t timestamp;
		//! @brief Sampled sensor data
		AnalogSensorRawData data;
	};

	//! @brief Get the last sample received
	//! @return the last sample received
	Sample GetLastSample();

	//! @brief Adds a callback called whenever a new sample is received
	//! @param callback Callback called when a sample is received
	void AddSampleCallback(std::function<void(Sample)> callback);

	//! @copydoc WeArtMessageListener::OnMessageReceived
	void OnMessageReceived(WeArtMessage* msg) override;

private:
	HandSide handSide;
	ActuationPoint actuationPoint;

	Sample lastSample;
	std::vector<std::function<void(Sample)>> callbacks;
};

