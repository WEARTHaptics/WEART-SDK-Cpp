/**
*	WEART - Message composer
*	https://www.weart.it/
*/
#pragma once

#include "WeArtCommon.h"
#include <string>
#include <vector>
#include <cassert>

// Utility Methods, called to serialize/deserialize types
HandSide StringToHandside(std::string& str);

std::string HandsideToString(HandSide hs);

std::string CalibrationHandSideToString(HandSide hs);

HandSide StringToCalibrationHandSide(std::string& str);

ActuationPoint StringToActuationPoint(std::string& str);

std::string ActuationPointToString(ActuationPoint ap);

TrackingType StringToTrackingType(const std::string& str);

std::string TrackingTypeToString(TrackingType trackType);

//! @brief Generic Weart message
class WeArtMessage {
public:
	//! @brief Allows to get the message ID, used to deserialize the correct message type
	//! @return the message ID
	virtual std::string getID() = 0;
	virtual std::vector<std::string> getValues() = 0;
	virtual void setValues(std::vector<std::string>& values) = 0;

	//! @brief Sets the hand to which the message is applied (if needed)
	//! @param hs Hand to which the message is applied
	virtual void setHandSide(HandSide hs) = 0;

	//! @brief Sets the actuation point to which the message is applied (if needed)
	//! @param ap Actuation point to which the message is applied
	virtual void setActuationPoint(ActuationPoint ap) = 0;
};


//! @brief Message without handside or actuation point parameters
class WeArtMessageNoParams : public WeArtMessage {
public:
	virtual void setHandSide(HandSide hs) override {};
	virtual void setActuationPoint(ActuationPoint ap) override {};

	// This message carries no values.
	virtual std::vector<std::string> getValues() override {
		return std::vector<std::string>();
	};
	virtual void setValues(std::vector<std::string>& values) override {};
};

//! @brief Message related to a given handside
class WeArtMessageHandSpecific : public WeArtMessage {
public:
	HandSide getHand() {
		return handSide;
	}
protected:
	HandSide handSide;
public:
	virtual void setHandSide(HandSide hs) override {
		handSide = hs;
	};
	virtual void setActuationPoint(ActuationPoint ap) override {};
};

//! @brief Message related to a given hand and actuation point
class WeArtMessageObjectSpecific : public WeArtMessage {
protected:
	HandSide handSide;
	ActuationPoint actuationPoint;
public:
	virtual void setHandSide(HandSide hs) override {
		handSide = hs;
	};
	virtual void setActuationPoint(ActuationPoint ap) override {
		actuationPoint = ap;
	};
};


class StartFromClientMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "StartFromClient";

	StartFromClientMessage(TrackingType trackType = TrackingType::WEART_HAND) : _trackType(trackType) {}

	virtual std::string getID() override {
		return ID;
	};

	virtual void setHandSide(HandSide hs) override {};
	virtual void setActuationPoint(ActuationPoint ap) override {};

	virtual std::vector<std::string> getValues() override;
	virtual void setValues(std::vector<std::string>& values) override;
private:
	TrackingType _trackType;
};

class StopFromClientMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "StopFromClient";

	virtual std::string getID() override {
		return ID;
	};
};

class StartCalibrationMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "StartCalibration";

	virtual std::string getID() override {
		return ID;
	}
};

class StopCalibrationMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "StopCalibration";

	virtual std::string getID() override {
		return ID;
	}
};

class CalibrationStatusMessage : public WeArtMessageHandSpecific {
public:
	static constexpr const char* ID = "CalibrationStatus";

	virtual std::string getID() override {
		return ID;
	}

	CalibrationStatus getStatus() {
		return status;
	}

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;

protected:
	CalibrationStatus status;
};

class CalibrationResultMessage : public WeArtMessageHandSpecific {
public:
	static constexpr const char* ID = "CalibrationResult";

	virtual std::string getID() override {
		return ID;
	}

	bool getSuccess() {
		return success;
	}

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;

protected:
	bool success;
};


class ExitMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "exit";

	virtual std::string getID() override {
		return std::string("exit");
	};
};

class DisconnectMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "disconnect";

	virtual std::string getID() override {
		return ID;
	};
};

class SetTemperatureMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "temperature";

	SetTemperatureMessage(const float t) : temperature(t) {};
	SetTemperatureMessage() {};
	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;

protected:
	float temperature;
};

class StopTemperatureMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "stopTemperature";

	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;
};

class SetForceMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "force";

	SetForceMessage(const float f[3]) : force{ f[0], f[1], f[2] } {};
	SetForceMessage() {};
	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;

protected:
	float force[3];
};

class StopForceMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "stopForce";

	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;
};

class SetTextureMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "texture";

	SetTextureMessage(const int idx, const float vel[3], const float vol) : index(idx), velocity{ vel[0], vel[1], vel[2] }, volume(vol) {};
	SetTextureMessage() {};
	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;

protected:
	int index;
	float velocity[3];
	float volume;
};

class StopTextureMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "stopTexture";

	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;
};

//! @brief Generic Tracking message, contains information on closure and abduction (based on tracking type)
//! 
//! Message containing the closure and abduction values of the different actuation points (thimbles and palm)
//! The abduction values available depends on the tracking type selected when the connection started.
//! 
//! In particular:
//! * DEFAULT: no abduction values (deprecated)
//! * CLAP_HAND: 3 angles (X,Y,Z) for the thumb, and a single abduction value for index and middle
//! * WEART_HAND: single abduction value only for the thumb
//! 
class TrackingMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "Tracking";

	virtual std::string getID() override {
		return std::string(ID);
	};

	struct Angles {
		float X;
		float Y;
		float Z;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;

	TrackingType GetType() {
		return _trackingType;
	}

	//! @brief Getter for abduction value (based on tracking type and given point)
	//! @param handSide			Hand from which to get the value
	//! @param actuationPoint	Actuation Point from which to get the value
	//! @return the requested abduction value
	float GetAbduction(HandSide handSide, ActuationPoint actuationPoint);

	//! @brief Getter for closure value
	//! @param handSide			Hand from which to get the value
	//! @param actuationPoint	Actuation Point from which to get the value
	//! @return the requested closure value (from 0 to 1)
	float GetClosure(HandSide handSide, ActuationPoint actuationPoint);
private:
	TrackingType _trackingType;

	// Closures
	uint8 RightThumbClosure;
	uint8 RightIndexClosure;
	uint8 RightMiddleClosure;
	uint8 RightPalmClosure;
	uint8 LeftThumbClosure;
	uint8 LeftIndexClosure;
	uint8 LeftMiddleClosure;
	uint8 LeftPalmClosure;

	// Abductions
	uint8 RightThumbAbduction;
	uint8 LeftThumbAbduction;
};


class RawSensorsData : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "SensorsData";

	int handSide;
	int actuationPoint;
	float accX;
	float accY;
	float accZ;
	float gyroX;
	float gyroY;
	float gyroZ;
	int TOF;

	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;

	HandSide GetHandSide();

	ActuationPoint GetActuationPoint();
};