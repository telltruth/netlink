#include "SchemaFactory.h"
#include <iostream>
#include <linux/if_addr.h>
#include <arpa/inet.h>

#if 0
#include "EthernetInterface.v1_1_0_MergepointEthernetInterface.v1_0_0_PluginInit.inc"
#ifndef IFA_F_DADFAILED
  #define IFA_F_DADFAILED 0x08
#endif
#include "osinet_interface.h"

using namespace std;

SchemaHandler::SchemaHandler(){
}

SchemaHandler::~SchemaHandler(){

}


IPv6ConfigBuilder::IPv6ConfigBuilder(const sIF_Conf* pIfConf)
:pIpv6Config_(NULL)
{
    this->buildIPv6ConfigProperty(pIfConf);
}

IPv6ConfigBuilder::~IPv6ConfigBuilder(){
    cleanIPv6AddressSetCollection(pIpv6Config_);
    cleanIPv6StaticRouterCollection(pIpv6Config_);
    cleanIPv6DynamicRouterCollection(pIpv6Config_);
    if (pIpv6Config_){
        std::string object_id = pIpv6Config_->getIndex();
        srObjRemoveObject(object_id.c_str());
        delete pIpv6Config_;
    }
}

RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* IPv6ConfigBuilder::getRFObject(){
    return pIpv6Config_;
}

int IPv6ConfigBuilder::initIpv6Config(){
    RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config = new RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration();
    if (pIpv6Config){
        pIpv6Config_ = pIpv6Config;
        rolInitialProperty_t propertyDocument[] = {
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_ALTERNATIVEDNS, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_DHCPV6ENABLED, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_DUID, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_DYNAMICROUTERENABLED, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_HOPLIMIT, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_IPV6ENABLED, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_STATICADDRESSESENABLED, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_IPV6STATICADDRESSES, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_IPV6STATICROUTERS, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_IPV6DYNAMICROUTERS, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_NAMESERVERORIGIN, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_PREFERREDDNS, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_SLAACENABLED, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_STATICROUTERENABLED, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6CONFIGURATION_STR_TRAFFICCLASS, SR_PROPERTY_NONINTERPRET}
        };
        pIpv6Config_->initObject(propertyDocument, 15);
        pIpv6Config_->writeToCache();
        return 0;
    }
    return -1;
}

void IPv6ConfigBuilder::buildIPv6ConfigProperty(const sIF_Conf* pIfConf){
    if (NULL == pIfConf) return;
    if (NULL == pIpv6Config_){
        if (0>this->initIpv6Config()){
            cout << "Init Ipv6Config failed." << endl;
            return;
        }
    }

    pIpv6Config_->setPreferredDNS(pIfConf->ifIPv6.szDNS1);
    pIpv6Config_->setAlternativeDNS(pIfConf->ifIPv6.szDNS2);
    pIpv6Config_->setDUID(pIfConf->ifIPv6.aDUID[0]);
    pIpv6Config_->setTrafficClass(pIfConf->ifIPv6.u16TrafficClass);
    pIpv6Config_->setHopLimit(pIfConf->ifIPv6.u16HopLimit);


    pIpv6Config_->setIPv6Enabled(RFL_FALSE);
    if (LAN_TRUE == pIfConf->ifIPv6.u8Enabled){
        pIpv6Config_->setIPv6Enabled(RFL_TRUE);
    }


    uint16_t    u16AutoConfOpt = pIfConf->ifIPv6.u16AutoConfOpt;

    pIpv6Config_->setDHCPv6Enabled(RFL_FALSE);
    if (V6_AUTOCONF_DHCP_ENABLE & u16AutoConfOpt){
        pIpv6Config_->setDHCPv6Enabled(RFL_TRUE);
    }

    pIpv6Config_->setSLAACEnabled(RFL_FALSE);
    if (V6_AUTOCONF_RA_ENABLE & u16AutoConfOpt){
        pIpv6Config_->setSLAACEnabled(RFL_TRUE);
    }

    pIpv6Config_->setStaticAddressesEnabled(RFL_FALSE);
    if (V6_AUTOCONF_STATIC_ENABLE & u16AutoConfOpt){
        pIpv6Config_->setStaticAddressesEnabled(RFL_TRUE);
    }
    if (V6_AUTOCONF_OPT_GET_DNS & u16AutoConfOpt){ 
        pIpv6Config_->setNameServerOrigin("DHCPv6");
    }else{
        pIpv6Config_->setNameServerOrigin("Static");
    }

   
    uint8_t u8RouterConfig = pIfConf->ifIPv6.u8RouterEnable;
    pIpv6Config_->setStaticRouterEnabled(RFL_FALSE); 
    if (V6_ROUTER_STATIC_ENABLE & u8RouterConfig){
        pIpv6Config_->setStaticRouterEnabled(RFL_TRUE);
    }

    pIpv6Config_->setDynamicRouterEnabled(RFL_FALSE);
    if (V6_ROUTER_DYNAMIC_ENABLE & u8RouterConfig){
        pIpv6Config_->setDynamicRouterEnabled(RFL_TRUE);
    }
    
    this->buildIPv6AddressSetProperty(pIpv6Config_, pIfConf);
    this->buildIPv6StaticRouterProperty(pIpv6Config_, pIfConf);
    this->buildIPv6DynamicRouterProperty(pIpv6Config_, pIfConf);

    pIpv6Config_->writeToCache();
}

void IPv6ConfigBuilder::cleanIPv6AddressSetCollection(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config){
    if (NULL == pIpv6Config) return;

    while (!IPv6AddressSetCollection_.empty()){
        RF_MergepointEthernetInterface_v1_0_0_IPv6AddressSet* pElement = *(IPv6AddressSetCollection_.begin());
        if (NULL != pElement){
            std::string object_id = pElement->getIndex();
            srObjRemoveObject(object_id.c_str());
            delete pElement;
            IPv6AddressSetCollection_.erase(IPv6AddressSetCollection_.begin());
        }
    }

    RF_Array* paIPv6StaticAddresses = pIpv6Config->getIPv6StaticAddresses();
    if (NULL == paIPv6StaticAddresses) return;
    paIPv6StaticAddresses->clearArray();
}

void IPv6ConfigBuilder::cleanIPv6StaticRouterCollection(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config){
    if (NULL == pIpv6Config) return;

    while (!IPv6StaticRouterCollection_.empty()){
        RF_MergepointEthernetInterface_v1_0_0_IPv6StaticRouter* pElement = *(IPv6StaticRouterCollection_.begin());
        if (NULL != pElement){
            std::string object_id = pElement->getIndex();
            srObjRemoveObject(object_id.c_str());
            delete pElement;
            IPv6StaticRouterCollection_.erase(IPv6StaticRouterCollection_.begin());
        }
    }

    RF_Array* paIPv6StaticRouters = pIpv6Config->getIPv6StaticRouters();
    if (NULL == paIPv6StaticRouters) return;
    paIPv6StaticRouters->clearArray();
}

void IPv6ConfigBuilder::cleanIPv6DynamicRouterCollection(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config){
    if (NULL == pIpv6Config) return;

    while (!IPv6DynamicRouterCollection_.empty()){
        RF_MergepointEthernetInterface_v1_0_0_IPv6DynamicRouter* pElement = *(IPv6DynamicRouterCollection_.begin());
        if (NULL != pElement){
            std::string object_id = pElement->getIndex();
            srObjRemoveObject(object_id.c_str());
            delete pElement;
            IPv6DynamicRouterCollection_.erase(IPv6DynamicRouterCollection_.begin());
        }
    }

    RF_Array* paIPv6DynamicRouters = pIpv6Config->getIPv6DynamicRouters();
    if (NULL == paIPv6DynamicRouters) return;
    paIPv6DynamicRouters->clearArray();
}


void IPv6ConfigBuilder::buildIPv6AddressSetProperty(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config, const sIF_Conf* pIfConf){
    if ( (NULL == pIpv6Config) || (NULL == pIfConf)) return;
    //Erase all objects.
    this->cleanIPv6AddressSetCollection(pIpv6Config);

    RF_Array* paIPv6StaticAddresses = pIpv6Config->getIPv6StaticAddresses();
    if (NULL == paIPv6StaticAddresses) return;

    for (unsigned int i = 0; i < MAX_IPV6_STATIC_ADDR_NUM; i++)
    {
        RF_MergepointEthernetInterface_v1_0_0_IPv6AddressSet* pIPv6AddressSet = new RF_MergepointEthernetInterface_v1_0_0_IPv6AddressSet();
        rolInitialProperty_t propertyDocument[] = {
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6ADDRESSSET_STR_ADDRESS,        SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6ADDRESSSET_STR_ENABLED,        SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6ADDRESSSET_STR_PREFIXLENGTH,   SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6ADDRESSSET_STR_STATUS,         SR_PROPERTY_NONINTERPRET}
        };
        pIPv6AddressSet->initObject(propertyDocument, 4);

        pIPv6AddressSet->setAddress(pIfConf->ifIPv6.AddrList[i].szAddr);
        pIPv6AddressSet->setPrefixLength(pIfConf->ifIPv6.AddrList[i].u16PrefixLen);
        pIPv6AddressSet->setEnabled(RFL_FALSE);
        if (LAN_TRUE == pIfConf->ifIPv6.AddrList[i].u8Enabled) pIPv6AddressSet->setEnabled(RFL_TRUE);
        /*
        typedef enum
        {
            V6_ADDR_STATUS_ACTIVE = 0,
            V6_ADDR_STATUS_DISABLED,
            V6_ADDR_STATUS_PENDING,
            V6_ADDR_STATUS_FAILED,
            V6_ADDR_STATUS_DEPRECATED,
            V6_ADDR_STATUS_INVALID
        }
        V6_ADDR_STATUS;
        */
        switch (pIfConf->ifIPv6.AddrList[i].u8AddrStatus){
            case V6_ADDR_STATUS_ACTIVE:
                {
                    pIPv6AddressSet->setStatus("Active");
                }
                break;
            case V6_ADDR_STATUS_DISABLED:
                {
                    pIPv6AddressSet->setStatus("Disabled");
                }
                break;
            case V6_ADDR_STATUS_PENDING:
                {
                    pIPv6AddressSet->setStatus("Pending");
                }
                break;
            case V6_ADDR_STATUS_FAILED:
                {
                    pIPv6AddressSet->setStatus("Failed");
                }
                break;
            case V6_ADDR_STATUS_DEPRECATED:
                {
                    pIPv6AddressSet->setStatus("Deprecated");
                }
                break;
            case V6_ADDR_STATUS_INVALID:
                {
                    pIPv6AddressSet->setStatus("Invalid");
                }
                break;
        }
        pIPv6AddressSet->writeToCache();
        IPv6AddressSetCollection_.push_back(pIPv6AddressSet);

        paIPv6StaticAddresses->addItem(pIPv6AddressSet->getIndex());
    }
    paIPv6StaticAddresses->writeToCache();
}

void IPv6ConfigBuilder::buildIPv6StaticRouterProperty(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config, const sIF_Conf* pIfConf){
    if ( (NULL == pIpv6Config) || (NULL == pIfConf)) return;
    //Erase all objects.
    this->cleanIPv6StaticRouterCollection(pIpv6Config);

    RF_Array* paIPv6StaticRouters = pIpv6Config->getIPv6StaticRouters();
    if (NULL == paIPv6StaticRouters) return;


    for (unsigned int i = 0; i < MAX_IPV6_ROUTER_NUM; i++)
    {
        RF_MergepointEthernetInterface_v1_0_0_IPv6StaticRouter* pIPv6StaticRouter = new RF_MergepointEthernetInterface_v1_0_0_IPv6StaticRouter();
        rolInitialProperty_t propertyDocument[] = {
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6STATICROUTER_STR_ADDRESS,      SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6STATICROUTER_STR_MACADDRESS,   SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6STATICROUTER_STR_PREFIXLENGTH, SR_PROPERTY_NONINTERPRET}
        };
        pIPv6StaticRouter->initObject(propertyDocument, 3);
        pIPv6StaticRouter->setAddress(pIfConf->ifIPv6.StaticIPv6Router[i].szAddr);
        pIPv6StaticRouter->setPrefixLength(pIfConf->ifIPv6.StaticIPv6Router[i].u16PrefixLen);
        char buffer[64] = {0};
        snprintf(buffer, 64, "%02x:%02x:%02x:%02x:%02x:%02x", 
                                    pIfConf->ifIPv6.StaticIPv6Router[i].au8MAC[0],
                                    pIfConf->ifIPv6.StaticIPv6Router[i].au8MAC[1],
                                    pIfConf->ifIPv6.StaticIPv6Router[i].au8MAC[2],
                                    pIfConf->ifIPv6.StaticIPv6Router[i].au8MAC[3],
                                    pIfConf->ifIPv6.StaticIPv6Router[i].au8MAC[4],
                                    pIfConf->ifIPv6.StaticIPv6Router[i].au8MAC[5]);
        pIPv6StaticRouter->setMACAddress(buffer);

        pIPv6StaticRouter->writeToCache();
        IPv6StaticRouterCollection_.push_back(pIPv6StaticRouter);

        paIPv6StaticRouters->addItem(pIPv6StaticRouter->getIndex());
    }
    paIPv6StaticRouters->writeToCache();
}

