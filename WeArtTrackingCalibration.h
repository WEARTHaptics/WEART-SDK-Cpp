#pragma once
#include "WeArtMessageListener.h"
#include "WeArtCommon.h"
#include <functional>

//! @brief Calibration status/result observer
class WeArtTrackingCalibration : public WeArtMessageListener {
public:
	WeArtTrackingCalibration();

	HandSide getCurrentHand() {
		return currentHand;
	}

	CalibrationStatus getStatus() {
		return status;
	}

	bool getResult() {
		return result;
	}

	//! @brief Adds a callback for calibration status update
	//! @param callback Callback called on calibration status update
	void AddStatusCallback(std::function<void(HandSide, CalibrationStatus)> callback);

	//! @brief Adds a callback for calibration result
	//! @param callback Callback called when a calibration result is received
	void AddResultCallback(std::function<void(HandSide, bool)> callback);

private:
	void OnMessageReceived(WeArtMessage* message) override;

	HandSide currentHand;
	CalibrationStatus status;
	bool result;

	std::vector<std::function<void(HandSide, CalibrationStatus)>> statusCallbacks;
	std::vector<std::function<void(HandSide, bool)>> resultCallbacks;
};