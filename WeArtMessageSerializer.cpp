#include "WeArtMessageSerializer.h"

WeArtMessage* WeArtMessageSerializer::createMessage(std::string& ID) {
	switch (HashStringToInt(ID.c_str())) {
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

std::string WeArtMessageSerializer::Serialize(WeArtMessage* message) {
	// Unlike C#, C++ does not allow dynamic runtime
		// reflection, hence we have to use dedicated functions
		// to get the values of the class members.
	std::string messageID = message->getID();
	std::vector<std::string> serializedValues = message->getValues();

	// Join the string arrays.
	serializedValues.insert(serializedValues.begin(), messageID);

	// Then merge using our separator.
	std::stringstream ss;
	auto it = serializedValues.begin();
	ss << *it++;
	for (; it != serializedValues.end(); it++) {
		ss << separator;
		ss << *it;
	}

	return ss.str();
}

uint8* WeArtMessageSerializer::Serialize(std::string text) {
	// Simple reinterpret cast, not even necessary
	// to allocate new memory.
	uint8* byteData = reinterpret_cast<uint8*>(&text[0]);
	return byteData;
}

WeArtMessage* WeArtMessageSerializer::Deserialize(std::string data) {
	std::vector<std::string> strings;
	std::istringstream dataStream(data);
	std::string s;
	while (getline(dataStream, s, separator)) {
		strings.push_back(s);
	}

	// Create the correct message class from first (ID) string.
	std::string messageID = strings[0];
	WeArtMessage* msg = createMessage(messageID);

	// Initialize the message values using the rest of the strings.
	if (msg) {
		strings.erase(strings.begin());
		msg->setValues(strings);
	}

	return msg;
}

std::string WeArtMessageSerializer::Deserialize(uint8* byteData, int byteCount) {
	std::string text = std::string(reinterpret_cast<char*>(byteData));
	return text;
}
