#include "NetIntfBuilder.h"
#include "SchemaFactory.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <arpa/inet.h>  //inet_ntop
#include <net/if.h> //if_indextoname

#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

using std::endl;
using std::cout;
using std::string;
using std::setw;

AddressInfoBase::AddressInfoBase(string sAddress)
:sAddress_(sAddress),
ifa_family_(0),
ifa_scope_(0),
ifa_flags_(0),
ifa_prefixlen_(0)
{

}

std::string AddressInfoBase::getAddress(){
    return sAddress_;
}

void AddressInfoBase::setFamily(unsigned int family){
    ifa_family_ = family;
}

void AddressInfoBase::setScope(unsigned int scope){
    ifa_scope_ = scope;
}

void AddressInfoBase::setFlags(unsigned int flags){
    ifa_flags_ = flags;
}

unsigned int AddressInfoBase::getFlags(){
    return ifa_flags_;
}

void AddressInfoBase::setPrefixLength(unsigned int prefixlen){
    ifa_prefixlen_ = prefixlen;
}

unsigned int AddressInfoBase::getPrefixLength(){
    return ifa_prefixlen_;
}

IPv4AddressInfo::IPv4AddressInfo(std::string sAddress)
:AddressInfoBase(sAddress),
sNetMask_("")
{
    //cout << "IPv4 Address = " << sAddress << endl;
}

void IPv4AddressInfo::setPrefixLength(unsigned int prefixlen){
    //This also convert prefix to mask. Example: /24 ==> 255.255.255.0
    char ipaddr_str[INET6_ADDRSTRLEN] = {0};
    AddressInfoBase::setPrefixLength(prefixlen);
    uint32_t ipv4Netmask = 0xFFFFFFFF;
    ipv4Netmask <<= 32 - prefixlen;
    ipv4Netmask = ntohl(ipv4Netmask);
    inet_ntop(AF_INET, &ipv4Netmask, ipaddr_str, sizeof(ipaddr_str));
    sNetMask_ = ipaddr_str;
}

std::string IPv4AddressInfo::getNetMask(){
    return sNetMask_;
}

void IPv4AddressInfo::parseData(){
    cout << "    == IPv4 Family ==" << endl;
    cout << "        Family: "  << ifa_family_  << endl;
    cout << "        Address: " << sAddress_  << endl;
    cout << "        PrefixLength: " << ifa_prefixlen_  << endl;
    cout << "        Scope: " << ifa_scope_  << endl;
    cout << "        Flags: " << ifa_flags_  << endl;
}

IPv6AddressInfo::IPv6AddressInfo(std::string sAddress)
:AddressInfoBase(sAddress)
{
    //cout << "IPv6 Address = " << sAddress << endl;
}

void IPv6AddressInfo::parseData(){
    cout << "    == IPv6 Family ==" << endl;
    cout << "        Family: "  << ifa_family_  << endl;
    cout << "        Address: " << sAddress_  << endl;
    cout << "        PrefixLength: " << ifa_prefixlen_  << endl;
    cout << "        Scope: " << ifa_scope_  << endl;
    cout << "        Flags: " << ifa_flags_  << endl;
}


NetworkInterfaceInfo::NetworkInterfaceInfo(int iIntfIndex)
:iIntfIndex_(iIntfIndex),
sIntfName_(""),
uiDeviceTypeId_(0),
sMACAddress_(""),
uiMTUSize_(1500),
uiSpeedMbps_(100),
bFullDuplex_(true),
bAutoNeg_(true),
bGlobalAddrStatus_(false),
sSchemaObjUUID("")
{
    char name_str2[IFNAMSIZ];
    if_indextoname(iIntfIndex, name_str2);
    sIntfName_ = name_str2;

    //cout << "Interface: " << sIntfName_ << "( " << iIntfIndex_ << " ) Constructed." << endl;
}

NetworkInterfaceInfo::~NetworkInterfaceInfo(){
    this->eraseAddress();
}

