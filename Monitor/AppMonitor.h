#pragma once
#include "helpers/Console.h"

// Monitor app that manages Server's lifecycle and monitors its state
class Monitor
{
public:
    Monitor();
    virtual ~Monitor();
    bool init();
    bool init(const std::vector<bool>& processes_live); // launches Server
    std::vector<bool> check(); // checks Server state
    static void reset(const std::vector<bool>& processes_live); // terminates irresponsive Server
    static void reset();

private:
    Console m_console;
};
