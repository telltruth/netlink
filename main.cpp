#include <stdio.h>
#include <string.h>
#include <iostream>
#include "NetIntfBuilder.h"

using namespace std;

int main(int argc, const char *argv[])
{
    cout << __FUNCTION__ << endl;
    NetworkIntfBuilder netifBuilder;
    netifBuilder.collectResource(); 
    return 0;
}
