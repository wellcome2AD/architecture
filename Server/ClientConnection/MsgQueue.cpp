#include "MsgQueue.h"

MsgQueue MsgQueue::_instance;

MsgQueue& MsgQueue::GetInstance()
{
    return _instance;
}

void MsgQueue::Push(std::shared_ptr<Message> m)
{
    std::lock_guard<std::mutex> lg(_mutex);
    _msg_queue.push(m);
}

std::shared_ptr<Message> MsgQueue::Pop()
{
    std::lock_guard<std::mutex> lg(_mutex);
    auto m = _msg_queue.front();
    _msg_queue.pop();
    return m;
}

bool MsgQueue::IsEmpty() const
{
    return _msg_queue.empty();
}
