#include <vector>
#include <fstream>

#include "Appclient.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"

int main (int argc, char *argv[])
{
    if(argc < 3 || split(argv[1], ":").size() != 2)
    {
        printf("USAGE: Client.exe <HOST:PORT> <MESSAGE>\n\n");
        printf("Example: Client.exe 192.168.1.1:12345 Hello world!\n");
        return 0;
    }
    
    std::string msg = argv[2];
    for(int i = 3; i < argc; i++)
        msg += std::string(" ") + argv[i];

    Client c;
    c.send(argv[1], msg);
}