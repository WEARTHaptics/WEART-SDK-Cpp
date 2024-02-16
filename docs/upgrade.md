# Update from older SDK versions

To update your application to the latest SDK, download and extract the C++ sdk archive, then copy the source/header files in the same place as the older SDK version.

The new version includes additional files, so it's necessary to add them to the project in order to avoid linking errors.

This can be done on Visual Studio by right-clicking on the solution, then clicking on Add -> Existing Item and selecting all the SDK files.
On other systems (e.g. cmake) the procedure might be different.

## Code Migration

The new SDK version is mostly compatible with the older version.
The only breaking change introduced relates to the WeArtTexture class, in which the ```textureVelocity``` methods and parameter has been reduced from three values (x, y and z) to a single one, representing the speed at which the texture vibration is played by the thimble.
The 