void IPv6ConfigBuilder::buildIPv6DynamicRouterProperty(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config, const sIF_Conf* pIfConf){
    if ( (NULL == pIpv6Config) || (NULL == pIfConf)) return;
    //Erase all objects.
    this->cleanIPv6DynamicRouterCollection(pIpv6Config);

    RF_Array* paIPv6DynamicRouters = pIpv6Config->getIPv6DynamicRouters();
    if (NULL == paIPv6DynamicRouters) return;

    for (unsigned int i = 0; i < MAX_IPV6_ROUTER_NUM; i++) {
        RF_MergepointEthernetInterface_v1_0_0_IPv6DynamicRouter* pIPv6DynamicRouter = new RF_MergepointEthernetInterface_v1_0_0_IPv6DynamicRouter();
        rolInitialProperty_t propertyDocument[] = {
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6DYNAMICROUTER_STR_ADDRESS,      SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6DYNAMICROUTER_STR_MACADDRESS,   SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV6DYNAMICROUTER_STR_PREFIXLENGTH, SR_PROPERTY_NONINTERPRET}
        };
        pIPv6DynamicRouter->initObject(propertyDocument, 3);
        pIPv6DynamicRouter->setAddress(pIfConf->ifIPv6.DynamicIPv6Router[i].szAddr);
        pIPv6DynamicRouter->setPrefixLength(pIfConf->ifIPv6.DynamicIPv6Router[i].u16PrefixLen);
        char buffer[64] = {0};
        snprintf(buffer, 64, "%02x:%02x:%02x:%02x:%02x:%02x", 
                                    pIfConf->ifIPv6.DynamicIPv6Router[i].au8MAC[0],
                                    pIfConf->ifIPv6.DynamicIPv6Router[i].au8MAC[1],
                                    pIfConf->ifIPv6.DynamicIPv6Router[i].au8MAC[2],
                                    pIfConf->ifIPv6.DynamicIPv6Router[i].au8MAC[3],
                                    pIfConf->ifIPv6.DynamicIPv6Router[i].au8MAC[4],
                                    pIfConf->ifIPv6.DynamicIPv6Router[i].au8MAC[5]);
        pIPv6DynamicRouter->setMACAddress(buffer);

        pIPv6DynamicRouter->writeToCache();
        IPv6DynamicRouterCollection_.push_back(pIPv6DynamicRouter);

        paIPv6DynamicRouters->addItem(pIPv6DynamicRouter->getIndex());
    }
    paIPv6DynamicRouters->writeToCache();
}



VertivEthernetInterfaceBuilder::VertivEthernetInterfaceBuilder()
:pVertivEtherneInterface_(NULL),
pIpv4Config_(NULL),
pIPv6ConfigBuilder_(NULL),
pVLanConfig_(NULL)
{


}

VertivEthernetInterfaceBuilder::~VertivEthernetInterfaceBuilder(){
    cleanPropertyFromRedis();

    if (pVertivEtherneInterface_) delete pVertivEtherneInterface_;
    if (pIpv4Config_) delete pIpv4Config_;
    if (pIPv6ConfigBuilder_) delete pIPv6ConfigBuilder_;
    if (pVLanConfig_) delete pVLanConfig_;
}

void VertivEthernetInterfaceBuilder::cleanPropertyFromRedis(){
    if (pVertivEtherneInterface_){
        std::string object_id = pVertivEtherneInterface_->getIndex();
        srObjRemoveObject(object_id.c_str());
    }
    if (pIpv4Config_){
        std::string object_id = pIpv4Config_->getIndex();
        srObjRemoveObject(object_id.c_str());
    }
    if (pVLanConfig_){
        std::string object_id = pVLanConfig_->getIndex();
        srObjRemoveObject(object_id.c_str());
    }
}

int VertivEthernetInterfaceBuilder::initIpv4Config(){
    RF_MergepointEthernetInterface_v1_0_0_IPv4Configuration* pIpv4Config = new RF_MergepointEthernetInterface_v1_0_0_IPv4Configuration();
    if (pIpv4Config){
        pIpv4Config_ = pIpv4Config;
        rolInitialProperty_t propertyDocument[] = {
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV4CONFIGURATION_STR_ALTERNATIVEDNS, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV4CONFIGURATION_STR_DHCPENABLED, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV4CONFIGURATION_STR_GATEWAY, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV4CONFIGURATION_STR_IPADDRESS, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV4CONFIGURATION_STR_MASK, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV4CONFIGURATION_STR_NAMESERVERORIGIN, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_IPV4CONFIGURATION_STR_PREFERREDDNS, SR_PROPERTY_NONINTERPRET}
        };
        pIpv4Config_->initObject(propertyDocument, 7);
        pIpv4Config_->writeToCache();
        return 0;
    }
    return -1;
}


int VertivEthernetInterfaceBuilder::initVLanConfig(){
    RF_MergepointEthernetInterface_v1_0_0_VLAN* pVLanConfig = new RF_MergepointEthernetInterface_v1_0_0_VLAN();
    if (pVLanConfig){
        pVLanConfig_ = pVLanConfig;
        rolInitialProperty_t propertyDocument[] = {
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_VLAN_STR_VLANENABLE, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_VLAN_STR_VLANID, SR_PROPERTY_NONINTERPRET},
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_VLAN_STR_VLANPRIORITY, SR_PROPERTY_NONINTERPRET}
        };
        pVLanConfig_->initObject(propertyDocument, 3);
        pVLanConfig_->writeToCache();
        return 0;    
    }
    return -1;
}

void VertivEthernetInterfaceBuilder::buildVLanProperty(const sIF_Conf* pIfConf){
    if (NULL == pIfConf) return;
    if (NULL == pVLanConfig_){
        if (0>this->initVLanConfig()){
            cout << __FUNCTION__ << __LINE__ << ": initVLanConfig failed." << endl;
            return;
        }
    }

    bool bVLanEnable = false;
    unsigned int uiVLanId = 0;
    unsigned int uiVLanPriority = 0;
    if (LAN_TRUE == pIfConf->ifVlan.u8Enabled){
        bVLanEnable = true;
    }
    uiVLanId = pIfConf->ifVlan.u16ID;
    uiVLanPriority = pIfConf->ifVlan.u8Priority;
    pVLanConfig_->setVLANEnable(RFL_FALSE);
    pVLanConfig_->setVLANId(uiVLanId);
    pVLanConfig_->setVLANPriority(uiVLanPriority);
    if (bVLanEnable) pVLanConfig_->setVLANEnable(RFL_TRUE);
    pVLanConfig_->writeToCache();
}

void VertivEthernetInterfaceBuilder::buildIPv4ConfigProperty(const sIF_Conf* pIfConf){
    if (NULL == pIfConf) return;
    if (NULL == pIpv4Config_){ 
        if (0>this->initIpv4Config()){
            cout << __FUNCTION__ << __LINE__ << ": initIpv4Config failed." << endl;
            return;
        }
    }
    pIpv4Config_->setIPAddress(pIfConf->ifIPv4.szIPAddress);
    pIpv4Config_->setMask(pIfConf->ifIPv4.szNetMask);
    pIpv4Config_->setGateway(pIfConf->ifIPv4.szGW);
    pIpv4Config_->setPreferredDNS(pIfConf->ifIPv4.szDNS1);
    pIpv4Config_->setAlternativeDNS(pIfConf->ifIPv4.szDNS2);


    uint16_t    u16DHCPConfOpt = pIfConf->ifIPv4.u16DHCPConfOpt;

    pIpv4Config_->setDHCPEnabled(RFL_FALSE);
    if (V4_DHCP_ENABLE & u16DHCPConfOpt){
        pIpv4Config_->setDHCPEnabled(RFL_TRUE);
    }

    if (V4_DHCP_OPT_GET_DNS & u16DHCPConfOpt){ 
        pIpv4Config_->setNameServerOrigin("DHCP");
    }else{
        pIpv4Config_->setNameServerOrigin("Static");
    }

    pIpv4Config_->writeToCache();
}

void VertivEthernetInterfaceBuilder::buildIPv6ConfigProperty(const sIF_Conf* pIfConf){
    if (pIPv6ConfigBuilder_){
        delete pIPv6ConfigBuilder_;
        pIPv6ConfigBuilder_ = NULL;
    }
    pIPv6ConfigBuilder_ = new IPv6ConfigBuilder(pIfConf);
}

int VertivEthernetInterfaceBuilder::buildProperty(std::string sIntfName, const sIF_Conf* pIfConf){
    if (NULL == pVertivEtherneInterface_){
        RF_MergepointEthernetInterface_v1_0_0_MergepointEthernetInterface* pVertivEtherneInterface = new RF_MergepointEthernetInterface_v1_0_0_MergepointEthernetInterface();
        if (pVertivEtherneInterface){
            pVertivEtherneInterface_ = pVertivEtherneInterface;
        }
    }
    this->buildVLanProperty(pIfConf);
    this->buildIPv4ConfigProperty(pIfConf);
    this->buildIPv6ConfigProperty(pIfConf);
    RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config = NULL;
    if (pIPv6ConfigBuilder_) pIpv6Config = pIPv6ConfigBuilder_->getRFObject();

    if ((pVertivEtherneInterface_ == NULL) || (pVLanConfig_ == NULL) || (pIpv6Config == NULL) || (pIpv4Config_ == NULL)){
        cout << "Property construction failed." << endl;
        return -1;
    }

    {
        //Limitation: SSOROL doesn't generate methods to manipulate these properties.
        //TODO: Remove workaround.
        char cstrIpv4ConfigObjUUID[pIpv4Config_->getIndex().length() + 1];
        char cstrIpv6ConfigObjUUID[pIpv6Config->getIndex().length() + 1];
        char cstrVLanConfigObjUUID[pVLanConfig_->getIndex().length() + 1];

        strcpy(cstrIpv4ConfigObjUUID, pIpv4Config_->getIndex().c_str());
        strcpy(cstrIpv6ConfigObjUUID, pIpv6Config->getIndex().c_str());
        strcpy(cstrVLanConfigObjUUID, pVLanConfig_->getIndex().c_str());
        
        rolInitialProperty_t propertyDocument[] = {
            { RFL_MERGEPOINTETHERNETINTERFACE_V1_0_0_MERGEPOINTETHERNETINTERFACE_STR_NETMODE, SR_PROPERTY_NONINTERPRET },
            { "IPv4Configuration", cstrIpv4ConfigObjUUID},
            { "IPv6Configuration", cstrIpv6ConfigObjUUID},
            { "VLAN", cstrVLanConfigObjUUID}
        };

        pVertivEtherneInterface_->initObject(propertyDocument, 4);
    }
    OsinetInterface* pOsinetInterface = OsinetInterface::getInstance();
    uint8_t netmode = pOsinetInterface->GetNetMode();
    pVertivEtherneInterface_->setNetMode(netmode);

    pVertivEtherneInterface_->writeToCache();

    return 0;
}


RF_MergepointEthernetInterface_v1_0_0_MergepointEthernetInterface* VertivEthernetInterfaceBuilder::getRFObject(){
    return pVertivEtherneInterface_;
}


