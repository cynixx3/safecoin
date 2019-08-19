// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "clientversion.h"
#include "init.h"
#include "key_io.h"
#include "main.h"
#include "net.h"
#include "netbase.h"
#include "rpc/server.h"
#include "timedata.h"
#include "txmempool.h"
#include "util.h"
#include "safecoin_defs.h"
#include "safecoin_structs.h"
#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#include "wallet/walletdb.h"
#endif

#include <stdint.h>

#include <boost/assign/list_of.hpp>

#include <univalue.h>

#include "zcash/Address.hpp"

using namespace std;

/**
 * @note Do not add or change anything in the information returned by this
 * method. `getinfo` exists for backwards-compatibility only. It combines
 * information from wildly different sources in the program, which is a mess,
 * and is thus planned to be deprecated eventually.
 *
 * Based on the source of the information, new information should be added to:
 * - `getblockchaininfo`,
 * - `getnetworkinfo` or
 * - `getwalletinfo`
 *
 * Or alternatively, create a specific query method for the information.
 **/

int32_t Jumblr_depositaddradd(char *depositaddr);
int32_t Jumblr_secretaddradd(char *secretaddr);
uint64_t safecoin_interestsum();
int32_t safecoin_longestchain();
int32_t safecoin_notarized_height(int32_t *prevhtp,uint256 *hashp,uint256 *txidp);
bool safecoin_txnotarizedconfirmed(uint256 txid);
uint32_t safecoin_chainactive_timestamp();
int32_t safecoin_whoami(char *pubkeystr,int32_t height,uint32_t timestamp);
extern uint64_t SAFECOIN_INTERESTSUM,SAFECOIN_WALLETBALANCE;
extern int32_t SAFECOIN_LASTMINED,JUMBLR_PAUSE,SAFECOIN_LONGESTCHAIN;
extern char ASSETCHAINS_SYMBOL[SAFECOIN_ASSETCHAIN_MAXLEN];
uint32_t safecoin_segid32(char *coinaddr);
int64_t safecoin_coinsupply(int64_t *zfundsp,int64_t *sproutfundsp,int32_t height);
int32_t notarizedtxid_height(char *dest,char *txidstr,int32_t *safenotarized_heightp);
#define SAFECOIN_VERSION "2.0.3"
#define VERUS_VERSION "0.4.0g"
extern uint16_t ASSETCHAINS_P2PPORT,ASSETCHAINS_RPCPORT;
extern uint32_t ASSETCHAINS_CC;
extern uint32_t ASSETCHAINS_MAGIC;
extern uint64_t ASSETCHAINS_COMMISSION,ASSETCHAINS_STAKED,ASSETCHAINS_SUPPLY,ASSETCHAINS_LASTERA;
extern int32_t ASSETCHAINS_LWMAPOS,ASSETCHAINS_SAPLING;
extern uint64_t ASSETCHAINS_ENDSUBSIDY[],ASSETCHAINS_REWARD[],ASSETCHAINS_HALVING[],ASSETCHAINS_DECAY[];
extern std::string NOTARY_PUBKEY; extern uint8_t NOTARY_PUBKEY33[];
extern std::vector<std::string> vs_safecoin_notaries(int32_t height, uint32_t timestamp);
extern std::string str_safe_address(std::string pubkey);
extern bool getAddressFromIndex(const int &type, const uint160 &hash, std::string &address);
extern bool heightSort(std::pair<CAddressUnspentKey, CAddressUnspentValue> a, std::pair<CAddressUnspentKey, CAddressUnspentValue> b);


UniValue getinfo(const UniValue& params, bool fHelp)
{
    uint256 notarized_hash,notarized_desttxid; int32_t prevMoMheight,notarized_height,longestchain,safenotarized_height,txid_height;
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getinfo\n"
            "Returns an object containing various state info.\n"
            "\nResult:\n"
            "{\n"
            "  \"version\": xxxxx,           (numeric) the server version\n"
            "  \"protocolversion\": xxxxx,   (numeric) the protocol version\n"
            "  \"walletversion\": xxxxx,     (numeric) the wallet version\n"
            "  \"balance\": xxxxxxx,         (numeric) the total Safecoin balance of the wallet\n"
            "  \"blocks\": xxxxxx,           (numeric) the current number of blocks processed in the server\n"
            "  \"timeoffset\": xxxxx,        (numeric) the time offset\n"
            "  \"connections\": xxxxx,       (numeric) the number of connections\n"
            "  \"tls_connections\": xxxxx,   (numeric) the number of TLS connections\n"
            "  \"proxy\": \"host:port\",     (string, optional) the proxy used by the server\n"
            "  \"difficulty\": xxxxxx,       (numeric) the current difficulty\n"
            "  \"testnet\": true|false,      (boolean) if the server is using testnet or not\n"
            "  \"keypoololdest\": xxxxxx,    (numeric) the timestamp (seconds since GMT epoch) of the oldest pre-generated key in the key pool\n"
            "  \"keypoolsize\": xxxx,        (numeric) how many new keys are pre-generated\n"
            "  \"unlocked_until\": ttt,      (numeric) the timestamp in seconds since epoch (midnight Jan 1 1970 GMT) that the wallet is unlocked for transfers, or 0 if the wallet is locked\n"
            "  \"paytxfee\": x.xxxx,         (numeric) the transaction fee set in " + CURRENCY_UNIT + "/kB\n"
            "  \"relayfee\": x.xxxx,         (numeric) minimum relay fee for non-free transactions in " + CURRENCY_UNIT + "/kB\n"
            "  \"errors\": \"...\"           (string) any error messages\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("getinfo", "")
            + HelpExampleRpc("getinfo", "")
        );
//#ifdef ENABLE_WALLET
//    LOCK2(cs_main, pwalletMain ? &pwalletMain->cs_wallet : NULL);
//#else
    LOCK(cs_main);
//#endif

    proxyType proxy;
    GetProxy(NET_IPV4, proxy);
    notarized_height = safecoin_notarized_height(&prevMoMheight,&notarized_hash,&notarized_desttxid);
    //fprintf(stderr,"after notarized_height %u\n",(uint32_t)time(NULL));

    UniValue obj(UniValue::VOBJ);
    obj.push_back(Pair("version", CLIENT_VERSION));
    obj.push_back(Pair("protocolversion", PROTOCOL_VERSION));
    obj.push_back(Pair("SAFEversion", SAFECOIN_VERSION));
    //obj.push_back(Pair("VRSCversion", VERUS_VERSION));
    obj.push_back(Pair("notarized", notarized_height));
    obj.push_back(Pair("prevMoMheight", prevMoMheight));
    obj.push_back(Pair("notarizedhash", notarized_hash.ToString()));
    obj.push_back(Pair("notarizedtxid", notarized_desttxid.ToString()));
    txid_height = notarizedtxid_height(ASSETCHAINS_SYMBOL[0] != 0 ? (char *)"SAFE" : (char *)"BTC",(char *)notarized_desttxid.ToString().c_str(),&safenotarized_height);
    if ( txid_height > 0 )
        obj.push_back(Pair("notarizedtxid_height", txid_height));
    else obj.push_back(Pair("notarizedtxid_height", "mempool"));
    if ( ASSETCHAINS_SYMBOL[0] != 0 )
        obj.push_back(Pair("SAFEnotarized_height", safenotarized_height));
    obj.push_back(Pair("notarized_confirms", txid_height < safenotarized_height ? (safenotarized_height - txid_height + 1) : 0));
    //fprintf(stderr,"after notarized_confirms %u\n",(uint32_t)time(NULL));
#ifdef ENABLE_WALLET
    if (pwalletMain) {
        obj.push_back(Pair("walletversion", pwalletMain->GetVersion()));
        if ( ASSETCHAINS_SYMBOL[0] == 0 )
        {
            obj.push_back(Pair("interest",       ValueFromAmount(SAFECOIN_INTERESTSUM)));
            obj.push_back(Pair("balance",       ValueFromAmount(SAFECOIN_WALLETBALANCE))); //pwalletMain->GetBalance()
        }
        else
        {
            obj.push_back(Pair("balance",       ValueFromAmount(pwalletMain->GetBalance()))); //
        }
    }
#endif
    //fprintf(stderr,"after wallet %u\n",(uint32_t)time(NULL));
    obj.push_back(Pair("blocks",        (int)chainActive.Height()));
    if ( (longestchain= SAFECOIN_LONGESTCHAIN) != 0 && chainActive.Height() > longestchain )
        longestchain = chainActive.Height();
    //fprintf(stderr,"after longestchain %u\n",(uint32_t)time(NULL));
    obj.push_back(Pair("longestchain",        longestchain));
    obj.push_back(Pair("timeoffset",    GetTimeOffset()));
    if ( chainActive.LastTip() != 0 )
        obj.push_back(Pair("tiptime", (int)chainActive.LastTip()->nTime));
    obj.push_back(Pair("connections",   (int)vNodes.size()));
    obj.push_back(Pair("tls_connections", (int)std::count_if(vNodes.begin(), vNodes.end(), [](CNode* n) {return n->ssl != NULL;})));
    obj.push_back(Pair("proxy",         (proxy.IsValid() ? proxy.proxy.ToStringIPPort() : string())));
    obj.push_back(Pair("difficulty",    (double)GetDifficulty()));
    obj.push_back(Pair("testnet",       Params().TestnetToBeDeprecatedFieldRPC()));
#ifdef ENABLE_WALLET
    if (pwalletMain) {
        obj.push_back(Pair("keypoololdest", pwalletMain->GetOldestKeyPoolTime()));
        obj.push_back(Pair("keypoolsize",   (int)pwalletMain->GetKeyPoolSize()));
    }
    if (pwalletMain && pwalletMain->IsCrypted())
        obj.push_back(Pair("unlocked_until", nWalletUnlockTime));
    obj.push_back(Pair("paytxfee",      ValueFromAmount(payTxFee.GetFeePerK())));
