/**
*	WEART - Raw Sensor Data thimble TD
*	https://www.weart.it/
*/

#include "pch.h"
#include "WeArtTrackingRawData.h"
#include "WeArtController.h"

WeArtTrackingRawData::WeArtTrackingRawData(HandSide side, ActuationPoint actuation)
	: WeArtMessageListener({ RawSensorsData::ID, RawSensorsDataTDPro::ID }) {
	handSide = side;
	actuationPoint = actuation;
}

WeArtTrackingRawData::Sample WeArtTrackingRawData::GetLastSample() {
	if (samples.size() == 0)
		return WeArtTrackingRawData::Sample();
	return samples.back();
}

void WeArtTrackingRawData::AddSampleCallback(std::function<void(Sample)> callback) {
	callbacks.push_back(callback);
}

void WeArtTrackingRawData::OnMessageReceived(WeArtMessage* msg) {
	
	Sample sample;

	if(msg->getID() == RawSensorsData::ID) {
		
		RawSensorsData* rawSensorsData = static_cast<RawSensorsData*>(msg);
		
		if (rawSensorsData == nullptr)
		return;

		if (rawSensorsData->getHand() != handSide)
			return;

		if (!rawSensorsData->hasSensor(actuationPoint))
			return;
		
		sample.timestamp = rawSensorsData->timestamp();
		sample.data = rawSensorsData->getSensor(actuationPoint);

		samples.push(sample);
		while (samples.size() > K_NUM_SAMPLES)
			samples.pop();

	}
	else if (msg->getID() == RawSensorsDataTDPro::ID) {
		
		RawSensorsDataTDPro* rawSensorsDataTDPro = static_cast<RawSensorsDataTDPro*>(msg);
		
		if (rawSensorsDataTDPro == nullptr)
		return;

		if (rawSensorsDataTDPro->getHand() != handSide)
			return;

		if (!rawSensorsDataTDPro->hasSensor(actuationPoint))
			return;
		
		sample.timestamp = rawSensorsDataTDPro->timestamp();
		sample.data = rawSensorsDataTDPro->getSensor(actuationPoint);

		samples.push(sample);
		while (samples.size() > K_NUM_SAMPLES)
			samples.pop();
		
	}
	else
	{
		return;
	}
	
	for (std::function<void(Sample)> callback : callbacks)
		callback(sample);
}
