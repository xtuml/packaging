// umlrthashmap.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrthashmap.hh"
#include "umlrtguard.hh"
#include "basefatal.hh"
#include "basedebugtype.hh"
#include "basedebug.hh"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

UMLRTHashMap::~UMLRTHashMap ( )
{
    mutex.take();

    for (int i = 0; i < mapSize; ++i)
    {
        if (map[i].object != NULL)
        {
            free( (void *)map[i].object );
        }
    }
    if (mapSize)
    {
        free( map );
    }
}

/*static*/ int UMLRTHashMap::compareString ( const void * k1, const void * k2 )
{
    if ((k1 == NULL) && (k2 != NULL))
    {
        return -1;
    }
    else if ((k1 != NULL) && (k2 == NULL))
    {
        return 1;
    }
    else if ((k1 == NULL) && (k2 == NULL))
    {
        return 0;
    }
    return strcmp((const char *)k1, (const char *)k2);
}

/*static*/ int UMLRTHashMap::compareValue ( const void * k1, const void * k2 )
{
    return (char *)k1 - (char *)k2;
}

void UMLRTHashMap::debugPrintfKey ( const void * key ) const
{
    char ptr[80];
    snprintf(ptr, sizeof(ptr), "%p", key);
    BDEBUG(BD_HASHMAP, "key(%s)", compare == UMLRTHashMap::compareString ? key ? (char*)key : "-null-" : ptr);
}

void UMLRTHashMap::debugOutput ( const char * where, const void * key, const char * intname, int intvalue, bool test, const char * truemsg, const char * falsemsg ) const
{
    if (base::debugTypeEnabled(BD_HASHMAP))
    {
        BDEBUG(BD_HASHMAP, "%s map %s sz %d %s(%d) ", where, name, mapSize, intname, intvalue);
        debugPrintfKey(key);
        BDEBUG(BD_HASHMAP, " %s\n", test ? truemsg : falsemsg);

        for (int i = 0; i < mapSize; ++i)
        {
            BDEBUG(BD_HASHMAP, "      [%d] ", i );
            debugPrintfKey( map[i].key );
            char objptr[80];
            snprintf(objptr, sizeof(objptr), "%p", map[i].object);
            BDEBUG(BD_HASHMAP, " %s\n", objectIsString ? (char*)map[i].object : objptr);
        }
    }
}

UMLRTHashMap::MapEntry * UMLRTHashMap::getEntry ( const void * key ) const
{
    // Assumes mutex is taken.
    MapEntry * entry = NULL;

    int location = locate(key);

    if ((location >= 0) && (location < mapSize))
    {
        if (compare(map[location].key, key) == 0)
        {
            entry = &map[location];
        }
    }
    debugOutput("UMLRTHashMap::getEntry", key, "location", location, entry == NULL, "NOT found", "found");

    return entry;
}

void * UMLRTHashMap::getFirstObject ( ) const
{
    // Return object associated with key. Return NULL if entry not found.
    UMLRTGuard g(mutex);

    void * object = NULL;
    if (mapSize > 0)
    {
        object = map[0].object;
    }
    return object;
}

void * UMLRTHashMap::getObject ( const void * key ) const
{
    // Return object associated with key. Return NULL if entry not found.
    UMLRTGuard g(mutex);

    void * object = NULL;
    MapEntry * entry = getEntry(key);

    if (entry != NULL)
    {
        object = entry->object;
    }
    return object;
}

const void * UMLRTHashMap::getKey ( int location ) const
{
    // Return key associated with entry at location.
    UMLRTGuard g(mutex);

    const void * key = NULL;

    if ((location >= 0) && (location < mapSize))
    {
        key = map[location].key;
    }
    return key;
}

void * UMLRTHashMap::getObject ( int location ) const
{
    // Called from iterator. Assumes lock is already obtained
    void * object = NULL;

    if ((location >= 0) && (location < mapSize))
    {
        object = map[location].object;
    }
    return object;
}