#endif
    obj.push_back(Pair("relayfee",      ValueFromAmount(::minRelayTxFee.GetFeePerK())));
    obj.push_back(Pair("errors",        GetWarnings("statusbar")));
    {
        char pubkeystr[65]; int32_t notaryid;
        if ( (notaryid= safecoin_whoami(pubkeystr,(int32_t)chainActive.LastTip()->GetHeight(),safecoin_chainactive_timestamp())) >= 0 )
        {
            obj.push_back(Pair("notaryid",        notaryid));
            obj.push_back(Pair("pubkey",        pubkeystr));
            if ( SAFECOIN_LASTMINED != 0 )
                obj.push_back(Pair("lastmined",        SAFECOIN_LASTMINED));
        } else if ( NOTARY_PUBKEY33[0] != 0 ) {
            obj.push_back(Pair("pubkey", NOTARY_PUBKEY));
        }
    }
    if ( ASSETCHAINS_CC != 0 )
        obj.push_back(Pair("CCid",        (int)ASSETCHAINS_CC));
    obj.push_back(Pair("name",        ASSETCHAINS_SYMBOL[0] == 0 ? "SAFE" : ASSETCHAINS_SYMBOL));
    obj.push_back(Pair("sapling", ASSETCHAINS_SAPLING));

    obj.push_back(Pair("p2pport",        ASSETCHAINS_P2PPORT));
    obj.push_back(Pair("rpcport",        ASSETCHAINS_RPCPORT));
    if ( ASSETCHAINS_SYMBOL[0] != 0 )
    {
        //obj.push_back(Pair("name",        ASSETCHAINS_SYMBOL));
        obj.push_back(Pair("magic",        (int)ASSETCHAINS_MAGIC));
        obj.push_back(Pair("premine",        ASSETCHAINS_SUPPLY));

        if ( ASSETCHAINS_REWARD[0] != 0 || ASSETCHAINS_LASTERA > 0 )
        {
            std::string acReward = "", acHalving = "", acDecay = "", acEndSubsidy = "";
            for (int i = 0; i <= ASSETCHAINS_LASTERA; i++)
            {
                if (i == 0)
                {
                    acReward = std::to_string(ASSETCHAINS_REWARD[i]);
                    acHalving = std::to_string(ASSETCHAINS_HALVING[i]);
                    acDecay = std::to_string(ASSETCHAINS_DECAY[i]);
                    acEndSubsidy = std::to_string(ASSETCHAINS_ENDSUBSIDY[i]);
                }
                else
                {
                    acReward += "," + std::to_string(ASSETCHAINS_REWARD[i]);
                    acHalving += "," + std::to_string(ASSETCHAINS_HALVING[i]);
                    acDecay += "," + std::to_string(ASSETCHAINS_DECAY[i]);
                    acEndSubsidy += "," + std::to_string(ASSETCHAINS_ENDSUBSIDY[i]);
                }
            }
            if (ASSETCHAINS_LASTERA > 0)
                obj.push_back(Pair("eras", ASSETCHAINS_LASTERA + 1));
            obj.push_back(Pair("reward", acReward));
            obj.push_back(Pair("halving", acHalving));
            obj.push_back(Pair("decay", acDecay));
            obj.push_back(Pair("endsubsidy", acEndSubsidy));
        }

        if ( ASSETCHAINS_COMMISSION != 0 )
            obj.push_back(Pair("commission",        ASSETCHAINS_COMMISSION));
        if ( ASSETCHAINS_STAKED != 0 )
            obj.push_back(Pair("staked",        ASSETCHAINS_STAKED));
        if ( ASSETCHAINS_LWMAPOS != 0 )
            obj.push_back(Pair("veruspos", ASSETCHAINS_LWMAPOS));
    }
    return obj;
}

#ifdef ENABLE_WALLET
class DescribeAddressVisitor : public boost::static_visitor<UniValue>
{
public:
    UniValue operator()(const CNoDestination &dest) const { return UniValue(UniValue::VOBJ); }

    UniValue operator()(const CKeyID &keyID) const {
        UniValue obj(UniValue::VOBJ);
        CPubKey vchPubKey;
        obj.push_back(Pair("isscript", false));
        if (pwalletMain && pwalletMain->GetPubKey(keyID, vchPubKey)) {
            obj.push_back(Pair("pubkey", HexStr(vchPubKey))); // should return pubkeyhash, but not sure about compatibility impact
            obj.push_back(Pair("iscompressed", vchPubKey.IsCompressed()));
        }
        return obj;
    }

    UniValue operator()(const CPubKey &key) const {
        UniValue obj(UniValue::VOBJ);
        obj.push_back(Pair("isscript", false));
        if (pwalletMain && key.IsValid()) {
            obj.push_back(Pair("pubkey", HexStr(key)));
            obj.push_back(Pair("iscompressed", key.IsCompressed()));
        }
        else
        {
            obj.push_back(Pair("pubkey", "invalid"));
        }
        return obj;
    }

    UniValue operator()(const CScriptID &scriptID) const {
        UniValue obj(UniValue::VOBJ);
        CScript subscript;
        obj.push_back(Pair("isscript", true));
        if (pwalletMain && pwalletMain->GetCScript(scriptID, subscript)) {
            std::vector<CTxDestination> addresses;
            txnouttype whichType;
            int nRequired;
            ExtractDestinations(subscript, whichType, addresses, nRequired);
            obj.push_back(Pair("script", GetTxnOutputType(whichType)));
            obj.push_back(Pair("hex", HexStr(subscript.begin(), subscript.end())));
            UniValue a(UniValue::VARR);
            for (const CTxDestination& addr : addresses) {
                a.push_back(EncodeDestination(addr));
            }
            obj.push_back(Pair("addresses", a));
            if (whichType == TX_MULTISIG)
                obj.push_back(Pair("sigsrequired", nRequired));
        }
        return obj;
    }
};
#endif

UniValue coinsupply(const UniValue& params, bool fHelp)
{
    int32_t height = 0; int32_t currentHeight; int64_t sproutfunds,zfunds,supply = 0; UniValue result(UniValue::VOBJ);
    if (fHelp || params.size() > 1)
        throw runtime_error("coinsupply <height>\n"
            "\nReturn coin supply information at a given block height. If no height is given, the current height is used.\n"
            "\nArguments:\n"
            "1. \"height\"     (integer, optional) Block height\n"
            "\nResult:\n"
            "{\n"
            "  \"result\" : \"success\",         (string) If the request was successful.\n"
            "  \"coin\" : \"SAFE\",               (string) The currency symbol of the coin for asset chains, otherwise SAFE.\n"
            "  \"height\" : 420,               (integer) The height of this coin supply data\n"
            "  \"supply\" : \"777.0\",           (float) The transparent coin supply\n"
            "  \"zfunds\" : \"0.777\",           (float) The shielded coin supply (in zaddrs)\n"
            "  \"sprout\" : \"0.077\",           (float) The sprout coin supply (in zcaddrs)\n"
           "  \"total\" :  \"777.777\",         (float) The total coin supply, i.e. sum of supply + zfunds\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("coinsupply", "420")
            + HelpExampleRpc("coinsupply", "420")
        );
    if ( params.size() == 0 )
        height = chainActive.Height();
    else height = atoi(params[0].get_str());
    currentHeight = chainActive.Height();

    if (height >= 0 && height <= currentHeight) {
        if ( (supply= safecoin_coinsupply(&zfunds,&sproutfunds,height)) > 0 )
        {
            result.push_back(Pair("result", "success"));
            result.push_back(Pair("coin", ASSETCHAINS_SYMBOL[0] == 0 ? "SAFE" : ASSETCHAINS_SYMBOL));
            result.push_back(Pair("height", (int)height));
            result.push_back(Pair("supply", ValueFromAmount(supply)));
            result.push_back(Pair("zfunds", ValueFromAmount(zfunds)));
            result.push_back(Pair("sprout", ValueFromAmount(sproutfunds)));
            result.push_back(Pair("total", ValueFromAmount(zfunds + supply)));
        } else result.push_back(Pair("error", "couldnt calculate supply"));
    } else {
        result.push_back(Pair("error", "invalid height"));
    }
    return(result);
}

UniValue jumblr_deposit(const UniValue& params, bool fHelp)
{
    int32_t retval; UniValue result(UniValue::VOBJ);
    if (fHelp || params.size() != 1)
        throw runtime_error("jumblr_deposit \"depositaddress\"\n");
    CBitcoinAddress address(params[0].get_str());
    bool isValid = address.IsValid();
    if ( isValid != 0 )
    {
        string addr = params[0].get_str();
        if ( (retval= Jumblr_depositaddradd((char *)addr.c_str())) >= 0 )
        {
            result.push_back(Pair("result", retval));
            JUMBLR_PAUSE = 0;
        }
        else result.push_back(Pair("error", retval));
    } else result.push_back(Pair("error", "invalid address"));
    return(result);
}

UniValue jumblr_secret(const UniValue& params, bool fHelp)
{
    int32_t retval; UniValue result(UniValue::VOBJ);
    if (fHelp || params.size() != 1)
        throw runtime_error("jumblr_secret \"secretaddress\"\n");
    CBitcoinAddress address(params[0].get_str());
    bool isValid = address.IsValid();
    if ( isValid != 0 )
    {
        string addr = params[0].get_str();
        retval = Jumblr_secretaddradd((char *)addr.c_str());
        result.push_back(Pair("result", "success"));
        result.push_back(Pair("num", retval));
        JUMBLR_PAUSE = 0;
    } else result.push_back(Pair("error", "invalid address"));
    return(result);
}

UniValue jumblr_pause(const UniValue& params, bool fHelp)
{
    int32_t retval; UniValue result(UniValue::VOBJ);
    if (fHelp )
        throw runtime_error("jumblr_pause\n");
    JUMBLR_PAUSE = 1;
    result.push_back(Pair("result", "paused"));
    return(result);
}

