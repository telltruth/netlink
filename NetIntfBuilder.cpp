#include "NetIntfBuilder.h"

#include <arpa/inet.h>  //inet_ntop
#include <net/if.h> //if_indextoname
#include <string.h>
#include <iostream>

#include <linux/rtnetlink.h>
#include <sys/socket.h>

using std::cout;
using std::endl;
using std::string;
using std::thread;

NetworkIntfDector::NetworkIntfDector()
{

}

NetworkIntfDector::~NetworkIntfDector(){

}


nlNetworkIntfDector::nlNetworkIntfDector()
:NetworkIntfDector(),
thNetIntfEventHandler_(NULL),
inWorkingState_(false)
{
}

nlNetworkIntfDector::~nlNetworkIntfDector(){
    inWorkingState_ = false;
    if (thNetIntfEventHandler_){
        thNetIntfEventHandler_->join();    //Wait thread exit.
        delete thNetIntfEventHandler_; 
    }
}

int nlNetworkIntfDector::startDiscovery(){
    if (NULL == thNetIntfEventHandler_){
        thNetIntfEventHandler_ = new std::thread(std::bind(&nlNetworkIntfDector::NetIntfEventHandler_, this));
        return 0;
    }
    return -1;
}

void nlNetworkIntfDector::NetIntfEventHandler_(){
    struct sockaddr_nl sa;
    int fd = -1;
    scopeNetlinkSocket nl_socket;   

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = RTMGRP_LINK;

    if (0 > (fd = nl_socket.getfd())){
        std::cout << "Unable to create socket." << std::endl;
        return;
    }
    if (-1 == bind(fd, (struct sockaddr *)&sa, sizeof(sa))){
        perror("bind");
        return;     
    }

    inWorkingState_ = true;
    while (inWorkingState_){
        nl_read_event_(fd);
    }
    return;
}

