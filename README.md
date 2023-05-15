# Introduction

This is the C++ SDK for Weart. It allows to communicate with the Weart Middleware application and use the TouchDiver devices.

## Architecture
![C++ SDK Architecture](./architecture.png)

## Setup

Turn on the Middleware and connect it to the TouchDiver
Create a header file and Include the following headers

~~~~~~~~~~~~~{.cpp}
	#include <WEART_SDK/WeArtClient.h>
	#include <WEART_SDK/WeArtHapticObject.h>
	#include <WEART_SDK/WeArtThimbleTrackingObject.h>
	#include <WEART_SDK/WeArtTrackingCalibration.h>
	#include "TouchEffect.h"
~~~~~~~~~~~~~

In the same header file add these variables

~~~~~~~~~~~~~{.cpp}
	WeArtClient* weArtClient;
	WeArtHapticObject* hapticObject;
	TouchEffect* touchEffect;
	WeArtThimbleTrackingObject* indexThimbleTracking;
~~~~~~~~~~~~~

Create the “.cpp” file for the newly created header file. inside it you can use the following:

Create the client

~~~~~~~~~~~~~{.cpp}
	// Create a client giving IP and port of the middleware
	weArtClient = new WeArtClient("127.0.0.1", WeArtConstants::DEFAULT_TCP_PORT);
~~~~~~~~~~~~~

Setting the haptic objects

~~~~~~~~~~~~~{.cpp}
	// create haptic object to manage actuation on Right hand and Index Thimble
	hapticObject = new WeArtHapticObject(weArtClient);
	hapticObject->handSideFlag = HandSide::Right;
	hapticObject->actuationPointFlag = ActuationPoint::Index;
~~~~~~~~~~~~~

The values handSideFlag and actuationPointFlag accept multiple values, the next example presents a single haptic object that, when applied a WeArtEffect, will affect both hands and set the haptic feeling to all fingers.

~~~~~~~~~~~~~{.cpp}
	hapticObject->handSideFlag = HandSide::Right | HandSide::Left;
	hapticObject->actuationPointFlag = ActuationPoint::Index | ActuationPoint::Middle | ActuationPoint::Thumb;
~~~~~~~~~~~~~

Creating and applying an initial WeArtTouchEffect

~~~~~~~~~~~~~{.cpp}
	// define feeling properties to create an effect
	WeArtTemperature temperature = WeArtTemperature();
	WeArtForce force = WeArtForce();
	WeArtTexture texture = WeArtTexture();

	// instance a new effect with feeling properties and add effect to thimble
	touchEffect = new TouchEffect(temperature, force, texture);
	hapticObject->AddEffect(touchEffect);
~~~~~~~~~~~~~

Set a thimble tracker object for monitoring the closure value of the finger

~~~~~~~~~~~~~{.cpp}
	// reference a thimble tracking object to read closure value
	indexThimbleTracking = new WeArtThimbleTrackingObject(HandSide::Right, ActuationPoint::Index);
	weArtClient->AddThimbleTracking(indexThimbleTracking);
~~~~~~~~~~~~~

Accessing the WeArtThimbleTrackingObject closure value (form 0 to 1)

~~~~~~~~~~~~~{.cpp}
	indexThimbleTracking->GetClosure();
~~~~~~~~~~~~~

Initiate the client communication

~~~~~~~~~~~~~{.cpp}
	// run socket communication 
	weArtClient->Run();
~~~~~~~~~~~~~

### Client Management
After the setup is finished, you can start the client communication with the Middleware 

~~~~~~~~~~~~~{.cpp}
	weArtClient->Start();
~~~~~~~~~~~~~

Stopping the connection

~~~~~~~~~~~~~{.cpp}
	weArtClient->Stop();
~~~~~~~~~~~~~

### Devices calibration
After starting the communication with the middleware, to receive the tracking data (closures, abduction), you need to calibrate the device.

First, create the calibration tracking object and add it to the client

~~~~~~~~~~~~~{.cpp}
	// Create calibration tracker and add to client
	WeArtTrackingCalibration* calibration = new WeArtTrackingCalibration();
	weArtClient->AddMessageListener(calibration);
~~~~~~~~~~~~~

Then, start the calibration procedure

~~~~~~~~~~~~~{.cpp}
	weArtClient->StartCalibration();
~~~~~~~~~~~~~

It’s possible to get the calibration status and result from the tracker object itself, or through callbacks

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

### Create and manage Effects programmatically
#### Add or update Effect

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

#### Remove Effect

~~~~~~~~~~~~~{.cpp}
	hapticObject->RemoveEffect(touchEffect);
~~~~~~~~~~~~~