UniValue jumblr_resume(const UniValue& params, bool fHelp)
{
    int32_t retval; UniValue result(UniValue::VOBJ);
    if (fHelp )
        throw runtime_error("jumblr_resume\n");
    JUMBLR_PAUSE = 0;
    result.push_back(Pair("result", "resumed"));
    return(result);
}

UniValue validateaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "validateaddress \"safecoinaddress\"\n"
            "\nReturn information about the given Safecoin address.\n"
            "\nArguments:\n"
            "1. \"safecoinaddress\"     (string, required) The Safecoin address to validate\n"
            "\nResult:\n"
            "{\n"
            "  \"isvalid\" : true|false,         (boolean) If the address is valid or not. If not, this is the only property returned.\n"
            "  \"address\" : \"safecoinaddress\",   (string) The Safecoin address validated\n"
            "  \"scriptPubKey\" : \"hex\",       (string) The hex encoded scriptPubKey generated by the address\n"
            "  \"ismine\" : true|false,          (boolean) If the address is yours or not\n"
            "  \"isscript\" : true|false,        (boolean) If the key is a script\n"
            "  \"pubkey\" : \"publickeyhex\",    (string) The hex value of the raw public key\n"
            "  \"iscompressed\" : true|false,    (boolean) If the address is compressed\n"
            "  \"account\" : \"account\"         (string) DEPRECATED. The account associated with the address, \"\" is the default account\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("validateaddress", "\"RTZMZHDFSTFQst8XmX2dR4DaH87cEUs3gC\"")
            + HelpExampleRpc("validateaddress", "\"RTZMZHDFSTFQst8XmX2dR4DaH87cEUs3gC\"")
        );

#ifdef ENABLE_WALLET
    LOCK2(cs_main, pwalletMain ? &pwalletMain->cs_wallet : NULL);
#else
    LOCK(cs_main);
#endif

    CTxDestination dest = DecodeDestination(params[0].get_str());
    bool isValid = IsValidDestination(dest);

    UniValue ret(UniValue::VOBJ);
    ret.push_back(Pair("isvalid", isValid));
    if (isValid)
    {
        std::string currentAddress = EncodeDestination(dest);
        ret.push_back(Pair("address", currentAddress));

        CScript scriptPubKey = GetScriptForDestination(dest);
        ret.push_back(Pair("scriptPubKey", HexStr(scriptPubKey.begin(), scriptPubKey.end())));
        ret.push_back(Pair("segid", (int32_t)safecoin_segid32((char *)params[0].get_str().c_str()) & 0x3f));
#ifdef ENABLE_WALLET
        isminetype mine = pwalletMain ? IsMine(*pwalletMain, dest) : ISMINE_NO;
        ret.push_back(Pair("ismine", (mine & ISMINE_SPENDABLE) ? true : false));
        ret.push_back(Pair("iswatchonly", (mine & ISMINE_WATCH_ONLY) ? true: false));
        UniValue detail = boost::apply_visitor(DescribeAddressVisitor(), dest);
        ret.pushKVs(detail);
        if (pwalletMain && pwalletMain->mapAddressBook.count(dest))
            ret.push_back(Pair("account", pwalletMain->mapAddressBook[dest].name));
#endif
    }
    return ret;
}


class DescribePaymentAddressVisitor : public boost::static_visitor<UniValue>
{
public:
    UniValue operator()(const libzcash::InvalidEncoding &zaddr) const { return UniValue(UniValue::VOBJ); }

    UniValue operator()(const libzcash::SproutPaymentAddress &zaddr) const {
        UniValue obj(UniValue::VOBJ);
        obj.push_back(Pair("type", "sprout"));
        obj.push_back(Pair("payingkey", zaddr.a_pk.GetHex()));
        obj.push_back(Pair("transmissionkey", zaddr.pk_enc.GetHex()));
#ifdef ENABLE_WALLET
        if (pwalletMain) {
            obj.push_back(Pair("ismine", pwalletMain->HaveSproutSpendingKey(zaddr)));
        }
#endif
        return obj;
    }

    UniValue operator()(const libzcash::SaplingPaymentAddress &zaddr) const {
        UniValue obj(UniValue::VOBJ);
        obj.push_back(Pair("type", "sapling"));
        obj.push_back(Pair("diversifier", HexStr(zaddr.d)));
        obj.push_back(Pair("diversifiedtransmissionkey", zaddr.pk_d.GetHex()));
#ifdef ENABLE_WALLET
        if (pwalletMain) {
            libzcash::SaplingIncomingViewingKey ivk;
            libzcash::SaplingFullViewingKey fvk;
            bool isMine = pwalletMain->GetSaplingIncomingViewingKey(zaddr, ivk) &&
                pwalletMain->GetSaplingFullViewingKey(ivk, fvk) &&
                pwalletMain->HaveSaplingSpendingKey(fvk);
            obj.push_back(Pair("ismine", isMine));
        }
#endif
        return obj;
    }
};

UniValue z_validateaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "z_validateaddress \"zaddr\"\n"
            "\nReturn information about the given z address.\n"
            "\nArguments:\n"
            "1. \"zaddr\"     (string, required) The z address to validate\n"
            "\nResult:\n"
            "{\n"
            "  \"isvalid\" : true|false,      (boolean) If the address is valid or not. If not, this is the only property returned.\n"
            "  \"address\" : \"zaddr\",         (string) The z address validated\n"
            "  \"type\" : \"xxxx\",             (string) \"sprout\" or \"sapling\"\n"
            "  \"ismine\" : true|false,       (boolean) If the address is yours or not\n"
            "  \"payingkey\" : \"hex\",         (string) [sprout] The hex value of the paying key, a_pk\n"
            "  \"transmissionkey\" : \"hex\",   (string) [sprout] The hex value of the transmission key, pk_enc\n"
            "  \"diversifier\" : \"hex\",       (string) [sapling] The hex value of the diversifier, d\n"
            "  \"diversifiedtransmissionkey\" : \"hex\", (string) [sapling] The hex value of pk_d\n"

            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("z_validateaddress", "\"zcWsmqT4X2V4jgxbgiCzyrAfRT1vi1F4sn7M5Pkh66izzw8Uk7LBGAH3DtcSMJeUb2pi3W4SQF8LMKkU2cUuVP68yAGcomL\"")
            + HelpExampleRpc("z_validateaddress", "\"zcWsmqT4X2V4jgxbgiCzyrAfRT1vi1F4sn7M5Pkh66izzw8Uk7LBGAH3DtcSMJeUb2pi3W4SQF8LMKkU2cUuVP68yAGcomL\"")
        );


#ifdef ENABLE_WALLET
    LOCK2(cs_main, pwalletMain->cs_wallet);
#else
    LOCK(cs_main);
#endif

    string strAddress = params[0].get_str();
    auto address = DecodePaymentAddress(strAddress);
    bool isValid = IsValidPaymentAddress(address);

    UniValue ret(UniValue::VOBJ);
    ret.push_back(Pair("isvalid", isValid));
    if (isValid)
    {
        ret.push_back(Pair("address", strAddress));
        UniValue detail = boost::apply_visitor(DescribePaymentAddressVisitor(), address);
        ret.pushKVs(detail);
    }
    return ret;
}


/**
 * Used by addmultisigaddress / createmultisig:
 */
CScript _createmultisig_redeemScript(const UniValue& params)
{
    int nRequired = params[0].get_int();
    const UniValue& keys = params[1].get_array();

    // Gather public keys
    if (nRequired < 1)
        throw runtime_error("a multisignature address must require at least one key to redeem");
    if ((int)keys.size() < nRequired)
        throw runtime_error(
            strprintf("not enough keys supplied "
                      "(got %u keys, but need at least %d to redeem)", keys.size(), nRequired));
    if (keys.size() > 16)
        throw runtime_error("Number of addresses involved in the multisignature address creation > 16\nReduce the number");
    std::vector<CPubKey> pubkeys;
    pubkeys.resize(keys.size());
    for (unsigned int i = 0; i < keys.size(); i++)
    {
        const std::string& ks = keys[i].get_str();
#ifdef ENABLE_WALLET
        // Case 1: Bitcoin address and we have full public key:
        CTxDestination dest = DecodeDestination(ks);
        if (pwalletMain && IsValidDestination(dest)) {
            const CKeyID *keyID = boost::get<CKeyID>(&dest);
            if (!keyID) {
                throw std::runtime_error(strprintf("%s does not refer to a key", ks));
            }
            CPubKey vchPubKey;
            if (!pwalletMain->GetPubKey(*keyID, vchPubKey)) {
                throw std::runtime_error(strprintf("no full public key for address %s", ks));
            }
            if (!vchPubKey.IsFullyValid())
                throw runtime_error(" Invalid public key: "+ks);
            pubkeys[i] = vchPubKey;
        }

        // Case 2: hex public key
        else
#endif
        if (IsHex(ks))
        {
            CPubKey vchPubKey(ParseHex(ks));
            if (!vchPubKey.IsFullyValid())
                throw runtime_error(" Invalid public key: "+ks);
            pubkeys[i] = vchPubKey;
        }
        else
        {
            throw runtime_error(" Invalid public key: "+ks);
        }
    }
    CScript result = GetScriptForMultisig(nRequired, pubkeys);

    if (result.size() > MAX_SCRIPT_ELEMENT_SIZE)
        throw runtime_error(
                strprintf("redeemScript exceeds size limit: %d > %d", result.size(), MAX_SCRIPT_ELEMENT_SIZE));

    return result;
}

