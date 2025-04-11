#include "WeArtMessageSerializer.h"
#include <sstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

WeArtMessage* WeArtMessageSerializer::Deserialize(std::string data) {
	std::string id = extractID(data);
	WeArtMessage* msg = createMessage(id);
	if (msg != nullptr)
		msg->deserialize(data);
	return msg;
}

std::string WeArtMessageSerializer::extractID(std::string& data)
{
	// Get first non-whitespace character to check
	size_t first_char_idx = data.find_first_not_of(" \n\r\t");
	if (first_char_idx < 0 || first_char_idx >= data.size())
		return "";
	char first_char = data[first_char_idx];

	// Try parse JSON message
	if (first_char == '{' || first_char == '[') {
		try {
			json j = json::parse(data);
			return j["type"].template get<std::string>();
		}
		catch (json::parse_error) {}
	}

	// Parse CSV message, get first field
	std::string id = "";
	std::istringstream dataStream(data);
	std::getline(dataStream, id, ':');
	return id;
}

#define CREATE_MESSAGE_ID(TYPE) case HashStringToInt(TYPE::ID): return new TYPE()

WeArtMessage* WeArtMessageSerializer::createMessage(std::string& id) {
	switch (HashStringToInt(id.c_str())) {
		CREATE_MESSAGE_ID(StartFromClientMessage);
		CREATE_MESSAGE_ID(StopFromClientMessage);
		CREATE_MESSAGE_ID(CalibrationResultMessage);
		CREATE_MESSAGE_ID(CalibrationStatusMessage);
		CREATE_MESSAGE_ID(ExitMessage);
		CREATE_MESSAGE_ID(DisconnectMessage);
		CREATE_MESSAGE_ID(SetTemperatureMessage);
		CREATE_MESSAGE_ID(StopTemperatureMessage);
		CREATE_MESSAGE_ID(SetForceMessage);
		CREATE_MESSAGE_ID(StopForceMessage);
		CREATE_MESSAGE_ID(SetTextureMessage);
		CREATE_MESSAGE_ID(StopTextureMessage);
		CREATE_MESSAGE_ID(TrackingMessage);
		CREATE_MESSAGE_ID(TrackingBendingG2Message);
		CREATE_MESSAGE_ID(RawSensorsData);
		CREATE_MESSAGE_ID(RawSensorsDataTDPro);
		CREATE_MESSAGE_ID(GetMiddlewareStatus);
		CREATE_MESSAGE_ID(MiddlewareStatusMessage);
		CREATE_MESSAGE_ID(WeArtAppStatusMessage);
		CREATE_MESSAGE_ID(GetDevicesStatusMessage);
		CREATE_MESSAGE_ID(DevicesStatusMessage);
		CREATE_MESSAGE_ID(TDProStatusMessage);
		CREATE_MESSAGE_ID(AnalogSensorsData);
		default: return nullptr;
	}
}
