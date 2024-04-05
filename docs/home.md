# Low-Level C++ SDK

Welcome to the Weart Low-Level C++ SDK documentation.

The SDK allows to connect to the Weart middleware and perform various actions with the TouchDIVER devices:
* Start and Stop the middleware operations
* Calibrate the device
* Receive tracking data from the devices
* Receive raw data from the thimble's motion sensors 
* Receive analog raw data from the thimble's senosrs
* Send haptic effects to the devices

# Architecture
<img src="./architecture.svg" width="100%" alt="C++ SDK Architecture" />

## Links

* Github source code repository, go [here](https://github.com/WEARTHaptics/WEART-SDK-Cpp)
* An example source code application is available [here](https://github.com/WEARTHaptics/WEART-SDK-Cpp-Example).

# Setup
The minimum setup to use the weart SDK consists of:
* A PC with the Middleware installed
* A TouchDIVER device
* A C++ project using the Low-Level SDK 

The SDK can be downloaded as a zip file containing all the necessary files.
To use it in your C++ project, unzip it and move the files in a folder inside your project.
Then, add the folder with the sdk files to the project's include path, and the .cpp files to your project's sources.
In Visual Studio, this can be done by including the sdk directory into the project (Right Click on Solution -> Add -> Existing Directory).

To start using the SDK in your project, start the Middleware application and connect a TouchDIVER device to it.
Then, create a header file and Include the following headers:
~~~~~~~~~~~~~{.cpp}
	#include <WEART_SDK/WeArtCommon.h>
	#include <WEART_SDK/WeArtClient.h>
	#include <WEART_SDK/WeArtHapticObject.h>
	#include <WEART_SDK/WeArtThimbleTrackingObject.h>
	#include <WEART_SDK/WeArtTrackingCalibration.h>
	#include <WEART_SDK/MiddlewareStatusListener.h>
~~~~~~~~~~~~~

Finally, create the WeArtClient and start the communication with the middleware:

~~~~~~~~~~~~~{.cpp}
	// Instantiate SDK client object
	WeArtClient* weArtClient = new WeArtClient("127.0.0.1", WeArtConstants::DEFAULT_TCP_PORT);

	// Run communication thread
	weArtClient->Run();
~~~~~~~~~~~~~

@note The network thread created by the  ```Run``` method will stop once the connection is closed. To keep the connection open, call the method again upon disconnection or error (they can be notified with WeArtClient::AddConnectionStatusCallback and WeArtClient::AddErrorCallback).

# Features

## Start/Stop Client
Once connected to the middleware, it's still not possible to receive tracking data and send haptic commands to the devices.
In order to do so, it's important to start the middleware with the proper command.

To start the middleware operations, call the Start() method.

~~~~~~~~~~~~~{.cpp}
	weArtClient->Start();
~~~~~~~~~~~~~

To stop the middleware, call the Stop() method.

~~~~~~~~~~~~~{.cpp}
	weArtClient->Stop();
~~~~~~~~~~~~~

## Devices calibration
After starting the communication with the middleware, it's now possible to calibrate the TouchDIVER devices.
The calibration allows to set the initial offsets of each thimble relative to the control unit position, in order to improve the finger tracking output.

First, create the calibration tracking object and add it to the client. The WeArtTrackingCalibration object allows to listen for calibration messages
from the middleware, and get notified when the calibration process ends.

~~~~~~~~~~~~~{.cpp}
	// Create calibration tracker and add to client
	WeArtTrackingCalibration* calibration = new WeArtTrackingCalibration();
	weArtClient->AddMessageListener(calibration);
~~~~~~~~~~~~~

Then, start the calibration procedure. This will allow the middleware to calibrate the hand sensor offsets based on the current setup (thimbles and control device position, hand inclination, personal differences in the fingers etc..).

~~~~~~~~~~~~~{.cpp}
	weArtClient->StartCalibration();
~~~~~~~~~~~~~

It’s possible to get the calibration status and result from the tracker object itself, or through callbacks (in the form of std::function).

~~~~~~~~~~~~~{.cpp}
	// Get hand, status and result from tracking object
	HandSide hand = calibration->getCurrentHand();
	CalibrationStatus status = calibration->getStatus();
	bool result = calibration->getResult();

	// Be notified by callbacks when the calibration status changes
	calibration->AddStatusCallback([](HandSide hand, CalibrationStatus status) {
		...
		});

	// Be notified by callbacks when a calibration result is available
	calibration->AddResultCallback([](HandSide hand, bool result) {
		... insert code here
		});
~~~~~~~~~~~~~

## Haptic feedback

The TouchDIVER allows to perform haptic feedback on the user's finger through its *thimbles*.
Every thimble can apply a certain amount of pressure, temperature and vibration based on the processed object and texture.

### Haptic Object

A WeArtHapticObject is the basic object used to apply haptic feedback.
To create one, use the following code:

~~~~~~~~~~~~~{.cpp}
	// create haptic object to manage actuation on Right hand and Index Thimble
	hapticObject = new WeArtHapticObject(weArtClient);
	hapticObject->handSideFlag = HandSide::Right;
	hapticObject->actuationPointFlag = ActuationPoint::Index;
~~~~~~~~~~~~~

The values handSideFlag and actuationPointFlag accept multiple values.
The next example presents a single haptic object that, when applied a WeArtEffect, will affect both hands and all fingers.

~~~~~~~~~~~~~{.cpp}
	hapticObject->handSideFlag = HandSide::Right | HandSide::Left;
	hapticObject->actuationPointFlag = ActuationPoint::Index | ActuationPoint::Middle | ActuationPoint::Thumb;
~~~~~~~~~~~~~

### Create Effect

The SDK contains a basic TouchEffect class to apply effects to the haptic device.
The TouchEffect class contains the effects without any processing.
For different use cases (e.g. values not directly set, but computed from other parameters), create a different effect class by implementing the WeArtEffect interface.

Create the object on which the temperature, force and texture values will be applied:

~~~~~~~~~~~~~{.cpp}
TouchEffect* touchEffect = new TouchEffect(WeArtTemperature(), WeArtForce(), WeArtTexture());
~~~~~~~~~~~~~

### Add or Update Effect

It's possible to add a new effect to an haptic object, or to update an existing one.

In the example below, the effect created in the previous section is updated with a new temperature, force and texture.
It is then added to the haptic object if not already present, otherwise the haptic object is updated in order to send the new 
effect parameters to the middleware and then to the device.

~~~~~~~~~~~~~{.cpp}
    // define temperature
	WeArtTemperature temperature = WeArtTemperature();
	temperature.active = true;
	temperature.value(0.7f);

	// define force
	WeArtForce force = WeArtForce();
	force.active = true;
	force.value(0.8f);

	// define TextileMeshMedium Texture
	WeArtTexture texture = WeArtTexture();
	texture.active = true;
	texture.textureType(TextureType::TextileMeshMedium);
	texture.textureVelocity(0.5f);

	// set properties to effect
	touchEffect->Set(temperature, force, texture);

	// add effect to thimble or update
	if (hapticObject->activeEffects.size() <= 0)
		hapticObject->AddEffect(touchEffect);
	else
		hapticObject->UpdateEffects();
~~~~~~~~~~~~~

@note When multiple effects are added to a WeArtHapticObject, which effect is applied depends on the order in which the effects are added. In particular, for each value (temperature, force, texture) only the latest active one will be applied.

### Remove Effect

If an effect is not needed anymore, it can be removed from the haptic object with the WeArtHapticObject::RemoveEffect method.

~~~~~~~~~~~~~{.cpp}
	hapticObject->RemoveEffect(touchEffect);
~~~~~~~~~~~~~

## Tracking

After starting the middleware and performing the device calibration, it's possible to receive tracking data
related to the TouchDIVER thimbles.

To read these values, create and set a thimble tracker object for monitoring the closure/abduction value of a given finger:
~~~~~~~~~~~~~{.cpp}
	WeArtThimbleTrackingObject* thumbThimbleTracking = new WeArtThimbleTrackingObject(HandSide::Right, ActuationPoint::Thumb);
	weArtClient->AddThimbleTracking(thumbThimbleTracking);
~~~~~~~~~~~~~

Once this object is added to the client, it will start receiving the tracking values.
To access the closure and abduction values, simply use the getters provided by the thimble tracking object.

The closure value ranges from 0 (opened) to 1 (closed).

The abduction value ranges from 0 (finger near the hand's central axis) to 1 (finger far from the hand central axis).

~~~~~~~~~~~~~{.cpp}
	float closure = thumbThimbleTracking->GetClosure();
	float abduction = thumbThimbleTracking->GetAbduction();
~~~~~~~~~~~~~

@note The **closure** value is available for all thimbles, while the **abduction** value is available only for the thumb (other thimbles will have a value of 0).

## Tracking Raw Data

It's possible to receive the raw data from the tracking sensors on each thimble (and the control unit), in addition to the tracking data.
Each sensor has:
* 3-axis accelerometer
* 3-axis gyroscope
* Time of Flight sensor

To read these values, create a WeArtTrackingRawData object and add it to the client.
~~~~~~~~~~~~~{.cpp}
	WeArtTrackingRawData* trackingRawSensorData = new WeArtTrackingRawData(HandSide::Right, ActuationPoint::Index);
	weArtClient->AddMessageListener(trackingRawSensorData);
~~~~~~~~~~~~~

Once this object is added to the client, it will listen for raw data messages.
To start receiving raw data from the middleware, call the WeArtClient::StartRawData() method.
To stop receiving raw data, call the WeArtClient::StopRawData() method.

To get the sensors data, get the latest sample (WeArtTrackingRawData::Sample) from the WeArtTrackingRawData object.
The sample contains the accelerometer, gyroscope and time of flight data, in addition to the timestamp of its sampling (generated by the middleware and represented as milliseconds in unix epoch time).
~~~~~~~~~~~~~{.cpp}
	WeArtTrackingRawData::Sample sample = trackingRawSensorData->GetLatestSample();
~~~~~~~~~~~~~

@note The Palm (control unit) doesn't contain a Time-Of-Flight sensor, so its value is always set to 0.

In addition to getting the latest sample by polling the tracking object, it's possible to add a callback called whenever a new sensor data sample is
received from the TouchDIVER.

~~~~~~~~~~~~~{.cpp}
	std::function<void(WeArtTrackingRawData::Sample)> callback = (WeArtTrackingRawData::Sample sample) => {
		// process the sensor data sample
	};
	rawSensorData.AddSampleCallback(callback);
~~~~~~~~~~~~~

## Analog Raw Sensors Data

It's possible to receive the raw data from the sensors on each thimble (and the control unit), instead of the tracking data when this function is activated on the Middleware.
Each sensor has:
* NTC - Negative Temperature Coefficient (raw data and converted degree)
* FSR - force sensing resistor (raw adata and converted newton)

To read these values, create a WeArtAnalogSensorData object and add it to the client.
~~~~~~~~~~~~~{.cpp}
WeArtAnalogSensorData* analogSensorData = new WeArtAnalogSensorData(HandSide::Right, ActuationPoint::Index);weArtClient->AddMessageListener(analogSensorData);
~~~~~~~~~~~~~

Once this object is added to the client, it will listen for raw data messages as soon the Middleware is on start.

To get the sensors data, get the latest sample (WeArtAnalogSensorData::Sample) from the AnalogSensorRawData object.
The sample contains the accelerometer, gyroscope and time of flight data, in addition to the timestamp of its sampling (generated by the middleware and represented as milliseconds in unix epoch time).
~~~~~~~~~~~~~{.cpp}
	WeArtAnalogSensorData::Sample sample = anlogSensorData->GetLatestSample();
~~~~~~~~~~~~~

@note The Palm (control unit) doesn't contain a analog sensor, so its value is always set to 0.



## Middleware and Devices status tracking

The SDK allows to track and receives updates about the middleware and the connected devices status.

In particular, the information is available through a MiddlewareStatusListener object, that must be added as listener to the client object:
~~~~~~~~~~~~~{.cpp}
mwListener = new MiddlewareStatusListener();
weArtClient->AddMessageListener(mwListener);
~~~~~~~~~~~~~

The MiddlewareListener tracks the messages from the middleware, saving and notifying about status changes.
In particular, it's possible to register callbacks for the middleware and devices status.

The status callback will receive a struct with the MiddlewareStatusUpdate type, which includes:
* Middleware version
* Middleware status (MiddlewareStatus)
* Status code and description  
* Whether actuations are enabled or not
* List of the connected devices. For each device:
	* Mac Address
	* Assigned HandSide
	* Overall battery level
	* Status of each thimble (actuation point, connected or not, status code etc..)

~~~~~~~~~~~~~{.cpp}
std::function<void(MiddlewareStatusUpdate)> callback = [](MiddlewareStatusUpdate data) {
	... access middleware status data ...	
};
mwListener->AddStatusCallback(callback);
~~~~~~~~~~~~~

The same information can be asked to the MiddlewareListener (in polling fashion) by calling the MiddlewareListener::lastStatus method.

### Status Codes
The MiddlewareListener object allows to get the middleware status, which includes the latest status code sent by the middleware while performing
its operations.

The current status codes (along with their description) are:

| Status Code |   | Description |
|---|---|---|
| 0 | OK | Ok |
| 100 | START_GENERIC_ERROR | Can't start generic error: Stopping |
| 101 | CONNECT_THIMBLE | Unable to start, connect at least one thimble and retry |
| 102 | WRONG_THIMBLES | Unable to start, connect the right thimbles matched to the bracelet and retry |
| 103 | BATTERY_TOO_LOW | Battery is too low, cannot start |
| 104 | FIRMWARE_COMPATIBILITY | Can't start while the devices are connected to the power supply |
| 105 | SET_IMU_SAMPLE_RATE_ERROR | Error while setting IMU Sample Rate! Device Disconnected! |
| 106 | RUNNING_SENSOR_ON_MASK | Inconsistency on Analog Sensors raw data! Please try again or Restart your device/s! |
| 107 | RUNNING_DEVICE_CHARGING | Can't start while the devices are connected to the power supply |
| 200 | CONSECUTIVE_TRACKING_ERRORS | Too many consecutive running sensor errors, stopping session |
| 201 | DONGLE_DISCONNECT_RUNNING | BLE Dongle disconnected while running, stopping session |
| 202 | TD_DISCONNECT_RUNNING | TouchDIVER disconnected while running, stopping session |
| 203 | DONGLE_CONNECTION_ERROR | Error on Dongle during connection phase! |
| 300 | STOP_GENERIC_ERROR | Generic error occurred while stopping session |

@note The description of each status code might change between different Middleware versions, use the status code to check instead of the description.