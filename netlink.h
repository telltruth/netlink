#ifndef NETLINK_INTERFACE_H
#define NETLINK_INTERFACE_H
#include "NetIntfBuilder.h"
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>
class NetworkIntfFactoryMethods;
class NetworkInterfaceInfo;

class scopeSocket{
    private:
        int fd_;
    public:
        scopeSocket(int iDomain, int iType, int iProtocol);
        virtual ~scopeSocket();
        int getfd(); 
};

class scopeNetlinkSocket: public scopeSocket{
    public:
        scopeNetlinkSocket();
        virtual ~scopeNetlinkSocket();
};

//Utils
class EthtoolInterface{
    public:
        static int renewNetIntfStatus(NetworkInterfaceInfo* pNetIntfInfo);
        //Warning: Linux Kernel 4.6.1 ==> struct ethtool_cmd - DEPRECATED, link control and status
        //http://elixir.free-electrons.com/linux/v4.6.1/source/include/uapi/linux/ethtool.h
};
class NetlinkInterface{
    private:
        unsigned int uiPageSize_;
        NetworkIntfFactoryMethods* pNetIfFactory_;
        int send_RTM_GETADDR();
        void process_RTM_GETADDR(struct nlmsghdr *nlmsg_ptr, int nlmsg_len);
        int send_RTM_GETLINK();
        void process_RTM_GETLINK(struct nlmsghdr *nlmsg_ptr, int nlmsg_len);
        int send_RTM_GETROUTE();
        void process_RTM_GETROUTE(struct nlmsghdr *nlmsg_ptr, int nlmsg_len);
    public:
        NetlinkInterface(NetworkIntfFactoryMethods* pNetIfFactory);
        bool IsInterfaceCarrier(std::string sIntfName);

        int createInterfaceInfo();
        int nlCollectInterfaceInfo();
        int nlCollectRouteInfo();
};


#endif
