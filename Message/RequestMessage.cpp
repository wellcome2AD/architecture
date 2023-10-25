#include "RequestMessage.h"
#include "../Reader/Reader.h"

RequestMessage::RequestMessage(const std::string& message) : _message(message) {}

format RequestMessage::GetFormat() const { return getReq; }

std::string RequestMessage::GetMsg() const { return _message; }

void RequestMessage::SetMsg(const std::string& m) { _message = m; }

void RequestMessage::Read(Reader* r) {}

void RequestMessage::Write(Writer* w) const {}
