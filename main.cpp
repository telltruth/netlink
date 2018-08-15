#include <string.h>
#include <iostream>
#include "NetIntfBuilder.h"

using std::cout;
using std::endl;

int main(int argc, const char *argv[])
{
    cout << __FUNCTION__ << endl;
    NetworkIntfBuilder netifBuilder;
    netifBuilder.collectResource(); 
    return 0;
}