UniValue createmultisig(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 2)
    {
        string msg = "createmultisig nrequired [\"key\",...]\n"
            "\nCreates a multi-signature address with n signature of m keys required.\n"
            "It returns a json object with the address and redeemScript.\n"

            "\nArguments:\n"
            "1. nrequired      (numeric, required) The number of required signatures out of the n keys or addresses.\n"
            "2. \"keys\"       (string, required) A json array of keys which are Safecoin addresses or hex-encoded public keys\n"
            "     [\n"
            "       \"key\"    (string) Safecoin address or hex-encoded public key\n"
            "       ,...\n"
            "     ]\n"

            "\nResult:\n"
            "{\n"
            "  \"address\":\"multisigaddress\",  (string) The value of the new multisig address.\n"
            "  \"redeemScript\":\"script\"       (string) The string value of the hex-encoded redemption script.\n"
            "}\n"

            "\nExamples:\n"
            "\nCreate a multisig address from 2 addresses\n"
            + HelpExampleCli("createmultisig", "2 \"[\\\"RTZMZHDFSTFQst8XmX2dR4DaH87cEUs3gC\\\",\\\"RNKiEBduBru6Siv1cZRVhp4fkZNyPska6z\\\"]\"") +
            "\nAs a json rpc call\n"
            + HelpExampleRpc("createmultisig", "2, \"[\\\"RTZMZHDFSTFQst8XmX2dR4DaH87cEUs3gC\\\",\\\"RNKiEBduBru6Siv1cZRVhp4fkZNyPska6z\\\"]\"")
        ;
        throw runtime_error(msg);
    }

    // Construct using pay-to-script-hash:
    CScript inner = _createmultisig_redeemScript(params);
    CScriptID innerID(inner);

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("address", EncodeDestination(innerID)));
    result.push_back(Pair("redeemScript", HexStr(inner.begin(), inner.end())));

    return result;
}

UniValue verifymessage(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 3)
        throw runtime_error(
            "verifymessage \"safecoinaddress\" \"signature\" \"message\"\n"
            "\nVerify a signed message\n"
            "\nArguments:\n"
            "1. \"safecoinaddress\"    (string, required) The Safecoin address to use for the signature.\n"
            "2. \"signature\"       (string, required) The signature provided by the signer in base 64 encoding (see signmessage).\n"
            "3. \"message\"         (string, required) The message that was signed.\n"
            "\nResult:\n"
            "true|false   (boolean) If the signature is verified or not.\n"
            "\nExamples:\n"
            "\nUnlock the wallet for 30 seconds\n"
            + HelpExampleCli("walletpassphrase", "\"mypassphrase\" 30") +
            "\nCreate the signature\n"
            + HelpExampleCli("signmessage", "\"RNKiEBduBru6Siv1cZRVhp4fkZNyPska6z\" \"my message\"") +
            "\nVerify the signature\n"
            + HelpExampleCli("verifymessage", "\"RNKiEBduBru6Siv1cZRVhp4fkZNyPska6z\" \"signature\" \"my message\"") +
            "\nAs json rpc\n"
            + HelpExampleRpc("verifymessage", "\"RNKiEBduBru6Siv1cZRVhp4fkZNyPska6z\", \"signature\", \"my message\"")
        );

    LOCK(cs_main);

    string strAddress  = params[0].get_str();
    string strSign     = params[1].get_str();
    string strMessage  = params[2].get_str();

    CTxDestination destination = DecodeDestination(strAddress);
    if (!IsValidDestination(destination)) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid address");
    }

    const CKeyID *keyID = boost::get<CKeyID>(&destination);
    if (!keyID) {
        throw JSONRPCError(RPC_TYPE_ERROR, "Address does not refer to key");
    }

    bool fInvalid = false;
    vector<unsigned char> vchSig = DecodeBase64(strSign.c_str(), &fInvalid);

    if (fInvalid)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Malformed base64 encoding");

    CHashWriter ss(SER_GETHASH, 0);
    ss << strMessageMagic;
    ss << strMessage;

    CPubKey pubkey;
    if (!pubkey.RecoverCompact(ss.GetHash(), vchSig))
        return false;

    return (pubkey.GetID() == *keyID);
}

UniValue setmocktime(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "setmocktime timestamp\n"
            "\nSet the local time to given timestamp (-regtest only)\n"
            "\nArguments:\n"
            "1. timestamp  (integer, required) Unix seconds-since-epoch timestamp\n"
            "   Pass 0 to go back to using the system time."
        );

    if (!Params().MineBlocksOnDemand())
        throw runtime_error("setmocktime for regression testing (-regtest mode) only");

    // cs_vNodes is locked and node send/receive times are updated
    // atomically with the time change to prevent peers from being
    // disconnected because we think we haven't communicated with them
    // in a long time.
    LOCK2(cs_main, cs_vNodes);

    RPCTypeCheck(params, boost::assign::list_of(UniValue::VNUM));
    SetMockTime(params[0].get_int64());

    uint64_t t = GetTime();
    BOOST_FOREACH(CNode* pnode, vNodes) {
        pnode->nLastSend = pnode->nLastRecv = t;
    }

    return NullUniValue;
}

bool getAddressFromIndex(const int &type, const uint160 &hash, std::string &address)
{
    if (type == 2) {
        address = CBitcoinAddress(CScriptID(hash)).ToString();
    } else if (type == 1) {
        address = CBitcoinAddress(CKeyID(hash)).ToString();
    }
    else {
        return false;
    }
    return true;
}

bool getAddressesFromParams(const UniValue& params, std::vector<std::pair<uint160, int> > &addresses)
{
    if (params[0].isStr()) {
        CBitcoinAddress address(params[0].get_str());
        uint160 hashBytes;
        int type = 0;
        if (!address.GetIndexKey(hashBytes, type)) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid address");
        }
        addresses.push_back(std::make_pair(hashBytes, type));
    } else if (params[0].isObject()) {

        UniValue addressValues = find_value(params[0].get_obj(), "addresses");
        if (!addressValues.isArray()) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Addresses is expected to be an array");
        }

        std::vector<UniValue> values = addressValues.getValues();

        for (std::vector<UniValue>::iterator it = values.begin(); it != values.end(); ++it) {

            CBitcoinAddress address(it->get_str());
            uint160 hashBytes;
            int type = 0;
            if (!address.GetIndexKey(hashBytes, type)) {
                throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid addresses");
            }
            addresses.push_back(std::make_pair(hashBytes, type));
        }
    } else {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid addresse");
    }

    return true;
}

bool heightSort(std::pair<CAddressUnspentKey, CAddressUnspentValue> a,
                std::pair<CAddressUnspentKey, CAddressUnspentValue> b) {
    return a.second.blockHeight < b.second.blockHeight;
}

bool timestampSort(std::pair<CMempoolAddressDeltaKey, CMempoolAddressDelta> a,
                   std::pair<CMempoolAddressDeltaKey, CMempoolAddressDelta> b) {
    return a.second.time < b.second.time;
}

UniValue getaddressmempool(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getaddressmempool\n"
            "\nReturns all mempool deltas for an address (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ]\n"
            "}\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"address\"  (string) The base58check encoded address\n"
            "    \"txid\"  (string) The related txid\n"
            "    \"index\"  (number) The related input or output index\n"
            "    \"satoshis\"  (number) The difference of satoshis\n"
            "    \"timestamp\"  (number) The time the transaction entered the mempool (seconds)\n"
            "    \"prevtxid\"  (string) The previous txid (if spending)\n"
            "    \"prevout\"  (string) The previous transaction output index (if spending)\n"
            "  }\n"
            "]\n"
            "\nExamples:\n"
            + HelpExampleCli("getaddressmempool", "'{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}'")
            + HelpExampleRpc("getaddressmempool", "{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}")
        );

    std::vector<std::pair<uint160, int> > addresses;

    if (!getAddressesFromParams(params, addresses)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid address");
    }

    std::vector<std::pair<CMempoolAddressDeltaKey, CMempoolAddressDelta> > indexes;

    if (!mempool.getAddressIndex(addresses, indexes)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
    }

    std::sort(indexes.begin(), indexes.end(), timestampSort);

    UniValue result(UniValue::VARR);

    for (std::vector<std::pair<CMempoolAddressDeltaKey, CMempoolAddressDelta> >::iterator it = indexes.begin();
         it != indexes.end(); it++) {

        std::string address;
        if (!getAddressFromIndex(it->first.type, it->first.addressBytes, address)) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unknown address type");
        }

        UniValue delta(UniValue::VOBJ);
        delta.push_back(Pair("address", address));
        delta.push_back(Pair("txid", it->first.txhash.GetHex()));
        delta.push_back(Pair("index", (int)it->first.index));
        delta.push_back(Pair("satoshis", it->second.amount));
        delta.push_back(Pair("timestamp", it->second.time));
        if (it->second.amount < 0) {
            delta.push_back(Pair("prevtxid", it->second.prevhash.GetHex()));
            delta.push_back(Pair("prevout", (int)it->second.prevout));
        }
        result.push_back(delta);
    }

    return result;
}