namespace EthernetInterface_v1_1_0{
#endif
//TODO: We may not need to use namespace...
NetworkInterfaceAdapter::NetworkInterfaceAdapter(NetworkIntfBuilder* pBuilder, int iIntfIndex)
:NetworkInterfaceInfo(iIntfIndex),
pBuilder_(pBuilder)
{
#if 0
    pEthernetInterface = new RF_EthernetInterface_v1_1_0_EthernetInterface();
    pEthernetInterfaceOem = new RF_EthernetInterface_v1_1_0_EthernetInterface_Oem(); 

    pEthernetInterface->initObject( EthernetInterface_v1_1_0_EthernetInterfaceDocument, numOfEthernetInterface_v1_1_0_EthernetInterfaceFields );
    pEthernetInterface->setId(this->getInterfaceName());

    pEthernetInterfaceOem->initObject( EthernetInterface_v1_1_0_EthernetInterface_OemDocument, numOfEthernetInterface_v1_1_0_EthernetInterface_OemFields );
    pEthernetInterface->setOem( pEthernetInterfaceOem);
    pEthernetInterfaceOem->writeToCache();
    pEthernetInterface->writeToCache();
    cout << "NetworkInterfaceAdapter for " << this->getInterfaceName() << " interface (index=" << iIntfIndex << ") constructed at " << pEthernetInterface->getIndex() << endl;
    if (pBuilder_) pBuilder_->notifyOwner(pEthernetInterface, true);
    //TODO: After off-shell schema related knowledge, we might implement dedicate function with internally using pEthernetInterface->getIndex()
    sSchemaObjUUID = pEthernetInterface->getIndex();
#endif
}

NetworkInterfaceAdapter::~NetworkInterfaceAdapter(){
#if 0
    this->eraseAddressObject();

    if (pVertivEthernetInterfaceBuilder_){
        delete pVertivEthernetInterfaceBuilder_;
    }

    if (pEthernetInterfaceOem){ 
        std::string object_id = pEthernetInterfaceOem->getIndex();
        srObjRemoveObject(object_id.c_str());
        delete pEthernetInterfaceOem;
    }

    if (pVLanNetIntf_){
        std::string object_id = pVLanNetIntf_->getIndex();
        srObjRemoveObject(object_id.c_str());
        delete pVLanNetIntf_;
    } 
    if (pEthernetInterface){ 
        std::string object_id = pEthernetInterface->getIndex();
        //items in database shall be removed first. THIS IS THE REQUIREMENT FROM FRONTEND.
        //srObjRemoveObject(object_id.c_str());  Will show stingrayAPI.c:8326: Get typeId from hash 80000000-0020-0001-0001-6363dbab0000 failed!
        if (pBuilder_) pBuilder_->notifyOwner(pEthernetInterface, false);
       // srObjRemoveObject(object_id.c_str());
        delete pEthernetInterface;
    }
#endif
}
#if 0
void NetworkInterfaceAdapter::eraseAddressObject(){
    while (!IPv4AddressCollection_.empty()){
        RF_IPAddresses_v1_0_3_IPv4Address* pRF_IPAddresses_v1_0_3_IPv4Address = *(IPv4AddressCollection_.begin());
        if (NULL != pRF_IPAddresses_v1_0_3_IPv4Address){
            std::string object_id = pRF_IPAddresses_v1_0_3_IPv4Address->getIndex();
            srObjRemoveObject(object_id.c_str());
            delete pRF_IPAddresses_v1_0_3_IPv4Address;
            IPv4AddressCollection_.erase(IPv4AddressCollection_.begin());
        }
    }
    while (!IPv6AddressCollection_.empty()){
        RF_IPAddresses_v1_0_3_IPv6Address* pRF_IPAddresses_v1_0_3_IPv6Address = *(IPv6AddressCollection_.begin());
        if (NULL != pRF_IPAddresses_v1_0_3_IPv6Address){
            std::string object_id = pRF_IPAddresses_v1_0_3_IPv6Address->getIndex();
            srObjRemoveObject(object_id.c_str());
            delete pRF_IPAddresses_v1_0_3_IPv6Address;
            IPv6AddressCollection_.erase(IPv6AddressCollection_.begin());
        }
    }
}

string NetworkInterfaceAdapter::getGatewayFromOsinet(int iNetFamily, char* InfNmae) {
    if (strcmp(InfNmae, "lo")==0) {
        return "";
    }
    OsinetInterface* pOsinetInterface = OsinetInterface::getInstance();
    if (NULL == pOsinetInterface) return "";
    uint8_t IfIndex = pOsinetInterface->GetIfIndex(InfNmae);
    sIF_Conf        if_conf;
    int status = avct_osinet_ifconfget(IfIndex, &if_conf);
    if(status != OSINET_STATUS_OK){
        cout << "avct_osinet_ifconfget return failed for interface name: " << InfNmae << ", id = " << (int) IfIndex << endl;
    }
    if (iNetFamily == AF_INET) {
        return string(if_conf.ifIPv4.szGW);
    }
    else {
        return string(if_conf.ifIPv6.szGW);
    }
}

RF_IPAddresses_v1_0_3_IPv4Address* NetworkInterfaceAdapter::createIPv4AddressObj(IPv4AddressInfo* pIPv4AddressInfo){
    if (NULL == pIPv4AddressInfo) return NULL;

    rolInitialProperty_t propertyDoc[] = {
        { RFL_IPADDRESSES_V1_0_3_IPV4ADDRESS_STR_ADDRESS,               SR_PROPERTY_NONINTERPRET },
        { RFL_IPADDRESSES_V1_0_3_IPV4ADDRESS_STR_ADDRESSORIGIN,         SR_PROPERTY_NONINTERPRET },
        { RFL_IPADDRESSES_V1_0_3_IPV4ADDRESS_STR_GATEWAY,               SR_PROPERTY_NONINTERPRET },
        { RFL_IPADDRESSES_V1_0_3_IPV4ADDRESS_STR_SUBNETMASK,            SR_PROPERTY_NONINTERPRET }
    };

    const unsigned int numOfpropertyDoc = 4;

    RF_IPAddresses_v1_0_3_IPv4Address* pObject = new RF_IPAddresses_v1_0_3_IPv4Address();
    pObject->initObject(propertyDoc, numOfpropertyDoc); 
    pObject->setAddress(pIPv4AddressInfo->getAddress());
    pObject->setSubnetMask(pIPv4AddressInfo->getNetMask());
    
   
    if (pBuilder_){
        std::string sGatewayIP = getGatewayFromOsinet(AF_INET, (char*)this->getInterfaceName().c_str());
        if(sGatewayIP != ""){
            pObject->setGateway(sGatewayIP);
        }
    }
    pObject->writeToCache();
    IPv4AddressCollection_.push_back(pObject); 
    return pObject;
}

int NetworkInterfaceAdapter::initIPv4Addresses(){
    RF_Array* pAddressArray = pEthernetInterface->getIPv4Addresses();
    if (pAddressArray == NULL) return -1;
    
    //TODO: Required to use MUTEX...    
    std::map<std::string, IPv4AddressInfo*>::iterator it = IPv4AddressInfoCollection_.begin();
    while (it != IPv4AddressInfoCollection_.end()){
        if (it->second != NULL){
            RF_IPAddresses_v1_0_3_IPv4Address* pElement = NULL;
            if (NULL == (pElement = createIPv4AddressObj(it->second))){
                cout << "Failed to create IPv4Addresses object." << endl;
                continue;
            }
            if (false == pAddressArray->addItem(pElement->getIndex())){
                cout << "Unable to add item to pAddressArray" << endl;
            }
        }
        it++;
    }

    
    return 0;
}



RF_IPAddresses_v1_0_3_IPv6Address* NetworkInterfaceAdapter::createIPv6AddressObj(IPv6AddressInfo* pIPv6AddressInfo){
    if (NULL == pIPv6AddressInfo) return NULL;

    rolInitialProperty_t propertyDoc[] = {
        { RFL_IPADDRESSES_V1_0_3_IPV6ADDRESS_STR_ADDRESS,               SR_PROPERTY_NONINTERPRET },
        { RFL_IPADDRESSES_V1_0_3_IPV6ADDRESS_STR_ADDRESSORIGIN,         SR_PROPERTY_NONINTERPRET },
        { RFL_IPADDRESSES_V1_0_3_IPV6ADDRESS_STR_ADDRESSSTATE,               SR_PROPERTY_NONINTERPRET },
        { RFL_IPADDRESSES_V1_0_3_IPV6ADDRESS_STR_PREFIXLENGTH,            SR_PROPERTY_NONINTERPRET }
    };

    const unsigned int numOfpropertyDoc = 4;

    RF_IPAddresses_v1_0_3_IPv6Address* pObject = new RF_IPAddresses_v1_0_3_IPv6Address();
    pObject->initObject(propertyDoc, numOfpropertyDoc); 
    pObject->setAddress(pIPv6AddressInfo->getAddress());
    pObject->setPrefixLength( pIPv6AddressInfo->getPrefixLength() );

    unsigned int addressFlag = pIPv6AddressInfo->getFlags();
    if (!(addressFlag & IFA_F_NODAD)){
        //Supported DAD, we will generate address state using setAddressState(RFE_IPAddresses_v1_0_3_AddressState val);
        if (addressFlag & IFA_F_TENTATIVE){
            pObject->setAddressState(IPADDRESSES_V1_0_3_ADDRESSSTATE_TENTATIVE);
        }
        if (addressFlag & IFA_F_PERMANENT){
            pObject->setAddressState(IPADDRESSES_V1_0_3_ADDRESSSTATE_PREFERRED);
        }
        if (addressFlag & IFA_F_DEPRECATED){
            pObject->setAddressState(IPADDRESSES_V1_0_3_ADDRESSSTATE_DEPRECATED);
        }
        if (addressFlag & IFA_F_DADFAILED){
            pObject->setAddressState(IPADDRESSES_V1_0_3_ADDRESSSTATE_FAILED);
        }
    }

    pObject->writeToCache();
    IPv6AddressCollection_.push_back(pObject); 
    return pObject;
}

void NetworkInterfaceAdapter::setupIPv6AddressOrigin(const sIF_Conf* pIfConf){
    if (NULL == pIfConf) return;

    std::vector<RF_IPAddresses_v1_0_3_IPv6Address*>::iterator it = IPv6AddressCollection_.begin();
    while (it != IPv6AddressCollection_.end()){
        RF_IPAddresses_v1_0_3_IPv6Address* pIPv6Address = *it;
        if(pIPv6Address){
            std::string sAddress = pIPv6Address->getAddress();
            bool bOriginIdentified = false;
            if (true == OsinetUtility::isLinkLocalAddress(sAddress, pIfConf)){
                pIPv6Address->setAddressOrigin(IPADDRESSES_V1_0_3_IPV6ADDRESSORIGIN_LINKLOCAL);
                bOriginIdentified = true;
            }else if (true == OsinetUtility::isStaticAddress(sAddress, pIfConf)){
                pIPv6Address->setAddressOrigin(IPADDRESSES_V1_0_3_IPV6ADDRESSORIGIN_STATIC);
                bOriginIdentified = true;
            }else if(true == OsinetUtility::isSLAACAddress(sAddress, pIfConf)){
                pIPv6Address->setAddressOrigin(IPADDRESSES_V1_0_3_IPV6ADDRESSORIGIN_SLAAC);
                bOriginIdentified = true;
            }else if(true == OsinetUtility::isDHCPv6Address(sAddress, pIfConf)){
                pIPv6Address->setAddressOrigin(IPADDRESSES_V1_0_3_IPV6ADDRESSORIGIN_DHCPV6);
                bOriginIdentified = true;
            }
            if (bOriginIdentified) pIPv6Address->writeToCache();
        }
        it ++;
    }
}


int NetworkInterfaceAdapter::initIPv6Addresses(){
    RF_Array* pAddressArray = pEthernetInterface->getIPv6Addresses();
    if (pAddressArray == NULL) return -1;

    //TODO: Required to use MUTEX...    
    std::map<std::string, IPv6AddressInfo*>::iterator it = IPv6AddressInfoCollection_.begin();
    while (it != IPv6AddressInfoCollection_.end()){
        if (it->second != NULL){
            RF_IPAddresses_v1_0_3_IPv6Address* pElement = NULL;
            if (NULL == (pElement = createIPv6AddressObj(it->second))){
                cout << "Failed to create IPv6Addresses object." << endl;
                continue;
            }
            if (false == pAddressArray->addItem(pElement->getIndex())){
                cout << "Unable to add item to pAddressArray" << endl;
            }
        }
        it++;
    }

    
    return 0;
}

int NetworkInterfaceAdapter::initEthernetInterfaceOem(){
    pEthernetInterfaceOem->writeToCache();
    return 0;
}

int NetworkInterfaceAdapter::initEthernetInterface(){
    //http://elixir.free-electrons.com/linux/latest/source/include/uapi/linux/if_arp.h#L28 
    const unsigned int ARPHRD_ETHER = 1;
    const unsigned int ARPHRD_LOOPBACK = 772;
    std::string sName = "BMC " + this->getInterfaceName() + " Network Interface.";
    pEthernetInterface->setName(sName);
    pEthernetInterface->setInterfaceEnabled(RFL_TRUE);
    if (this->getLinkStatus() == BMC_ETH_STATUS::LINK_UP) { 
        pEthernetInterface->setLinkStatus(ETHERNETINTERFACE_V1_1_0_LINKSTATUS_LINKUP);

    }
    else if (this->getLinkStatus() == BMC_ETH_STATUS::NO_LINK) {
        pEthernetInterface->setLinkStatus(ETHERNETINTERFACE_V1_1_0_LINKSTATUS_NOLINK);

    }
    else if (this->getLinkStatus() == BMC_ETH_STATUS::LINK_DOWN) {
        pEthernetInterface->setLinkStatus(ETHERNETINTERFACE_V1_1_0_LINKSTATUS_LINKDOWN);

    }
    //Different device type may have different valid 
    switch (this->getDeviceTypeId()){
        case ARPHRD_ETHER:
            {
                pEthernetInterface->setMACAddress(this->getMACAddress());
                pEthernetInterface->setMTUSize(this->getMTUSize());
                pEthernetInterface->setSpeedMbps(this->getSpeedMbps());
                if(true == this->isFullDuplex()){
                    pEthernetInterface->setFullDuplex(RFL_TRUE);
                }else{
                    pEthernetInterface->setFullDuplex(RFL_FALSE);
                }
                if(true == this->isAutoNeg()){
                    pEthernetInterface->setAutoNeg(RFL_TRUE);
                }else{
                    pEthernetInterface->setAutoNeg(RFL_FALSE);
                }
                break;
            }
        case ARPHRD_LOOPBACK:
            {
                pEthernetInterface->setMTUSize(this->getMTUSize());
                break;
            } 
        default:
            {
                cout << "Unsupported device type: " << this->getDeviceTypeId() << endl;
            }
    }


    initIPv4Addresses();
    initIPv6Addresses();

    if (pBuilder_){
        std::string sGatewayIP = getGatewayFromOsinet(AF_INET6, (char*)this->getInterfaceName().c_str());
        if(sGatewayIP != ""){
            pEthernetInterface->setIPv6DefaultGateway(sGatewayIP);
        }
    } 

    initEthernetInterfaceOem();    
    pEthernetInterface->writeToCache();  
    return 0;
}

void NetworkInterfaceAdapter::buildCommonPropertyFromOsinet(){
    sGlobal_Conf    glb_conf;
    int status = avct_osinet_glbconfget(&glb_conf);
    if (status != OSINET_STATUS_OK) return;

    string sHostName = glb_conf.szHostName;
    pEthernetInterface->setHostName(sHostName);

    //TODO: Each interface might have their own FQDN?!
    string sDomainName = glb_conf.szDomainName;
    if (sDomainName.empty()){
        pEthernetInterface->setFQDN(sHostName);
    }else{
        string sFQDN = sHostName + "." + sDomainName;
        pEthernetInterface->setFQDN(sFQDN);
    }
    pEthernetInterface->writeToCache();
}

void NetworkInterfaceAdapter::buildVLanProperty(const sIF_Conf* pIfConf){
    if (NULL == pIfConf) return;
    //Osinet doesn't have good support for VLAN. (API interface design work well when there is only one VLAN instance.)

    //Depending on whether VLan is enabled or not we may move configuration to VLAN interface.
    //For example, we have VLAN id 345 for ethernet interface eth0. Configuration will be move the ethernet interface eth0.345.
    bool bVLanEnable = false;
    unsigned int uiVLanId = 0;
    //unsigned int uiVLanPriority = 0;
    if (LAN_TRUE == pIfConf->ifVlan.u8Enabled){
        bVLanEnable = true;
    }
    uiVLanId = pIfConf->ifVlan.u16ID;
    //uiVLanPriority = pIfConf->ifVlan.u8Priority;
    if (NULL == pVLanNetIntf_){
        //Create the VLAN interface and 
        RF_VLanNetworkInterface_v1_0_2_VLAN* pVLanNetIntfObj = new RF_VLanNetworkInterface_v1_0_2_VLAN();
        rolInitialProperty_t propertyDoc[] = {
            { RFL_VLANNETWORKINTERFACE_V1_0_2_VLAN_STR_VLANENABLE,               SR_PROPERTY_NONINTERPRET },
            { RFL_VLANNETWORKINTERFACE_V1_0_2_VLAN_STR_VLANID,         SR_PROPERTY_NONINTERPRET }
        };
        if (pVLanNetIntfObj){
            pVLanNetIntfObj->initObject(propertyDoc, 2);
            pVLanNetIntf_ = pVLanNetIntfObj;
        }
        
    }
    if (pVLanNetIntf_){
        pVLanNetIntf_->setVLANEnable(RFL_FALSE);
        pVLanNetIntf_->setVLANId(uiVLanId);
        if (bVLanEnable) pVLanNetIntf_->setVLANEnable(RFL_TRUE);
        pVLanNetIntf_->writeToCache();

        if (pEthernetInterface){
            //Limitation: SSIMold doesn't generate interface to manipulate VLAN property.
            //TODO: Remove workaoung...
            RF_EthernetInterface_v1_1_0_EthernetInterface ethIntf;
            ethIntf.setIndex(pEthernetInterface->getIndex());

            char cstrPropertyObjUUID[pVLanNetIntf_->getIndex().length() + 1];
            strcpy(cstrPropertyObjUUID, pVLanNetIntf_->getIndex().c_str());
            rolInitialProperty_t propertyDoc[] = {
                { RFL_ETHERNETINTERFACE_V1_1_0_ETHERNETINTERFACE_STR_VLAN, cstrPropertyObjUUID }
            };
            ethIntf.initObject(propertyDoc, 1);
            ethIntf.writeToCache();
        }
    }
}

void NetworkInterfaceAdapter::buildInterfaceSpecificProperty(){
    //The implementaion might be dirty.
    string sInterfaceName = this->getInterfaceName();

    if (sInterfaceName == "lo") return;

    OsinetInterface* pOsinetInterface = OsinetInterface::getInstance();
    if (NULL == pOsinetInterface) return;
    this->buildCommonPropertyFromOsinet();    //Osinet might contains information shared with all interfaces.

    sIF_Conf        if_conf;
    uint8_t IfIndex = pOsinetInterface->GetIfIndex((char*)this->getInterfaceName().c_str());
    if (IfIndex > 0 ){
        int status = avct_osinet_ifconfget(IfIndex, &if_conf);
        if(status != OSINET_STATUS_OK){
            cout << "avct_osinet_ifconfget return failed for interface name: " << this->getInterfaceName() << ", id = " << (int) IfIndex << endl;
        }

        this->setupIPv6AddressOrigin(&if_conf);

        if (NULL == pVertivEthernetInterfaceBuilder_){
            VertivEthernetInterfaceBuilder* pVertivEthernetInterfaceBuilder = new VertivEthernetInterfaceBuilder();
            if (NULL == pVertivEthernetInterfaceBuilder) return;
            pVertivEthernetInterfaceBuilder_ = pVertivEthernetInterfaceBuilder;
        } 
        pVertivEthernetInterfaceBuilder_->buildProperty(this->getInterfaceName(), &if_conf);
 
        RF_MergepointEthernetInterface_v1_0_0_MergepointEthernetInterface* pVertivEthIntfObj = pVertivEthernetInterfaceBuilder_->getRFObject();
        if(pVertivEthIntfObj){
            pEthernetInterfaceOem->setVertivEthernetInterface(pVertivEthIntfObj);
            pEthernetInterfaceOem->writeToCache();
        }
    }else{
        uint8_t VLanIfIndex = pOsinetInterface->GetVLanIfIndex((char*)this->getInterfaceName().c_str());
        if (VLanIfIndex > 0){
            int status = avct_osinet_ifconfget(VLanIfIndex, &if_conf);
            if(status != OSINET_STATUS_OK){
                cout << "VLAN: avct_osinet_ifconfget return failed for vlan interface from: " << this->getInterfaceName() << ", id = " << (int) VLanIfIndex << endl;
            }

            RF_EthernetInterface_v1_1_0_EthernetInterface_Oem EthOemObj;
            //TODO: REMOVE WORKAROUND
            //      rmVertivEthernetInterface will remove RFL_ETHERNETINTERFACE_V1_1_0_ETHERNETINTERFACE_OEM_STR_VERTIVETHERNETINTERFACE and then Refinement is not going to work.
            //      The workaround is to assign SR_PROPERTY_NONINTERPRET to it. 
            EthOemObj.setIndex(pEthernetInterfaceOem->getIndex());
  
            rolInitialProperty_t propertyDoc[] = {
                { RFL_ETHERNETINTERFACE_V1_1_0_ETHERNETINTERFACE_OEM_STR_VERTIVETHERNETINTERFACE, SR_PROPERTY_NONINTERPRET}
            };
            EthOemObj.initObject(propertyDoc, 1);
            EthOemObj.writeToCache();

        }else{
            return;
        }
    }
    this->buildVLanProperty(&if_conf);
}

int NetworkInterfaceAdapter::buildSchema(){
    std::string object_id = "";
    if (pEthernetInterface){
        //This will clean up the whole object 
        object_id = pEthernetInterface->getIndex();
        srObjRemoveObject(object_id.c_str());
        delete pEthernetInterface;
    }
    pEthernetInterface = new RF_EthernetInterface_v1_1_0_EthernetInterface();
    pEthernetInterface->initObject( EthernetInterface_v1_1_0_EthernetInterfaceDocument, numOfEthernetInterface_v1_1_0_EthernetInterfaceFields );
    if (object_id != ""){
        //Use previous defined object id used in Redis.
        pEthernetInterface->setIndex(object_id);
    }
    pEthernetInterface->setId(this->getInterfaceName());
    pEthernetInterface->setOem( pEthernetInterfaceOem);
    this->eraseAddressObject();
    
    this->initEthernetInterface();
    this->buildInterfaceSpecificProperty();
    //if (pBuilder_) pBuilder_->notifyOwner(pEthernetInterface, true);
    srClearETagCache(object_id.c_str());
    return 0;
}

int NetworkInterfaceAdapter::handleHttpPatch(std::string sRequestObjectId, std::string sRreturnChannelId){
    NetworkInterfaceHttpPatchHandler aNetworkInterfaceHttpPatchHandler(sRreturnChannelId);

    if (0 == this->getInterfaceName().compare("lo")){
        //STR modify all schemas at the same time. But in the real case, it is possible instance of resource can't support that method.
        //Below is stupid hardcode and workaound. 
        //ssiFillReturnMessage( &sMsgBuf, "MethodNotAllowed", 2, "/redfish/v1/Managers/1/EthernetInterfaces/lo", "PATCH" );
        //ssiSendReturnMessage( (char*)sRreturnChannelId.c_str(), sMsgBuf ); 
        return -1;
    }
    //ssiFillReturnMessage( &sMsgBuf, "PropertyValueNotInList", 2, pcBootSourceOverrideEnabled, "BootSourceOverrideEnabled");
    //ssiFillReturnMessage( &sMsgBuf, "BackendInternalException", 2, "IPMI Chassis Control Command", error_code );

    OsinetInterface* pOsinetInterface = OsinetInterface::getInstance();
    uint8_t IfIndex = pOsinetInterface->GetIfIndex((char*)this->getInterfaceName().c_str());
    if (IfIndex > 0 ){
        if(0 > aNetworkInterfaceHttpPatchHandler.processRequest(sRequestObjectId, IfIndex)){
            cout << __FUNCTION__ << __LINE__ << ": NetworkInterfaceHttpPatchHandler::processRequest failed." << endl;
        }
    }else{
        uint8_t VLanIfIndex = pOsinetInterface->GetVLanIfIndex((char*)this->getInterfaceName().c_str());
        if (VLanIfIndex > 0){
            if(0 > aNetworkInterfaceHttpPatchHandler.processRequest(sRequestObjectId, VLanIfIndex)){
                cout << __FUNCTION__ << __LINE__ << ": NetworkInterfaceHttpPatchHandler::processRequest failed." << endl;
            }
        }else{
            char cFailedIntf[5] = {0};
            snprintf(cFailedIntf, 5, "%d", VLanIfIndex);
            //ssiFillReturnMessage( &sMsgBuf, "BackendInternalException", 2, "GetVLanIfIndex", cFailedIntf );
            //ssiSendReturnMessage( (char*)sRreturnChannelId.c_str(), sMsgBuf ); 
            return -1;
        }
    }
    return 0;
}

}
#endif

