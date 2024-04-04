# Example Project

An example application is available for download together with the C# and C++ SDK [here](https://www.weart.it/repository/downloads/samples/weart-integration-cpp-sdk-v1.2.0.zip).

The application is implemented as a Universal Windows app in C++. To execute the application, just open the solution with Visual Studio and Run it (by clicking the play button or pressing F5).

![Example application](./example_app/ExampleApp_Screen.png)

## SDK Integration 

The SDK is integrated in the example application by copying the sdk source and header files into the ```WEART_C++_API_Integration/WEART_SDK``` directory, and adding them to the visual studio project.
Most of the application code can be found in the ```WEART_C++_API_Integration/MainPage.xaml.cpp``` file.

### Connection to Middleware
The connection is managed using the WeArtClient class together with callbacks to monitor the connection status. 

In particular, upon loading the main window, the application initialises the sdk client and adds a callback for when the connection
status changes (connected or disconnected).

The callback enables or disables UI elements based on the status, and tries to reconnect upon disconnection.

~~~~~~~~~~~~~{.cpp}

// Initialise client and attach callback
MainPage::MainPage()
{
	weArtClient = new WeArtClient(WeArtConstants::DEFAULT_IP_ADDRESS, WeArtConstants::DEFAULT_TCP_PORT); //IP ADDRESS and PORT of Middleware PC	

	// Add connection status callback to get notified when the client connects and disconnects from the middleware
	weArtClient->AddConnectionStatusCallback([this](bool connected) { OnConnectionStatusChanged(connected); });

	...
}

// Callback for connection status change, 
void WEART_C___API_Integration::MainPage::OnConnectionStatusChanged(bool connected) {

	// Update UI elements
	Dispatcher->RunAsync(CoreDispatcherPriority::High,
		ref new DispatchedHandler([this, connected]() {
			TextConnectionStatus->Text = connected ? "Connected" : "Not Connected";
			TextConnectionStatus->Foreground = ref new SolidColorBrush(connected ? Windows::UI::Colors::Green : Windows::UI::Colors::Red);

			ButtonStartClient->IsEnabled = connected;
			ButtonStopClient->IsEnabled = connected;

			ButtonStartCalibration->IsEnabled = connected;

			ButtonEffectSample1->IsEnabled = connected;
			ButtonEffectSample2->IsEnabled = connected;
			ButtonEffectSample3->IsEnabled = connected;
			ButtonRemoveEffect->IsEnabled = connected;
			}));

	// Reconnect if needed
	if (!connected)
		Connect();
}

void WEART_C___API_Integration::MainPage::Connect() {
	// Lauch network connection thread
	auto workItem = ref new WorkItemHandler([this](IAsyncAction^ workItem) {
		while (!weArtClient->IsConnected())
			weArtClient->Run();
		});
	ThreadPool::RunAsync(workItem);
}
~~~~~~~~~~~~~

### Start/Stop middleware
By clicking on the "Start" and "Stop" buttons, it's possible to start and stop the middleware operations.
When clicking on the buttons, the application simply calls the corresponding SDK methods.

~~~~~~~~~~~~~{.cpp}
void WEART_C___API_Integration::MainPage::ButtonStartClient_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	weArtClient->Start();
}

void WEART_C___API_Integration::MainPage::ButtonStopClient_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	weArtClient->Stop();
}
~~~~~~~~~~~~~

### Calibration
When the "Calibrate" button is clicked, if the middleware is started, the application will start the calibration procedure.

In addition, the WeArtTrackingCalibration object is used to keep track of the calibration status and display it to the user.

Instead of registering a callback to the calibration tracking object, the object is checked periodically using a timer
(the same used for rendering the tracking values) and reading the current calibration status to update the user.

~~~~~~~~~~~~~{.cpp}
// Initialise client and attach callback
MainPage::MainPage()
{
	... 

	calibration = new WeArtTrackingCalibration();
	weArtClient->AddMessageListener(calibration);

	...

	TimeSpan period;
	period.Duration = 0.1 * 10000000; // 0.1sec
	ThreadPoolTimer::CreatePeriodicTimer(ref new TimerElapsedHandler(this, &MainPage::TestTimer), period);
}

// Start calibration
void WEART_C___API_Integration::MainPage::ButtonStartCalibration_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	calibrating = true;
	weArtClient->StartCalibration();
}

// Render calibration and tracking data
void MainPage::TestTimer(Windows::System::Threading::ThreadPoolTimer^ timer)
{
	Dispatcher->RunAsync(CoreDispatcherPriority::High,
		ref new DispatchedHandler([this]()
			{
				RenderCalibrationStatus();
				RenderClosureAndAbductionValues();
			}));
}

