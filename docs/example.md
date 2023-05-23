# Example Project

An example application is available for download together with the C# and C++ SDK [here](https://www.weart.it/repository/downloads/samples/weart-integration-cpp-sdk-v1.1.zip).

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
	weArtClient = new WeArtClient("127.0.0.1", WeArtConstants::DEFAULT_TCP_PORT); //IP ADDRESS and PORT of Middleware PC	

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
	// Create and register all thimbles (index, middle and thumb for right and left hands)

	indexRightThimbleTracking = new WeArtThimbleTrackingObject(HandSide::Right, ActuationPoint::Index);
	weArtClient->AddThimbleTracking(indexRightThimbleTracking);

	thumbRightThimbleTracking = new WeArtThimbleTrackingObject(HandSide::Right, ActuationPoint::Thumb);
	weArtClient->AddThimbleTracking(thumbRightThimbleTracking);

	...
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