#if 0
scopeSRObject::scopeSRObject(std::string objectUUID)
:dbObject_(NULL)
{
    dbObject_ = srObjGetObject( objectUUID.c_str() );
}

scopeSRObject::~scopeSRObject(){
    if (dbObject_){
        dbObject_->free( dbObject_ );
    }
}

const srProperty_t* scopeSRObject::getsrProperty_t(){
    if (NULL == dbObject_) return NULL;
    return dbObject_->head;
}

int NetworkInterfaceHttpPatchHandler::processRequest(const std::string sRequestObjectId, const unsigned int uiIntfIndex){
    if (0 > processNetGlobalInfoRequest(sRequestObjectId, uiIntfIndex)){
        cout << __FUNCTION__ << __LINE__ << ": Error: processNetGlobalInfoRequest return failed." << endl;
        RF_Message_v1_0_3_Message aMessage;
        aMessage.initObject( NULL, 0 );
        
        return -1;
    }

    OsinetIfConfHelper anOsinetIfConfHelper(pExtendedInfoHelper_, uiIntfIndex);
    if (0 > anOsinetIfConfHelper.processRequest(sRequestObjectId)){
        return -1;
    }

    //TODO: It looks like below woraound is not working.
    srObjAddProperty((char*)sRequestObjectId.c_str(), "HasRespData", "No");
    return 0;
}

int NetworkInterfaceHttpPatchHandler::processNetDeviceInfoRequest(const std::string sRequestObjectId, const unsigned int uiIntfIndex){
    return 0;
}

