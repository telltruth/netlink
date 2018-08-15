#ifndef SCHMEAFACTORY_H
#define SCHEMAFACTORY_H
#include "NetIntfBuilder.h"
#include <vector>

#if 0
#include <RF_Base.h>
#include "RF_EthernetInterface.v1_1_0_EthernetInterface_Oem.h"
#include "RF_EthernetInterface.v1_1_0_EthernetInterface.h"
#include "EthernetInterface.v1_1_0_MergepointEthernetInterface.v1_0_0_Plugin.h"
//#include "EthernetInterface.v1_1_0_MergepointEthernetInterface.v1_0_0_PluginInit.inc"
#include "RF_IPAddresses.v1_0_3_IPv4Address.h"

#include "RF_MergepointEthernetInterface.v1_0_0_IPv4Configuration.h"
#include "RF_MergepointEthernetInterface.v1_0_0_IPv6AddressSet.h"
#include "RF_MergepointEthernetInterface.v1_0_0_IPv6Configuration.h"
#include "RF_MergepointEthernetInterface.v1_0_0_IPv6StaticRouter.h"
#include "RF_MergepointEthernetInterface.v1_0_0_IPv6DynamicRouter.h"
#include "RF_MergepointEthernetInterface.v1_0_0_VLAN.h"
#include "RF_VLanNetworkInterface.v1_0_2_VLAN.h"
#include "RF_MergepointEthernetInterface.v1_0_0_MergepointEthernetInterface.h"
#include "RF_Message.v1_0_3_Message.h"

class SchemaHandler{
    public:
        SchemaHandler();
        virtual ~SchemaHandler();
};


class IPv6ConfigBuilder{
    private:
        RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config_;

        std::vector<RF_MergepointEthernetInterface_v1_0_0_IPv6AddressSet*> IPv6AddressSetCollection_;
        std::vector<RF_MergepointEthernetInterface_v1_0_0_IPv6StaticRouter*> IPv6StaticRouterCollection_;
        std::vector<RF_MergepointEthernetInterface_v1_0_0_IPv6DynamicRouter*> IPv6DynamicRouterCollection_;

        int initIpv6Config();

        void cleanIPv6AddressSetCollection(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config);
        void cleanIPv6StaticRouterCollection(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config);
        void cleanIPv6DynamicRouterCollection(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config);

        void buildIPv6ConfigProperty(const sIF_Conf* pIfConf);
        void buildIPv6AddressSetProperty(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config, const sIF_Conf* pIfConf);
        void buildIPv6StaticRouterProperty(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config, const sIF_Conf* pIfConf);
        void buildIPv6DynamicRouterProperty(RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config, const sIF_Conf* pIfConf);
    public:
        IPv6ConfigBuilder(const sIF_Conf* if_conf);
        virtual ~IPv6ConfigBuilder();

        RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* getRFObject(); 
};


class VertivEthernetInterfaceBuilder{
    private:
        RF_MergepointEthernetInterface_v1_0_0_MergepointEthernetInterface* pVertivEtherneInterface_;

        RF_MergepointEthernetInterface_v1_0_0_IPv4Configuration* pIpv4Config_;
        //RF_MergepointEthernetInterface_v1_0_0_IPv6Configuration* pIpv6Config_;
        IPv6ConfigBuilder* pIPv6ConfigBuilder_;

        RF_MergepointEthernetInterface_v1_0_0_VLAN* pVLanConfig_;

        void cleanPropertyFromRedis();  //Clean the cache in Redis.

        int initMergepointEthernetInterface();

        int initIpv4Config();
        int initVLanConfig();

        void buildVLanProperty(const sIF_Conf* pIfConf);
        void buildIPv4ConfigProperty(const sIF_Conf* pIfConf);
        void buildIPv6ConfigProperty(const sIF_Conf* pIfConf);

    public:
        VertivEthernetInterfaceBuilder();
        virtual ~VertivEthernetInterfaceBuilder();

        int buildProperty(std::string sIntfName, const sIF_Conf* pIfConf);

        RF_MergepointEthernetInterface_v1_0_0_MergepointEthernetInterface* getRFObject();
};

class SchemaDataTypeHelper{
    public:
        static int formatAddressString(int iFamily, const std::string sInputAddress, std::string& sProcessedAddress);
        static int checkMACAddressString(const char* mac);
        static int convBoolean(const std::string sProperty, bool& retData);
        static int convDouble(const std::string sProperty, double& retData);
};


