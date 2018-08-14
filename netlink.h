#include <stdio.h>
#include <thread>
#include <iostream>

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

class NetlinkInterface{
    private:
        unsigned int uiPageSize_;
        int send_RTM_GETADDR();
        void process_RTM_GETADDR(struct nlmsghdr *nlmsg_ptr, int nlmsg_len);
        int send_RTM_GETLINK();
        void process_RTM_GETLINK(struct nlmsghdr *nlmsg_ptr, int nlmsg_len);
        int send_RTM_GETROUTE();
        void process_RTM_GETROUTE(struct nlmsghdr *nlmsg_ptr, int nlmsg_len);
    public:
        NetlinkInterface();

        int createInterfaceInfo();
        int nlCollectInterfaceInfo();
        int nlCollectRouteInfo();
};

