/**
*	WEART - Raw Analog Sensor Data thimble TD
*	https://www.weart.it/
*/

#include "pch.h"
#include "WeArtAnalogSensorData.h"
#include "WeArtController.h"

WeArtAnalogSensorData::WeArtAnalogSensorData(HandSide side, ActuationPoint actuation)
	: WeArtMessageListener({ AnalogSensorsData::ID }) {
	handSide = side;
	actuationPoint = actuation;
}

WeArtAnalogSensorData::Sample WeArtAnalogSensorData::GetLastSample() {
	return lastSample;
}

void WeArtAnalogSensorData::AddSampleCallback(std::function<void(Sample)> callback) {
	callbacks.push_back(callback);
}

void WeArtAnalogSensorData::OnMessageReceived(WeArtMessage* msg) {
	AnalogSensorsData* rawSensorsData = static_cast<AnalogSensorsData*>(msg);
	if (rawSensorsData == nullptr)
		return;

	if (rawSensorsData->getHand() != handSide)
		return;

	if (!rawSensorsData->hasSensor(actuationPoint))
		return;

	Sample sample;
	sample.timestamp = rawSensorsData->timestamp();
	sample.data = rawSensorsData->getSensor(actuationPoint);

	lastSample = sample;

	for (std::function<void(Sample)> callback : callbacks)
		callback(sample);
}
