#include "netlink.h"
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
//#include <linux/if_addr.h>
//#include <arpa/inet.h>

#include <net/if.h>
//#include <inttypes.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <iomanip>

using namespace std;

/*******************************************************
 * Netlink structure 
 * https://tools.ietf.org/html/rfc3549
 * http://man7.org/linux/man-pages/man7/rtnetlink.7.html
 * *****************************************************/
//RTM_NEWADDR, RTM_DELADDR, RTM_GETADDR
struct sNetLinkReq_ifaddrmsg{
    struct nlmsghdr     nlmsg_info;
    struct ifaddrmsg    payload;
};

//RTM_NEWLINK, RTM_DELLINK, RTM_GETLINK
struct sNetLinkReq_ifinfomsg{
    struct nlmsghdr     nlmsg_info;
    struct ifinfomsg    payload;
};

//RTM_NEWROUTE, RTM_DELROUTE, RTM_GETROUTE
struct sNetLinkReq_rtmsg{
    struct nlmsghdr     nlmsg_info;
    struct rtmsg        payload;
};

//Helper function make some resource alive in a scope.
scopeSocket::scopeSocket(int iDomain, int iType, int iProtocol)
:fd_(-1)
{
    fd_ = socket(iDomain, iType, iProtocol);
    if(fd_ < 0){
        perror ("socket(): ");
    }
}

scopeSocket::~scopeSocket(){
    if (fd_ >= 0){ 
        close(fd_);
    }
}

int scopeSocket::getfd(){ 
    return fd_;
}

scopeNetlinkSocket::scopeNetlinkSocket()
:scopeSocket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE){

}

scopeNetlinkSocket::~scopeNetlinkSocket(){

}

NetlinkInterface::NetlinkInterface()
:uiPageSize_(4096)
{
    int pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize){
        uiPageSize_ = pagesize;
    }
    cout << "Page size = " << uiPageSize_ << endl;
}

int NetlinkInterface::send_RTM_GETLINK(){
    struct sNetLinkReq_ifinfomsg nl_req;
    int fd = -1;
    char read_buffer[uiPageSize_];
    scopeNetlinkSocket nl_socket;   

    struct nlmsghdr* nlmsg_ptr = NULL;
    int nlmsg_len = -1;
 
    if (0 > (fd = nl_socket.getfd())){
        return -1;
    }

    memset(&nl_req, 0, sizeof(nl_req));

    nl_req.nlmsg_info.nlmsg_len = NLMSG_LENGTH(sizeof(nl_req.payload));
    nl_req.nlmsg_info.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nl_req.nlmsg_info.nlmsg_type = RTM_GETLINK;
    nl_req.nlmsg_info.nlmsg_pid = getpid();

    nl_req.payload.ifi_family = AF_PACKET;

    if (-1 == send (fd, &nl_req, nl_req.nlmsg_info.nlmsg_len, 0)){
        perror ("send(): ");
        return -1;
    }

    while(1)
    {
        memset(read_buffer, 0, sizeof(read_buffer));

        if (-1 == (nlmsg_len = recv(fd, read_buffer, uiPageSize_, 0))){
            perror ("recv(): ");
            return -1;
        }
        nlmsg_ptr = (struct nlmsghdr *) read_buffer;

        // fprintf (stderr, "received %d bytes\n", rtn);

        if (nlmsg_len < sizeof (struct nlmsghdr))
        {
            fprintf (stderr, "received an uncomplete netlink packet\n");
            return -1;
        }

        if (nlmsg_ptr->nlmsg_type == NLMSG_DONE){
            break;
        }
        process_RTM_GETLINK(nlmsg_ptr, nlmsg_len);
    }
    return 0;
}

