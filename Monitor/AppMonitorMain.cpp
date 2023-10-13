#include <windows.h>
#include "AppMonitor.h"

int main()
{
    Monitor m;
    m.init();
    while(true)
    {
        if (!m.check())
        {
            printf("Reset server\n");
            m.reset();
            if (!m.init()) // in practice some sort of alarm should be raised here
            {
                printf("Can't initialize monitor\n");
                return -1;
            }
        }
    }    
}