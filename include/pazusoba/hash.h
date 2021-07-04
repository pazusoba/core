/// String hash functions
/// From https://stackoverflow.com/a/7666577 &
/// http://www.cse.yorku.ca/~oz/hash.html

#ifndef _HASH_H_
#define _HASH_H_

namespace pazusoba::hash {
inline unsigned long djb2_hash(const unsigned char* str) {
    unsigned long hash = 5381;
    int c;

    while (*str) {
        c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

inline unsigned long sdbm_hash(const unsigned char* str) {
    unsigned long hash = 0;
    int c;

    while (*str) {
        c = *str++;
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}
}  // namespace pazusoba::hash

#endif