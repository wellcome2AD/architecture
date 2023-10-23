#define _CRT_SECURE_NO_WARNINGS

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <vector>
#include <string>
#include <fstream>

#include "AppMonitor.h"
#include "helpers/Process.h"
#include "../helpers/UtilString.h"
#include "../helpers/UtilFile.h"

static std::vector<Process> sServers(2);
static std::string sPort = "0";

Monitor::Monitor()
{
    remove("resources/CREATED");
    m_console.handleCtrlC(Monitor::reset); // if Monitor's execution is aborted via Ctrl+C, reset() cleans up its internal state
}

Monitor::~Monitor()
{
    remove("resources/CREATED");
}

static void synchPort()
{
    while (!fileExists("resources/CREATED"))
    {
        Sleep(1000);
    }
    auto data = fileReadStr("resources/CREATED");
    sPort = split(data, ",")[0];
    delete[] data;
}

bool Monitor::init()
{
    printf("---INITIALIZE---\n");
    auto processes_live = std::vector<bool>(sServers.size(), false);
    
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    
    processes_live[0] = sServers[0].create(cmd);
    
    bool ok = processes_live[0];
    if (ok)
    {
        ok = init(processes_live);
    }
    printf("----------------\n");
    return ok;
}

bool Monitor::init(const std::vector<bool>& processes_live)
{
    char cmd[256] = {};
    synchPort();
    sprintf(cmd, "Server.exe %s", sPort.c_str());

    bool ok = true; // launching Server
    for (size_t i = 0; i < sServers.size(); ++i)
    {        
        if (!processes_live[i])
        {            
            ok &= sServers[i].create(cmd);
            if (ok)
                printf("Server%s was started\n", sServers[i].pid().c_str());
            else
                printf("Cannot create server\n");
        }
        else
        {
            printf("Server%s is continuing working\n", sServers[i].pid().c_str());
        }
    }
    return ok;
}

std::vector<bool> Monitor::check()
{
    const time_t timeout = 5;
    std::vector<bool> res(sServers.size(), true);
    bool dead_proc_was_found = false;
    for (size_t i = 0; i < sServers.size(); ++i)
    {
        auto& s = sServers[i];
        auto filename = std::string("resources/ALIVE") + s.pid();
        struct stat result;
        if (stat(filename.c_str(), &result) == 0)
        {
            auto current_time = time(NULL);
            auto mod_time = result.st_mtime;
            if (current_time - mod_time > timeout)
            {                
                dead_proc_was_found = true;
                res[i] = false;
            }
        }
        s.wait(3000);
    }
    if (dead_proc_was_found)
    {
        printf("-----CHECK------\n");
        for (size_t i = 0; i < sServers.size(); ++i)
        {
            if (res[i] == false)
            {
                printf("Server%s isn't alive for more than %lld sec\n", sServers[i].pid().c_str(), timeout);
            }
        }
        printf("----------------\n");
    }
    return res;
}

void Monitor::reset(const std::vector<bool>& processes_live)
{
    bool dead_proc_was_found = std::find(processes_live.begin(), processes_live.end(), false) != processes_live.end();
    if (dead_proc_was_found)
    {
        printf("-----RESET------\n");
    }
    for (size_t i = 0; i < sServers.size(); ++i)
    {
        auto& s = sServers[i];
        if (!processes_live[i])
        {
            printf("Reset server\n");
            auto filename = std::string("resources/ALIVE") + s.pid();
            if (remove(filename.c_str()) != 0)
            {
                printf("Can't remove file %s\n", filename.c_str());
            }
            s.terminate();
        }
    }
    if (dead_proc_was_found)
    {
        printf("----------------\n");
    }
}

void Monitor::reset()
{
    reset(std::vector<bool>(sServers.size(), true));
}
