// Copyright (c) 2018 Michael Toutonghi
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_NONCE_H
#define BITCOIN_PRIMITIVES_NONCE_H

#include "serialize.h"
#include "uint256.h"
#include "arith_uint256.h"


/** For POS blocks, the nNonce of a block header holds the entropy source for the POS contest
 * in the latest VerusHash protocol
 * */
class CPOSNonce : public uint256
{
public:
    static bool NewPOSActive(int32_t height);
    static bool NewNonceActive(int32_t height);

    static arith_uint256 entropyMask;
    static arith_uint256 posDiffMask;

    CPOSNonce() : uint256() { }
    CPOSNonce(const base_blob<256> &b) : uint256(b) { }
    CPOSNonce(const std::vector<unsigned char> &vch) : uint256(vch) { }

    int32_t GetPOSTarget() const
    {
        uint32_t nBits = 0;

        for (const unsigned char *p = begin() + 3; p >= begin(); p--)
        {
            nBits <<= 8;
            nBits += *p;
        }
        return nBits;
    }


    void SetPOSEntropy(const uint256 &pastHash, uint256 txid, int32_t voutNum);
    bool CheckPOSEntropy(const uint256 &pastHash, uint256 txid, int32_t voutNum);
};

#endif // BITCOIN_PRIMITIVES_NONCE_H
