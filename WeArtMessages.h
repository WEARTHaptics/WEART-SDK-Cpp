/// @private
#pragma once

#include "WeArtCommon.h"
#include <string>
#include <vector>
#include <cassert>
#include <cstdint>
#include <map>
#include "nlohmann/json.hpp"

// Utility Methods, called to serialize/deserialize types
HandSide StringToHandside(std::string& str);

std::string HandsideToString(HandSide hs);

std::string CalibrationHandSideToString(HandSide hs);

HandSide StringToCalibrationHandSide(std::string& str);

ActuationPoint StringToActuationPoint(std::string& str);

std::string ActuationPointToString(ActuationPoint ap);

TrackingType StringToTrackingType(const std::string& str);

std::string TrackingTypeToString(TrackingType trackType);

std::string MiddlewareStatusToString(MiddlewareStatus status);

//! @brief Generic Weart message
//! @private
class WeArtMessage {
public:
	//! @brief Allows to get the message ID, used to deserialize the correct message type
	//! @return the message ID
	virtual std::string getID() = 0;
	
	//! @brief Sets the hand to which the message is applied (if needed)
	//! @param hs Hand to which the message is applied
	virtual void setHandSide(HandSide hs) = 0;

	//! @brief Sets the actuation point to which the message is applied (if needed)
	//! @param ap Actuation point to which the message is applied
	virtual void setActuationPoint(ActuationPoint ap) = 0;

	virtual std::string serialize() = 0;
	virtual void deserialize(std::string message) = 0;
};

//! @private
class WeArtCsvMessage : public WeArtMessage {
public:
	const char field_separator = ':';

	virtual std::vector<std::string> getValues() = 0;
	virtual void setValues(std::vector<std::string>& values) = 0;

	virtual std::string serialize() override;
	virtual void deserialize(std::string message) override;
};

//! @private
class WeArtJsonMessage : public WeArtMessage {
public:
	WeArtJsonMessage();

	virtual std::string serialize() override;
	virtual void deserialize(std::string message) override;
	std::uint64_t timestamp() { return _timestamp; }

	virtual void setHandSide(HandSide hs) override {};
	virtual void setActuationPoint(ActuationPoint ap) override {};

protected:
	virtual nlohmann::json serializePayload() { return nlohmann::json(); }
	virtual void deserializePayload(nlohmann::json payload) {}
	std::uint64_t _timestamp;
};


//! @brief Message without handside or actuation point parameters
//! @private
class WeArtMessageNoParams : public WeArtCsvMessage {
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
//! @private
class WeArtMessageHandSpecific : public WeArtCsvMessage {
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
//! @private
class WeArtMessageObjectSpecific : public WeArtCsvMessage {
public:
	HandSide getHand() {
		return handSide;
	}

	virtual void setHandSide(HandSide hs) override {
		handSide = hs;
	};

	ActuationPoint getActuationPoint() {
		return actuationPoint;
	}

	virtual void setActuationPoint(ActuationPoint ap) override {
		actuationPoint = ap;
	};


protected:
	HandSide handSide;
	ActuationPoint actuationPoint;
};


//! @private
class StartFromClientMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "StartFromClient";

	StartFromClientMessage(TrackingType trackType = TrackingType::WEART_HAND) : _trackType(trackType) {}

	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;
	virtual void setValues(std::vector<std::string>& values) override;
private:
	TrackingType _trackType;
};

//! @private
class StopFromClientMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "StopFromClient";

	virtual std::string getID() override {
		return ID;
	};
};

//! @private
class StartCalibrationMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "StartCalibration";

	virtual std::string getID() override {
		return ID;
	}
};

//! @private
class StopCalibrationMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "StopCalibration";

	virtual std::string getID() override {
		return ID;
	}
};

//! @private
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

//! @private
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

//! @private
class ExitMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "exit";

	virtual std::string getID() override {
		return std::string("exit");
	};
};

//! @private
class DisconnectMessage : public WeArtMessageNoParams {
public:
	static constexpr const char* ID = "disconnect";

	virtual std::string getID() override {
		return ID;
	};
};

//! @private
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

//! @private
class StopTemperatureMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "stopTemperature";

	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;
};

//! @private
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

