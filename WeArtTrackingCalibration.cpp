#include "pch.h"
#include "WeArtTrackingCalibration.h"

WeArtTrackingCalibration::WeArtTrackingCalibration()
	: WeArtMessageListener({ CalibrationStatusMessage::ID, CalibrationResultMessage::ID }) {}

void WeArtTrackingCalibration::AddStatusCallback(std::function<void(HandSide, CalibrationStatus)> callback) {
	statusCallbacks.push_back(callback);
}

void WeArtTrackingCalibration::AddResultCallback(std::function<void(HandSide, bool)> callback) {
	resultCallbacks.push_back(callback);
}

void WeArtTrackingCalibration::OnMessageReceived(WeArtMessage* msg) {
	if (msg->getID() == CalibrationStatusMessage::ID) {
		CalibrationStatusMessage* message = static_cast<CalibrationStatusMessage*>(msg);
		currentHand = message->getHand();
		status = message->getStatus();
		for (auto callback : statusCallbacks)
			callback(currentHand, status);

	} else if (msg->getID() == CalibrationResultMessage::ID) {
		CalibrationResultMessage* message = static_cast<CalibrationResultMessage*>(msg);
		currentHand = message->getHand();
		result = message->getSuccess();
		for (auto callback : resultCallbacks)
			callback(currentHand, result);
	}
}
