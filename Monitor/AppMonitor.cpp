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

static Process sServer;
static std::string sPort = "0";

Monitor::~Monitor()
{
    remove("resources/CREATED");
}

bool Monitor::init()
{
    m_console.handleCtrlC(Monitor::reset); // if Monitor's execution is aborted via Ctrl+C, reset() cleans up its internal state
    char cmd[256] = {};
    std::ifstream created_file("resources/CREATED");
    if (created_file.is_open())
    {
        std::string temp;
        getline(created_file, temp, ',');
        sPort = temp;
    }
    remove("resources/CREATED");
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    bool ok = sServer.create(cmd); // launching Server
    printf(ok ? "monitoring \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd);
    return ok;
}

bool Monitor::check()
{
    const time_t timeout = 5;
    auto filename = std::string("resources/ALIVE") + sServer.pid();
    struct stat result;
    if (stat(filename.c_str(), &result) == 0)
    {
        auto current_time = time(NULL);
        auto mod_time = result.st_mtime;
        if (current_time - mod_time > timeout)
        {
            printf("Server isn't alive for more than %lld sec\n", timeout);
            return false;
        }
    }
    sServer.wait(3000);
    return true;
}

void Monitor::reset()
{
    auto filename = std::string("resources/ALIVE") + sServer.pid();
    if (remove(filename.c_str()) != 0)
    {
        printf("Can't remove file %s\n", filename.c_str());
    }
    sServer.terminate();
}