// Update UI (calibration button, calibration status text)
void WEART_C___API_Integration::MainPage::RenderCalibrationStatus() {
	if (!calibrating) {
		if(weArtClient->IsConnected())
			ButtonStartCalibration->IsEnabled = true;
		return;
	}

	TextCalibrationStatus->Text = "Calibrating...";

	if(calibration->getStatus() ==  CalibrationStatus::Running) { 
		if (calibration->getResult()) {
			TextCalibrationStatus->Text = "Calibrated!!";
		} else {
			TextCalibrationStatus->Text = "Calibration Error";
		}
		ButtonStartCalibration->IsEnabled = true;
		calibrating = false;
	}
}
~~~~~~~~~~~~~

### Tracking data

The tracking data is read periodically (with the same timer as the calibration result) from multiple WeArtThimbleTrackingObject objects,
declared when the application is opened.

~~~~~~~~~~~~~{.cpp}
MainPage::MainPage()
{
	... 

	// Add all sensor data trackers
	AddSensor("LEFT", "INDEX");
	AddSensor("LEFT", "MIDDLE");
	AddSensor("LEFT", "THUMB");
	AddSensor("LEFT", "PALM");
	AddSensor("RIGHT", "INDEX");
	AddSensor("RIGHT", "MIDDLE");
	AddSensor("RIGHT", "THUMB");
	AddSensor("RIGHT", "PALM");

	...
}

// Add sensor to the listener
void MainPage::AddSensor(std::string handSide, std::string actuationPoint) {

	HandSide hs = StringToHandside(handSide);
	ActuationPoint ap = StringToActuationPoint(actuationPoint);

	//Tracking data
	auto sensor = new WeArtTrackingRawData(hs, ap);
	trackingSensors[std::make_pair(handSide, actuationPoint)] = sensor;
	weArtClient->AddMessageListener(sensor);

	// Analog sensor data
	auto analogSensor = new WeArtAnalogSensorData(hs, ap);
	analogSensors[std::make_pair(handSide, actuationPoint)] = analogSensor;
	weArtClient->AddMessageListener(analogSensor);
}

// Render data
void MainPage::TestTimer(Windows::System::Threading::ThreadPoolTimer^ timer)
{
	Dispatcher->RunAsync(CoreDispatcherPriority::High,
		ref new DispatchedHandler([this]()
			{
				RenderCalibrationStatus();
				RenderClosureAbduction();
				RenderTrackingRawSensorsData();
				RenderAnalogRawSensorData();
				RenderMiddlewareStatus();
				RenderDevicesStatus();
			}));
}

void WEART_C___API_Integration::MainPage::RenderClosureAndAbductionValues() {
	ValueIndexRightClosure->Text = indexRightThimbleTracking->GetClosure().ToString();
	ValueThumbRightClosure->Text = thumbRightThimbleTracking->GetClosure().ToString();
	ValueThumbRightAbduction->Text = thumbRightThimbleTracking->GetAbduction().ToString();
	ValueMiddleRightClosure->Text = middleRightThimbleTracking->GetClosure().ToString();

	ValueIndexLeftClosure->Text = indexLeftThimbleTracking->GetClosure().ToString();
	ValueThumbLeftClosure->Text = thumbLeftThimbleTracking->GetClosure().ToString();
	ValueThumbLeftAbduction->Text = thumbLeftThimbleTracking->GetAbduction().ToString();
	ValueMiddleLeftClosure->Text = middleLeftThimbleTracking->GetClosure().ToString();
}
~~~~~~~~~~~~~

### Applying effects

To apply effects to the user hands, the application contains a custom ```TouchEffect``` class, which derives from WeArtEffect. It's implemented in the ```TouchEffect.h/cpp``` file.

The application apply a different effect to the index finger when one of the "Add effect sample" buttons is clicked.

For example, when clicking the "Add effect sample 1" button, a slight pressure and cold temperature (without texture vibration)
are applied to the right index finger, as shown below:

~~~~~~~~~~~~~{.cpp}
MainPage::MainPage()
{
	...

	// create haptic object to manage actuation on Righ hand and Index Thimble
	hapticObject = new WeArtHapticObject(weArtClient);
	hapticObject->handSideFlag = HandSide::Right;
	hapticObject->actuationPointFlag = ActuationPoint::Index;

	...
}

void WEART_C___API_Integration::MainPage::ButtonEffectSample1_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// define temperature
	WeArtTemperature temperature = WeArtTemperature();
	temperature.active = true; //must be active if you want feel
	temperature.value(0.2f);

	// define force
	WeArtForce force = WeArtForce();
	force.active = true;
	force.value(0.4f);

	// define null Texture
	WeArtTexture texture = WeArtTexture();

	// set properties to effect
	touchEffect->Set(temperature, force, texture);

	// add effect to thimble or update
	if (hapticObject->activeEffects.size() <= 0)
		hapticObject->AddEffect(touchEffect);
	else 
		hapticObject->UpdateEffects();
}
~~~~~~~~~~~~~

### Raw Sensors Data tracking

![Raw Sensors Data Panel](./example_app/ExampleApp_RawData.png)

In the right section of the window, the application displays the raw data of the different sensors aboard the TouchDIVER.
In particular, it's possible to choose the hand and actuation point from which to visualize:
* Timestamp of the last sample received
* Accelerometer data (on the x,y,z axis)
* Gyroscope data (on the x,y,z axis)
* Time of Flight distance (in mm)