void NetworkInterfaceInfo::eraseAddress(){

    //TODO: Required to use MUTEX...    
    while (!IPv4AddressInfoCollection_.empty()){
        IPv4AddressInfo* pIPv4AddressInfo = (IPv4AddressInfoCollection_.begin())->second;
        if (NULL != pIPv4AddressInfo){
            delete pIPv4AddressInfo;
        }
        IPv4AddressInfoCollection_.erase(IPv4AddressInfoCollection_.begin());
    }
    //TODO: Required to use MUTEX...    
    while (!IPv6AddressInfoCollection_.empty()){
        IPv6AddressInfo* pIPv6AddressInfo = (IPv6AddressInfoCollection_.begin())->second;
        if (NULL != pIPv6AddressInfo){
            delete pIPv6AddressInfo;
        }
        IPv6AddressInfoCollection_.erase(IPv6AddressInfoCollection_.begin());
    }
    if (0 > EthtoolInterface::renewNetIntfStatus(this)){
        cout << "Renew interface information failed." << endl;
    }
}


void NetworkInterfaceInfo::setDeviceTypeId(unsigned int uiTypeId){  //http://elixir.free-electrons.com/linux/latest/source/include/uapi/linux/if_arp.h#L28
    uiDeviceTypeId_ = uiTypeId;
}
unsigned int NetworkInterfaceInfo::getDeviceTypeId(){
    return uiDeviceTypeId_;
}

void NetworkInterfaceInfo::setMACAddress(std::string sMACAddress){
    sMACAddress_ = sMACAddress;
}
std::string NetworkInterfaceInfo::getMACAddress(){
    return sMACAddress_;
}
void NetworkInterfaceInfo::setMTUSize(unsigned int uiMTUSize){
    uiMTUSize_ = uiMTUSize;
}
unsigned int NetworkInterfaceInfo::getMTUSize(){
    return uiMTUSize_;
}
void NetworkInterfaceInfo::setSpeedMbps(unsigned int uiSpeedMbps){
    uiSpeedMbps_ = uiSpeedMbps;
}
unsigned int NetworkInterfaceInfo::getSpeedMbps(){
    return uiSpeedMbps_;
}
void NetworkInterfaceInfo::setFullDuplex(bool isFullDuplex){
    bFullDuplex_ = isFullDuplex;
}
bool NetworkInterfaceInfo::isFullDuplex(){
    return bFullDuplex_;
}
void NetworkInterfaceInfo::setAutoNeg(bool isAutoNeg){
    bAutoNeg_ = isAutoNeg;
}
bool NetworkInterfaceInfo::isAutoNeg(){
    return bAutoNeg_;
}
void NetworkInterfaceInfo::setLinkStatus(int eLinkStatus)
{
    eLinkStatus_ = eLinkStatus;
}
int NetworkInterfaceInfo::getLinkStatus(){
    return eLinkStatus_;
}
void NetworkInterfaceInfo::setGlobalAddrStatus(bool isGlobalAddrStatus){
    bGlobalAddrStatus_ = isGlobalAddrStatus;
}
bool NetworkInterfaceInfo::getGlobalAddrStatus(){
    return bGlobalAddrStatus_;
}




AddressInfoBase* NetworkInterfaceInfo::getAddressInfo(int iFamily, std::string sAddress){
    //TODO: Required to use MUTEX...    
    AddressInfoBase* pAddressInfo = NULL;
    switch (iFamily){
        case AF_INET:
            {
                std::map<std::string, IPv4AddressInfo*>::iterator it = IPv4AddressInfoCollection_.find(sAddress); 
                if (it != IPv4AddressInfoCollection_.end()) pAddressInfo = it->second;
                break;
            }
        case AF_INET6: 
            {
                std::map<std::string, IPv6AddressInfo*>::iterator it = IPv6AddressInfoCollection_.find(sAddress); 
                if (it != IPv6AddressInfoCollection_.end()) pAddressInfo = it->second;
                break;
            }
        default:
            {
                cout << "Unknown family = " << iFamily << endl;
                break;
            }
    }
    return pAddressInfo;
}
/*
int NetworkInterfaceInfo::putAddressInfo(IPv4AddressInfo* pAddressInfo){
    //TODO: Required to use MUTEX...    
    if (pAddressInfo == NULL) return -1;

    std::map<std::string, IPv4AddressInfo*>::iterator it = IPv4AddressInfoCollection_.find(pAddressInfo->getAddress()); 
    if (it == IPv4AddressInfoCollection_.end()){
        IPv4AddressInfoCollection_[pAddressInfo->getAddress()] = pAddressInfo;
        return 1;
    }else{
        cout << "TODO: Not sure if we need to replace existed one..." << endl;
        return 0;
    }
}

int NetworkInterfaceInfo::putAddressInfo(IPv6AddressInfo* pAddressInfo){
    //TODO: Required to use MUTEX...    
    if (pAddressInfo == NULL) return -1;

    std::map<std::string, IPv6AddressInfo*>::iterator it = IPv6AddressInfoCollection_.find(pAddressInfo->getAddress()); 
    if (it == IPv6AddressInfoCollection_.end()){
        IPv6AddressInfoCollection_[pAddressInfo->getAddress()] = pAddressInfo;
        return 1;
    }else{
        cout << "TODO: Not sure if we need to replace existed one..." << endl;
        return 0;
    }
}
*/

