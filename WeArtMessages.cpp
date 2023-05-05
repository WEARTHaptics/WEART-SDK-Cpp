#include "pch.h"
#include "WeArtMessages.h"

// Util / Conversion methods

HandSide StringToHandside(std::string& str) {
	if (str == "LEFT") {
		return HandSide::Left;
	} else if (str == "RIGHT") {
		return HandSide::Right;
	} else {
		assert(false);
		return HandSide::Left;
	}
}

std::string HandsideToString(HandSide hs) {
	if (hs == HandSide::Left) {
		return "LEFT";
	} else if (hs == HandSide::Right) {
		return "RIGHT";
	} else {
		assert(false);
		return "";
	}
}

std::string CalibrationHandSideToString(HandSide hs) {
	return hs == HandSide::Left ? "0" : "1";
}

HandSide StringToCalibrationHandSide(std::string& str) {
	if (str == "0")
		return HandSide::Left;
	else if (str == "1")
		return HandSide::Right;

	assert(false);
	return HandSide::Left;
}

ActuationPoint StringToActuationPoint(std::string& str) {
	if (str == "THUMB") {
		return ActuationPoint::Thumb;
	} else if (str == "INDEX") {
		return ActuationPoint::Index;
	} else if (str == "MIDDLE") {
		return ActuationPoint::Middle;
	} else if (str == "PALM") {
		return ActuationPoint::Palm;
	} else {
		assert(false);
		return ActuationPoint::Thumb;
	}
}

std::string ActuationPointToString(ActuationPoint ap) {
	if (ap == ActuationPoint::Thumb) {
		return "THUMB";
	} else if (ap == ActuationPoint::Index) {
		return "INDEX";
	} else if (ap == ActuationPoint::Middle) {
		return "MIDDLE";
	} else if (ap == ActuationPoint::Palm) {
		return "PALM";
	} else {
		assert(false);
		return "";
	}
}

TrackingType StringToTrackingType(const std::string& str) {
	if (str == "TrackType1")
		return TrackingType::WEART_HAND;
	return TrackingType::DEFAULT;
}

std::string TrackingTypeToString(TrackingType trackType) {
	switch (trackType) {
		case TrackingType::DEFAULT:
			return "";
		case TrackingType::WEART_HAND:
			return "TrackType1";
	}
	return "";
}

// StartFromClient message

std::vector<std::string> StartFromClientMessage::getValues() {
	std::vector<std::string> ret;
	if (_trackType != TrackingType::DEFAULT) {
		ret.push_back(WeArtConstants::WEART_SDK_TYPE);
		ret.push_back(WeArtConstants::WEART_SDK_VERSION);
		ret.push_back(TrackingTypeToString(_trackType));
	}
	return ret;
}

void StartFromClientMessage::setValues(std::vector<std::string>& values) {
	if (values.empty())
		_trackType = TrackingType::DEFAULT;
	else
		_trackType = StringToTrackingType(values[2]);
}

// Calibration Status

std::vector<std::string> CalibrationStatusMessage::getValues() {
	std::vector<std::string> ret;
	ret.push_back(CalibrationHandSideToString(handSide));
	ret.push_back(std::to_string((int)status));
	return ret;
}

void CalibrationStatusMessage::setValues(std::vector<std::string>& values) {
	assert(values.size() == 2);
	handSide = StringToCalibrationHandSide(values[0]);
	status = (CalibrationStatus)std::stoi(values[1]);
}

// Calibration Result
std::vector<std::string> CalibrationResultMessage::getValues() {
	std::vector<std::string> ret;
	ret.push_back(CalibrationHandSideToString(handSide));
	ret.push_back(success ? "0" : "1");
	return ret;
}

void CalibrationResultMessage::setValues(std::vector<std::string>& values) {
	assert(values.size() == 2);
	handSide = StringToCalibrationHandSide(values[0]);
	success = std::stoi(values[1]) == 0;
}

// Set Temperature

