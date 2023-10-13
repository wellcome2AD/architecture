#define _CRT_SECURE_NO_WARNINGS

#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>

#include <vector>
#include <string>
#include "AppMonitor.h"
#include "helpers/Process.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"

static Process sServer;
static std::string sPort = "0";

bool Monitor::init()
{
    m_console.handleCtrlC(Monitor::reset); // if Monitor's execution is aborted via Ctrl+C, reset() cleans up its internal state
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    remove("resources/CREATED");
    bool ok = sServer.create(cmd); // launching Server
    printf(ok ? "monitoring \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd);
    return ok;
}

bool Monitor::check()
{
    printf("PID of current server = %s\n", sServer.pid().c_str());
    printf("FILENAME = %s\n", (std::string("resources/ALIVE") + sServer.pid()).c_str());
    auto filename = (std::string("resources/ALIVE") + sServer.pid()).c_str();
    if (fopen(filename, "rb") == NULL) {
        return false;
    }
    struct stat result;
    if (stat(filename, &result) == 0)
    {
        auto mod_time = result.st_mtime;
        if (time(NULL) - mod_time > 5000)
        {
            printf("Server isn't alive for more than %ld sec\n", 5000 / CLOCKS_PER_SEC);
            return false;
        }
    }
    sServer.wait(3000);
    return true;
}

void Monitor::reset()
{
    printf("PID of current server = %s\n", sServer.pid().c_str());
    auto filename = (std::string("resources/ALIVE") + sServer.pid()).c_str();
    remove(filename);
    sServer.terminate();
}