IPv4AddressInfo* NetworkInterfaceInfo::createIPv4AddressInfo(std::string sAddress){
    std::map<std::string, IPv4AddressInfo*>::iterator it = IPv4AddressInfoCollection_.find(sAddress); 
    if (it != IPv4AddressInfoCollection_.end()){
        IPv4AddressInfo* pIPv4AddressInfo = it->second;
        return pIPv4AddressInfo;
    }
    IPv4AddressInfo* pIPv4AddressInfo = new IPv4AddressInfo(sAddress);
    if (pIPv4AddressInfo){
        IPv4AddressInfoCollection_[sAddress] = pIPv4AddressInfo;
    }
    return pIPv4AddressInfo;
}

IPv6AddressInfo* NetworkInterfaceInfo::createIPv6AddressInfo(std::string sAddress){
    std::map<std::string, IPv6AddressInfo*>::iterator it = IPv6AddressInfoCollection_.find(sAddress); 
    if (it != IPv6AddressInfoCollection_.end()){
        IPv6AddressInfo* pIPv6AddressInfo = it->second;
        return pIPv6AddressInfo;
    }
    IPv6AddressInfo* pIPv6AddressInfo = new IPv6AddressInfo(sAddress);
    if (pIPv6AddressInfo){
        IPv6AddressInfoCollection_[sAddress] = pIPv6AddressInfo;
    }
    return pIPv6AddressInfo;
}


string NetworkInterfaceInfo::getInterfaceName(){
    return sIntfName_; 
}

int NetworkInterfaceInfo::getInterfaceIndex(){
    return iIntfIndex_; 
}

void NetworkInterfaceInfo::parseData(){
    //TODO: Required to use MUTEX...    
    cout << "== " << sIntfName_ << " Interface ==" << endl;
    std::map<std::string, IPv4AddressInfo*>::iterator itIPv4 = IPv4AddressInfoCollection_.begin();
    while (itIPv4 != IPv4AddressInfoCollection_.end()){
        if (itIPv4->second != NULL){
            itIPv4->second->parseData();
        }
        itIPv4++;
    }
    cout << endl << endl;
    std::map<std::string, IPv6AddressInfo*>::iterator itIPv6 = IPv6AddressInfoCollection_.begin();
    while (itIPv6 != IPv6AddressInfoCollection_.end()){
        if (itIPv6->second != NULL){
            itIPv6->second->parseData();
        }
        itIPv6++;
    }
}


int NetworkInterfaceInfo::buildSchema(){
    cout << __FUNCTION__ << __LINE__ << " not implemented function." << endl;
    return 0;
}

//int NetworkInterfaceInfo::handleHttpPatch(std::string sRequestObjectId, std::string sRreturnChannelId){
//cout << __FUNCTION__ << __LINE__ << " not implemented function." << endl;
//return 0;
//}

RouteInfo::RouteInfo()
:bDefaultGateway(false),
bGateway(false),
rtm_family(0),
rtm_dst_len(0),
rtm_protocol(0),
rtm_scope(0),
rtm_type(0),
sRTA_DST(""),
iRTA_OIF(-1),
sRTA_GATEWAY(""),
iRTA_PRIORITY(-1)
{

}

RouteInfo::~RouteInfo(){

}

void RouteInfo::parseData(){
    cout << "Family: " << setw(3) << (unsigned int) rtm_family;
    cout << ", OIF: " << setw(3) <<iRTA_OIF;
    cout << ", Priority: " << setw(4) << iRTA_PRIORITY;
    if (rtm_dst_len >0){
        cout << ", DestAddr: " << sRTA_DST << "/" << (unsigned int) rtm_dst_len;
    }
    if(bGateway){
        cout << endl;
        cout << "      : ";
        if(bDefaultGateway){
            cout << "Default Gateway: " << sRTA_GATEWAY;
        }else{
            cout << "Gateway: " << sRTA_GATEWAY;
        }
    }
    cout << endl;
}

