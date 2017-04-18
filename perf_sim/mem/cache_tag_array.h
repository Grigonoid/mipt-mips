/**
 * cache_tag_array.h
 * Header for the cache tag array model.
 * MIPT-MIPS Assignment 5.
 * Ladin Oleg.
 */

/* Protection from multi-including. */
#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

/* C++ libraries. */
#include <list>
#include <vector>

/* Simulator modules. */
#include <common/types.h>
#include <common/log.h>

/* The set (line) of the cache. */
struct CacheSet
{
    uint64 line = 0; // data
    bool is_valid = false; // validaty
};

/* Replacement algorithm modules (LRU). */
struct LRUInfo
{
    /*
     * "lru" contains sequences of using for each set. Last number in set list
     * is the last using.
     */
    std::vector< std::list< unsigned int> > lru = {};

    LRUInfo( unsigned int ways, unsigned int sets)
    {
        std::list< unsigned int> l;
        for ( unsigned int i = 0; i < ways; ++i)
        {
            l.push_back( i);
        }
        lru = std::vector< std::list< unsigned int> >( sets, l);
    }
    /*
     * On hit - mark (push back) way that contains the set.
     * It's an inversed search because of progs usually have time locality.
     */
    void update( int set, unsigned int way)
    {
	// Use reverse iterator for simulation speed
        auto& list = lru[ set];
        for ( auto it = list.rbegin(); it != list.rend(); ++it)
        {
            if ( *it == way)
            {
                list.splice( list.end(), list, std::next( it).base());
                return;
            }
        }
    }
    /* Get number of the Least Resently Used way and push back it.*/
    int update( int set)
    {
        auto& list = lru[ set];
        list.splice( list.end(), list, list.begin());
        return list.back();
    }
};

class CacheTagArray : protected Log
{
    private:
        const unsigned int size_in_bytes;
        const unsigned int ways;
        const unsigned short block_size_in_bytes;
        const unsigned short addr_size_in_bits;
        CacheSet** set = nullptr; // array of tags
        LRUInfo* lru = nullptr; // LRU algorithm module

        /* Checks if it possiable to create cache. */
        void checkArgs( unsigned int size_in_bytes,
                        unsigned int ways,
                        unsigned short block_size_in_bytes,
                        unsigned short addr_size_in_bits);
        unsigned int getSetNum( uint64 addr);
        uint64 getTagNum( uint64 addr);
        CacheTagArray& operator=( const CacheTagArray&) = delete;
        CacheTagArray( const CacheTagArray&) = delete;

    public:
        CacheTagArray( unsigned int size_in_bytes,
                       unsigned int ways,
                       unsigned short block_size_in_bytes = 4,
                       unsigned short addr_size_in_bits = 32);
        ~CacheTagArray();
        bool read( uint64 addr); // find in the cache
        void write( uint64 addr); // add to the cache
};

#endif // #ifndef CACHE_TAG_ARRAY_H