int nlNetworkIntfDector::nl_read_event_(int sockint)
{
    int status;
    int ret = 0;
    char buf[4096];
    struct iovec iov = { buf, sizeof buf };
    struct sockaddr_nl snl;
    struct msghdr msg = { (void*)&snl, sizeof snl, &iov, 1, NULL, 0, 0};
    struct nlmsghdr *h;

    status = recvmsg(sockint, &msg, 0);

    if(status < 0)
    {
        /* Socket non-blocking so bail out once we have read everything */
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return ret;

        /* Anything else is an error */
        printf("read_netlink: Error recvmsg: %d\n", status);
        perror("read_netlink: Error: ");
        return status;
    }

    if(status == 0)
    {
        printf("read_netlink: EOF\n");
    }
    printf("=====[Netlink event]=====\n");
    /* We need to handle more than one message per 'recvmsg' */
    for(h = (struct nlmsghdr *) buf; NLMSG_OK (h, (unsigned int)status); h = NLMSG_NEXT (h, status))
    {
        /* Finish reading */
        if (h->nlmsg_type == NLMSG_DONE)
            return ret;

        /* Message is some kind of error */
        if (h->nlmsg_type == NLMSG_ERROR)
        {
            printf("read_netlink: Message is an error - decode TBD\n");
            return -1; // Error
        }
        switch (h->nlmsg_type)
        {
            case RTM_NEWADDR:
                printf("msg_handler: RTM_NEWADDR :\n");
                break;
            case RTM_DELADDR:
                printf("msg_handler: RTM_DELADDR :\n");
                break;
            case RTM_NEWLINK:
                {
                    struct ifaddrmsg *ifa= (struct ifaddrmsg *) NLMSG_DATA(h);
                    struct ifinfomsg *ifi= (struct ifinfomsg *) NLMSG_DATA(h);
                    char ifname[1024];
                    if_indextoname(ifa->ifa_index,ifname);
                    printf("RTM_NEWLINK (%s): ", ifname);
                    if (ifi->ifi_flags & IFF_UP)            printf("IFF_UP, ");
                    if (ifi->ifi_flags & IFF_BROADCAST)     printf("IFF_BROADCAST, ");
                    if (ifi->ifi_flags & IFF_DEBUG)         printf("IFF_DEBUG, ");
                    if (ifi->ifi_flags & IFF_LOOPBACK)      printf("IFF_LOOPBACK, ");
                    if (ifi->ifi_flags & IFF_POINTOPOINT)   printf("IFF_POINTOPOINT, ");
                    if (ifi->ifi_flags & IFF_NOTRAILERS)    printf("IFF_NOTRAILERS, ");
                    if (ifi->ifi_flags & IFF_RUNNING)       printf("IFF_RUNNING, ");
                    if (ifi->ifi_flags & IFF_NOARP)         printf("IFF_NOARP, ");
                    printf ("\n");
/*
                    char ifname[1024];
                    cout << __FUNCTION__ << " - ifa->ifa_index: " << ifa->ifa_index << endl;
                    if_indextoname(ifa->ifa_index,ifname);
                    printf("RTM_NEWLINK (%s): ", ifname);

                #if 0
                    if (strcmp(ifname, "lo")) {
                        printf("RTM_NEWLINK (%s)\n", ifname);
                        return ret;
                    }
                #endif

                    printf("    netlink_link_state: Link %s %s (Flag: %0X)\n", 
                        ifname,(ifi->ifi_flags & IFF_UP)?"Up":"Down", ifi->ifi_flags);
*/
                    //Network interface lifecycle management depending on ifi_flags...
                    if (ifi->ifi_flags & IFF_UP){
                        printf("    netlink_link_state: Link is UP - %d.\n", ifa->ifa_index);
                    }
                    else {
                        printf("    netlink_link_state: Link is DOWN - %d.\n", ifa->ifa_index);
                    }
/* Returned VLan Id is not really VLAN ID. Consider to get this information from osinet.
                    struct rtattr *rtattr_ptr = NULL;
                    int rtattrlen = IFA_PAYLOAD(h);
                    rtattr_ptr = (struct rtattr *) IFLA_RTA(ifi); //IFA_RTA and IFLA_RTA is depending on rta_type. Make sure you use correct one.
                    for (;RTA_OK(rtattr_ptr, rtattrlen); rtattr_ptr = RTA_NEXT(rtattr_ptr, rtattrlen)) {
                        switch(rtattr_ptr->rta_type) {
                            case IFLA_VLAN_ID:
                                {
                                    __u16 *uiVlanId = (__u16 *) RTA_DATA(rtattr_ptr);
                                    cout << "   " << "interface " << ifname << " VLAN Detected: " << *uiVlanId << endl;
                                    break;
                                }
                        }
                    }
*/
                }
                break;
            case RTM_DELLINK:
                {
                    printf("msg_handler: RTM_DELLINK :\n");
                    struct ifaddrmsg *ifa= (struct ifaddrmsg *) NLMSG_DATA(h);
                    //struct ifinfomsg *ifi= (struct ifinfomsg *) NLMSG_DATA(h);
                    char ifname[1024];
                    if_indextoname(ifa->ifa_index,ifname);
                    std::cout << "Inteface " << ifname << " (" << ifa->ifa_index << ") is going to remove." << std::endl;
                }
                break;
            default:
                printf("msg_handler: Unknown netlink nlmsg_type %d\n",
                        h->nlmsg_type);
                break;
        }

        
    }
    return ret;
}

using namespace std;

NetworkIntfBuilder::NetworkIntfBuilder()
:nlIntf(NULL),
netIntfDector_(NULL)
{
    nlIntf = new NetlinkInterface();
    if (NULL != (netIntfDector_ = new nlNetworkIntfDector())){
        if(0 > netIntfDector_->startDiscovery()){
            cout << "Interface discovery may not working." << endl;
        }
    }
}

NetworkIntfBuilder::~NetworkIntfBuilder()
{

}

void NetworkIntfBuilder::collectResource(){
    if (NULL != nlIntf){
        if (0 > nlIntf->nlCollectInterfaceInfo()){
            std::cout << "Got something wrong while nlCollectInterfaceInfo. It may not discover all interfaces. " << std::endl;
        }
        if (0 > nlIntf->nlCollectRouteInfo()){
            std::cout << "Got something wrong while nlCollectRouteInfo. It may not discover all route entries. " << std::endl;
        }
    } 
}