NetworkIntfDector::NetworkIntfDector(NetworkIntfFactoryMethods* pNetIfFactory)
:pNetIfFactory_(pNetIfFactory)
{

}

NetworkIntfDector::~NetworkIntfDector(){

}

NetworkIntfFactoryMethods* NetworkIntfDector::getNetIfFactory(){
    return pNetIfFactory_; 
}

NetworkIntfBuilder::NetworkIntfBuilder()
:nlIntf(NULL),
netIntfDector_(NULL)
{
    nlIntf = new NetlinkInterface(this);
    if (NULL != (netIntfDector_ = new nlNetworkIntfDector(this))){
        if(0 > netIntfDector_->startDiscovery()){
            cout << "Interface discovery may not working." << endl;
        }
    } 
}

NetworkIntfBuilder::~NetworkIntfBuilder(){
    if (nlIntf) delete nlIntf;
    if (netIntfDector_) delete netIntfDector_;
}


void NetworkIntfBuilder::destructResources(){
    //TODO: Required to use MUTEX...    
    while (!IntfInfoCollection_.empty()){
        NetworkInterfaceInfo* pNetworkInterfaceInfo = IntfInfoCollection_.begin()->second;
        if (NULL != pNetworkInterfaceInfo){
            delete pNetworkInterfaceInfo;
            IntfInfoCollection_.erase(IntfInfoCollection_.begin());
        }
    }
    //TODO: Required to use MUTEX...    
    while (!RouteInfoCollection_.empty()){
        RouteInfo* pRouteInfo = *(RouteInfoCollection_.begin());
        if (NULL != pRouteInfo){
            delete pRouteInfo;
            RouteInfoCollection_.erase(RouteInfoCollection_.begin());
        }
    }
}


void NetworkIntfBuilder::parseData(){
    //TODO: Required to use MUTEX...    
    std::map<int, NetworkInterfaceInfo*>::iterator nii_it = IntfInfoCollection_.begin();
    while (nii_it != IntfInfoCollection_.end()){
        if (nii_it->second != NULL){
            nii_it->second->parseData();
        }
        nii_it++;
    }

    //TODO: Required to use MUTEX...    
    std::vector<RouteInfo*>::iterator ri_it = RouteInfoCollection_.begin();
    while (ri_it != RouteInfoCollection_.end()){
        (*ri_it)->parseData();
        ri_it++;
    }
}


#if 0
void NetworkIntfBuilder::reviseInterface() {
    string lastInterfaceName;
    std::map<int, NetworkInterfaceInfo*>::iterator it = IntfInfoCollection_.begin();
    for (auto it = IntfInfoCollection_.rbegin(); it != IntfInfoCollection_.rend(); ++it) {
        if (it->second != NULL){
            string infName = it->second->getInterfaceName();
            if (infName.compare("lo")==0) {
                continue;
            }
            if (lastInterfaceName.size()) {
                if (infName.compare(0, infName.length(), lastInterfaceName, 0, infName.length())==0) {
                    lastInterfaceName = infName; 
                    continue;
                }
            }
            OsinetInterface* pOsinetInterface = OsinetInterface::getInstance();
            if (NULL == pOsinetInterface) return;
            if (pOsinetInterface->IsInterfaceValid((char*)infName.c_str())) {

            }
            else {
                this->removeNetworkInterfaceInfo(it->first); 
            }
            lastInterfaceName = infName;
        }
    }
}
#endif

void NetworkIntfBuilder::collectResource(){
    //TODO: Required to use MUTEX...    
    std::map<int, NetworkInterfaceInfo*>::iterator nii_it = IntfInfoCollection_.begin();
    while (nii_it != IntfInfoCollection_.end()){
        if (nii_it->second != NULL){
            nii_it->second->eraseAddress();
        }
        nii_it++;
    }
    if (NULL != nlIntf){
        if (0 > nlIntf->nlCollectInterfaceInfo()){
            cout << "Got something wrong while nlCollectInterfaceInfo. It may not discover all interfaces. " << endl;
        }
        if (0 > nlIntf->nlCollectRouteInfo()){
            cout << "Got something wrong while nlCollectRouteInfo. It may not discover all route entries. " << endl;
        }
    } 
}

