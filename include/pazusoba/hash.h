//
// hash.h
// String hash functions
// From https://stackoverflow.com/a/7666577 &
// http://www.cse.yorku.ca/~oz/hash.html
//
// Mofified by Yiheng Quan on 31/07/2021
//

#ifndef _HASH_H_
#define _HASH_H_

namespace pazusoba::hash {
inline size_t djb2_hash(const unsigned char* str) {
    size_t hash = 5381;
    int c;

    while (*str) {
        c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

inline size_t sdbm_hash(const unsigned char* str) {
    size_t hash = 0;
    int c;

    while (*str) {
        c = *str++;
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}
}  // namespace pazusoba::hash

#endif