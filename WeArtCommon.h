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
	Thumb = 1 << 0,
	Index = 1 << 1,
	Middle = 1 << 2,
	Annular = 1 << 3,
	Pinky = 1 << 4,
	Palm = 1 << 5
};
NLOHMANN_JSON_SERIALIZE_ENUM(ActuationPoint, {
	{ActuationPoint::Thumb, "THUMB"},
	{ActuationPoint::Index, "INDEX"},
	{ActuationPoint::Middle, "MIDDLE"},
	{ActuationPoint::Annular, "ANNULAR"},
	{ActuationPoint::Pinky, "PINKY"},
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

//! @brief Accelerometer data
struct AccelerometerData {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AccelerometerData, x, y, z)

//! @brief Gyroscope data
struct GyroscopeData {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GyroscopeData, x,y,z)

//! @brief Data from theTime-of-Flight sensor
struct TofData {
	int distance = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TofData, distance)

//! @brief Sensor data from the TouchDIVER device
struct SensorData {
	AccelerometerData accelerometer;
	GyroscopeData gyroscope;
	TofData timeOfFlight;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SensorData, accelerometer, gyroscope, timeOfFlight)

//! @brief Sensor data from the TouchDIVER Pro device
struct SensorDataG2 {
	AccelerometerData accelerometer;
	GyroscopeData gyroscope;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SensorDataG2, accelerometer, gyroscope)

//! @brief Analog sensor data from the TouchDIVER device
struct AnalogSensorRawData {
	float ntcTemperatureRaw;
	float ntcTemperatureConverted;
	float forceSensingRaw;
	float forceSensingConverted;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnalogSensorRawData, ntcTemperatureRaw, ntcTemperatureConverted, forceSensingRaw, forceSensingConverted)

//! @brief Status of a connected TouchDIVER device. This is a short info version of @ref ConnectedDeviceStatus or @ref ConnectedG2DeviceStatus
struct MiddlewareConnectedDevice {
	std::string macAddress;
	HandSide handSide;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MiddlewareConnectedDevice, macAddress, handSide)

//! @brief Middleware status
struct MiddlewareStatusData {
	std::uint64_t timestamp;
	MiddlewareStatus status;	//!< Middleware status
	std::string version;		//!< Middleware version
	int statusCode;				//!< Middleware status code (error)
	std::string errorDesc;		//!< Error description. This field is related to @ref statusCode
	int warningCode;			//!< Middleware warning code @note Only available for TouchDIVER Pro
	std::string warningDesc;	//!< Warning description. This field is related to @ref warningCode @note Only available for TouchDIVER Pro
	bool actuationsEnabled;		//!< Whether actuations are enabled or not
	std::string connectionType;	//!< Connection type (e.g. "BLE", "USB") @note Only available for TouchDIVER Pro
	bool autoconnection;		//!< Whether the autoconnection is enabled or not @note Only available for TouchDIVER Pro
	bool trackingPlayback;		//! @note Only available for TouchDIVER Pro
	bool rawDataLog;			//! @note Only available for TouchDIVER Pro
	bool sensorOnMask;			//! @note Only available for TouchDIVER Pro
	std::vector<MiddlewareConnectedDevice> connectedDevices; //!< Listh of connected devices with their short infos.
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MiddlewareStatusData, status, version, statusCode, errorDesc, warningCode, warningDesc, actuationsEnabled, connectedDevices);

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

//! @brief Status of a single node connected to a device
struct NodeStatus {
	//! @brief Actuation Point to which the thimble is assigned
	ActuationPoint id;
	//! @brief Tells whether the thimble is connected to the device or not
	bool connected;
	//! @brief Whether the node IMU is in fault state or not
	bool imuFault;
	//! @brief Whether the node ADC is in fault state or not
	bool adcFault;
	//! @brief Whether the node TOF is in fault state or not
	bool tofFault;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NodeStatus, id, connected, imuFault, adcFault, tofFault);

//! @brief Connection status of the device
struct ConnectionStatus {
	//! Whether the bluetooth is on or not
	bool bluetoothOn;
	//! Whether the bluetooth is connected or not
	bool bluetoothConnected;
	//! Whether the wifi is on or not
	bool wifiOn;
	//! Whether the wifi is connected or not
	bool wifiConnected;
	//! Whether the usb is connected or not
	bool usbConnected;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConnectionStatus, bluetoothOn, bluetoothConnected, wifiOn, wifiConnected, usbConnected);

//! @brief Status of the master
struct MasterStatus {
	//! @brief The battery level
	int batteryLevel;
	//! @brief Tells whether the device is under charge (true) or not (false)
	bool charging;
	//! @brief Tells whether the device has finished the charge or not
	bool chargeCompleted;
	//! @brief The connection status of the device
	ConnectionStatus connection;
	//! @brief Whether the node IMU is in fault state or not
	bool imuFault;
	//! @brief Whether the node ADC is in fault state or not
	bool adcFault;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MasterStatus, batteryLevel, charging, chargeCompleted, connection, imuFault, adcFault);

//! @brief Status of a connected TouchDIVER device
//! @note This is only available for TouchDIVER. For TouchDIVER Pro devices see @ref ConnectedG2DeviceStatus
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

//! @brief Status of a connected TouchDIVER Pro device
//! @note This is only available for TouchDIVER Pro. For TouchDIVER devices see @ref ConnectedDeviceStatus
struct ConnectedG2DeviceStatus {
	//! @brief Device BLE mac address
	std::string macAddress;

	//! @brief Hand to which the device is assigned
	HandSide handSide;

	//! @brief 	RSSI signal strength in dBm (mean of last 3 samples)
	double signalStrength;

	//! @brief Master status
	MasterStatus master;

	//! @brief Status of the device thimbles
	std::vector<NodeStatus> nodes;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ConnectedG2DeviceStatus, macAddress, handSide, signalStrength, master, nodes);

// Constants shared by the WeArt components
namespace WeArtConstants {
	const PCSTR DEFAULT_IP_ADDRESS = "127.0.0.1";
	const PCSTR DEFAULT_TCP_PORT = "13031";

	const std::string WEART_SDK_VERSION = "2.0.0";
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