UniValue getaddressutxos(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getaddressutxos\n"
            "\nReturns all unspent outputs for an address (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ],\n"
            "  \"chainInfo\"  (boolean) Include chain info with results\n"
            "}\n"
            "\nResult\n"
            "[\n"
            "  {\n"
            "    \"address\"  (string) The address base58check encoded\n"
            "    \"txid\"  (string) The output txid\n"
            "    \"height\"  (number) The block height\n"
            "    \"outputIndex\"  (number) The output index\n"
            "    \"script\"  (strin) The script hex encoded\n"
            "    \"satoshis\"  (number) The number of satoshis of the output\n"
            "  }\n"
            "]\n"
            "\nExamples:\n"
            + HelpExampleCli("getaddressutxos", "'{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}'")
            + HelpExampleRpc("getaddressutxos", "{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}")
            );

    bool includeChainInfo = false;
    if (params[0].isObject()) {
        UniValue chainInfo = find_value(params[0].get_obj(), "chainInfo");
        if (chainInfo.isBool()) {
            includeChainInfo = chainInfo.get_bool();
        }
    }

    std::vector<std::pair<uint160, int> > addresses;

    if (!getAddressesFromParams(params, addresses)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid address");
    }

    std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue> > unspentOutputs;

    for (std::vector<std::pair<uint160, int> >::iterator it = addresses.begin(); it != addresses.end(); it++) {
        if (!GetAddressUnspent((*it).first, (*it).second, unspentOutputs)) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
        }
    }

    std::sort(unspentOutputs.begin(), unspentOutputs.end(), heightSort);

    UniValue utxos(UniValue::VARR);

    for (std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue> >::const_iterator it=unspentOutputs.begin(); it!=unspentOutputs.end(); it++) {
        UniValue output(UniValue::VOBJ);
        std::string address;
        if (!getAddressFromIndex(it->first.type, it->first.hashBytes, address)) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unknown address type");
        }

        output.push_back(Pair("address", address));
        output.push_back(Pair("txid", it->first.txhash.GetHex()));
        output.push_back(Pair("outputIndex", (int)it->first.index));
        output.push_back(Pair("script", HexStr(it->second.script.begin(), it->second.script.end())));
        output.push_back(Pair("satoshis", it->second.satoshis));
        output.push_back(Pair("height", it->second.blockHeight));
        utxos.push_back(output);
    }

    if (includeChainInfo) {
        UniValue result(UniValue::VOBJ);
        result.push_back(Pair("utxos", utxos));

        LOCK(cs_main);
        result.push_back(Pair("hash", chainActive.LastTip()->GetBlockHash().GetHex()));
        result.push_back(Pair("height", (int)chainActive.Height()));
        return result;
    } else {
        return utxos;
    }
}


UniValue listutxos(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "listutxos \"address\" ( minconf )\n"
            "\nReturns all unspent outputs for an address (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "1. \"address\"  (string) The base58check encoded address\n"
            "2. minconf (number) Minimum number of confirmations\n"
            "}\n"
            "\nResult\n"
            "[\n"
            "  {\n"
            "    \"address\"  (string) The address base58check encoded\n"
            "    \"txid\"  (string) The output txid\n"
            "    \"height\"  (number) The block height\n"
            "    \"outputIndex\"  (number) The output index\n"
            "    \"script\"  (strin) The script hex encoded\n"
            "    \"satoshis\"  (number) The number of satoshis of the output\n"
            "  }\n"
            "]\n"
            "\nExamples:\n"
            + HelpExampleCli("listutxos", "\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\" 100")
            + HelpExampleRpc("listutxos", "\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\" 100")
            );
	
	std::string str_address = params[0].get_str();
	uint32_t minconf = (params.size() == 2) ? params[1].get_int() : 1;
	
	CBitcoinAddress address(str_address);
    uint160 hashBytes;
    int type = 0;
    std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue> > unspentOutputs;
    
    if (!address.GetIndexKey(hashBytes, type))
    {
		throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid addresses");
    }

    if (!GetAddressUnspent(hashBytes, type, unspentOutputs))
    {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
    }

    std::sort(unspentOutputs.begin(), unspentOutputs.end(), heightSort);

    
    UniValue uv_result(UniValue::VOBJ);
    UniValue uv_utxos(UniValue::VARR);
    UniValue uv_balance(UniValue::VOBJ);
	int64_t balance_satoshis = 0;
	
    for (std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue>>::const_iterator it = unspentOutputs.begin(); it != unspentOutputs.end(); it++)
    {
        UniValue output(UniValue::VOBJ);
        std::string tmp_address;
        if (!getAddressFromIndex(it->first.type, it->first.hashBytes, tmp_address)) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unknown address type");
        }
		
		uint32_t confirmations = chainActive.Height() - it->second.blockHeight;
		
		if (confirmations >= minconf)
		{
			output.push_back(Pair("address", tmp_address));
			output.push_back(Pair("txid", it->first.txhash.GetHex()));
			output.push_back(Pair("outputIndex", (int)it->first.index));
			output.push_back(Pair("script", HexStr(it->second.script.begin(), it->second.script.end())));
			output.push_back(Pair("satoshis", it->second.satoshis));
			output.push_back(Pair("height", it->second.blockHeight));
			output.push_back(Pair("confirmations", (int32_t)confirmations));
			uv_utxos.push_back(output);
			balance_satoshis += it->second.satoshis;
		}
    }
    
    uv_result.push_back(Pair("utxos", uv_utxos));
    uv_result.push_back(Pair("balance_satoshis", balance_satoshis));
	//uv_result.push_back(uv_balance);
	
	return uv_result;
}


UniValue getaddressdeltas(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1 || !params[0].isObject())
        throw runtime_error(
            "getaddressdeltas\n"
            "\nReturns all changes for an address (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ]\n"
            "  \"start\" (number) The start block height\n"
            "  \"end\" (number) The end block height\n"
            "  \"chainInfo\" (boolean) Include chain info in results, only applies if start and end specified\n"
            "}\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"satoshis\"  (number) The difference of satoshis\n"
            "    \"txid\"  (string) The related txid\n"
            "    \"index\"  (number) The related input or output index\n"
            "    \"height\"  (number) The block height\n"
            "    \"address\"  (string) The base58check encoded address\n"
            "  }\n"
            "]\n"
            "\nExamples:\n"
            + HelpExampleCli("getaddressdeltas", "'{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}'")
            + HelpExampleRpc("getaddressdeltas", "{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}")
        );


    UniValue startValue = find_value(params[0].get_obj(), "start");
    UniValue endValue = find_value(params[0].get_obj(), "end");

    UniValue chainInfo = find_value(params[0].get_obj(), "chainInfo");
    bool includeChainInfo = false;
    if (chainInfo.isBool()) {
        includeChainInfo = chainInfo.get_bool();
    }

    int start = 0;
    int end = 0;

    if (startValue.isNum() && endValue.isNum()) {
        start = startValue.get_int();
        end = endValue.get_int();
        if (start <= 0 || end <= 0) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Start and end is expected to be greater than zero");
        }
        if (end < start) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "End value is expected to be greater than start");
        }
    }

    std::vector<std::pair<uint160, int> > addresses;

    if (!getAddressesFromParams(params, addresses)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid address");
    }

    std::vector<std::pair<CAddressIndexKey, CAmount> > addressIndex;

    for (std::vector<std::pair<uint160, int> >::iterator it = addresses.begin(); it != addresses.end(); it++) {
        if (start > 0 && end > 0) {
            if (!GetAddressIndex((*it).first, (*it).second, addressIndex, start, end)) {
                throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
            }
        } else {
            if (!GetAddressIndex((*it).first, (*it).second, addressIndex)) {
                throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
            }
        }
    }

    UniValue deltas(UniValue::VARR);

    for (std::vector<std::pair<CAddressIndexKey, CAmount> >::const_iterator it=addressIndex.begin(); it!=addressIndex.end(); it++) {
        std::string address;
        if (!getAddressFromIndex(it->first.type, it->first.hashBytes, address)) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unknown address type");
        }

        UniValue delta(UniValue::VOBJ);
        delta.push_back(Pair("satoshis", it->second));
        delta.push_back(Pair("txid", it->first.txhash.GetHex()));
        delta.push_back(Pair("index", (int)it->first.index));
        delta.push_back(Pair("blockindex", (int)it->first.txindex));
        delta.push_back(Pair("height", it->first.blockHeight));
        delta.push_back(Pair("address", address));
        deltas.push_back(delta);
    }

    UniValue result(UniValue::VOBJ);

    if (includeChainInfo && start > 0 && end > 0) {
        LOCK(cs_main);

        if (start > chainActive.Height() || end > chainActive.Height()) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Start or end is outside chain range");
        }

        CBlockIndex* startIndex = chainActive[start];
        CBlockIndex* endIndex = chainActive[end];

        UniValue startInfo(UniValue::VOBJ);
        UniValue endInfo(UniValue::VOBJ);

        startInfo.push_back(Pair("hash", startIndex->GetBlockHash().GetHex()));
        startInfo.push_back(Pair("height", start));

        endInfo.push_back(Pair("hash", endIndex->GetBlockHash().GetHex()));
        endInfo.push_back(Pair("height", end));

        result.push_back(Pair("deltas", deltas));
        result.push_back(Pair("start", startInfo));
        result.push_back(Pair("end", endInfo));

        return result;
    } else {
        return deltas;
    }
}

UniValue getaddressbalance(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getaddressbalance\n"
            "\nReturns the balance for an address(es) (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ]\n"
            "}\n"
            "\nResult:\n"
            "{\n"
            "  \"balance\"  (string) The current balance in satoshis\n"
            "  \"received\"  (string) The total number of satoshis received (including change)\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("getaddressbalance", "'{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}'")
            + HelpExampleRpc("getaddressbalance", "{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}")
        );

    std::vector<std::pair<uint160, int> > addresses;

    if (!getAddressesFromParams(params, addresses)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid address");
    }

    std::vector<std::pair<CAddressIndexKey, CAmount> > addressIndex;

    for (std::vector<std::pair<uint160, int> >::iterator it = addresses.begin(); it != addresses.end(); it++) {
        if (!GetAddressIndex((*it).first, (*it).second, addressIndex)) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
        }
    }

    CAmount balance = 0;
    CAmount received = 0;

    for (std::vector<std::pair<CAddressIndexKey, CAmount> >::const_iterator it=addressIndex.begin(); it!=addressIndex.end(); it++) {
        if (it->second > 0) {
            received += it->second;
        }
        balance += it->second;
    }

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("balance", balance));
    result.push_back(Pair("received", received));

    return result;

}

UniValue safecoin_snapshot(int top);

