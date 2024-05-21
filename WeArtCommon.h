//! @brief Collection of constants used throughout the SDK

#pragma once
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <string>
#include "nlohmann/json.hpp"

enum class TrackingType {
	DEFAULT,	//!< Deprecated, contains only closure values 
	WEART_HAND,	//!< Tracking with closures, and abduction value for thumb
};

enum class HandSide {
	//HSnone = 0	
	Left = 1 << 0,
	Right = 1 << 1,
};
NLOHMANN_JSON_SERIALIZE_ENUM(HandSide, {
	{HandSide::Left, "LEFT"},
	{HandSide::Right, "RIGHT"},
})


enum class ActuationPoint {
	//APnone	= 0
	Thumb = 1 << 0,
	Index = 1 << 1,
	Middle = 1 << 2,
	Palm = 1 << 3,
};
NLOHMANN_JSON_SERIALIZE_ENUM(ActuationPoint, {
	{ActuationPoint::Thumb, "THUMB"},
	{ActuationPoint::Index, "INDEX"},
	{ActuationPoint::Middle, "MIDDLE"},
	{ActuationPoint::Palm, "PALM"},
})


enum HandClosingState {
	Open = 0,
	Closing = 1,
	Closed = 2
};


enum GraspingState {
	Grabbed = 0,
	Released = 1
};

enum CalibrationStatus {
	IDLE = 0,
	Calibrating = 1,
	Running = 2,
};

enum class MiddlewareStatus {
	DISCONNECTED,
	IDLE,
	STARTING,
	RUNNING,
	STOPPING,
	UPLOADING_TEXTURES,
	CONNECTING_DEVICE,
	CALIBRATION,
};
NLOHMANN_JSON_SERIALIZE_ENUM(MiddlewareStatus, {
	{MiddlewareStatus::DISCONNECTED, "DISCONNECTED"},
	{MiddlewareStatus::IDLE, "IDLE"},
	{MiddlewareStatus::STARTING, "STARTING"},
	{MiddlewareStatus::RUNNING, "RUNNING"},
	{MiddlewareStatus::STOPPING, "STOPPING"},
	{MiddlewareStatus::UPLOADING_TEXTURES, "UPLOADING_TEXTURES"},
	{MiddlewareStatus::CONNECTING_DEVICE, "CONNECTING_DEVICE"},
	{MiddlewareStatus::CALIBRATION, "CALIBRATION"},
})


enum class TextureType : uint8_t {
	ClickNormal = 0, ClickSoft = 1, DoubleClick = 2,
	AluminiumFineMeshSlow = 3, AluminiumFineMeshFast = 4,
	PlasticMeshSlow = 5, ProfiledAluminiumMeshMedium = 6, ProfiledAluminiumMeshFast = 7,
	RhombAluminiumMeshMedium = 8,
	TextileMeshMedium = 9,
	CrushedRock = 10,
	VenetianGranite = 11,
	SilverOak = 12,
	LaminatedWood = 13,
	ProfiledRubberSlow = 14,
	VelcroHooks = 15,
	VelcroLoops = 16,
	PlasticFoil2 = 17,
	Leather = 18,
	Cotton1 = 19,
	Aluminium = 20,
	DoubleSidedTape = 21
};


struct AccelerometerData {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AccelerometerData, x, y, z)

struct GyroscopeData {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GyroscopeData, x,y,z)

struct TofData {
	int distance = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TofData, distance)

struct SensorData {
	AccelerometerData accelerometer;
	GyroscopeData gyroscope;
	TofData timeOfFlight;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SensorData, accelerometer, gyroscope, timeOfFlight)

struct AnalogSensorRawData {
	float ntcTemperatureRaw;
	float ntcTemperatureConverted;
	float forceSensingRaw;
	float forceSensingConverted;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnalogSensorRawData, ntcTemperatureRaw, ntcTemperatureConverted, forceSensingRaw, forceSensingConverted)

struct MiddlewareConnectedDevice {
	std::string macAddress;
	HandSide handSide;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MiddlewareConnectedDevice, macAddress, handSide)

struct MiddlewareStatusData {
	std::uint64_t timestamp;
	MiddlewareStatus status;
	std::string version;
	int statusCode;
	std::string errorDesc;
	bool actuationsEnabled;
	std::vector<MiddlewareConnectedDevice> connectedDevices;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MiddlewareStatusData, status, version, statusCode, errorDesc, actuationsEnabled, connectedDevices);

//! @brief Status of a single thimble connected to a device
struct ThimbleStatus {
	//! @brief Actuation Point to which the thimble is assigned
	ActuationPoint id;
	//! @brief Tells whether the thimble is connected to the device or not
	bool connected;
	//! @brief Current status code of the thimble (0 = OK)
	int statusCode;
	//! @brief Description of the thimble status code
	std::string errorDesc;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ThimbleStatus, id, connected, statusCode, errorDesc);

//! @brief Status of a connected TouchDIVER device
struct ConnectedDeviceStatus {
	//! @brief Device BLE mac address
	std::string macAddress;

	//! @brief Hand to which the device is assigned
	HandSide handSide;

	//! @brief Battery charge level (from 0 to 100)
	int batteryLevel;

	//! @brief Tells whether the device is under charge (true) or not (false)
	bool charging;

	//! @brief Status of the device thimbles
	std::vector<ThimbleStatus> thimbles;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConnectedDeviceStatus, macAddress, handSide, batteryLevel, charging, thimbles);

// Constants shared by the WeArt components
namespace WeArtConstants {
	const PCSTR DEFAULT_IP_ADDRESS = "127.0.0.1";
	const PCSTR DEFAULT_TCP_PORT = "13031";

	const std::string WEART_SDK_VERSION = "1.0.0";
	const std::string WEART_SDK_TYPE = "SdkLLCPP";

	const float defaultTemperature = 0.5f;
	const float minTemperature = 0.0f;
	const float maxTemperature = 1.0f;

	const float defaultForce = 0.0f;
	const float minForce = 0.0f;
	const float maxForce = 1.0f;

	const float defaultClosure = 0.0f;
	const float minClosure = 0.0f;
	const float maxClosure = 1.0f;

	const float defaultAbduction = 0.442f;

	const int defaultTextureIndex = 0;
	const int minTextureIndex = 0;
	const int maxTextureIndex = 21;
	const int nullTextureIndex = 255;

	const float defaultCollisionMultiplier = 20.0f;
	const float minCollisionMultiplier = 0.0f;
	const float maxCollisionMultiplier = 100.0f;

	const float defaultVolumeTexture = 100.0f;
	const float minVolumeTexture = 0.0f;
	const float maxVolumeTexture = 100.0f;

	const float thresholdThumbClosure = 0.5f;
	const float thresholdIndexClosure = 0.5f;
	const float thresholdMiddleClosure = 0.5f;

	const float defaultGraspForce = 0.3f;
	const float dynamicForceSensibility = 10.0f;
};
