#include <windows.h>
#include <algorithm>

#include "AppMonitor.h"

int main()
{
    Monitor m;
    m.init();
    while(true)
    {
        std::vector<bool> processes_live;        
        processes_live = m.check();
        m.reset(processes_live);
        bool all_are_alive = std::all_of(processes_live.begin(), processes_live.end(), [](bool v1) { return v1; });
        if (!all_are_alive)
        {
            printf("---INITIALIZE---\n");
            if (!m.init(processes_live)) // in practice some sort of alarm should be raised here
            {
                printf("Can't initialize monitor\n");
                return -1;
            }
            printf("----------------\n");
        }
    }    
}