void UMLRTHashMap::insert ( const void * key, void * object )
{
    UMLRTGuard g(mutex);

    MapEntry * entry = getEntry(key);
    if (entry)
    {
        if (base::debugTypeEnabled(BD_HASHMAP))
        {
            char objwas[80];
            char objis[80];
            snprintf(objwas, sizeof(objwas), "%p", entry->object);
            snprintf(objis, sizeof(objis), "%p", object);
            BDEBUG(BD_HASHMAP, "UMLRTHashMap::insert existed - object was (%s) now (%s)\n",
                    objectIsString ? entry->object : objwas,
                    objectIsString ? object : objis);

            debugOutput("UMLRTHashMap::insert existed", key, "", 0, false, "", "");
        }
        entry->object = object;
    }
    else
    {
        // Entry not find. Insert it.
        int location = locate(key);

        if (mapSize == 0)
        {
            map = (MapEntry *)malloc(sizeof(MapEntry));
        }
        else
        {
            map = (MapEntry *)realloc(map, (mapSize + 1) * sizeof(MapEntry));
            memmove(&map[location + 1], &map[location], (mapSize - location) * sizeof(MapEntry));
        }
        ++mapSize;
        map[location].key = key;
        map[location].object = object;
        debugOutput("UMLRTHashMap::insert new", key, "location", location, false, "", "");
    }
}

int UMLRTHashMap::locate ( const void * key ) const
{
    // Assumes mutex is taken.
    // Returns either location of entry holding 'key' or the location where 'key' belongs (first entry where entry->key > input key).
    int min, max, mid;
    int location = -1;

    min = 0;
    max = mapSize - 1;

    while (location == -1)
    {
        if (min > max)
        {
            // Zeroed in on location. It's 'min' or the one after.
            location = min;
            if (min < (mapSize-1))
            {
                if (compare(map[min].key, key) < 0)
                {
                    // 'min' was one before location we want.
                    location = min + 1;
                }
            }
        }
        else
        {
            mid = (min + max) / 2;
            if (compare(map[mid].key, key) == 0)
            {
                // located it
                location = mid;
            }
            else if (compare(map[mid].key, key) < 0)
            {
                // location must be after 'mid'.
                if ((min = (mid + 1)) >= mapSize)
                {
                    // location falls after last item
                    location = mapSize;
                }
            }
            else if ((max = (mid - 1)) < 0)
            {
                // location falls before first item
                location = 0;
            }
        }
    }
    return location;
}

const void * UMLRTHashMap::remove ( const void * key )
{
    UMLRTGuard g(mutex);

    int location = locate(key);
    const void * keyRemoved = NULL;

    if (mapSize)
    {
        if (location < mapSize)
        {
            debugOutput("UMLRTHashMap::remove before", key, "location", location, compare(map[location].key, key) == 0, "FOUND", "not found" );
            if (compare(map[location].key, key) == 0)
            {
                keyRemoved = map[location].key;
                if (location != (mapSize-1))
                {
                    BDEBUG(BD_HASHMAP, "UMLRTHashMap::remove memmove %s location(%d) mapSize(%d)\n",name, location, mapSize);
                    memmove(&map[location], &map[location + 1], (mapSize - location - 1) * sizeof(MapEntry));
                }
                if ((--mapSize) == 0)
                {
                    free(map);
                    map = NULL;
                }
            }
            debugOutput("UMLRTHashMap::remove after", key, "location", location, false, "", "");
        }
    }
    return keyRemoved;
}

UMLRTHashMap::Iterator UMLRTHashMap::getIterator ( ) const
{
    if (!mapSize)
    {
        return Iterator(this, -1);
    }
    return Iterator(this, 0);
}

UMLRTHashMap::Iterator UMLRTHashMap::Iterator::end ( ) const
{
    return Iterator(map, -1);
}

UMLRTHashMap::Iterator UMLRTHashMap::Iterator::next ( ) const
{
    if (index == -1)
    {
        return end();
    }
    else if (map->isEmpty())
    {
        return end();
    }
    else if (index == (map->getSize() - 1))
    {
        return end();
    }
    return Iterator(map, index + 1);
}

const void * UMLRTHashMap::Iterator::getKey ( ) const
{
    const void * key = NULL;

    if (index != -1)
    {
        key = map->getKey(index);
    }
    return key;
}

void * UMLRTHashMap::Iterator::getObject ( ) const
{
    void * object = NULL;

    if (index != -1)
    {
        if (map->getSize() > 0)
        {
            object = map->getObject(index);
        }
    }
    return object;
}
