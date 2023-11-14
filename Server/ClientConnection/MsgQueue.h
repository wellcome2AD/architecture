#pragma once

#include <memory>
#include <queue>
#include <mutex>

#include "../../Message/IMessage.h"

class Message
{
public:
	Message(int number, std::shared_ptr<IMessage> msg) : _number(number), _msg(msg) {}
	const IMessage* GetMsg() const { return _msg.get(); }
	int GetNumber() const { return _number; }
private:
	int _number;
	std::shared_ptr<IMessage> _msg;
};

class MsgQueue
{
public:
	static MsgQueue& GetInstance();

	void Push(std::shared_ptr<Message> m);

	std::shared_ptr<Message> Pop();

	bool IsEmpty() const;

private:
	MsgQueue() = default;
	MsgQueue(const Message&) = delete;
	MsgQueue(Message&&) = delete;
	MsgQueue& operator=(const Message&) = delete;

private:
	std::queue<std::shared_ptr<Message>> _msg_queue;
	static MsgQueue _instance;
	std::mutex _mutex;
};