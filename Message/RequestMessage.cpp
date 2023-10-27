#include "RequestMessage.h"
#include "../Serialiser/Serialiser.h"

RequestMessage::RequestMessage(const std::string& message) : _message(message) {}

format RequestMessage::GetFormat() const { return getReq; }

std::string RequestMessage::GetMsg() const { return _message; }

void RequestMessage::SetMsg(const std::string& m) { _message = m; }

void RequestMessage::Deserialise(Deserialiser& r) {}

void RequestMessage::Serialise(Serialiser& w) const {}