void NetlinkInterface::process_RTM_GETLINK(struct nlmsghdr *nlmsg_ptr, int nlmsg_len) 
{
    for(; NLMSG_OK(nlmsg_ptr, nlmsg_len); nlmsg_ptr = NLMSG_NEXT(nlmsg_ptr, nlmsg_len))
    {
        struct ifinfomsg *ifinfomsg_ptr = NULL;
        int ifinfomsg_len = 0;
        struct rtattr *rtattr_ptr = NULL;

        ifinfomsg_len = IFA_PAYLOAD(nlmsg_ptr);
        ifinfomsg_ptr = (struct ifinfomsg *) NLMSG_DATA(nlmsg_ptr);
        rtattr_ptr = (struct rtattr *) IFLA_RTA(ifinfomsg_ptr); //IFA_RTA and IFLA_RTA is depending on rta_type. Make sure you use correct one.

        //cout << "Handle interface Index = " << ifinfomsg_ptr->ifi_index << " ,Device Type = " << ifinfomsg_ptr->ifi_type << endl;

        // WARNING: You may get "disabled" network interface from this api. For now we just collect enabled interface.
        cout << "Index = " << ifinfomsg_ptr->ifi_index << " ,Device Type = " << ifinfomsg_ptr->ifi_type << endl;

        for(;RTA_OK(rtattr_ptr, ifinfomsg_len); rtattr_ptr = RTA_NEXT(rtattr_ptr, ifinfomsg_len)) {
            switch(rtattr_ptr->rta_type) {
                /*
                case IFLA_IFNAME:
                    {
                        char name_str[IFNAMSIZ];
                        snprintf(name_str, sizeof(name_str), "%s", (char *) RTA_DATA(rtattr_ptr));
                        cout << "   Name  = " << name_str << endl;
                        break;
                    }
                */
                case IFLA_ADDRESS:
                    {
                        char buffer[64];
                        unsigned char* ptr = (unsigned char*)RTA_DATA(rtattr_ptr);
                        snprintf(buffer, 64, "%02x:%02x:%02x:%02x:%02x:%02x", 
                            ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
                        cout << __FUNCTION__ << " - MACAddress: " << buffer << endl;
                        break;        
                    }
                case IFLA_MTU:
                    {
                        unsigned int *uiMtu = (unsigned int *) RTA_DATA(rtattr_ptr);
                        cout << __FUNCTION__ << " - MTUSize: " << *uiMtu << endl;
                        break;
                    }
            } 
        }   
    }
}

int NetlinkInterface::send_RTM_GETROUTE(){
    struct sNetLinkReq_rtmsg nl_req;
    int fd = -1;
    char read_buffer[uiPageSize_];
    scopeNetlinkSocket nl_socket;   
    struct nlmsghdr* nlmsg_ptr = NULL;
 
    if (0 > (fd = nl_socket.getfd())){
        return -1;
    }

    memset(&nl_req, 0, sizeof(nl_req));

    nl_req.nlmsg_info.nlmsg_len = NLMSG_LENGTH(sizeof(nl_req.payload));
    nl_req.nlmsg_info.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nl_req.nlmsg_info.nlmsg_type = RTM_GETROUTE;
    nl_req.nlmsg_info.nlmsg_pid = getpid();

    nl_req.payload.rtm_family = AF_PACKET;

    if (-1 == send (fd, &nl_req, nl_req.nlmsg_info.nlmsg_len, 0)){
        perror("send");
        return -1;
    }

    while(1)
    {
        int nlmsg_len = -1;
        memset(read_buffer, 0, sizeof(read_buffer));

        if (-1 == (nlmsg_len = recv(fd, read_buffer, uiPageSize_, 0))){
            perror ("recv(): ");
            return -1;
        }
        nlmsg_ptr = (struct nlmsghdr *) read_buffer;

        if (nlmsg_len < sizeof (struct nlmsghdr))
        {
            fprintf (stderr, "received an uncomplete netlink packet\n");
            return -1;
        }

        if (nlmsg_ptr->nlmsg_type == NLMSG_DONE){
            break;
        }
        process_RTM_GETROUTE(nlmsg_ptr, nlmsg_len);
    }
    return 0;
}

void NetlinkInterface::process_RTM_GETROUTE(struct nlmsghdr *nlmsg_ptr, int nlmsg_len){

    for(; NLMSG_OK(nlmsg_ptr, nlmsg_len); nlmsg_ptr = NLMSG_NEXT(nlmsg_ptr, nlmsg_len))
    {
        struct rtmsg *rtmsg_ptr = NULL;
        int rtmsg_len = 0;
        struct rtattr *rtattr_ptr = NULL;

        rtmsg_len = IFA_PAYLOAD(nlmsg_ptr);
        rtmsg_ptr = (struct rtmsg *) NLMSG_DATA(nlmsg_ptr);
        rtattr_ptr = (struct rtattr *) RTM_RTA(rtmsg_ptr); //IFA_RTA and IFLA_RTA is depending on rta_type. Make sure you use correct one.
        //if (RTPROT_KERNEL == rtmsg_ptr->rtm_protocol){ 
        
        //if (rtmsg_ptr->rtm_table != RT_TABLE_MAIN) continue;
        if (rtmsg_ptr->rtm_table != RT_TABLE_MAIN){
            //cout << "***** RTM_TABLE = " << (unsigned int ) rtmsg_ptr->rtm_table << endl;
            continue;
        }
        //if (rtmsg_ptr->rtm_family != AF_INET6) continue;
        if (rtmsg_ptr->rtm_scope != RT_SCOPE_UNIVERSE) continue;
        
        cout << "RTM Family = " << (unsigned int) rtmsg_ptr->rtm_family << endl;
        cout << "    PrefixLength = " << (unsigned int) rtmsg_ptr->rtm_dst_len << endl;
        cout << "    Scope = " << (unsigned int) rtmsg_ptr->rtm_scope << endl;
        cout << "    Type = " << (unsigned int) rtmsg_ptr->rtm_type << endl;

   
        for(;RTA_OK(rtattr_ptr, rtmsg_len); rtattr_ptr = RTA_NEXT(rtattr_ptr, rtmsg_len)) {
            switch(rtattr_ptr->rta_type) {
                case RTA_OIF:
                    {
                        int *iOIF = (int *) RTA_DATA(rtattr_ptr);
                        cout << __FUNCTION__ << " - *iOIF: " << *iOIF << endl;
                        break;
                    }
                case RTA_PRIORITY:
                    {
                        int *iPriority = (int *) RTA_DATA(rtattr_ptr);
                        cout << __FUNCTION__ <<" - Priority: " << *iPriority << endl;
                        break;
                    }
                case RTA_GATEWAY:
                    {
                        char ipaddr_str[INET6_ADDRSTRLEN] = {0};
                        inet_ntop(rtmsg_ptr->rtm_family, RTA_DATA(rtattr_ptr), ipaddr_str, sizeof(ipaddr_str));
                        cout << __FUNCTION__ <<" - Gateway: " << ipaddr_str << endl;
                        break;        
                    }
/*
                case RTA_PREFSRC:
                    {
                        char ipaddr_str[INET6_ADDRSTRLEN];
                        inet_ntop(rtmsg_ptr->rtm_family, RTA_DATA(rtattr_ptr), ipaddr_str, sizeof(ipaddr_str));
                        cout << "        SRC = " << ipaddr_str << endl;
                        break;        
                    }
*/
                case RTA_DST:
                    {
                        char ipaddr_str[INET6_ADDRSTRLEN];
                        inet_ntop(rtmsg_ptr->rtm_family, RTA_DATA(rtattr_ptr), ipaddr_str, sizeof(ipaddr_str));
                        cout << __FUNCTION__ <<" - DST: " << ipaddr_str << endl;
                        break;        
                    }
            }
        } 
    }
}


int NetlinkInterface::send_RTM_GETADDR(){
    struct sNetLinkReq_ifaddrmsg nl_req;
    int fd = -1;
    char read_buffer[uiPageSize_];
    scopeNetlinkSocket nl_socket;   

    struct nlmsghdr* nlmsg_ptr = NULL;
    int nlmsg_len = -1;
 
    if (0 > (fd = nl_socket.getfd())){
        return -1;
    }

    memset(&nl_req, 0, sizeof(nl_req));

    nl_req.nlmsg_info.nlmsg_len = NLMSG_LENGTH(sizeof(nl_req.payload));
    nl_req.nlmsg_info.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nl_req.nlmsg_info.nlmsg_type = RTM_GETADDR;
    nl_req.nlmsg_info.nlmsg_pid = getpid();

    nl_req.payload.ifa_family = AF_PACKET;

    if (-1 == send (fd, &nl_req, nl_req.nlmsg_info.nlmsg_len, 0)){
        perror ("send(): ");
        return -1;
    }

    while(1)
    {
        memset(read_buffer, 0, sizeof(read_buffer));

        if (-1 == (nlmsg_len = recv(fd, read_buffer, uiPageSize_, 0))){
            perror ("recv(): ");
            return -1;
        }

        nlmsg_ptr = (struct nlmsghdr *) read_buffer;

        if (nlmsg_len < sizeof (struct nlmsghdr))
        {
            fprintf (stderr, "received an uncomplete netlink packet\n");
            return -1;
        }

        if (nlmsg_ptr->nlmsg_type == NLMSG_DONE){
            break;
        }
        process_RTM_GETADDR(nlmsg_ptr, nlmsg_len);
    }
    return 0;
}

void NetlinkInterface::process_RTM_GETADDR(struct nlmsghdr *nlmsg_ptr, int nlmsg_len) 
{
    for(; NLMSG_OK(nlmsg_ptr, nlmsg_len); nlmsg_ptr = NLMSG_NEXT(nlmsg_ptr, nlmsg_len))
    {
        struct ifaddrmsg *ifaddrmsg_ptr;
        struct rtattr *rtattr_ptr;
        int ifaddrmsg_len;
        char name_str[INET6_ADDRSTRLEN];
        char cacheinfo_str[INET6_ADDRSTRLEN];
        char ipaddr_str[INET6_ADDRSTRLEN];
        char scope_str[16];

/*
        char anycast_str[INET6_ADDRSTRLEN];
        char localaddr_str[INET6_ADDRSTRLEN];
        char bcastaddr_str[INET6_ADDRSTRLEN];
        char multicast_str[INET6_ADDRSTRLEN];
*/

        ifaddrmsg_ptr = (struct ifaddrmsg *) NLMSG_DATA(nlmsg_ptr);
        memset(&name_str, 0, sizeof(name_str));
        memset(&ipaddr_str, 0, sizeof(ipaddr_str));
        memset(&cacheinfo_str, 0, sizeof(cacheinfo_str));
        memset(&scope_str, 0, sizeof(scope_str));
        
/*
        anycast_str[0] = 0;
        ipaddr_str[0] = 0;
        localaddr_str[0] = 0;
        bcastaddr_str[0] = 0;
        multicast_str[0] = 0;
        scope_str[0] = 0;
*/
        rtattr_ptr = (struct rtattr *) IFA_RTA(ifaddrmsg_ptr);
        ifaddrmsg_len = IFA_PAYLOAD(nlmsg_ptr);
        cout << __FUNCTION__ << " - ifaddrmsg_ptr->ifa_index= " << ifaddrmsg_ptr->ifa_index << endl;

        char name_str2[IFNAMSIZ];
        if_indextoname(ifaddrmsg_ptr->ifa_index, name_str2);
        cout << __FUNCTION__ << " - ifa_index: " << ifaddrmsg_ptr->ifa_index << " - name: " << name_str2 << endl;
    #if 0
        if (strcmp(name_str2, "lo")) {
            cout << __FUNCTION__ << " - name_str2= " << name_str2  << endl;
            continue;
        }
    #endif

        if (ifaddrmsg_ptr->ifa_scope == RT_SCOPE_UNIVERSE) {
            strcpy (scope_str, "global");
            cout << __FUNCTION__ << ": global" << endl;
        }
        else if (ifaddrmsg_ptr->ifa_scope == RT_SCOPE_SITE) {
            strcpy (scope_str, "site");
            cout << __FUNCTION__ << ": site" << endl;
        }
        else if (ifaddrmsg_ptr->ifa_scope == RT_SCOPE_LINK) {
            strcpy (scope_str, "link");
            cout << __FUNCTION__ << ": link" << endl;
        }
        else if (ifaddrmsg_ptr->ifa_scope == RT_SCOPE_HOST) {
            strcpy (scope_str, "host");
            cout << __FUNCTION__ << ": host" << endl;
        }
        else if (ifaddrmsg_ptr->ifa_scope == RT_SCOPE_NOWHERE) {
            strcpy (scope_str, "nowhere");
            cout << __FUNCTION__ << ": nowhere" << endl;
        }
        else
            snprintf (scope_str, sizeof(scope_str), "%d", ifaddrmsg_ptr->ifa_scope);


        for(;RTA_OK(rtattr_ptr, ifaddrmsg_len); rtattr_ptr = RTA_NEXT(rtattr_ptr, ifaddrmsg_len)) {

            switch(rtattr_ptr->rta_type) {
            case IFA_ADDRESS:
                inet_ntop(ifaddrmsg_ptr->ifa_family, RTA_DATA(rtattr_ptr), ipaddr_str, sizeof(ipaddr_str));
                break;
/*
            case IFA_LOCAL:
                inet_ntop(ifaddrmsg_ptr->ifa_family, RTA_DATA(rtattr_ptr), localaddr_str, sizeof(localaddr_str));
                break;
            case IFA_BROADCAST:
                inet_ntop(ifaddrmsg_ptr->ifa_family, RTA_DATA(rtattr_ptr), bcastaddr_str, sizeof(bcastaddr_str));
                break;
            case IFA_ANYCAST:
                inet_ntop(ifaddrmsg_ptr->ifa_family, RTA_DATA(rtattr_ptr), anycast_str, sizeof(anycast_str));
                break;
            case IFA_MULTICAST:
                inet_ntop(ifaddrmsg_ptr->ifa_family, RTA_DATA(rtattr_ptr), multicast_str, sizeof(multicast_str));
                break;
            default:
                printf ("unknown rta_type: %d\n", (int)rtattr_ptr->rta_type);
                break; 
*/
            }
        }
        cout << __FUNCTION__<< " - ipaddr_str: " << ipaddr_str << endl;
        //In this scope, it has no difference between IPv4 and IPv6 address. Using their base class is ok.
        switch (ifaddrmsg_ptr->ifa_family){
            case AF_INET:
                {
                    cout << __FUNCTION__ << " - ifaddrmsg_ptr->ifa_scope: " << ifaddrmsg_ptr->ifa_scope << endl;
                    cout << __FUNCTION__ << " - ifaddrmsg_ptr->ifa_flags: " << ifaddrmsg_ptr->ifa_flags << endl;
                    cout << __FUNCTION__ << " - ifaddrmsg_ptr->ifa_prefixlen: " << ifaddrmsg_ptr->ifa_prefixlen << endl;
                    break;
                }
            case AF_INET6: 
                {
                    cout << __FUNCTION__ << " - ifaddrmsg_ptr->ifa_scope6: " << ifaddrmsg_ptr->ifa_scope << endl;
                    cout << __FUNCTION__ << " - ifaddrmsg_ptr->ifa_flags6: " << ifaddrmsg_ptr->ifa_flags << endl;
                    cout << __FUNCTION__ << " - ifaddrmsg_ptr->ifa_prefixlen6: " << ifaddrmsg_ptr->ifa_prefixlen << endl;
                    break;
                }
            default:
                {
                    cout << "Unsupported family for address." << endl;
                    continue;
                }
        }
    }
    printf("\n");
}



int NetlinkInterface::nlCollectInterfaceInfo(){
    int rc = 0;
    if (0 > send_RTM_GETADDR()){
        cout << "send_RTM_GETADDR got error." << endl;
        rc --;
    }
    if (0 > send_RTM_GETLINK()){
        cout << "send_RTM_GETLINK got error." << endl;
        rc --;
    } 
    return rc;
}

int NetlinkInterface::nlCollectRouteInfo(){
    return send_RTM_GETROUTE(); 
}

