#include "WeArtMessageSerializer.h"
#include <sstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

WeArtMessage* WeArtMessageSerializer::Deserialize(std::string data) {
	WeArtMessage* msg = createMessage(data);
	if (msg != nullptr)
		msg->deserialize(data);
	return msg;
}

std::string WeArtMessageSerializer::extractID(std::string& data)
{
	std::string id = "";
	try {
		// JSON message
		json j = json::parse(data);
		id = j["type"].template get<std::string>();
	}
	catch (json::parse_error& ex)
	{
		// CSV message, get first field
		std::istringstream dataStream(data);
		std::getline(dataStream, id, ':');
	}
	return id;
}

WeArtMessage* WeArtMessageSerializer::createMessage(std::string& id) {
	switch (HashStringToInt(id.c_str())) {
		case HashStringToInt(StartFromClientMessage::ID): return new StartFromClientMessage();
		case HashStringToInt(StopFromClientMessage::ID): return new StopFromClientMessage();
		case HashStringToInt(CalibrationResultMessage::ID): return new CalibrationResultMessage();
		case HashStringToInt(CalibrationStatusMessage::ID): return new CalibrationStatusMessage();
		case HashStringToInt(ExitMessage::ID): return new ExitMessage();
		case HashStringToInt(DisconnectMessage::ID): return new DisconnectMessage();
		case HashStringToInt(SetTemperatureMessage::ID): return new SetTemperatureMessage();
		case HashStringToInt(StopTemperatureMessage::ID): return new StopTemperatureMessage();
		case HashStringToInt(SetForceMessage::ID): return new SetForceMessage();
		case HashStringToInt(StopForceMessage::ID): return new StopForceMessage();
		case HashStringToInt(SetTextureMessage::ID): return new SetTextureMessage();
		case HashStringToInt(StopTextureMessage::ID): return new StopTextureMessage();
		case HashStringToInt(TrackingMessage::ID): return new TrackingMessage();
		case HashStringToInt(RawSensorsData::ID): return new RawSensorsData();
		default: return nullptr;
	}
}