int NetworkInterfaceHttpPatchHandler::processNetGlobalInfoRequest(const std::string sRequestObjectId, const unsigned int uiIntfIndex){
    sGlobal_Conf aCurrentGlobalConf;
    sGlobal_Conf aNewGlobalConf;

    uint16_t u16GlobalConfSize = sizeof(sGlobal_Conf);
    int status = avct_osinet_glbconfdatainit(&aNewGlobalConf, &u16GlobalConfSize);
    if( status != OSINET_STATUS_OK){
        cout << "avct_osinet_glbconfdatainit return failed. " <<endl; 
        printf( "   Status code: %s (0x%02X)\n", OsinetUtility::resolveStatusCode(status), status );
        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
        if (NULL!=pReturnMessage){
            ssiFillReturnMessage( pReturnMessage, "BackendInternalException", 2, "avct_osinet_glbconfdatainit", (char*)OsinetUtility::resolveStatusCode(status));
        }
        return -1;
    }
    status = avct_osinet_glbconfget(&aCurrentGlobalConf);
    if( status != OSINET_STATUS_OK){
        cout << "avct_osinet_glbconfget return failed. " <<endl; 
        printf( "   Status code: %s (0x%02X)\n", OsinetUtility::resolveStatusCode(status), status );
        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
        if (NULL!=pReturnMessage){
            ssiFillReturnMessage( pReturnMessage, "BackendInternalException", 2, "avct_osinet_glbconfget", (char*)OsinetUtility::resolveStatusCode(status));
        }
        return -1;
    }
        
    string propName("");
    string propValue("");
    scopeSRObject dbObject(sRequestObjectId);
    srProperty_t* sProp = (srProperty_t*) dbObject.getsrProperty_t();
    while (sProp){
        propName = std::string( sProp->name );
        if(sProp->value){
            propValue = std::string( sProp->value );
            if (0 == propName.compare("HostName")){
                if (propValue.length() > 63) {
                    sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                    if (NULL!=pReturnMessage){
                        ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        return -1;
                    }
                }
                if (0 != propValue.compare(aCurrentGlobalConf.szHostName)){
                    snprintf(aNewGlobalConf.szHostName, MAX_STR_LEN, "%s", propValue.c_str());
                }
            }
        }
        sProp = sProp->next;
    }

    status = avct_osinet_glbconfset(&aNewGlobalConf);
    if( status != OSINET_STATUS_OK){
        cout << "avct_osinet_glbconfset return failed. " <<endl; 
        printf( "   Status code: %s (0x%02X)\n", OsinetUtility::resolveStatusCode(status), status );
        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
        if (NULL!=pReturnMessage){
            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, "avct_osinet_glbconfset", (char*)OsinetUtility::resolveStatusCode(status));
        }
        return -1;
    }
    return 0;
}

OsinetIfConfHelper::OsinetIfConfHelper(ExtendedInfoHelper* pExtendedInfoHelper, unsigned int uiIntfIndex)
:pExtendedInfoHelper_(pExtendedInfoHelper),
uiIntfIndex_(uiIntfIndex)
{

}

OsinetIfConfHelper::~OsinetIfConfHelper(){

}


int OsinetIfConfHelper::processRequest(const std::string sRequestObjectId){
    sIF_Conf        if_conf = {0};
    sIF_Conf        newIfConf = {0};
    int status = avct_osinet_ifconfget(uiIntfIndex_, &if_conf);
    if(status != OSINET_STATUS_OK){
        printf( "   Status code: %s (0x%02X)\n", OsinetUtility::resolveStatusCode(status), status );
        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
        if (NULL!=pReturnMessage){
            ssiFillReturnMessage( pReturnMessage, "BackendInternalException", 2, "avct_osinet_glbconfset", (char*)OsinetUtility::resolveStatusCode(status));
        }
        return -1;
    }

    scopeSRObject dbObject(sRequestObjectId);
    srProperty_t* sProp = (srProperty_t*) dbObject.getsrProperty_t();
    if (sProp){
        if (0 > processRequest_(sProp, &if_conf, &newIfConf)){
            return -1;
        }
    }else{
        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
        if (NULL!=pReturnMessage){
            ssiFillReturnMessage( pReturnMessage, "BackendInternalException", 2, "srProperty_t", "NULL");
        }
        return -1; 
    }

    status = avct_osinet_ifconfset(uiIntfIndex_, &newIfConf);
    if(status != OSINET_STATUS_OK){
        printf( "   Status code: %s (0x%02X)\n", OsinetUtility::resolveStatusCode(status), status );
        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
        if (NULL!=pReturnMessage){
            ssiFillReturnMessage( pReturnMessage, "BackendInternalException", 2, "avct_osinet_glbconfset", (char*)OsinetUtility::resolveStatusCode(status));
        }
        return -1; 
    }
}


int OsinetIfConfHelper::processRequest_(const srProperty_t* sProp, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    int iErrorCount = 0;
    string propName("");
    string propValue("");

    if (0 > initOsinetIfConf(pCurrentIfConf, pNewIfConf)){
        return -1;
    }

    if (NULL != sProp){
        while (sProp){
            propName = std::string( sProp->name );
            if(sProp->value){
                propValue = std::string( sProp->value );
                if (0 == propName.compare("VLAN")){
                    std::string sPropertyPath = "#/" + propName;

                    scopeSRObject dbObject(propValue);
                    srProperty_t* sPropVLAN = (srProperty_t*) dbObject.getsrProperty_t();
                    if (sPropVLAN){
                        if (0 > processVLAN(sPropVLAN, sPropertyPath, pCurrentIfConf, pNewIfConf)){
                            iErrorCount ++;
                        }
                    }else{
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propName.c_str(), "Object" );
                        }
                    }
                }else if (0 == propName.compare("FullDuplex")){
                    std::string sPropertyPath = "#/" + propName;
                    bool bCurrentFullDuplex = false;
                    bool bRequestFullDuplex = false;
                    if (DEV_DUPLEX_FULL == pCurrentIfConf->ifDev.u8NetDuplex){
                        bCurrentFullDuplex = true;
                    }else{
                        bCurrentFullDuplex = false; 
                    }
                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestFullDuplex)){
                        if (bRequestFullDuplex != bCurrentFullDuplex){
                            if (true == bRequestFullDuplex){
                                pNewIfConf->ifDev.u8NetDuplex = DEV_DUPLEX_FULL;
                            }else{
                                pNewIfConf->ifDev.u8NetDuplex = DEV_DUPLEX_HALF;
                            }
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            //ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("AutoNeg")){
                    std::string sPropertyPath = "#/" + propName;
                    bool bCurrentSetting = false;
                    bool bRequestSetting = false;
                    if (LAN_TRUE == pCurrentIfConf->ifDev.bAutoNegotiate){
                        bCurrentSetting = true;
                    }else{
                        bCurrentSetting = false; 
                    }
                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestSetting)){
                        if (bRequestSetting != bCurrentSetting){
                            if (true == bRequestSetting){
                                pNewIfConf->ifDev.bAutoNegotiate = LAN_TRUE;
                            }else{
                                pNewIfConf->ifDev.bAutoNegotiate = LAN_FALSE;
                            }
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            //ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("SpeedMbps")){
                    std::string sPropertyPath = "#/" + propName;
                    unsigned int uiCurrentSpeedMbps = DEV_SPEED_NO_CHANGE;
                    if (DEV_SPEED_10000 & pCurrentIfConf->ifDev.u8NetSpeed){
                        uiCurrentSpeedMbps = 10000;
                    }else if(DEV_SPEED_1000 & pCurrentIfConf->ifDev.u8NetSpeed){
                        uiCurrentSpeedMbps = 1000;
                    }else if(DEV_SPEED_100 & pCurrentIfConf->ifDev.u8NetSpeed){
                        uiCurrentSpeedMbps = 100;
                    }else if(DEV_SPEED_10 & pCurrentIfConf->ifDev.u8NetSpeed){
                        uiCurrentSpeedMbps = 10;
                    }
                    double dRequestSpeedMbps = DEV_SPEED_NO_CHANGE;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestSpeedMbps)){
                        if (dRequestSpeedMbps != uiCurrentSpeedMbps){
                            switch((unsigned int) dRequestSpeedMbps){
                                case 10000:
                                    pNewIfConf->ifDev.u8NetSpeed = DEV_SPEED_10000;
                                    break;
                                case 1000:
                                    pNewIfConf->ifDev.u8NetSpeed = DEV_SPEED_1000;
                                    break;
                                case 100:
                                    pNewIfConf->ifDev.u8NetSpeed = DEV_SPEED_100;
                                    break;
                                case 10:
                                    pNewIfConf->ifDev.u8NetSpeed = DEV_SPEED_10;
                                    break;
                                default:
                                    cout << __FUNCTION__ << __LINE__ << ": Configured SpeedMbps is out of range." << endl;
                                    iErrorCount ++;
                                    sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                                    if (NULL!=pReturnMessage){
                                        //ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                                        ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                                    }
                            }
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not double value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            //ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }                

                }else if (0 == propName.compare("MTUSize")){
                    std::string sPropertyPath = "#/" + propName;
                    const uint16_t u16CurrentMTUSize =  pCurrentIfConf->ifDev.u16MTU;
                    double dRequestMTUSize = 1500;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestMTUSize)){
                        if ((dRequestMTUSize <= DEV_MTU_MAX) && (dRequestMTUSize >= DEV_MTU_MIN)){
                            if (dRequestMTUSize != u16CurrentMTUSize){
                                pNewIfConf->ifDev.u16MTU = (uint16_t) dRequestMTUSize;
                            }
                        }else{
                            cout << __FUNCTION__ << __LINE__ << ": MTUSize is out of range. (560-1500)" << endl;
                            iErrorCount ++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                //ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not double value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            //ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("Oem")){
                    std::string sPropertyPath = "#/" + propName;

                    scopeSRObject dbObject(propValue);
                    srProperty_t* sPropOem = (srProperty_t*) dbObject.getsrProperty_t();
                    if (sPropOem){
                        if (0 > processOem(sPropOem, sPropertyPath, pCurrentIfConf, pNewIfConf)){
                            iErrorCount ++;
                        }
                    }else{
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*) propName.c_str() );
                        }
                    }
                }else if ( (0 == propName.compare("FQDN")) || 
                           (0 == propName.compare("IPv6Addresses")) ||
                           (0 == propName.compare("IPv4Addresses")) ||
                           (0 == propName.compare("InterfaceEnabled")) ||
                           (0 == propName.compare("MACAddress")) ||
                           (0 == propName.compare("Id"))){
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyNotWritable", 1, (char*) propName.c_str());
                        }
                }
            }
            sProp = sProp->next;
        }
    }else{
        iErrorCount ++;
    }
    return (-1 * iErrorCount);
}

int OsinetIfConfHelper::processVLAN(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;
    int iErrorCount = 0;
    string propName("");
    string propValue("");

    if (NULL != sProp){
        while (sProp){
            propName = std::string( sProp->name );
            if(sProp->value){
                propValue = std::string( sProp->value );
                if (0 == propName.compare("VLANId")){
                    std::string sPropertyPath = sPropertyPrefix + propName;
                    double dRequestSetting = 0;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestSetting)){
                        uint16_t u16RequestSetting = (uint16_t) dRequestSetting;
                        // 0 < VLAN ID < 4095
                        if (u16RequestSetting == 0 || u16RequestSetting >= 4095) {
                            iErrorCount++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                            sProp = sProp->next;
                            continue;
                        }
                        if (u16RequestSetting != pCurrentIfConf->ifVlan.u16ID){
                            pNewIfConf->ifVlan.u16ID = u16RequestSetting;
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input can't be converted to data type double." << endl;
                        iErrorCount++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("VLANEnable")){
                    std::string sPropertyPath = sPropertyPrefix + propName;
                    if (0 == propValue.compare("true")){
                        pNewIfConf->ifVlan.u8Enabled = LAN_TRUE;                
                    }else if(0 == propValue.compare("false")){
                        pNewIfConf->ifVlan.u8Enabled = LAN_FALSE;
                    }else{
                        iErrorCount++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }               
                }else if (0 == propName.compare("VLANPriority")){
                    std::string sPropertyPath = sPropertyPrefix + propName;
                    double dRequestSetting = 0;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestSetting)){
                        int u8RequestSetting = (int) dRequestSetting;
                        // Priority field of 802.1q fields. To set the priority of the VLAN ID, enter a number from 0 to 7.
                        if (u8RequestSetting < 0 || u8RequestSetting > 7) {
                            iErrorCount++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                            sProp = sProp->next;
                            continue;
                        }
                        if (u8RequestSetting != pCurrentIfConf->ifVlan.u8Priority){
                            pNewIfConf->ifVlan.u8Priority = (uint8_t)u8RequestSetting;
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input can't be converted to data type double." << endl;
                        iErrorCount++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }
                else {
                    iErrorCount ++;
                    sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                    if (NULL!=pReturnMessage){
                        //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                        //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                        ssiFillReturnMessage( pReturnMessage, "InvalidObject", 1, (char*) propName.c_str());
                    }
                }
            }
            sProp = sProp->next;
        }
    }else{
        //Can't read the hash table. In general case it is caused by invlid format. 
        iErrorCount ++;
    }
    return (-1 * iErrorCount);
}

