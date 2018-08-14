#include "netlink.h"
#include <stdio.h>
#include <iostream>

#include <vector>

class NetworkIntfDector{
    public:
        NetworkIntfDector();
        virtual ~NetworkIntfDector();
        virtual int startDiscovery() = 0;
};

class nlNetworkIntfDector:public NetworkIntfDector{
    private:
        bool inWorkingState_;                   //Used to control network interface event handler.
        std::thread* thNetIntfEventHandler_;    //C++11 thread used to receive netlink event.
        void NetIntfEventHandler_();            //Hook function for thread.
        int nl_read_event_(int sockint);        //Event handler.
    public:
        nlNetworkIntfDector();
        virtual ~nlNetworkIntfDector();
        virtual int startDiscovery(); 
};

class NetworkIntfBuilder{
    private:
        NetlinkInterface* nlIntf;   //Interface used to collect network information.
        NetworkIntfDector* netIntfDector_; //Interface used to detect network interface status change.
    public:
        NetworkIntfBuilder();
        ~NetworkIntfBuilder();
        void collectResource();
};