void NetworkIntfBuilder::buildSchema(){
    //TODO: Required to use MUTEX...    
    std::map<int, NetworkInterfaceInfo*>::iterator it = IntfInfoCollection_.begin();
    while (it != IntfInfoCollection_.end()){
        if (it->second != NULL){
            it->second->buildSchema();
        }
        it++;
    }
}

void NetworkIntfBuilder::setOwnerId(std::string sOwnerId){
    G_OwnerId = sOwnerId; 
}

int NetworkIntfBuilder::notifyOwner(bool isPresence){
    if (isPresence){
        int rc = 0;
        return rc;
    }else{
        int rc = 0;
        return rc;
    }
}

NetworkInterfaceInfo* NetworkIntfBuilder::findNetworkInterfaceInfo(int iIntfIndex){
    NetworkInterfaceInfo* pNetworkInterfaceInfo = NULL;
    std::map<int, NetworkInterfaceInfo*>::iterator it = IntfInfoCollection_.find(iIntfIndex);
    if (it != IntfInfoCollection_.end()) pNetworkInterfaceInfo = it->second;
     
    return pNetworkInterfaceInfo;
}

void NetworkIntfBuilder::removeNetworkInterfaceInfo(int iIntfIndex){
    NetworkInterfaceInfo* pNetworkInterfaceInfo = NULL;
    std::map<int, NetworkInterfaceInfo*>::iterator it = IntfInfoCollection_.find(iIntfIndex);
    if (it != IntfInfoCollection_.end()){
        pNetworkInterfaceInfo = it->second;
        IntfInfoCollection_.erase(it);
        delete pNetworkInterfaceInfo;
    }
}


RouteInfo* NetworkIntfBuilder::createRouteInfo(){
    RouteInfo* pRouteInfo = new RouteInfo();
    RouteInfoCollection_.push_back(pRouteInfo); 
    return pRouteInfo;
}




//TODO: Create a subclass and move "version" specific function to the subclass. Move the implmentation to SchemaFactory as it is version specific.
NetworkInterfaceInfo* NetworkIntfBuilder::createNetworkInterfaceInfo(int iIntfIndex){
    //TODO: We might lookup the interface name and construct objects depending on "eth0", "eth0.{vlan id}".
    //      We might bind different handler to SchemaHandler as well. 
    NetworkInterfaceInfo* pNetworkInterfaceInfo = NULL;
    pNetworkInterfaceInfo = new NetworkInterfaceAdapter(this, iIntfIndex);
    IntfInfoCollection_[iIntfIndex] = pNetworkInterfaceInfo;
    return pNetworkInterfaceInfo;
}

NetworkInterfaceInfo* NetworkIntfBuilder::findSchemaHandler(std::string sSchemaObjUUID){
    std::map<int, NetworkInterfaceInfo*>::iterator nii_it = IntfInfoCollection_.begin();
    while (nii_it != IntfInfoCollection_.end()){
        if (nii_it->second != NULL){
            if (sSchemaObjUUID == nii_it->second->sSchemaObjUUID){
                return nii_it->second;
            }
        }
        nii_it++;
    }
    return NULL;
}

nlNetworkIntfDector::nlNetworkIntfDector(NetworkIntfFactoryMethods* pNetIfFactory)
:NetworkIntfDector(pNetIfFactory),
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
        cout << "Unable to create socket." << endl;
        return;
    }
    if (-1 == bind(fd, (struct sockaddr *) &sa, sizeof(sa))){
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
                        if (NULL != this->getNetIfFactory()){
                            NetworkInterfaceInfo* nif = this->getNetIfFactory()->findNetworkInterfaceInfo(ifa->ifa_index);
                            if (nif == NULL){
                                if (NULL != (nif = this->getNetIfFactory()->createNetworkInterfaceInfo(ifa->ifa_index))){
                                    //nif->buildSchema();
                                }
                            }
                        }
                    }
                    else {
                        printf("    netlink_link_state: Link is DOWN - %d.\n", ifa->ifa_index);
                        if (NULL != this->getNetIfFactory()){
                            this->getNetIfFactory()->removeNetworkInterfaceInfo(ifa->ifa_index);
                        }
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
                    if (NULL != this->getNetIfFactory()){
                        cout << "Inteface " << ifname << " (" << ifa->ifa_index << ") is going to remove." << endl;
                        this->getNetIfFactory()->removeNetworkInterfaceInfo(ifa->ifa_index);
                    }
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

