#ifndef NETINTFBUILDER_H
#define NETINTFBUILDER_H
#include "netlink.h"
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <thread>


class NetlinkInterface;
/*static int msg_handler(struct sockaddr_nl *nl, struct nlmsghdr *msg);*/

class AddressInfoBase{
    private:

    protected:
        AddressInfoBase(std::string sAddress);
        std::string sAddress_;
        unsigned int ifa_family_;
        unsigned int ifa_scope_; 
        unsigned int ifa_flags_;
        unsigned int ifa_prefixlen_;
    public:
        static AddressInfoBase* createAddressInfo(unsigned int family, std::string sAddress);
        std::string getAddress();

        void setFamily(unsigned int family);
        void setScope(unsigned int scope);
        void setFlags(unsigned int flags);
        unsigned int getFlags();
        virtual void setPrefixLength(unsigned int prefixlen);
        unsigned int getPrefixLength();

        virtual void parseData(){};
};

class IPv4AddressInfo:public AddressInfoBase{
    private:
        std::string sNetMask_;
    public:
        IPv4AddressInfo(std::string sAddress);
        virtual void setPrefixLength(unsigned int prefixlen);

        std::string getNetMask();
        void parseData();
};

class IPv6AddressInfo:public AddressInfoBase{
    public:
        IPv6AddressInfo(std::string sAddress);

        void parseData();
};


/* NetworkInterfaceInfo 
 *      Base class for NetworkIntface. This package contains most of information associate with network interface.
 *      
 *
 */
class NetworkInterfaceInfo{
    private:
        int iIntfIndex_;
        unsigned int uiDeviceTypeId_;
        std::string sIntfName_;
        std::string sMACAddress_;
        unsigned int uiMTUSize_;
        unsigned int uiSpeedMbps_;
        bool bFullDuplex_;
        bool bAutoNeg_;
        int eLinkStatus_;
        bool bGlobalAddrStatus_;
    protected:
        std::map<std::string, IPv4AddressInfo*> IPv4AddressInfoCollection_;  
        std::map<std::string, IPv6AddressInfo*> IPv6AddressInfoCollection_;
        int getInterfaceIndex(); 
    public:
        NetworkInterfaceInfo(int iIntfIndex);
        std::string getInterfaceName();
        virtual ~NetworkInterfaceInfo();
        void eraseAddress();


        void setDeviceTypeId(unsigned int uiTypeId);  //http://elixir.free-electrons.com/linux/latest/source/include/uapi/linux/if_arp.h#L28
        unsigned int getDeviceTypeId();

        void setMACAddress(std::string sMACAddress);
        std::string getMACAddress();
        void setMTUSize(unsigned int uiMTUSize);
        unsigned int getMTUSize();
        void setSpeedMbps(unsigned int uiSpeedMbps);
        unsigned int getSpeedMbps();
        void setFullDuplex(bool isFullDuplex);
        bool isFullDuplex();
        void setAutoNeg(bool isAutoNeg);
        bool isAutoNeg();
        void setLinkStatus(int eLinkStatus);
        int getLinkStatus();
        void setGlobalAddrStatus(bool isGlobalAddrStatus);
        bool getGlobalAddrStatus();


        AddressInfoBase* getAddressInfo(int iFamily, std::string sAddress);
        IPv4AddressInfo* getIPv4AddressInfo(std::string sAddress);
        IPv6AddressInfo* getIPv6AddressInfo(std::string sAddress);
/*
        int putAddressInfo(IPv4AddressInfo* pAddressInfo);
        int putAddressInfo(IPv6AddressInfo* pAddressInfo);
*/

        IPv4AddressInfo* createIPv4AddressInfo(std::string sAddress);
        IPv6AddressInfo* createIPv6AddressInfo(std::string sAddress);

        void parseData();

        //TODO: This is way to "Redfish specific". Might move this to subclass.
        std::string sSchemaObjUUID;
        virtual int buildSchema();
        /*virtual int handleHttpPatch(std::string sRequestObjectId, std::string sRreturnChannelId);*/
};

//It might be able to apply to 
class RouteInfo{
    public:
        RouteInfo();
        virtual ~RouteInfo();
        bool bDefaultGateway;
        bool bGateway;
        unsigned char rtm_family;   /* Address family of route */
        unsigned char rtm_dst_len;  /* Length of destination */
        unsigned char rtm_protocol; /* Routing protocol; See http://man7.org/linux/man-pages/man7/rtnetlink.7.html */ 
        unsigned char rtm_scope;    /* Only collect RT_SCOPE_UNIVERSE */
        unsigned char rtm_type;     /* See below */
        std::string sRTA_DST;       /* oute destination address. */
        int iRTA_OIF;               /* Output interface index. */
        std::string sRTA_GATEWAY;   /* The gateway of the route */
        int iRTA_PRIORITY;          /* Priority of route. */

        void parseData();
};

class NetworkIntfFactoryMethods{
    public:
        virtual NetworkInterfaceInfo* createNetworkInterfaceInfo(int iIntfIndex) = 0;
        virtual void removeNetworkInterfaceInfo(int iIntfIndex) = 0;
        virtual NetworkInterfaceInfo* findNetworkInterfaceInfo(int iIntfIndex) = 0;

        virtual RouteInfo* createRouteInfo() = 0;
};

class NetworkIntfDector{
    private:
        NetworkIntfFactoryMethods* pNetIfFactory_;
    public:
        NetworkIntfDector(NetworkIntfFactoryMethods* pNetIfFactory);
        virtual ~NetworkIntfDector();
        NetworkIntfFactoryMethods* getNetIfFactory();
        virtual int startDiscovery() = 0;
};

/* nlNetworkIntfDector: 
 *      Using Netlink eventing to manage lifecycle of network interfaces.
 *
 *
 */
class nlNetworkIntfDector:public NetworkIntfDector{
    private:
        bool inWorkingState_;                   //Used to control network interface event handler.
        std::thread* thNetIntfEventHandler_;    //C++11 thread used to receive netlink event.
        void NetIntfEventHandler_();            //Hook function for thread.
        int nl_read_event_(int sockint);        //Event handler.
    public:
        nlNetworkIntfDector(NetworkIntfFactoryMethods* pNetIfFactory);
        virtual ~nlNetworkIntfDector();
        virtual int startDiscovery(); 
};

class NetworkIntfBuilder: public NetworkIntfFactoryMethods {
    private:
        NetlinkInterface* nlIntf;   //Interface used to collect network information.
        NetworkIntfDector* netIntfDector_; //Interface used to detect network interface status change.
        std::map<int, NetworkInterfaceInfo*> IntfInfoCollection_;   //Doscovered network interfaces in the system.
        std::vector<RouteInfo*> RouteInfoCollection_;   //Rtoute table for the test.

        std::string G_OwnerId;
    public:
        virtual NetworkInterfaceInfo* createNetworkInterfaceInfo(int iIntfIndex);   //From factory...
        virtual void removeNetworkInterfaceInfo(int iIntfIndex);
        virtual NetworkInterfaceInfo* findNetworkInterfaceInfo(int iIntfIndex);
        virtual RouteInfo* createRouteInfo();
    public:
        //NetworkIntfBuilder();
        NetworkIntfBuilder();
        virtual ~NetworkIntfBuilder();

        void setOwnerId(std::string sOwnerId);
        int notifyOwner(bool bConstruct);
        
        void parseData();
        
        void destructResources();

        NetworkInterfaceInfo* findSchemaHandler(std::string sSchemaObjUUID);
        
        void collectResource();
        void reviseInterface();
        void buildSchema();
};
#endif
