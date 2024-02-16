# Weart Low-Level C++ SDK

The Weart Low-Level C++ SDK  allows to connect to the Weart middleware and perform various actions with the TouchDIVER devices:
* Start and Stop the middleware operations
* Calibrate the device
* Receive tracking data from the devices
* Send haptic effects to the devices

## Installation

Copy the source files (.cpp and .h) into your project folder and add them to your project.

On Visual Studio:
* Right-Click on your project
* Click "Add" -> "Existing Item..."
* Select all the sdk source files and click "Add"

## Documentation
For the documentation, go [here](https://weart.it/docs/sdkcpp/)

## Changelog

### Version 1.2.0 (latest)
* Add raw sensors data tracking object
* Add middleware status messages and event
* Add connected devices status and event
* WeArtSensorRawData become WeArtTrackingRawData
* Add analog sensor raw data object - WeArtAnalogSensorData

### Version 1.1.0
* Add calibration procedure start/stop and listener
* Add new default tracking message and values for closure
* Add callback for connection status changed in WeArtClient class
* Add basic effect class for haptic feedback
* Remove unused velocity parameters from texture


## Copyright

Copyright &copy; 2024 Weart S.r.l.