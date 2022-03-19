//
// hash.h
// String hash functions
// From https://stackoverflow.com/a/7666577 &
// http://www.cse.yorku.ca/~oz/hash.html
//
// Mofified by Yiheng Quan on 31/07/2021
//

#pragma once
#ifndef _HASH_H_
#define _HASH_H_

namespace pazusoba {
namespace hash {
inline long long int djb2_hash(const unsigned char* str) {
    long long int hash = 5381;
    int c;

    while (*str) {
        c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

/// Use djb2_hash and add the initial location
inline long long int pazusoba_hash(const unsigned char* str, int loc) {
    long long int hash = djb2_hash(str);
    hash = ((hash << 5) + hash) + loc;
    return hash;
}

inline long long int djb2_hash_shift(long long int hash, int c) {
    return ((hash << 5) + hash) + c;
}

inline long long int sdbm_hash(const unsigned char* str) {
    long long int hash = 0;
    int c;

    while (*str) {
        c = *str++;
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

inline long long int sdbm_hash_shift(long long int hash, int c) {
    return c + (hash << 6) + (hash << 16) - hash;
}
}  // namespace hash
}  // namespace pazusoba

#endif