int OsinetIfConfHelper::processOem(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    int iErrorCount = 0;
    string propName("");
    string propValue("");

    if (NULL != sProp){
        while (sProp){
            propName = std::string( sProp->name );
            if(sProp->value){
                propValue = std::string( sProp->value );
                if (0 == propName.compare("VertivEthernetInterface")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    scopeSRObject dbObject(propValue);
                    srProperty_t* sPropVertivEthernetInterface = (srProperty_t*) dbObject.getsrProperty_t();
                    if (sPropVertivEthernetInterface){
                        if (0 > processVertivEthernetInterface(sPropVertivEthernetInterface, sPropertyPath, pCurrentIfConf, pNewIfConf)){
                            iErrorCount ++;
                        }
                    }else{
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*) propName.c_str() );
                        }
                    }
                }
            }
            sProp = sProp->next;
        }
    }else{
        iErrorCount ++;
    }
    return (-1 * iErrorCount);
}


int OsinetIfConfHelper::processVertivEthernetInterface(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    int iErrorCount = 0;
    string propName("");
    string propValue("");

    if (NULL != sProp){
        while (sProp){
            propName = std::string( sProp->name );
            if(sProp->value){
                propValue = std::string( sProp->value );
                if (0 == propName.compare("VLAN")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    scopeSRObject dbObject(propValue);
                    srProperty_t* sPropVLAN = (srProperty_t*) dbObject.getsrProperty_t();
                    if (sPropVLAN){
                        if (0 > processVLAN(sPropVLAN, sPropertyPath, pCurrentIfConf, pNewIfConf)){
                            iErrorCount ++;
                        }
                    }else{
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*) propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("IPv4Configuration")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    scopeSRObject dbObject(propValue);
                    srProperty_t* sPropIPv4Configuration = (srProperty_t*) dbObject.getsrProperty_t();
                    if (sPropIPv4Configuration){
                        if (0 > processIPv4Configuration(sPropIPv4Configuration, sPropertyPath, pCurrentIfConf, pNewIfConf)){
                            iErrorCount ++;
                        }
                    }else{
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*) propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("IPv6Configuration")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    scopeSRObject dbObject(propValue);
                    srProperty_t* sPropIPv6Configuration = (srProperty_t*) dbObject.getsrProperty_t();
                    if (sPropIPv6Configuration){
                        if (0 > processIPv6Configuration(sPropIPv6Configuration, sPropertyPath, pCurrentIfConf, pNewIfConf)){
                            iErrorCount ++;
                        }
                    }else{
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*) propName.c_str() );
                        }
                    }
                }
                else if (0 == propName.compare("NetMode")){
                    std::string sPropertyPath = sPropertyPrefix + propName;
                    double dRequestSetting = 0;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestSetting)){
                        uint16_t u16RequestSetting = (uint16_t) dRequestSetting;
                        // net mode: 1~3
                        if (u16RequestSetting == 0 || u16RequestSetting > 3 ) {
                            iErrorCount++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                cout << __FUNCTION__ << " - propValue.c_str: " << propValue.c_str() << " - propName.c_str: " << propName.c_str() << endl;
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                            sProp = sProp->next;
                            continue;
                        }
                        uint8_t currentNetModeID = 0;
                        OSINET_STATUS status = avct_osinet_netmgetmode(&currentNetModeID);
                        if(status != OSINET_STATUS_OK)
                        {
                            iErrorCount++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                cout << __FUNCTION__ << " - propValue.c_str: " << propValue.c_str() << " - propName.c_str: " << propName.c_str() << endl;
                                ssiFillReturnMessage( pReturnMessage, "BackendInternalException", 2, "avct_osinet_netmgetmode", (char*)OsinetUtility::resolveStatusCode(status));
                            }
                            sProp = sProp->next;
                            continue;
                        }
                        if (u16RequestSetting != currentNetModeID){
                            OSINET_STATUS status = avct_osinet_netmsetmode(u16RequestSetting);
                            if(status != OSINET_STATUS_OK){
                                iErrorCount++;
                                sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                                if (NULL!=pReturnMessage){
                                    cout << __FUNCTION__ << " - propValue.c_str: " << propValue.c_str() << " - propName.c_str: " << propName.c_str() << endl;
                                    ssiFillReturnMessage( pReturnMessage, "BackendInternalException", 2, "avct_osinet_netmsetmode", (char*)OsinetUtility::resolveStatusCode(status));
                                }
                                sProp = sProp->next;
                                continue;
                            } 
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input can't be converted to data type double." << endl;
                        iErrorCount++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }
                else {
                    iErrorCount ++;
                    sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                    if (NULL!=pReturnMessage){
                        //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                        //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                        ssiFillReturnMessage( pReturnMessage, "InvalidObject", 1, (char*) propName.c_str());
                    }
                }
            }
            sProp = sProp->next;
        }
    }else{
        iErrorCount ++;
    }
    return (-1 * iErrorCount);
}

int OsinetIfConfHelper::processIPv4Configuration(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    int iErrorCount = 0;
    string propName("");
    string propValue("");
    if (NULL != sProp){
        while (sProp){
            propName = std::string( sProp->name );
            if(sProp->value){
                propValue = std::string( sProp->value );
                if (0 == propName.compare("NameServerOrigin")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sCurrentNameServerOrigin = "";
                    const uint16_t u16FeatureFlag = V4_DHCP_OPT_GET_DNS;                

                    const uint16_t    u16DHCPConfOpt = pCurrentIfConf->ifIPv4.u16DHCPConfOpt;
                    uint16_t    u16NewDHCPConfOpt = pNewIfConf->ifIPv4.u16DHCPConfOpt;

                    if (u16FeatureFlag & u16DHCPConfOpt){
                        sCurrentNameServerOrigin = "DHCP";
                    }else{
                        sCurrentNameServerOrigin = "Static";
                    }
                    
                    if (0 != propValue.compare(sCurrentNameServerOrigin)){
                        //Validate input
                        bool bConfigChanged = false;
                        if (0 == propValue.compare("DHCP")){
                            u16NewDHCPConfOpt |= u16FeatureFlag;
                            bConfigChanged = true;
                        }else if (0==propValue.compare("Static")){
                            u16NewDHCPConfOpt &= ~u16FeatureFlag;
                            bConfigChanged = true;
                        }else{
                            iErrorCount ++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                        }
                        //Confirm change.
                        if (bConfigChanged){
                            pNewIfConf->ifIPv4.u16DHCPConfOpt = u16NewDHCPConfOpt & (~CTRL_BITMASK_NOCONF_UINT16);
                            pNewIfConf->ifIPv4.u16DHCPConfOptMask |= u16FeatureFlag;    //OSINET REQUIREMENT: INDICATE WHICH BIT HAS CHANGED.
                        }
                    }

                }else if (0 == propName.compare("IPAddress")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET, propValue, sProcessedAddress)){
                        snprintf(pNewIfConf->ifIPv4.szIPAddress, INET_ADDRSTRLEN, "%s", (char*)sProcessedAddress.c_str());
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("Gateway")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET, propValue, sProcessedAddress)){
                        snprintf(pNewIfConf->ifIPv4.szGW, INET_ADDRSTRLEN, "%s", (char*)sProcessedAddress.c_str());
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("Mask")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET, propValue, sProcessedAddress)){
                        snprintf(pNewIfConf->ifIPv4.szNetMask, INET_ADDRSTRLEN, "%s", (char*)sProcessedAddress.c_str());
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("PreferredDNS")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET, propValue, sProcessedAddress)){
                        snprintf(pNewIfConf->ifIPv4.szDNS1, INET_ADDRSTRLEN, "%s", (char*)sProcessedAddress.c_str());
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("AlternativeDNS")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET, propValue, sProcessedAddress)){
                        snprintf(pNewIfConf->ifIPv4.szDNS2, INET_ADDRSTRLEN, "%s", (char*)sProcessedAddress.c_str());
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("DHCPEnabled")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    bool bReqDHCPEnable = false;
                    uint16_t    u16DHCPConfOpt = pCurrentIfConf->ifIPv4.u16DHCPConfOpt;
                    uint16_t    u16NewDHCPConfOpt = pNewIfConf->ifIPv4.u16DHCPConfOpt;
                    bool isDHCPEnabled = false;
                    if (u16DHCPConfOpt & V4_DHCP_ENABLE) isDHCPEnabled = true;
                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bReqDHCPEnable)){
                        //User has the request to change DHCP setting.
                        if ( bReqDHCPEnable != isDHCPEnabled){
                            if (bReqDHCPEnable == true){
                                u16NewDHCPConfOpt |= V4_DHCP_ENABLE;
                            }else{
                                u16NewDHCPConfOpt &= ~V4_DHCP_ENABLE;
                            }
                            pNewIfConf->ifIPv4.u16DHCPConfOpt = u16NewDHCPConfOpt & (~CTRL_BITMASK_NOCONF_UINT16);
                            pNewIfConf->ifIPv4.u16DHCPConfOptMask |= V4_DHCP_ENABLE;    //OSINET REQUIREMENT: INDICATE WHICH BIT HAS CHANGED.
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }
                else {
                    iErrorCount ++;
                    sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                    if (NULL!=pReturnMessage){
                        //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                        //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                        ssiFillReturnMessage( pReturnMessage, "InvalidObject", 1, (char*) propName.c_str());
                    }
                }
            }
            sProp = sProp->next;
        }
    }else{
        //Can't read the hash table. In general case it is caused by invlid format. 
        iErrorCount ++;
    }
    return (-1 * iErrorCount);
}