std::vector<std::string> SetTemperatureMessage::getValues() {
	std::vector<std::string> ret;
	ret.push_back(std::to_string(temperature));
	ret.push_back(HandsideToString(handSide));
	ret.push_back(ActuationPointToString(actuationPoint));
	return ret;
}

void SetTemperatureMessage::setValues(std::vector<std::string>& values) {
	assert(values.size() == 3);
	temperature = std::stof(values[0]);
	handSide = StringToHandside(values[1]);
	actuationPoint = StringToActuationPoint(values[2]);
};

// Stop temperature
std::vector<std::string> StopTemperatureMessage::getValues() {
	std::vector<std::string> ret;
	ret.push_back(HandsideToString(handSide));
	ret.push_back(ActuationPointToString(actuationPoint));
	return ret;
}

void StopTemperatureMessage::setValues(std::vector<std::string>& values) {
	assert(values.size() == 2);
	handSide = StringToHandside(values[0]);
	actuationPoint = StringToActuationPoint(values[1]);
};

// Set force

std::vector<std::string> SetForceMessage::getValues() {
	std::vector<std::string> ret;
	ret.push_back(std::to_string(force[0]));
	ret.push_back(std::to_string(force[1]));
	ret.push_back(std::to_string(force[2]));
	ret.push_back(HandsideToString(handSide));
	ret.push_back(ActuationPointToString(actuationPoint));
	return ret;
}

void SetForceMessage::setValues(std::vector<std::string>& values) {
	assert(values.size() == 5);
	force[0] = std::stof(values[0]);
	force[1] = std::stof(values[1]);
	force[2] = std::stof(values[2]);
	handSide = StringToHandside(values[3]);
	actuationPoint = StringToActuationPoint(values[4]);
};

// Stop force

std::vector<std::string> StopForceMessage::getValues() {
	std::vector<std::string> ret;
	ret.push_back(HandsideToString(handSide));
	ret.push_back(ActuationPointToString(actuationPoint));
	return ret;
}

void StopForceMessage::setValues(std::vector<std::string>& values) {
	assert(values.size() == 2);
	handSide = StringToHandside(values[0]);
	actuationPoint = StringToActuationPoint(values[1]);
};

// Set Texture

std::vector<std::string> SetTextureMessage::getValues() {
	std::vector<std::string> ret;

	// avoinding texture id out of bounds
	if (index < WeArtConstants::minTextureIndex || index > WeArtConstants::maxTextureIndex) {
		index = WeArtConstants::nullTextureIndex;
	}

	ret.push_back(std::to_string(index));
	ret.push_back(std::to_string(velocity[0]));
	ret.push_back(std::to_string(velocity[1]));
	ret.push_back(std::to_string(velocity[2]));
	ret.push_back(std::to_string(volume));
	ret.push_back(HandsideToString(handSide));
	ret.push_back(ActuationPointToString(actuationPoint));
	return ret;
}

void SetTextureMessage::setValues(std::vector<std::string>& values) {
	assert(values.size() == 6);
	index = std::stoi(values[0]);
	velocity[0] = std::stof(values[1]);
	velocity[1] = std::stof(values[2]);
	velocity[2] = std::stof(values[3]);
	volume = std::stof(values[4]);
	handSide = StringToHandside(values[5]);
	actuationPoint = StringToActuationPoint(values[6]);
};

// Stop texture

std::vector<std::string> StopTextureMessage::getValues() {
	std::vector<std::string> ret;
	ret.push_back(HandsideToString(handSide));
	ret.push_back(ActuationPointToString(actuationPoint));
	return ret;
}

void StopTextureMessage::setValues(std::vector<std::string>& values) {
	assert(values.size() == 2);
	handSide = StringToHandside(values[0]);
	actuationPoint = StringToActuationPoint(values[1]);
};

// Tracking