//! @private
class StopForceMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "stopForce";

	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;
};

//! @private
class SetTextureMessage : public WeArtMessageObjectSpecific {
public:
	static constexpr const char* ID = "texture";

	SetTextureMessage(const int idx, const float vel, const float vol) : index(idx), velocity{ 0.5f, 0.0f, vel }, volume(vol) {};
	SetTextureMessage() {};
	virtual std::string getID() override {
		return ID;
	};

	virtual std::vector<std::string> getValues() override;

	virtual void setValues(std::vector<std::string>& values) override;

protected:
	int index;
	float velocity[3] = { 0.5f, 0.0f, 0.0f };
	float volume;
};

//! @private
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
//! @private 
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
	uint8_t RightThumbClosure;
	uint8_t RightIndexClosure;
	uint8_t RightMiddleClosure;
	uint8_t RightPalmClosure;
	uint8_t LeftThumbClosure;
	uint8_t LeftIndexClosure;
	uint8_t LeftMiddleClosure;
	uint8_t LeftPalmClosure;

	// Abductions
	uint8_t RightThumbAbduction;
	uint8_t LeftThumbAbduction;
};

//! @private
class RawDataOn : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "RAW_DATA_ON";

	virtual std::string getID() override { return ID; }
	virtual void setHandSide(HandSide hs) override {}
	virtual void setActuationPoint(ActuationPoint ap) override {}
};

//! @private
class RawDataOff : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "RAW_DATA_OFF";

	virtual std::string getID() override { return ID; }
	virtual void setHandSide(HandSide hs) override {}
	virtual void setActuationPoint(ActuationPoint ap) override {}
};

//! @private
class RawSensorsData : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "RAW_DATA";

	virtual std::string getID() override { return ID; };
	virtual void setHandSide(HandSide hs) override { hand = hs; }
	virtual void setActuationPoint(ActuationPoint ap) override {}

	HandSide getHand() { return hand; }
	bool hasSensor(ActuationPoint ap);
	SensorData getSensor(ActuationPoint ap);

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

	HandSide hand;
	std::map<ActuationPoint, SensorData> sensors;

};

//! @private
class AnalogSensorsData : public WeArtJsonMessage {
public:
	static constexpr const char* ID = "RAW_SENSOR_ON_MASK";

	virtual std::string getID() override { return ID; };
	virtual void setHandSide(HandSide hs) override { hand = hs; }
	virtual void setActuationPoint(ActuationPoint ap) override {}

	HandSide getHand() { return hand; }
	bool hasSensor(ActuationPoint ap);
	AnalogSensorRawData getSensor(ActuationPoint ap);

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

	HandSide hand;
	std::map<ActuationPoint, AnalogSensorRawData> sensors;
};

//!@private
class GetMiddlewareStatus : public WeArtJsonMessage {
public:
	GetMiddlewareStatus() : WeArtJsonMessage() {}

	static constexpr const char* ID = "MW_GET_STATUS";
	virtual std::string getID() override { return ID; };
};

//!@private
class MiddlewareStatusMessage : public WeArtJsonMessage {
public:
	MiddlewareStatusMessage() : WeArtJsonMessage() {}

	static constexpr const char* ID = "MW_STATUS";
	virtual std::string getID() override { return ID; };

	MiddlewareStatusData data() { return _data; }

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

private:
	MiddlewareStatusData _data;	
};

//! @private 
class GetDevicesStatusMessage : public WeArtJsonMessage {
public:
	GetDevicesStatusMessage() : WeArtJsonMessage() {}
	static constexpr const char* ID = "DEVICES_GET_STATUS";
	virtual std::string getID() override { return ID; };
};

//! @private 
class DevicesStatusMessage : public WeArtJsonMessage {
public:
	DevicesStatusMessage() : WeArtJsonMessage() {}

	static constexpr const char* ID = "DEVICES_STATUS";
	virtual std::string getID() override { return ID; };

	std::vector<ConnectedDeviceStatus> devices() { return _devices; }

protected:
	virtual nlohmann::json serializePayload() override;
	virtual void deserializePayload(nlohmann::json payload) override;

private:
	std::vector<ConnectedDeviceStatus> _devices;
};