int OsinetIfConfHelper::processIPv6Configuration(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    int iErrorCount = 0;
    string propName("");
    string propValue("");

    if (NULL != sProp){
        while (sProp){
            propName = std::string( sProp->name );
            if(sProp->value){
                propValue = std::string( sProp->value );
                if (0 == propName.compare("DHCPv6Enabled")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    bool bCurrentDHCPv6Enabled = false;
                    bool bRequestDHCPv6Enabled = false;
                    uint16_t    u16AutoConfOpt = pCurrentIfConf->ifIPv6.u16AutoConfOpt;
                    uint16_t    u16NewAutoConfOpt = pNewIfConf->ifIPv6.u16AutoConfOpt;

                    if (V6_AUTOCONF_DHCP_ENABLE & u16AutoConfOpt){
                        bCurrentDHCPv6Enabled = true;
                    }else{
                        bCurrentDHCPv6Enabled = false; 
                    }

                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestDHCPv6Enabled)){
                        if (bRequestDHCPv6Enabled != bCurrentDHCPv6Enabled){
                            uint16_t u16ImpactedFlag = V6_AUTOCONF_DHCP_ENABLE;
                            if (true == bRequestDHCPv6Enabled){
                                u16NewAutoConfOpt |= u16ImpactedFlag;
                            }else{
                                u16NewAutoConfOpt &= ~u16ImpactedFlag;
                            }
                            pNewIfConf->ifIPv6.u16AutoConfOpt = u16NewAutoConfOpt & (~CTRL_BITMASK_NOCONF_UINT16);
                            pNewIfConf->ifIPv6.u16AutoConfOptMask |= u16ImpactedFlag;    //OSINET REQUIREMENT: INDICATE WHICH BIT HAS CHANGED.
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("HopLimit")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    const uint16_t u16CurrentSetting = pCurrentIfConf->ifIPv6.u16HopLimit;
                    double dRequestSetting;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestSetting)){
                        int u16RequestSetting = (int) dRequestSetting;
                        // Specifies the hop limit. Expected range: [0-255]
                        if (u16RequestSetting < 0 || u16RequestSetting > 255) {
                            iErrorCount ++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                            sProp = sProp->next;
                            continue;
                        }
                        if (u16RequestSetting != u16CurrentSetting){
                            pNewIfConf->ifIPv6.u16HopLimit = (uint16_t)u16RequestSetting;
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input can't be converted to data type double." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("TrafficClass")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    const uint16_t u16CurrentSetting = pCurrentIfConf->ifIPv6.u16TrafficClass;
                    double dRequestSetting;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestSetting)){
                        int u16RequestSetting = (int) dRequestSetting;
                        // Specifies the traffic class. Expected range: [0-255].
                        if (u16RequestSetting < 0 || u16RequestSetting > 255) {
                            iErrorCount ++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                            sProp = sProp->next;
                            continue;
                        }
                        if (u16RequestSetting != u16CurrentSetting){
                            pNewIfConf->ifIPv6.u16TrafficClass = (uint16_t)u16RequestSetting;
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input can't be converted to data type double." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("IPv6Enabled")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    bool bCurrentSetting = false;
                    bool bRequestSetting = false;
                    if (LAN_TRUE == pCurrentIfConf->ifIPv6.u8Enabled){
                        bCurrentSetting = true;
                    }else{
                        bCurrentSetting = false; 
                    }
                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestSetting)){
                        if (bRequestSetting != bCurrentSetting){
                            if (true == bRequestSetting){
                                pNewIfConf->ifIPv6.u8Enabled = LAN_TRUE;
                            }else{
                                pNewIfConf->ifIPv6.u8Enabled = LAN_FALSE;
                            }
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                    
                }else if (0 == propName.compare("DUID")){
                    std::string sPropertyPath = sPropertyPrefix + propName;
/*
                    cout << "Size of DUID = " << sizeof(pNewIfConf->ifIPv6.aDUID[0]) << endl;
                    cout << "Input DUID: " << propValue << endl;
                    cout << "Size of input: " << propValue.length() << endl;
*/
                    bool ret = IsValidDUIDString((char*)propValue.c_str());
                    if (ret == false) {
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                    snprintf(pNewIfConf->ifIPv6.aDUID[0], sizeof(pNewIfConf->ifIPv6.aDUID[0]), "%s", (char*)propValue.c_str());

                }else if (0 == propName.compare("NameServerOrigin")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sCurrentNameServerOrigin = "";
                    const uint16_t u16FeatureFlag = V6_AUTOCONF_OPT_GET_DNS;

                    const uint16_t u16AutoConfOpt = pCurrentIfConf->ifIPv6.u16AutoConfOpt;
                    uint16_t u16NewAutoConfOpt = pNewIfConf->ifIPv6.u16AutoConfOpt;

                    if (u16FeatureFlag & u16AutoConfOpt){
                        sCurrentNameServerOrigin = "DHCPv6";
                    }else{
                        sCurrentNameServerOrigin = "Static";
                    }
                    
                    if (0 != propValue.compare(sCurrentNameServerOrigin)){
                        //Request cause configuration change.
                        bool bConfigChanged = false;
                        if (0 == propValue.compare("DHCPv6")){
                            u16NewAutoConfOpt |= u16FeatureFlag;
                            bConfigChanged = true;
                        }else if (0 == propValue.compare("Static")){
                            u16NewAutoConfOpt &= ~u16FeatureFlag;
                            bConfigChanged = true;
                        }else{
                            iErrorCount ++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                        }
                        if (bConfigChanged){
                            pNewIfConf->ifIPv6.u16AutoConfOpt = u16NewAutoConfOpt & (~CTRL_BITMASK_NOCONF_UINT16);
                            pNewIfConf->ifIPv6.u16AutoConfOptMask |= u16FeatureFlag;    //OSINET REQUIREMENT: INDICATE WHICH BIT HAS CHANGED.
                        }
                    }

                }else if (0 == propName.compare("PreferredDNS")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET6, propValue, sProcessedAddress)){
                        if(0 != sProcessedAddress.compare(pCurrentIfConf->ifIPv6.szDNS1)){
                            snprintf(pNewIfConf->ifIPv6.szDNS1, INET6_ADDRSTRLEN, "%s", (char*)propValue.c_str());
                        }
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("AlternativeDNS")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET6, propValue, sProcessedAddress)){
                        if(0 != sProcessedAddress.compare(pCurrentIfConf->ifIPv6.szDNS2)){
                            snprintf(pNewIfConf->ifIPv6.szDNS2, INET6_ADDRSTRLEN, "%s", (char*)propValue.c_str());
                        }
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("SLAACEnabled")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    bool bCurrentSetting = false;
                    bool bRequestSetting = false;
                    const uint16_t u16FeatureFlag = V6_AUTOCONF_RA_ENABLE;

                    const uint16_t u16AutoConfOpt = pCurrentIfConf->ifIPv6.u16AutoConfOpt;
                    uint16_t u16NewAutoConfOpt = pNewIfConf->ifIPv6.u16AutoConfOpt;

                    if (u16FeatureFlag & u16AutoConfOpt){
                        bCurrentSetting = true;
                    }else{
                        bCurrentSetting = false; 
                    }

                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestSetting)){
                        if (bRequestSetting != bCurrentSetting){
                            if (true == bRequestSetting){
                                u16NewAutoConfOpt |= u16FeatureFlag;
                            }else{
                                u16NewAutoConfOpt &= ~u16FeatureFlag;
                            }
                            pNewIfConf->ifIPv6.u16AutoConfOpt = u16NewAutoConfOpt & (~CTRL_BITMASK_NOCONF_UINT16);
                            pNewIfConf->ifIPv6.u16AutoConfOptMask |= u16FeatureFlag;    //OSINET REQUIREMENT: INDICATE WHICH BIT HAS CHANGED.
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("StaticAddressesEnabled")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    bool bCurrentSetting = false;
                    bool bRequestSetting = false;
                    const uint16_t u16FeatureFlag = V6_AUTOCONF_STATIC_ENABLE;

                    const uint16_t    u16AutoConfOpt = pCurrentIfConf->ifIPv6.u16AutoConfOpt;
                    uint16_t    u16NewAutoConfOpt = pNewIfConf->ifIPv6.u16AutoConfOpt;

                    if (u16FeatureFlag & u16AutoConfOpt){
                        bCurrentSetting = true;
                    }else{
                        bCurrentSetting = false; 
                    }

                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestSetting)){
                        if (bRequestSetting != bCurrentSetting){
                            if (true == bRequestSetting){
                                u16NewAutoConfOpt |= u16FeatureFlag;
                            }else{
                                u16NewAutoConfOpt &= ~u16FeatureFlag;
                            }
                            pNewIfConf->ifIPv6.u16AutoConfOpt = u16NewAutoConfOpt & (~CTRL_BITMASK_NOCONF_UINT16);
                            pNewIfConf->ifIPv6.u16AutoConfOptMask |= u16FeatureFlag;    //OSINET REQUIREMENT: INDICATE WHICH BIT HAS CHANGED.
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("IPv6StaticAddresses")){
                    std::string sPropertyPath = sPropertyPrefix + propName;
                    RF_Array array;
                    array.setIndex(propValue);
                    if (true == array.readFromCache()){
                        if (0 > processIPv6StaticAddressList(&array, sPropertyPath, pCurrentIfConf, pNewIfConf)){
                            iErrorCount ++;
                        }
                    }else{
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. When usr implement any JSON data types other than Array, it could cause this case. 
                            //            It is possible this is redis internal error. (Unable to read data from database.)
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            //ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("DynamicRouterEnabled")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    bool bRequestSetting = false;
                    const uint8_t u8FeatureFlag = V6_ROUTER_DYNAMIC_ENABLE;
                    if (pNewIfConf->ifIPv6.u8RouterEnable & CTRL_BITMASK_NOCONF_UINT8){
                        //WARNING: DUE TO u8RouterEnable doesn't have "MASK" to indicicate which bit was configurated.
                        //         For schema opersation user might not provide all settings in u8RouterEnable.
                        //         To avoid setting be disabled during operation. Synchronze setting is necessary. 
                        pNewIfConf->ifIPv6.u8RouterEnable = pCurrentIfConf->ifIPv6.u8RouterEnable | CTRL_BITMASK_NOCONF_UINT8;
                    }

                    uint8_t u8NewRouterEnable = pNewIfConf->ifIPv6.u8RouterEnable;

                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestSetting)){
                        if (true == bRequestSetting){
                            u8NewRouterEnable |= u8FeatureFlag;
                        }else{
                            u8NewRouterEnable &= ~u8FeatureFlag;
                        }
                        pNewIfConf->ifIPv6.u8RouterEnable = u8NewRouterEnable & (~CTRL_BITMASK_NOCONF_UINT8);
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("StaticRouterEnabled")){
                    std::string sPropertyPath = sPropertyPrefix + propName;

                    bool bRequestSetting = false;
                    const uint8_t u8FeatureFlag = V6_ROUTER_STATIC_ENABLE;
                    if (pNewIfConf->ifIPv6.u8RouterEnable & CTRL_BITMASK_NOCONF_UINT8){
                        //WARNING: DUE TO u8RouterEnable doesn't have "MASK" to indicicate which bit was configurated.
                        //         For schema opersation user might not provide all settings in u8RouterEnable.
                        //         To avoid setting be disabled during operation. Synchronze setting is necessary. 
                        pNewIfConf->ifIPv6.u8RouterEnable = pCurrentIfConf->ifIPv6.u8RouterEnable | CTRL_BITMASK_NOCONF_UINT8;
                    }

                    uint8_t u8NewRouterEnable = pNewIfConf->ifIPv6.u8RouterEnable;

                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestSetting)){
                        if (true == bRequestSetting){
                            u8NewRouterEnable |= u8FeatureFlag;
                        }else{
                            u8NewRouterEnable &= ~u8FeatureFlag;
                        }
                        pNewIfConf->ifIPv6.u8RouterEnable = u8NewRouterEnable & (~CTRL_BITMASK_NOCONF_UINT8);
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }

                }else if (0 == propName.compare("IPv6StaticRouters")){
                    std::string sPropertyPath = sPropertyPrefix + propName;
                    RF_Array array;
                    array.setIndex( propValue );
                    if (true == array.readFromCache()){
                        if (0 > processIPv6StaticRouterList(&array, sPropertyPath, pCurrentIfConf, pNewIfConf)){
                            iErrorCount ++;
                        }
                    }else{
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            //ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }
                else {
                    iErrorCount ++;
                    sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                    if (NULL!=pReturnMessage){
                        //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                        //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                        ssiFillReturnMessage( pReturnMessage, "InvalidObject", 1, (char*) propName.c_str());
                    }
                }
            }
            sProp = sProp->next;
        }
        if (pNewIfConf->ifIPv6.u8RouterEnable & CTRL_BITMASK_NOCONF_UINT8){
            //Avoid corner case...
            //When CTRL_BITMASK_NOCONF_UINT8 flag ware asserted, other flags shall be deassert.
            pNewIfConf->ifIPv6.u8RouterEnable = 0 | CTRL_BITMASK_NOCONF_UINT8;
        }
    }else{
        iErrorCount ++;
    }
    return (-1 * iErrorCount);
}

int OsinetIfConfHelper::processIPv6StaticAddressList(RF_Array* pArray, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;
    int iErrorCount = 0;

    if (pArray){
        unsigned int uiArraySize = pArray->getArraySize();
        for(unsigned int index =0; index < uiArraySize; index ++){
            std::string sPrefix = sPropertyPrefix + "[" + to_string(index) + "]";

            scopeSRObject dbObject(pArray->getItemByIndex(index));
            srProperty_t* sPropIPv6StaticAddress = (srProperty_t*) dbObject.getsrProperty_t();
            if (sPropIPv6StaticAddress){
                if (0 > processIPv6StaticAddressEntry(sPropIPv6StaticAddress, sPrefix, index, pCurrentIfConf, pNewIfConf)){
                    iErrorCount ++;
                }
            }else{
                iErrorCount ++;
                sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                if (NULL!=pReturnMessage){
                    std::string sItem = pArray->getItemByIndex(index);
                    //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                    //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                    ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) sItem.c_str(), (char*) sPrefix.c_str() );
                    //ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                }
            }
        }   
    }else{
        iErrorCount ++;
    }   
    return (-1 * iErrorCount);
}

int OsinetIfConfHelper::processIPv6StaticAddressEntry(const srProperty_t* sProp, const std::string sPropertyPrefix, const unsigned int uiArrayIndex, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    int iErrorCount = 0;
    string propName("");
    string propValue("");

    if (NULL != sProp){
        while (sProp){
            propName = std::string( sProp->name );
            if(sProp->value){
                propValue = std::string( sProp->value );
                if (0 == propName.compare("Enabled")){

                    bool bCurrentSetting = false;
                    bool bRequestSetting = false;

                    if (LAN_TRUE == pCurrentIfConf->ifIPv6.AddrList[uiArrayIndex].u8Enabled){
                        bCurrentSetting = true;
                    }else{
                        bCurrentSetting = false; 
                    }

                    if (0 == SchemaDataTypeHelper::convBoolean(propValue, bRequestSetting)){
                        if (bRequestSetting != bCurrentSetting){
                            if (true == bRequestSetting){
                                pNewIfConf->ifIPv6.AddrList[uiArrayIndex].u8Enabled = LAN_TRUE;
                            }else{
                                pNewIfConf->ifIPv6.AddrList[uiArrayIndex].u8Enabled = LAN_FALSE;
                            }
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input is not boolean value." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("PrefixLength")){
                    const uint16_t u16CurrentSetting = pCurrentIfConf->ifIPv6.AddrList[uiArrayIndex].u16PrefixLen;
                    double dRequestSetting;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestSetting)){
                        int u16RequestSetting = (int) dRequestSetting;
                        // The Prefix must be a numeric value in the range from 0 to 128
                        if (u16RequestSetting < 0 || u16RequestSetting > 128) {
                            iErrorCount ++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                            sProp = sProp->next;
                            continue;
                        }
                        if (u16RequestSetting != u16CurrentSetting){
                            pNewIfConf->ifIPv6.AddrList[uiArrayIndex].u16PrefixLen = (uint16_t)u16RequestSetting;
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input can't be converted to data type double." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("Address")){
                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET6, propValue, sProcessedAddress)){
                        if(0 != sProcessedAddress.compare(pCurrentIfConf->ifIPv6.AddrList[uiArrayIndex].szAddr)){
                            snprintf(pNewIfConf->ifIPv6.AddrList[uiArrayIndex].szAddr, INET6_ADDRSTRLEN, "%s", (char*)propValue.c_str());
                        }
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("Status")){
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                            //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                            ssiFillReturnMessage( pReturnMessage, "PropertyNotWritable", 1, (char*) propName.c_str());
                        }
                }
                else {
                    iErrorCount ++;
                    sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                    if (NULL!=pReturnMessage){
                        //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                        //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                        ssiFillReturnMessage( pReturnMessage, "InvalidObject", 1, (char*) propName.c_str());
                    }
                }
            }
            sProp = sProp->next;
        }
    }else{
        //Can't read the hash table. In general case it is caused by invlid format. 
        iErrorCount ++;
    }
    return (-1 * iErrorCount);
}

int OsinetIfConfHelper::processIPv6StaticRouterList(RF_Array* pArray, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    int iErrorCount = 0;
    if (pArray){
        for(unsigned int index =0; index < pArray->getArraySize(); index ++){
            std::string sPrefix = sPropertyPrefix + "[" + to_string(index) + "]";

            scopeSRObject dbObject(pArray->getItemByIndex(index));
            srProperty_t* sPropIPv6StaticRouter = (srProperty_t*) dbObject.getsrProperty_t();
            if (sPropIPv6StaticRouter){
                if (0 > processIPv6StaticRouterEntry(sPropIPv6StaticRouter, sPrefix, index, pCurrentIfConf, pNewIfConf)){
                    iErrorCount ++;
                }
            }else{
                iErrorCount ++;
                sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                if (NULL!=pReturnMessage){
                    //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                    //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                    std::string sItem = pArray->getItemByIndex(index);
                    ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) sItem.c_str(), (char*) sPrefix.c_str());
                    //ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                }
            }
        }
    }else{
        iErrorCount ++;
    }   
    return (-1 * iErrorCount);
}

