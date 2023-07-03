#include "MiddlewareStatusListener.h"

void MiddlewareStatusListener::AddStatusCallback(std::function<void(MiddlewareStatusData)> callback)
{
	_callbacks.push_back(callback);
}

void MiddlewareStatusListener::OnMessageReceived(WeArtMessage* message)
{
	MiddlewareStatusMessage* castedMessage = static_cast<MiddlewareStatusMessage*>(message);
	if (castedMessage == nullptr)
		return;

	_data = castedMessage->data();
	for (auto callback : _callbacks) {
		callback(_data);
	}
}