To start receiving raw data, click on the "Start Raw Data" button, and to stop click on the "Stop Raw Data" button.

When it's loaded, the application creates a WeArt.Components.WeArtRawSensorsDataTrackingObject for each pair of (HandSide, ActuationPoint).
Using a timer, the application polls the chosen sensor and displays its data:

~~~~~~~~~~~~~{.cpp}
void MainPage::RenderTrackingRawSensorsData() {
	// Get chosen sensor
	std::pair<std::string, std::string> sensorChoice = GetSensorChoice();
	if (trackingSensors.find(sensorChoice) == trackingSensors.end())
		return;
	WeArtTrackingRawData* sensor = trackingSensors[sensorChoice];

	// Render raw data to screen
	WeArtTrackingRawData::Sample sample = sensor->GetLastSample();
	Acc_X->Text = sample.data.accelerometer.x.ToString();
	Acc_Y->Text = sample.data.accelerometer.y.ToString();
	Acc_Z->Text = sample.data.accelerometer.z.ToString();

	Gyro_X->Text = sample.data.gyroscope.x.ToString();
	Gyro_Y->Text = sample.data.gyroscope.y.ToString();
	Gyro_Z->Text = sample.data.gyroscope.z.ToString();

	TimeOfFlight->Text = sample.data.timeOfFlight.distance.ToString();

	LastSampleTime->Text = sample.timestamp.ToString();
}
~~~~~~~~~~~~~

### Analog Raw Sensors Data 

![Analog Sensors Data Panel](./example_app/ExampleApp_AnalogSensorData.png)

In the right section of the window, the application displays the anlog raw data of the different sensors aboard the TouchDIVER.
In particular, it's possible to choose the hand and actuation point from which to visualize:
* Timestamp of the last sample received
* NTC - Negative Temperature Coefficient (raw data and converted degree)
* FSR - force sensing resistor (raw adata and converted newton)

To start receiving analog sensor data, active this function on the Middleware and click on the "Start Raw Data" button, and to stop click on the "Stop Raw Data" button. In this modality the other tracking data will not received by the SDK.

When it's loaded, the application creates a WeArt.Components.WeArtAnalogSensorData for each pair of (HandSide, ActuationPoint).
Using a timer, the application polls the chosen sensor and displays its data:

~~~~~~~~~~~~~{.cpp}
void MainPage::RenderAnalogRawSensorData() {
	// Get chosen sensor
	std::pair<std::string, std::string> sensorChoice = GetSensorChoice();
	if (trackingSensors.find(sensorChoice) == trackingSensors.end())
		return;
	WeArtAnalogSensorData* sensor = analogSensors[sensorChoice];

	// Render raw data to screen
	WeArtAnalogSensorData::Sample sample = sensor->GetLastSample();

	LastSampleTime->Text = sample.timestamp.ToString();

	// raw data
	//sample.data.ntcTemperatureRaw.ToString();
	//sample.data.forceSensingRaw.ToString();

	NTCTemperature->Text = sample.data.ntcTemperatureConverted.ToString();
	FSR->Text = sample.data.forceSensingConverted.ToString();
}
~~~~~~~~~~~~~

### Middleware and connected devices status

![Middleware/Devices Status Panel](./example_app/ExampleApp_MiddlewareStatus.png)

In the leftmost section of the window, the application displays the middleware and the connected devices status, as sent by the middleware during the connection.

To track and show the middleware and devices status, the example application uses a MiddlewareStatusListener object added as listener to the client:
~~~~~~~~~~~~~{.cpp}
// Add Middleware status listener
mwListener = new MiddlewareStatusListener();
weArtClient->AddMessageListener(mwListener);
~~~~~~~~~~~~~

It then employs a timer to render the status data every 100ms (along with other tracking data such as closures/abduction and other statuses):

~~~~~~~~~~~~~{.cpp}
void MainPage::TestTimer(Windows::System::Threading::ThreadPoolTimer^ timer)
{
	Dispatcher->RunAsync(CoreDispatcherPriority::High,
		ref new DispatchedHandler([this]()
			{
				...
				RenderMiddlewareStatus();
				RenderDevicesStatus();
				...
			}));
}

void WEART_C___API_Integration::MainPage::RenderMiddlewareStatus() {
	MiddlewareStatusUpdate mwStatus = mwListener->lastStatus();
	...
}

void WEART_C___API_Integration::MainPage::RenderDevicesStatus() {
	std::vector<ConnectedDeviceStatus> devices = mwListener->lastStatus().devices;
	...
}
~~~~~~~~~~~~~

The RenderMiddlewareStatus method sets buttons and text properties based on the data inside the middleware status message,
while the RenderDevicesStatus method renders the TouchDIVER informations (e.g. connected, thimbles connected and/or errors, mac address, battery level, charging status etc..)
using an additional custom UserControl contained in the HandStatus.xaml/h/cpp files.