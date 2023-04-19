#pragma once
#include "WeArtMessageListener.h"
#include "WeArtCommon.h"
#include <functional>

//! @brief Calibration status/result observer
class WeArtTrackingCalibration : public WeArtMessageListener {
public:
	WeArtTrackingCalibration();

	HandSide getCurrentHand() { return currentHand; }
	CalibrationStatus getStatus() { return status; }
	bool getResult() { return result; }

	void AddStatusCallback(std::function<void(HandSide, CalibrationStatus)> callback);
	void AddResultCallback(std::function<void(HandSide, bool)> callback);

private:
	void OnMessageReceived(WeArtMessage* message) override;

	HandSide currentHand;
	CalibrationStatus status;
	bool result;

	std::vector<std::function<void(HandSide, CalibrationStatus)>> statusCallbacks;
	std::vector<std::function<void(HandSide, bool)>> resultCallbacks;
};