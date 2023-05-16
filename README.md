# Low-Level C++ SDK

Welcome to the Weart Low-Level C++ SDK documentation.

The SDK allows to connect to the Weart middleware and perform various actions with the TouchDIVER devices:
* Start and Stop the middleware operations
* Calibrate the device
* Receive tracking data from the devices
* Send haptic effects to the devices

# Architecture
<img src="./architecture.svg" width="100%" alt="C++ SDK Architecture" />

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
	#include <WEART_SDK/WeArtClient.h>
	#include <WEART_SDK/WeArtHapticObject.h>
	#include <WEART_SDK/WeArtThimbleTrackingObject.h>
	#include <WEART_SDK/WeArtTrackingCalibration.h>
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

In order to send haptic feedback to the device, an effect must be created by extending the WeArtEffect class.
The class shown here contains the effects without any processing, but it might be extended for other use cases
(e.g. values not directly set, but computed from other parameters).

~~~~~~~~~~~~~{.cpp}
public class TouchEffect : public WeArtEffect {
public:
	TouchEffect(WeArtTemperature temp, WeArtForce force, WeArtTexture texture)
		: _temperature(temp),  _force(force), _texture(texture) {}

	bool Set(WeArtTemperature temp, WeArtForce force, WeArtTexture texture) {
		_temperature = temp;
		_force = force;
		_texture = texture;
	}

	WeArtTemperature getTemperature(void) override { return _temperature; }

	WeArtForce getForce(void) override { return _force; }
	
	WeArtTexture getTexture(void) override { return _texture; }

private:
	WeArtTemperature _temperature;
	WeArtForce _force;
	WeArtTexture _texture;
};
~~~~~~~~~~~~~

After defining the effect class, create the object on which the temperature, force and texture values will be applied:

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
	texture.textureVelocity(0.5, 0, 0.5); // maximum recommended speed

	// set properties to effect
	touchEffect->Set(temperature, force, texture);

	// add effect to thimble or update
	if (hapticObject->activeEffects.size() <= 0)
		hapticObject->AddEffect(touchEffect);
	else
		hapticObject->UpdateEffects();
~~~~~~~~~~~~~

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
	WeArtThimbleTrackingObject* indexThimbleTracking = new WeArtThimbleTrackingObject(HandSide::Right, ActuationPoint::Index);
	weArtClient->AddThimbleTracking(indexThimbleTracking);
~~~~~~~~~~~~~

Once this object is added to the client, it will start receiving the tracking values.
To access the closure and abduction values, simply use the getters provided by the thimble tracking object.

The closure value ranges from 0 (opened) to 1 (closed).

The abduction value ranges from 0 (finger near the hand's central axis) to 1 (finger far from the hand central axis).

~~~~~~~~~~~~~{.cpp}
	float closure = indexThimbleTracking->GetClosure();
	float abduction = indexThimbleTracking->GetAbduction();
~~~~~~~~~~~~~