UniValue getsnapshot(const UniValue& params, bool fHelp)
{
    UniValue result(UniValue::VOBJ); int64_t total; int32_t top = 0;

    if (params.size() > 0 && !params[0].isNull()) {
        top = atoi(params[0].get_str().c_str());
    if (top <= 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, top must be a positive integer");
    }

    if ( fHelp || params.size() > 1)
    {
        throw runtime_error(
                            "getsnapshot\n"
			    "\nReturns a snapshot of (address,amount) pairs at current height (requires addressindex to be enabled).\n"
			    "\nArguments:\n"
			    "  \"top\" (number, optional) Only return this many addresses, i.e. top N richlist\n"
			    "\nResult:\n"
			    "{\n"
			    "   \"addresses\": [\n"
			    "    {\n"
			    "      \"addr\": \"RMEBhzvATA8mrfVK82E5TgPzzjtaggRGN3\",\n"
			    "      \"amount\": \"100.0\"\n"
			    "    },\n"
			    "    {\n"
			    "      \"addr\": \"RqEBhzvATAJmrfVL82E57gPzzjtaggR777\",\n"
			    "      \"amount\": \"23.45\"\n"
			    "    }\n"
			    "  ],\n"
			    "  \"total\": 123.45           (numeric) Total amount in snapshot\n"
			    "  \"average\": 61.7,          (numeric) Average amount in each address \n"
			    "  \"utxos\": 14,              (number) Total number of UTXOs in snapshot\n"
			    "  \"total_addresses\": 2,     (number) Total number of addresses in snapshot,\n"
			    "  \"start_height\": 91,       (number) Block height snapshot began\n"
			    "  \"ending_height\": 91       (number) Block height snapsho finished,\n"
			    "  \"start_time\": 1531982752, (number) Unix epoch time snapshot started\n"
			    "  \"end_time\": 1531982752    (number) Unix epoch time snapshot finished\n"
			    "}\n"
			    "\nExamples:\n"
			    + HelpExampleCli("getsnapshot","")
			    + HelpExampleRpc("getsnapshot", "1000")
                            );
    }
    result = safecoin_snapshot(top);
    if ( result.size() > 0 ) {
        result.push_back(Pair("end_time", (int) time(NULL)));
    } else {
	result.push_back(Pair("error", "no addressindex"));
    }
    return(result);
}

UniValue getaddresstxids(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getaddresstxids\n"
            "\nReturns the txids for an address(es) (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ]\n"
            "  \"start\" (number) The start block height\n"
            "  \"end\" (number) The end block height\n"
            "}\n"
            "\nResult:\n"
            "[\n"
            "  \"transactionid\"  (string) The transaction id\n"
            "  ,...\n"
            "]\n"
            "\nExamples:\n"
            + HelpExampleCli("getaddresstxids", "'{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}'")
            + HelpExampleRpc("getaddresstxids", "{\"addresses\": [\"RY5LccmGiX9bUHYGtSWQouNy1yFhc5rM87\"]}")
        );

    std::vector<std::pair<uint160, int> > addresses;

    if (!getAddressesFromParams(params, addresses)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid address");
    }

    int start = 0;
    int end = 0;
    if (params[0].isObject()) {
        UniValue startValue = find_value(params[0].get_obj(), "start");
        UniValue endValue = find_value(params[0].get_obj(), "end");
        if (startValue.isNum() && endValue.isNum()) {
            start = startValue.get_int();
            end = endValue.get_int();
        }
    }

    std::vector<std::pair<CAddressIndexKey, CAmount> > addressIndex;

    for (std::vector<std::pair<uint160, int> >::iterator it = addresses.begin(); it != addresses.end(); it++) {
        if (start > 0 && end > 0) {
            if (!GetAddressIndex((*it).first, (*it).second, addressIndex, start, end)) {
                throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
            }
        } else {
            if (!GetAddressIndex((*it).first, (*it).second, addressIndex)) {
                throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for address");
            }
        }
    }

    std::set<std::pair<int, std::string> > txids;
    UniValue result(UniValue::VARR);

    for (std::vector<std::pair<CAddressIndexKey, CAmount> >::const_iterator it=addressIndex.begin(); it!=addressIndex.end(); it++) {
        int height = it->first.blockHeight;
        std::string txid = it->first.txhash.GetHex();

        if (addresses.size() > 1) {
            txids.insert(std::make_pair(height, txid));
        } else {
            if (txids.insert(std::make_pair(height, txid)).second) {
                result.push_back(txid);
            }
        }
    }

    if (addresses.size() > 1) {
        for (std::set<std::pair<int, std::string> >::const_iterator it=txids.begin(); it!=txids.end(); it++) {
            result.push_back(it->second);
        }
    }

    return result;

}

UniValue listfromto(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2)
        throw runtime_error(
            "\nlistfromto \"src-address\" \"dst-address\" ( start-height )\n"
            "\nReturns payments txids from src-address to dst-address (requires addressindex to be enabled).\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"height\"         (numeric) Transaction block height\n"
            "    \"timestamp\"      (numeric) Transaction timestamp\n"
            "    \"txid\"           (string)  Transaction id\n"
            "    \"received_SAFE\"  (numeric) Amount received by dst-address in SAFE\n"
            "  }\n"
            "  , ...\n"
            "]\n"
            "\nExamples:\n"
            + HelpExampleCli("listfromto", "\"RsaEqW1sbbANmjbqD51zHfDJxGP1xMVoVG\" \"RjvitJRxtkeLYe9cvDdrWKrUCVSWk29Hmp\" 683820")
            + HelpExampleRpc("listfromto", "\"RsaEqW1sbbANmjbqD51zHfDJxGP1xMVoVG\", \"RjvitJRxtkeLYe9cvDdrWKrUCVSWk29Hmp\", 683820")
        );
    
    std::string str_src_address = params[0].get_str();
    std::string str_dst_address = params[1].get_str();
	
	uint32_t start_height = (params.size() == 3) ? params[2].get_int() : 1;
	uint32_t end_height = chainActive.LastTip()->GetHeight();
	
	LOCK(cs_main);
	
	CBitcoinAddress src_address(str_src_address);
	CBitcoinAddress dst_address(str_dst_address);
    uint160 src_hash_bytes, dst_hash_bytes;
    int src_type = 0, dst_type = 0;
    
	if (!src_address.GetIndexKey(src_hash_bytes, src_type))
    {
		throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid source address");
    }
    
    if (!dst_address.GetIndexKey(dst_hash_bytes, dst_type))
    {
		throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid destination address");
    }
    
    if (start_height > end_height)
    {
		throw JSONRPCError(RPC_INVALID_PARAMETER, "Start height above the current chain tip");
	}

 
	std::vector<std::pair<CAddressIndexKey, CAmount> > src_address_index;
	std::vector<std::pair<CAddressIndexKey, CAmount> > dst_address_index;

	if (!GetAddressIndex(src_hash_bytes, src_type, src_address_index, start_height, end_height))
	{
		throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for source address");
	}
	
	if (!GetAddressIndex(dst_hash_bytes, dst_type, dst_address_index, start_height, end_height))
	{
		throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "No information available for destination address");
	}
 
    std::set<std::pair<int, std::string> > src_txids, dst_txids, intersect_txids;
    
    for (std::vector<std::pair<CAddressIndexKey, CAmount> >::const_iterator it=src_address_index.begin(); it!=src_address_index.end(); it++)
	{
        int height = it->first.blockHeight;
        std::string txid = it->first.txhash.GetHex();
		src_txids.insert(std::make_pair(height, txid));
    }
    
    for (std::vector<std::pair<CAddressIndexKey, CAmount> >::const_iterator it=dst_address_index.begin(); it!=dst_address_index.end(); it++)
	{
        int height = it->first.blockHeight;
        std::string txid = it->first.txhash.GetHex();
		dst_txids.insert(std::make_pair(height, txid));
    }
    
    std::set_intersection(src_txids.begin(), src_txids.end(),
                          dst_txids.begin(), dst_txids.end(),
                          std::inserter(intersect_txids, intersect_txids.end()));

	UniValue result(UniValue::VARR);
	for (auto const &p: intersect_txids)
	{
	    uint32_t height = p.first;
	    std::string str_txid = p.second;
	    uint256 hash = ParseHashV(str_txid, "txid");

		CTransaction tx;
		uint256 hashBlock;
		int nBlockTime = 0;

		if (GetTransaction(hash, tx, hashBlock, true))
		{
			if (!tx.IsCoinBase()) // skip coinbase transactions
			{
							
				BlockMap::iterator mi = mapBlockIndex.find(hashBlock);
				if (mi != mapBlockIndex.end() && (*mi).second)
				{
					CBlockIndex* pindex = (*mi).second;
					nBlockTime = pindex->GetBlockTime();
				}

				// we want all inputs to be from src address
				bool flag_good_src = true;
				BOOST_FOREACH(const CTxIn& txin, tx.vin)
				{
					uint256 prevout_hash;
					CTransaction prevout_tx;
					CTxDestination prevout_address;
					
					if (GetTransaction(txin.prevout.hash, prevout_tx, prevout_hash, false))
					{
						if (ExtractDestination(prevout_tx.vout[txin.prevout.n].scriptPubKey, prevout_address))
						{
							flag_good_src = flag_good_src && (CBitcoinAddress(prevout_address) == src_address);
						}
						else flag_good_src = false;
					}
					else flag_good_src = false;
					
					// no need to wait end of the loop if we already know the result
					if (!flag_good_src) break;	
				
				}
				
				if (flag_good_src)
				{
					// we want at least one output to be dst address
					int good_vout_count = 0;
					CAmount received_satoshis = 0;
					BOOST_FOREACH(const CTxOut& txout, tx.vout)
					{
						CTxDestination out_address;
						if (ExtractDestination(txout.scriptPubKey, out_address))
						{
							if (CBitcoinAddress(out_address) == dst_address)
							{
								good_vout_count++;
								received_satoshis += txout.nValue; // only dst address received amount matters
							}
						}
					}
					
					if (good_vout_count > 0)
					{
						UniValue item(UniValue::VOBJ);
						item.push_back(Pair("height", (int64_t)height));
						item.push_back(Pair("timestamp", nBlockTime));
						item.push_back(Pair("txid", str_txid));
						item.push_back(Pair("received_SAFE", ValueFromAmount(received_satoshis)));
						result.push_back(item);
					}
				}
			}
		}
	}
	
    return result;
}