int OsinetIfConfHelper::processIPv6StaticRouterEntry(const srProperty_t* sProp, const std::string sPropertyPrefix, const unsigned int uiArrayIndex, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    int iErrorCount = 0;
    string propName("");
    string propValue("");
    if (NULL != sProp){
        while (sProp){
            propName = std::string( sProp->name );
            if(sProp->value){
                propValue = std::string( sProp->value );
                if (0 == propName.compare("MACAddress")){
                    if (0 == SchemaDataTypeHelper::checkMACAddressString((char*)propValue.c_str())){
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                        sProp = sProp->next;
                        continue;
                    }
                    const unsigned int numMac = 6;
                    char mac[numMac] = {0};
                    int iCount = sscanf((char*)propValue.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
                    if (numMac == iCount){
                        memcpy ( pNewIfConf->ifIPv6.StaticIPv6Router[uiArrayIndex].au8MAC, mac, numMac);
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Not valid MAC address." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                    //snprintf(pNewIfConf->ifIPv6.StaticIPv6Router[uiArrayIndex].au8MAC, 6, " 

                }else if (0 == propName.compare("PrefixLength")){
                    const uint16_t u16CurrentSetting = pCurrentIfConf->ifIPv6.StaticIPv6Router[uiArrayIndex].u16PrefixLen;
                    double dRequestSetting;
                    if (0 == SchemaDataTypeHelper::convDouble(propValue, dRequestSetting)){
                        int u16RequestSetting = (int) dRequestSetting;
                        // The Prefix must be a numeric value in the range from 0 to 128
                        if (u16RequestSetting < 0 || u16RequestSetting > 128) {
                            iErrorCount ++;
                            sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                            if (NULL!=pReturnMessage){
                                ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                            }
                            sProp = sProp->next;   
                            continue;
                        }
                        if (u16RequestSetting != u16CurrentSetting){
                            pNewIfConf->ifIPv6.StaticIPv6Router[uiArrayIndex].u16PrefixLen = (uint16_t)u16RequestSetting;
                        }
                    }else{
                        cout << __FUNCTION__ << __LINE__ << ": Input can't be converted to data type double." << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueTypeError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }else if (0 == propName.compare("Address")){
                    string sProcessedAddress = "";
                    if (0 == SchemaDataTypeHelper::formatAddressString(AF_INET6, propValue, sProcessedAddress)){
                        if(0 != sProcessedAddress.compare(pCurrentIfConf->ifIPv6.StaticIPv6Router[uiArrayIndex].szAddr)){
                            snprintf(pNewIfConf->ifIPv6.StaticIPv6Router[uiArrayIndex].szAddr, INET6_ADDRSTRLEN, "%s", (char*)propValue.c_str());
                        }
                    }else{
                        cout << "Address is illegal: " << propValue << endl;
                        iErrorCount ++;
                        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                        if (NULL!=pReturnMessage){
                            ssiFillReturnMessage( pReturnMessage, "PropertyValueFormatError", 2, (char*) propValue.c_str(), (char*)propName.c_str() );
                        }
                    }
                }
                else {
                    iErrorCount ++;
                    sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
                    if (NULL!=pReturnMessage){
                        //LIMITATION: Unfortunatelly, the value is not really user input. It might be the object uuid in Redis.
                        //TODO: sPropertyPrefix highlight the object path but might be inproper to the error message. Change implmentation if it is a concern.
                        ssiFillReturnMessage( pReturnMessage, "InvalidObject", 1, (char*) propName.c_str());
                    }
                }
            }
            sProp = sProp->next;
        }
    }else{
        //Can't read the hash table. In general case it is caused by invlid format. 
        iErrorCount ++;
    }
    return (-1 * iErrorCount);
}

int SchemaDataTypeHelper::convBoolean(const std::string sProperty, bool& retData){
    if (0 == sProperty.compare("true")){
        retData = true;                
    }else if(0 == sProperty.compare("false")){
        retData = false;                
    }else{
        cout << "Input = " << sProperty << ", Coverted value = " << retData <<  endl;
        return -1;
    }               
    return 0;
}

int SchemaDataTypeHelper::convDouble(const std::string sProperty, double& retData){
    try{
        retData = stod( sProperty );
    }catch(...){
        return -1;
    }
    return 0;
}

int SchemaDataTypeHelper::formatAddressString(int iFamily, const std::string sInputAddress, std::string& sProcessedAddress){
    char name_str[INET6_ADDRSTRLEN] = {0};
    switch (iFamily){
        case AF_INET6:
            {
                struct sockaddr_in6 sa = {0};
                if (0 < inet_pton(iFamily, sInputAddress.c_str(), &(sa.sin6_addr))){
                    if (NULL == inet_ntop(iFamily, &(sa.sin6_addr), name_str, INET6_ADDRSTRLEN)){
                        return -1;
                    }else{
                        sProcessedAddress = name_str;                   
                        return 0;
                    }
                }else{
                    cout << "inet_pton return failed while vaildate " << sInputAddress << endl;
                }
                break;
            }
        case AF_INET:
            {
                struct sockaddr_in sa = {0};
                if (0 < inet_pton(iFamily, sInputAddress.c_str(), &(sa.sin_addr))){
                    if (NULL == inet_ntop(iFamily, &(sa.sin_addr), name_str, INET6_ADDRSTRLEN)){
                        return -1;
                    }else{
                        sProcessedAddress = name_str;                   
                        return 0;
                    }
                    return 0;
                }else{
                    cout << "inet_pton return failed while vaildate " << sInputAddress << endl;
                }
                break;
            }
    }
    return -1;
}

int SchemaDataTypeHelper::checkMACAddressString(const char *mac){
    int i = 0;
    int s = 0;
    int cnt = 12;
    int num = 0;

    while (*mac) {
        if (isxdigit(*mac)) {
            i++;
            if (num > 1) {
                break;
            }
            num++;
        }
        else if (*mac == ':') {
            if (!num) {
                break;
            }
            num = 0;
            ++s;
        }
        else {
            s = -1;
        }
        ++mac;
    }
    return (s == 5);
}


int OsinetIfConfHelper::initOsinetIfConf(const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf){
    if (NULL == pCurrentIfConf) return -1;
    if (NULL == pNewIfConf) return -1;

    uint16_t ifConf_size = sizeof(*pNewIfConf);
    int status = avct_osinet_ifconfdatainit(pNewIfConf, &ifConf_size);
    if( status != OSINET_STATUS_OK){
        cout << "avct_osinet_ifconfdatainit return failed. " <<endl; 
        printf( "   Status code: %s (0x%02X)\n", OsinetUtility::resolveStatusCode(status), status );
        sReturnMessage* pReturnMessage = pExtendedInfoHelper_->createExtendedInfo();
        if (NULL!=pReturnMessage){
            ssiFillReturnMessage( pReturnMessage, "BackendInternalException", 2, "avct_osinet_glbconfset", (char*)OsinetUtility::resolveStatusCode(status));
        }
        return -1;
    }
    return 0;
}

NetworkInterfaceHttpPatchHandler::NetworkInterfaceHttpPatchHandler(const std::string sRreturnChannelId)
:pExtendedInfoHelper_(NULL)
{
    pExtendedInfoHelper_ = new ExtendedInfoHelper(sRreturnChannelId);
}

NetworkInterfaceHttpPatchHandler::~NetworkInterfaceHttpPatchHandler(){
    if (pExtendedInfoHelper_) delete pExtendedInfoHelper_;
}


ExtendedInfoHelper::ExtendedInfoHelper(std::string sReturnChannel)
:sReturnChannel_(sReturnChannel)
{


}

ExtendedInfoHelper::~ExtendedInfoHelper(){
    sendMessage();
    while (!retMsgs_.empty()){
        sReturnMessage* pElement = *(retMsgs_.begin());
        if (NULL != pElement){
            ssiCleanReturnMessage(pElement);
            delete pElement;
            retMsgs_.erase(retMsgs_.begin());
        }
    }
}


int ExtendedInfoHelper::addExtendedInfo(sReturnMessage* msg){
    if (NULL != msg){
        retMsgs_.push_back(msg); 
        return 0;
    }else{
        return -1;
    }
}

sReturnMessage* ExtendedInfoHelper::createExtendedInfo(){
    sReturnMessage* pReturnMessage = new sReturnMessage();
    if (NULL != pReturnMessage){
        addExtendedInfo(pReturnMessage);
    }else{
        cout << __FUNCTION__ << __LINE__ << ": new sReturnMessage() failed." << endl;
    }
    return pReturnMessage;
}

int ExtendedInfoHelper::sendMessage(){
    // Prepares a collection to store the return messages.
    char *pachColId = srNewContainerID( SR_CONT_COLLECTION );

    if ( pachColId )
    {
        if(retMsgs_.size() > 0){
            vector<sReturnMessage*>::iterator it = retMsgs_.begin();
            for (;it!=retMsgs_.end();it++){
                sReturnMessage* pRetMsg = *it;
                if (pRetMsg != NULL){
                    if ( STATUS_OK != srReturnMsgCreation( pachColId, pRetMsg->strMessageCode.c_str(),
                                                           (const char **) pRetMsg->pachParams, pRetMsg->uCount ) ){
                        cout << __FUNCTION__ << __LINE__ << ": srReturnMsgCreation return error." << endl;
                    }
                }
            }
        }else{
            sReturnMessage* pRetMsg = createExtendedInfo();
            if (pRetMsg != NULL){
                ssiFillReturnMessage( pRetMsg, "Success", 0 );
                if ( STATUS_OK != srReturnMsgCreation( pachColId, pRetMsg->strMessageCode.c_str(),
                                                       (const char **) pRetMsg->pachParams, pRetMsg->uCount ) ){
                    cout << __FUNCTION__ << __LINE__ << ": srReturnMsgCreation return error." << endl;
                }
            }else{
                cout << __FUNCTION__ << __LINE__ << ": createExtendedInfo return NULL." << endl;
            }
        }
        if ( STATUS_OK != srPublishChnl( sReturnChannel_.c_str(), pachColId ) ){
            SAPIV21_DEBUG( "The Stingray API srPublishChnl() returns failure.\n" );
            cout << __FUNCTION__ << __LINE__ << ": srPublishChnl return error." << endl;
            return -1;
        }
        free( pachColId );
        return 0;
    }
    return -1;
}
#endif