//Used to workaround ssi's bug
class ExtendedInfoHelper{
    private:
        std::vector<sReturnMessage*> retMsgs_;
        std::string sReturnChannel_;
        int addExtendedInfo(sReturnMessage* msg);
        int sendMessage();
    public:
        ExtendedInfoHelper(std::string sReturnChannel);
        virtual ~ExtendedInfoHelper();
        sReturnMessage* createExtendedInfo();
};

class OsinetIfConfHelper{
    private:
        ExtendedInfoHelper* pExtendedInfoHelper_;
        const unsigned int uiIntfIndex_;

        int processRequest_(const srProperty_t* sProp, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processVLAN(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processOem(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processVertivEthernetInterface(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processIPv4Configuration(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processIPv6Configuration(const srProperty_t* sProp, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processIPv6StaticAddressList(RF_Array* pArray, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processIPv6StaticAddressEntry(const srProperty_t* sProp, const std::string sPropertyPrefix, const unsigned int uiArrayIndex, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processIPv6StaticRouterList(RF_Array* pArray, const std::string sPropertyPrefix, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);
        int processIPv6StaticRouterEntry(const srProperty_t* sProp, const std::string sPropertyPrefix, const unsigned int uiArrayIndex, const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewConf);

        int initOsinetIfConf(const sIF_Conf* pCurrentIfConf, sIF_Conf* pNewIfConf);  //In come case, we need to get current status.
    public:
        OsinetIfConfHelper(ExtendedInfoHelper* pExtendedInfoHelper, unsigned int uiIntfIndex);
        virtual ~OsinetIfConfHelper();
        int processRequest(const std::string sRequestObjectId); 
};

class NetworkInterfaceHttpPatchHandler{
    private:
        ExtendedInfoHelper* pExtendedInfoHelper_;

        int processNetDeviceInfoRequest(const std::string sRequestObjectId, const unsigned int uiIntfIndex);
        int processNetGlobalInfoRequest(const std::string sRequestObjectId, const unsigned int uiIntfIndex);

    public:
        int processRequest(const std::string sRequestObjectId, const unsigned int uiIntfIndex);
        NetworkInterfaceHttpPatchHandler(const std::string sRreturnChannelId);
        virtual ~NetworkInterfaceHttpPatchHandler();
};

class scopeSRObject{
    private:
        srObject_t* dbObject_;
    public:
        scopeSRObject(std::string objectUUID);
        virtual ~scopeSRObject();   
        const srProperty_t* getsrProperty_t(); 
};
#endif
/*namespace EthernetInterface_v1_1_0{*/
class NetworkInterfaceAdapter: public NetworkInterfaceInfo{
    private:
        NetworkIntfBuilder* pBuilder_;
#if 0
            //Schema & properteis.
            RF_EthernetInterface_v1_1_0_EthernetInterface* pEthernetInterface;
            RF_VLanNetworkInterface_v1_0_2_VLAN* pVLanNetIntf_;

            RF_EthernetInterface_v1_1_0_EthernetInterface_Oem* pEthernetInterfaceOem;
            VertivEthernetInterfaceBuilder* pVertivEthernetInterfaceBuilder_;

            std::vector<RF_IPAddresses_v1_0_3_IPv4Address*> IPv4AddressCollection_;
            std::vector<RF_IPAddresses_v1_0_3_IPv6Address*> IPv6AddressCollection_;
            void eraseAddressObject();

            NetworkIntfBuilder* pBuilder_;

            RF_IPAddresses_v1_0_3_IPv4Address* createIPv4AddressObj(IPv4AddressInfo* pIPv4AddressInfo);
            RF_IPAddresses_v1_0_3_IPv6Address* createIPv6AddressObj(IPv6AddressInfo* pIPv6AddressInfo);
            int initIPv4Addresses();
            int initIPv6Addresses();
            void setupIPv6AddressOrigin(const sIF_Conf* if_conf);


            int initEthernetInterfaceOem();
            int initEthernetInterface();

            void buildCommonPropertyFromOsinet();
            void buildVLanProperty(const sIF_Conf* if_conf);
            void buildInterfaceSpecificProperty();
            string getGatewayFromOsinet(int iNetFamily, char* InfNmae);
#endif
    public:
        NetworkInterfaceAdapter(NetworkIntfBuilder* pBuilder, int iIntfIndex);
        virtual ~NetworkInterfaceAdapter();
#if 0
            virtual int buildSchema();
            virtual int handleHttpPatch(std::string sRequestObjectId, std::string sRreturnChannelId);
#endif
    };
/*};*/

#endif