UniValue getspentinfo(const UniValue& params, bool fHelp)
{

    if (fHelp || params.size() != 1 || !params[0].isObject())
        throw runtime_error(
            "getspentinfo\n"
            "\nReturns the txid and index where an output is spent.\n"
            "\nArguments:\n"
            "{\n"
            "  \"txid\" (string) The hex string of the txid\n"
            "  \"index\" (number) The start block height\n"
            "}\n"
            "\nResult:\n"
            "{\n"
            "  \"txid\"  (string) The transaction id\n"
            "  \"index\"  (number) The spending input index\n"
            "  ,...\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("getspentinfo", "'{\"txid\": \"0437cd7f8525ceed2324359c2d0ba26006d92d856a9c20fa0241106ee5a597c9\", \"index\": 0}'")
            + HelpExampleRpc("getspentinfo", "{\"txid\": \"0437cd7f8525ceed2324359c2d0ba26006d92d856a9c20fa0241106ee5a597c9\", \"index\": 0}")
        );

    UniValue txidValue = find_value(params[0].get_obj(), "txid");
    UniValue indexValue = find_value(params[0].get_obj(), "index");

    if (!txidValue.isStr() || !indexValue.isNum()) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid txid or index");
    }

    uint256 txid = ParseHashV(txidValue, "txid");
    int outputIndex = indexValue.get_int();

    CSpentIndexKey key(txid, outputIndex);
    CSpentIndexValue value;

    if (!GetSpentIndex(key, value)) {
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unable to get spent info");
    }

    UniValue obj(UniValue::VOBJ);
    obj.push_back(Pair("txid", value.txid.GetHex()));
    obj.push_back(Pair("index", (int)value.inputIndex));
    obj.push_back(Pair("height", value.blockHeight));

    return obj;
}

UniValue txnotarizedconfirmed(const UniValue& params, bool fHelp)
{
    bool notarizedconfirmed; uint256 txid;

    if (fHelp || params.size() < 1 || params.size() > 1)
    {
        string msg = "txnotarizedconfirmed txid\n"
            "\nReturns true if transaction is notarized on chain that has dPoW or if confirmation number is greater than 60 on chain taht does not have dPoW.\n"           

            "\nArguments:\n"
            "1. txid      (string, required) Transaction id.\n"            

            "\nResult:\n"
            "{\n"
            "  true,  (bool) The value the check.\n"            
            "}\n"            
        ;
        throw runtime_error(msg);
    }
    txid = uint256S((char *)params[0].get_str().c_str());
    notarizedconfirmed=safecoin_txnotarizedconfirmed(txid);
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("result", notarizedconfirmed));    
    return result;
}

UniValue getnodeinfo(const UniValue& params, bool fHelp)
{
    uint256 notarized_hash,notarized_desttxid; int32_t prevMoMheight,notarized_height,longestchain,safenotarized_height,txid_height;
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getnodeinfo\n"
            "Returns an object containing various SafeNode info.\n"
            "\nResult:\n"
            "{\n"
            "  \"executable_version\": (string) executable version with last commit number\n"
            "  \"protocolversion\":    (numeric) the protocol version\n"
            "  \"SAFE_version\":       (string) Safecoin daemon version\n"
            "  \"parentkey\":          (numeric) notary SafeNode pubkey\n"
            "  \"safekey\":            (numeric) SafeNode pubkey\n"
            "  \"SAFE_address\":       (string) SAFE address derived from safekey as a pubkey\n"
			"  \"safeheight\":         (string) safeheight\n"
			"  \"is_valid\":           (bool) safenode params validity\n"
            "  \"errors\":             (array) all error messages\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("getnodeinfo", "")
            + HelpExampleRpc("getnodeinfo", "")
        );

    LOCK(cs_main);
	
	UniValue obj(UniValue::VOBJ);
    UniValue errors(UniValue::VARR);
	
	bool safenode_valid = true; // presumption of innocence 
	
	std::string parentkey = GetArg("-parentkey", "");
	std::string safekey = GetArg("-safekey", "");
	std::string safeheight = GetArg("-safeheight", "");
	
	uint32_t timestamp = 0;
	int32_t height = chainActive.LastTip()->GetHeight();
	CBlockIndex *pblockindex = chainActive[height];
    if ( pblockindex != 0 ) timestamp = pblockindex->GetBlockTime();
	
    // checking parentkey
    std::vector<std::string> notaries = vs_safecoin_notaries(height, timestamp);
    std::vector<std::string>::iterator it = std::find(notaries.begin(), notaries.end(), parentkey);
    if (it == notaries.end())
    {
		errors.push_back("Invalid parentkey, should be a valid notary pubkey !");
		safenode_valid = false;
	}
	
	// checking safekey
	std::string safe_address = str_safe_address(safekey);
    if (safe_address == "invalid")
    {
		errors.push_back("Invalid safekey, should be a valid safenode pubkey !");
		safenode_valid = false;
	}
	
	// checking safeheight
	int32_t i_safeheight = 0; // presumption of guilt
	try 
	{
		i_safeheight = std::stoi(safeheight);
		if (i_safeheight < 750000) throw i_safeheight;
		if (i_safeheight > height) throw i_safeheight;
	}
	catch (...)
	{
		errors.push_back("Invalid safeheight, should be a number >= 750000 and <= current height !");
		safenode_valid = false;
	}
	
	
	
	
    obj.push_back(Pair("executable_version", FormatFullVersion()));
    obj.push_back(Pair("protocolversion", PROTOCOL_VERSION));
    obj.push_back(Pair("SAFE_version", SAFECOIN_VERSION));
    obj.push_back(Pair("parentkey", parentkey));
    obj.push_back(Pair("safekey", safekey));
    obj.push_back(Pair("SAFE_address", safe_address));
    obj.push_back(Pair("safeheight", safeheight));
    obj.push_back(Pair("is_valid", safenode_valid));
   
    UniValue gri_params;
	gri_params.clear();
	UniValue uv_registration_info = getregistrationinfo(gri_params, false);
	UniValue uv_last_reg_height = find_value(uv_registration_info, "last_reg_height");
	UniValue uv_valid_thru_height = find_value(uv_registration_info, "valid_thru_height");
	UniValue uv_reg_errors = find_value(uv_registration_info, "errors");
	
	if (uv_last_reg_height.isNull())
	{
		for (int i=0; i< uv_reg_errors.size(); i++)	errors.push_back(uv_reg_errors[i]);
	}
	else
	{
		obj.push_back(Pair("last_reg_height", uv_last_reg_height));
		obj.push_back(Pair("valid_thru_height", uv_valid_thru_height));
	}
    
    extern bool fAddressIndex;
    
    if (fAddressIndex)
    {
		UniValue params;
		params.clear();
		UniValue uv_collateral_info = getcollateralinfo(params, false);
		UniValue uv_collateral = find_value(uv_collateral_info, "collateral");
		UniValue uv_balance = find_value(uv_collateral_info, "current_balance");
		UniValue uv_tier = find_value(uv_collateral_info, "tier");
		obj.push_back(Pair("balance", uv_balance));
		obj.push_back(Pair("collateral", uv_collateral));
		obj.push_back(Pair("tier", uv_tier));
	}
    
    obj.push_back(Pair("errors", errors));
    
    return obj;
}

UniValue getcollateralinfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "getcollateralinfo {safekey}\n"
            "Returns an object containing info about SafeNode collateral.\n"
            "\nResult:\n"
            "{\n"
            "  \"collateral\":       (float) mature collateral value\n"
            "  \"balance\":          (float) current balance / immature collateral value\n"
            "  \"tier\":             (numeric) Tier eligibility according to collateral\n"
            "  \"errors\":           (array) all error messages\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("getcollateralinfo", "03caeaa88e6ab615ed85135fc5e6ef4a1ccb8cc1142389bbb64607fb47aeb492f0")
            + HelpExampleRpc("getcollateralinfo", "03caeaa88e6ab615ed85135fc5e6ef4a1ccb8cc1142389bbb64607fb47aeb492f0")
        );

    LOCK(cs_main);
	
	UniValue obj(UniValue::VOBJ);
    UniValue errors(UniValue::VARR);

	int32_t height = chainActive.LastTip()->GetHeight(); 

	std::string safe_key, safe_address;
	bool is_valid = true; 
	
	// Check if address index is enabled
	extern bool fAddressIndex;
	bool address_index_enabled = fAddressIndex;
	
	if (!address_index_enabled)
	{
		errors.push_back("Address index disabled !");
		is_valid = false;
	} 
	
	if (is_valid)
	{
		if (params.size() == 0)
		{
			// use safekey from conf
			safe_key = GetArg("-safekey", "");
		}
		else
		{
			// use safekey from param
			safe_key = params[0].get_str();
		}
		
		if (safe_key.length() != 66)
		{
			errors.push_back("Invalid safekey !");
			is_valid = false;
		} 
	}
	
	if (is_valid)
	{
		safe_address = str_safe_address(safe_key);
		if (safe_address == "invalid")
		{
			errors.push_back("Invalid safekey !");
			is_valid = false;
		}
	}
	
	int64_t balance_satoshis = 0;
	int64_t collateral_satoshis = 0;
	
	if (is_valid)
	{
		uint32_t minconf = COLLATERAL_MATURITY; 
		int type = 0;
		
		CBitcoinAddress address(safe_address);

		uint160 hashBytes;
		std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue> > unspentOutputs;
		if (address.GetIndexKey(hashBytes, type))
		{
			if (GetAddressUnspent(hashBytes, type, unspentOutputs))
			{
				std::sort(unspentOutputs.begin(), unspentOutputs.end(), heightSort);
				for (std::vector<std::pair<CAddressUnspentKey, CAddressUnspentValue>>::const_iterator it = unspentOutputs.begin(); it != unspentOutputs.end(); it++)
				{
					std::string tmp_address;
					if (getAddressFromIndex(it->first.type, it->first.hashBytes, tmp_address))
					{
						uint32_t confirmations = height - it->second.blockHeight;
						balance_satoshis += it->second.satoshis;
						if (confirmations > minconf) collateral_satoshis += it->second.satoshis;
					}
					else
					{
						errors.push_back("Unknown address type !");
						is_valid = false;
					} 
				}
			}
			else
			{
				errors.push_back("No information for address !");
				is_valid = false;
			} 
		}
		else
		{
			errors.push_back("Invalid address !");
			is_valid = false;
		}		
	}
		
	if (is_valid)
	{
		obj.push_back(Pair("safekey", safe_key));
		obj.push_back(Pair("SAFE_address", safe_address));
		obj.push_back(Pair("height", height));
		obj.push_back(Pair("current_balance", ValueFromAmount(balance_satoshis)));
		obj.push_back(Pair("collateral", ValueFromAmount(collateral_satoshis)));
		int tier = 0;
		if (collateral_satoshis >= (int64_t)(COLLATERAL_MIN_TIER_1 * 1e8)) tier = 1;
		if (collateral_satoshis >= (int64_t)(COLLATERAL_MIN_TIER_2 * 1e8)) tier = 2;
		if (collateral_satoshis >= (int64_t)(COLLATERAL_MIN_TIER_3 * 1e8)) tier = 3;
		obj.push_back(Pair("tier", tier));
		if (!tier)
		{
			errors.push_back("Insufficient collateral !");
		}
	}
		
	obj.push_back(Pair("errors", errors));
	
    return obj;
}

