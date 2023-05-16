# Update from older SDK versions

To update your application to the latest SDK, download and extract the C++ sdk archive, then copy the source/header files in the same place as the older SDK version.

The new version includes additional files, so it's necessary to add them to the project in order to avoid linking errors.

This can be done on Visual Studio by right-clicking on the solution, then clicking on Add -> Existing Item and selecting all the SDK files.
On other systems (e.g. cmake) the procedure might be different.

The SDK is retro-compatible with older versions, so there's no need to update the application code. To see the new feature and fixes added to each version, refer to the Changelog section.

## Changelog
### Version 1.1 (latest)
* Add calibration procedure start/stop and listener
* Add new default tracking message and values for closure
* Add callback for connection status changed in WeArtClient class
