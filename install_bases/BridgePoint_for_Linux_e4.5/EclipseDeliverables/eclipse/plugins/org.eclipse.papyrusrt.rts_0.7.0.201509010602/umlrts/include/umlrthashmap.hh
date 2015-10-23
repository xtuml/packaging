// umlrthashmap.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTHASHMAP_HH
#define UMLRTHASHMAP_HH

#include "umlrtmutex.hh"
#include <stdlib.h>

// Maps keys to entries. The map-name, 'keys' and 'objects' are stored as values.

// WARNING: The memory pointed to by map-name, 'key' and 'object' (if it is memory) is owned by the user and must persist.

// The #remove method returns the 'key' which can be deallocated by the user if need be.

// The #insert method overwrites the 'object' value if the 'key' was already in the map, so users must #remove
// any existing entry and deallocate the associated 'object' memory (if it is memory) to prevent memory leaks.

class UMLRTHashMap
{
public:
    typedef int ( * key_compare_t ) ( const void * k1, const void * k2 );

    // Two utility functions for creating maps with binary-value keys and string keys.
    static int compareValue ( const void * k1, const void * k2 );
    static int compareString ( const void * k1, const void * k2 );

    class Iterator {
    public:
        // All methods are O(c) 'constant time'.
        Iterator ( const UMLRTHashMap * map_, int index_ ) : map(map_), index(index_) {}
        Iterator end ( ) const;
        Iterator next ( ) const;
        const void * getKey ( ) const;
        void * getObject ( ) const;
        bool operator==(const UMLRTHashMap::Iterator &other) const { return (this->map == other.map) && (this->index == -1); }
        bool operator!=(const UMLRTHashMap::Iterator &other) const { return (this->map != other.map) || (this->index != -1); }
    private:
        const UMLRTHashMap * map;
        int index;
    };

    UMLRTHashMap ( const char * name_, key_compare_t compare_, bool objectIsString_ ) : name(name_), mapSize(0), map(NULL), compare(compare_), objectIsString(objectIsString_) {}
    ~UMLRTHashMap ( );

    bool isEmpty ( ) const { return mapSize == 0; }
    void insert ( const void * key, void * object ); // O(n) insert

    void * getFirstObject ( ) const;
    Iterator getIterator ( ) const; // Return an iterator to traverse map.
    // WARNING: traversing a map with an Iterator is not thread-safe.
    // If insertions and deletions are possible during traversal of a map with an iterator, the
    // user must call Lock() and Unlock() to prevent insertions/removals during traversal.

    void * getObject ( const void * key ) const; // O(log n) object fetch.
    int getSize ( ) const { return mapSize; }

    void Lock() const { mutex.take(); }
    void Unlock() const { mutex.give(); }

    const void * remove ( const void * key ); // O(n) remove

protected:
    const void * getKey ( int location ) const;
    void * getObject ( int location ) const;

private:
    UMLRTHashMap ( ) : mapSize(0), map(NULL), compare(NULL), objectIsString(false) {}

    struct MapEntry {
        const void * key; // If keys point to memory, the memory is owned by the app.
        void * object; // If object points to memory, the memory is owned by the app.
    };

    void removeEntry ( int location ); // O(n) remove.
    MapEntry * getEntry ( const void * key ) const;     // O(log n)
    int locate ( const void * key ) const; // O(log n) binary search. If key not found, returns index of where the key would be located.

    void debugOutput ( const char * where, const void * key, const char * intname, int intvalue, bool test, const char * truemsg, const char * falsemsg ) const;
    void debugPrintfKey ( const void * key ) const;

    const char * name;
    mutable UMLRTMutex mutex;
    int mapSize;
    MapEntry * map;
    key_compare_t compare;
    bool objectIsString;
};

#endif // UMLRTHASHMAP_HH