std::vector<std::string> TrackingMessage::getValues() {
	std::vector<std::string> ret;
	ret.push_back(TrackingTypeToString(_trackingType));
	switch (_trackingType) {
		case TrackingType::DEFAULT:
		{
			ret.push_back(std::to_string(RightThumbClosure));
			ret.push_back(std::to_string(RightIndexClosure));
			ret.push_back(std::to_string(RightMiddleClosure));
			ret.push_back(std::to_string(RightPalmClosure));
			ret.push_back(std::to_string(LeftThumbClosure));
			ret.push_back(std::to_string(LeftIndexClosure));
			ret.push_back(std::to_string(LeftMiddleClosure));
			ret.push_back(std::to_string(LeftPalmClosure));
			break;
		}
		case TrackingType::WEART_HAND:
		{
			// Right
			ret.push_back(std::to_string(RightIndexClosure));
			ret.push_back(std::to_string(RightThumbClosure));
			ret.push_back(std::to_string(RightThumbAbduction));
			ret.push_back(std::to_string(RightMiddleClosure));

			// Left
			ret.push_back(std::to_string(LeftIndexClosure));
			ret.push_back(std::to_string(LeftThumbClosure));
			ret.push_back(std::to_string(LeftThumbAbduction));
			ret.push_back(std::to_string(LeftMiddleClosure));
		}
	}

	return ret;
}

void TrackingMessage::setValues(std::vector<std::string>& values) {
	_trackingType = StringToTrackingType(values[0]);
	switch (_trackingType) {
		case TrackingType::DEFAULT:
		{
			assert(values.size() == 8);
			RightThumbClosure = std::stoi(values[0]);
			RightIndexClosure = std::stoi(values[1]);
			RightMiddleClosure = std::stoi(values[2]);
			RightPalmClosure = std::stoi(values[3]);
			LeftThumbClosure = std::stoi(values[4]);
			LeftIndexClosure = std::stoi(values[5]);
			LeftMiddleClosure = std::stoi(values[6]);
			LeftPalmClosure = std::stoi(values[7]);
			break;
		}
		case TrackingType::WEART_HAND:
		{
			assert(values.size() == 9);
			// Right
			RightIndexClosure = std::stoi(values[1]);
			RightThumbClosure = std::stoi(values[2]);
			RightThumbAbduction = std::stoi(values[3]);
			RightMiddleClosure = std::stoi(values[4]);

			// Left
			LeftIndexClosure = std::stoi(values[5]);
			LeftThumbClosure = std::stoi(values[6]);
			LeftThumbAbduction = std::stoi(values[7]);
			LeftMiddleClosure = std::stoi(values[8]);
		}
	}
}

float TrackingMessage::GetClosure(HandSide handSide, ActuationPoint actuationPoint) {
	uint8 byteValue = 0x00;
	switch (handSide) {
		case HandSide::Left:
			switch (actuationPoint) {
				case ActuationPoint::Thumb:  byteValue = LeftThumbClosure;  break;
				case ActuationPoint::Index:  byteValue = LeftIndexClosure;  break;
				case ActuationPoint::Middle: byteValue = LeftMiddleClosure; break;
				case ActuationPoint::Palm:   byteValue = LeftPalmClosure;   break;
			}
			break;
		case HandSide::Right:
			switch (actuationPoint) {
				case ActuationPoint::Thumb:  byteValue = RightThumbClosure;  break;
				case ActuationPoint::Index:  byteValue = RightIndexClosure;  break;
				case ActuationPoint::Middle: byteValue = RightMiddleClosure; break;
				case ActuationPoint::Palm:   byteValue = RightPalmClosure;   break;
			}
			break;
	}

	float num(byteValue);
	float denom(255);
	float closure = num / denom;

	return closure;
}

float TrackingMessage::GetAbduction(HandSide handSide, ActuationPoint actuationPoint) {
	float maxAbductionValue = 255;
	switch (handSide) {
		case HandSide::Left:
			if (actuationPoint == ActuationPoint::Thumb)  return ((float)LeftThumbAbduction) / maxAbductionValue;
			break;
		case HandSide::Right:
			if (actuationPoint == ActuationPoint::Thumb)  return ((float)RightThumbAbduction) / maxAbductionValue;
			break;
	}
	return WeArtConstants::defaultAbduction;
}
