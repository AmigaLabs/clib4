/* $Id: resolv_dns_cache.c,v 1.0 2026-03-05 10:00:00 clib4devs Exp $
 *
 * Simple DNS response cache for clib4.
 * Caches resolved addresses with TTL from DNS responses.
 */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#include "lookup.h"

/* Simple djb2 hash */
static uint32_t dns_hash(const char *name, int family) {
    uint32_t h = 5381;
    while (*name)
        h = h * 33 + (unsigned char)*name++;
    h = h * 33 + (unsigned)family;
    return h;
}

static uint32_t now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint32_t)ts.tv_sec;
}

int
__dns_cache_lookup(struct dns_cache *cache, const char *name, int family,
                   struct address *buf, char *canon) {
    if (!cache || !name)
        return 0;

    uint32_t h = dns_hash(name, family) % DNS_CACHE_SIZE;
    uint32_t ts = now_seconds();

    /* Linear probe from the hash slot */
    for (int i = 0; i < 4; i++) {
        uint32_t idx = (h + i) % DNS_CACHE_SIZE;
        struct dns_cache_entry *e = &cache->entries[idx];

        if (!e->in_use)
            continue;

        /* Check expiry */
        if (ts - e->timestamp > e->min_ttl) {
            e->in_use = 0;
            continue;
        }

        if (e->family == family && strcmp(e->name, name) == 0) {
            int cnt = e->naddrs;
            memcpy(buf, e->addrs, cnt * sizeof(struct address));
            if (canon && e->canon[0])
                strcpy(canon, e->canon);
            return cnt;
        }
    }

    return 0;
}

void
__dns_cache_store(struct dns_cache *cache, const char *name, int family,
                  const struct address *addrs, int naddrs, const char *canon,
                  uint32_t ttl) {
    if (!cache || !name || naddrs <= 0)
        return;

    /* Enforce minimum TTL of 10s and maximum of 3600s */
    if (ttl < 10) ttl = 10;
    if (ttl > 3600) ttl = 3600;

    int cnt = naddrs > DNS_CACHE_MAX_ADDRS ? DNS_CACHE_MAX_ADDRS : naddrs;
    uint32_t h = dns_hash(name, family) % DNS_CACHE_SIZE;
    uint32_t ts = now_seconds();

    /* Find a slot: matching entry, expired entry, or empty entry */
    uint32_t best = h;
    uint32_t oldest_ts = UINT32_MAX;

    for (int i = 0; i < 4; i++) {
        uint32_t idx = (h + i) % DNS_CACHE_SIZE;
        struct dns_cache_entry *e = &cache->entries[idx];

        if (!e->in_use) {
            best = idx;
            break;
        }

        /* Existing entry for the same name+family: overwrite */
        if (e->family == family && strcmp(e->name, name) == 0) {
            best = idx;
            break;
        }

        /* Expired entry: reuse */
        if (ts - e->timestamp > e->min_ttl) {
            best = idx;
            break;
        }

        /* Track oldest entry for eviction */
        if (e->timestamp < oldest_ts) {
            oldest_ts = e->timestamp;
            best = idx;
        }
    }

    struct dns_cache_entry *e = &cache->entries[best];
    size_t namelen = strnlen(name, 255);
    memcpy(e->name, name, namelen);
    e->name[namelen] = 0;
    e->family = family;
    e->naddrs = cnt;
    memcpy(e->addrs, addrs, cnt * sizeof(struct address));
    if (canon && canon[0]) {
        size_t clen = strnlen(canon, 255);
        memcpy(e->canon, canon, clen);
        e->canon[clen] = 0;
    } else {
        e->canon[0] = 0;
    }
    e->min_ttl = ttl;
    e->timestamp = ts;
    e->in_use = 1;
}