UniValue getregistrationinfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "getregistrationinfo {safekey}\n"
            "Returns an object containing info about SafeNode registration.\n"
            "\nExamples:\n"
            + HelpExampleCli("getregistrationinfo", "03caeaa88e6ab615ed85135fc5e6ef4a1ccb8cc1142389bbb64607fb47aeb492f0")
            + HelpExampleRpc("getregistrationinfo", "03caeaa88e6ab615ed85135fc5e6ef4a1ccb8cc1142389bbb64607fb47aeb492f0")
        );

    LOCK(cs_main);
	
	UniValue obj(UniValue::VOBJ);
    UniValue errors(UniValue::VARR);

	int32_t current_height = chainActive.LastTip()->GetHeight(); 

	std::string safe_key, safe_address;
	bool is_valid = true; 
	
	if (params.size() == 0)
	{
		// use safekey from conf
		safe_key = GetArg("-safekey", "");
	}
	else
	{
		// use safekey from param
		safe_key = params[0].get_str();
	}
	
	if (safe_key.length() != 66)
	{
		errors.push_back("Invalid safekey !");
		is_valid = false;
	} 
	
	if (is_valid)
	{
		safe_address = str_safe_address(safe_key);
		if (safe_address == "invalid")
		{
			errors.push_back("Invalid safekey !");
			is_valid = false;
		}
	}
	
	int32_t last_reg_height = 0;
	int32_t last_reg_duration = 0;
	
	if (is_valid)
	{
        obj.push_back(Pair("safekey", safe_key));
		obj.push_back(Pair("SAFE_address", safe_address));
        
		struct safecoin_kv *s;
		extern struct safecoin_kv *SAFECOIN_KV;
		extern pthread_mutex_t SAFECOIN_KV_mutex;
		
		pthread_mutex_lock(&SAFECOIN_KV_mutex);
		
		for(s = SAFECOIN_KV; s != NULL; s = (safecoin_kv*)s->hh.next)
		{
			uint8_t *value_ptr = s->value;
			uint16_t value_size = s->valuesize;
			
			// skip checking against records with invalid safeid size
			if (value_size == 66)
			{
				std::string str_saved_safeid = std::string((char*)value_ptr, (int)value_size);
				if (s->height > last_reg_height && str_saved_safeid == safe_key)
				{
					last_reg_height = s->height;
					last_reg_duration = ((s->flags >> 2) + 1) * SAFECOIN_KVDURATION;
				}
			} 
		}
		
		pthread_mutex_unlock(&SAFECOIN_KV_mutex);

		if (last_reg_height > 0)
		{
			obj.push_back(Pair("current_height", current_height));
			obj.push_back(Pair("last_reg_height", last_reg_height));
			obj.push_back(Pair("valid_thru_height", last_reg_height + last_reg_duration));
		}
		else
		{
			errors.push_back("No registration found !");
			is_valid = false;
		}
		
	}
			
	obj.push_back(Pair("errors", errors));
	
    return obj;
}

UniValue getactivenodes(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getactivenodes\n"
            "Returns an object containing list of all active SafeNodes.\n"
            "\nExamples:\n"
            + HelpExampleCli("getactivenodes", "")
            + HelpExampleRpc("getactivenodes", "")
        );

    LOCK(cs_main);
	
	UniValue obj(UniValue::VOBJ);
    UniValue uv_safenodes(UniValue::VARR);

	int32_t current_height = chainActive.LastTip()->GetHeight(); 

	std::vector<std::string> vs_safekeys;
	std::vector<std::string>::iterator it;
	
	struct safecoin_kv *s;
	extern struct safecoin_kv *SAFECOIN_KV;
	extern pthread_mutex_t SAFECOIN_KV_mutex;
	
	pthread_mutex_lock(&SAFECOIN_KV_mutex);
	
	for(s = SAFECOIN_KV; s != NULL; s = (safecoin_kv*)s->hh.next)
	{
		uint8_t *value_ptr = s->value;
		uint16_t value_size = s->valuesize;
		
		// skip checking against records with invalid safeid size
		if (value_size == 66)
		{
			std::string str_saved_safeid = std::string((char*)value_ptr, 66);
			it = std::find(vs_safekeys.begin(), vs_safekeys.end(), str_saved_safeid);
			if (it == vs_safekeys.end())
			{
				vs_safekeys.push_back(str_saved_safeid);
			}
		} 
	}
	
	pthread_mutex_unlock(&SAFECOIN_KV_mutex);
	
	int node_count = 0;
	int tier_0_count = 0;
	int tier_1_count = 0;
	int tier_2_count = 0;
	int tier_3_count = 0;
	extern bool fAddressIndex;
	
	for (int i = 0; i < vs_safekeys.size(); i++)
	{
		UniValue uv_one_node(UniValue::VOBJ), params(UniValue::VARR);
		params.push_back(vs_safekeys.at(i));
		UniValue uv_registration_info = getregistrationinfo(params, false);
		UniValue uv_safe_address = find_value(uv_registration_info, "SAFE_address");
		UniValue uv_last_reg_height = find_value(uv_registration_info, "last_reg_height");
		UniValue uv_valid_thru_height = find_value(uv_registration_info, "valid_thru_height");
		
		if (!uv_last_reg_height.isNull() && uv_valid_thru_height.get_int() >= current_height)
		{
			uv_one_node.push_back(Pair("safekey", vs_safekeys.at(i)));
			uv_one_node.push_back(Pair("SAFE_address", uv_safe_address));
			node_count++;
			if (fAddressIndex)
			{
				UniValue uv_collateral_info = getcollateralinfo(params, false);
				UniValue uv_collateral = find_value(uv_collateral_info, "collateral");
				UniValue uv_balance = find_value(uv_collateral_info, "current_balance");
				UniValue uv_tier = find_value(uv_collateral_info, "tier");
				uv_one_node.push_back(Pair("balance", uv_balance));
				uv_one_node.push_back(Pair("collateral", uv_collateral));
				uv_one_node.push_back(Pair("tier", uv_tier));
				if (uv_tier.get_int() == 0) tier_0_count++;
				if (uv_tier.get_int() == 1) tier_1_count++;
				if (uv_tier.get_int() == 2) tier_2_count++;
				if (uv_tier.get_int() == 3) tier_3_count++;
			}	
			uv_safenodes.push_back(uv_one_node);
		}
	}
	
	obj.push_back(Pair("SafeNodes", uv_safenodes));
	obj.push_back(Pair("node_count", node_count));
	obj.push_back(Pair("tier_0_count", tier_0_count));
	obj.push_back(Pair("tier_1_count", tier_1_count));
	obj.push_back(Pair("tier_2_count", tier_2_count));
	obj.push_back(Pair("tier_3_count", tier_3_count));
	
    return obj;
}

static const CRPCCommand commands[] =
{ //  category              name                      actor (function)         okSafeMode
  //  --------------------- ------------------------  -----------------------  ----------
    { "control",            "getinfo",                &getinfo,                true  }, /* uses wallet if enabled */
    { "control",            "getnodeinfo",            &getnodeinfo,            true  }, 
    { "control",            "getcollateralinfo",      &getcollateralinfo,      true  },
    { "control",            "getregistrationinfo",    &getregistrationinfo,    true  }, 
    { "control",            "getactivenodes",         &getactivenodes,         true  }, 
    { "util",               "validateaddress",        &validateaddress,        true  }, /* uses wallet if enabled */
    { "util",               "z_validateaddress",      &z_validateaddress,      true  }, /* uses wallet if enabled */
    { "util",               "createmultisig",         &createmultisig,         true  },
    { "util",               "verifymessage",          &verifymessage,          true  },

    /* Not shown in help */
    { "hidden",             "setmocktime",            &setmocktime,            true  },
};

void RegisterMiscRPCCommands(CRPCTable &tableRPC)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        tableRPC.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
