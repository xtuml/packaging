// umlrtobjectclass.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtobjectclass.hh"
#include "basedebugtype.hh"
#include "basedebug.hh"
#include <stdlib.h>

// Type descriptor used for encoding/decoding data of a given type.
// A number of UMLRTType_xxx descriptors are pre-defined by the library.

// Default initializer - zeros the data and returns a pointer to the byte after the initialized data.

uint8_t * globalDst = 0; // For debugging - initialized in umlrtsignalelement.cc
uint8_t * globalSrc = 0;

void * UMLRTObject_initialize( const UMLRTObject_class * desc, void * data )
{
    uint8_t * s = (uint8_t *)data;

    if (desc->super != NULL)
    {
        s = (uint8_t *)desc->super->initialize(desc->super, data); // Moves 's' past base type data.
    }
    if (desc->fields == NULL)
    {
        memset(s, 0, desc->size);
        s += desc->size;
    }
    else
    {
        for (int i = 0; i < desc->size; ++i)
        {
            const UMLRTObject_class * fldDesc = desc->fields[i].desc;
            for (int j = 0; j < desc->fields[i].arraySize; ++j)
            {
                s = (uint8_t *)fldDesc->initialize(fldDesc, s );
            }
        }
    }
    return (void *)s;
}

// Default copy - returns pointer to byte after destination location.
void * UMLRTObject_copy( const UMLRTObject_class * desc, const void * src, void * dst )
{
    if (desc->fields == NULL)
    {
        BDEBUG(BD_SERIALIZE, "copy: desc(%s) size(%d) gs %ld gd %ld\n", desc->name, desc->sizeDecoded, (uint8_t *)src - globalSrc, (uint8_t *)dst - globalDst);

        memcpy(dst, src, desc->sizeDecoded);
        dst = (void *)((uint8_t *)dst + desc->sizeDecoded);
    }
    else
    {
        for (int i = 0; i < desc->size; ++i)
        {
            const UMLRTObject_field * fld = &desc->fields[i];
            uint8_t * s = (uint8_t *)src + fld->offset;
            for (int j = 0; j < fld->arraySize; ++j)
            {
                BDEBUG(BD_SERIALIZE, "  copy: fld[%d]<%s> offset(%d) sizeDecoded(%d) (elem[%d] elemoff %ld gs %ld gd %ld)\n",
                        i, fld->name, fld->offset, fld->sizeDecoded, j, s - (uint8_t *)src, s - globalSrc, (uint8_t *)dst - globalDst);
                dst = fld->desc->copy(fld->desc, s, dst);
                s += fld->sizeDecoded;
            }
        }
    }
    return dst;
}

size_t UMLRTObject_getSize( const UMLRTObject_class * desc )
{
    int size = desc->size;

    if (desc->fields != NULL)
    {
        size = 0;
        for (int i = 0; i < desc->size; ++i)
        {
            size += desc->fields[i].desc->getSize(desc->fields[i].desc) * desc->fields[i].arraySize;
        }
    }
    return size;
}

const void * UMLRTObject_decode( const UMLRTObject_class * desc, const void * src, void * dst, int nest )
{
    if (desc->fields == NULL)
    {
        memcpy(dst, src, desc->size);
        src = (void *)((uint8_t *)src + desc->size);
    }
    else
    {
        for (int i = 0; i < desc->size; ++i)
        {
            const UMLRTObject_class * fldDesc = desc->fields[i].desc;
            uint8_t * d = (uint8_t *)dst + desc->fields[i].offset;
            for (int j = 0; j < desc->fields[i].arraySize; ++j)
            {
                src = fldDesc->decode(fldDesc, src, d, nest+1);
                d += desc->fields[i].sizeDecoded;
            }
        }
    }
    return src;
}

// Default encode is a copy (returns a pointer to the byte after the dst data).
void * UMLRTObject_encode(  const UMLRTObject_class * desc, const void * src, void * dst, int nest )
{
    if (desc->fields == NULL)
    {
        for (int n = 0; n < nest; ++n)
        {
            BDEBUG(BD_SERIALIZE, "    ");
        }
        BDEBUG(BD_SERIALIZE, "encode: desc(%s) size(%d) gs %ld gd %ld\n", desc->name, desc->size, (uint8_t *)src - globalSrc, (uint8_t *)dst - globalDst);
        memcpy(dst, src, desc->size);
        dst = (void *)((uint8_t *)dst + desc->size);
    }
    else
    {
        for (int n = 0; n < nest; ++n)
        {
            BDEBUG(BD_SERIALIZE, "    ");
        }
        BDEBUG(BD_SERIALIZE, "encode: desc(%s) nfields(%d)\n", desc->name, desc->size);
        for (int i = 0; i < desc->size; ++i)
        {
            const UMLRTObject_field * fld = &desc->fields[i];
            uint8_t * s = (uint8_t *)src + fld->offset;
            for (int j = 0; j < fld->arraySize; ++j)
            {
                BDEBUG(BD_SERIALIZE, "  ");
                for (int n = 0; n < nest; ++n)
                {
                    BDEBUG(BD_SERIALIZE, "    ");
                }
                BDEBUG(BD_SERIALIZE, "encode: fld[%d]<%s> offset(%d) sizeDecoded(%d) (elem[%d] elemoff %ld gs %ld gd %ld\n",
                        i, fld->name, fld->offset, fld->sizeDecoded, j, s - (uint8_t *)src, s - globalSrc, (uint8_t *)dst - globalDst);
                dst = fld->desc->encode(fld->desc, s, dst, nest+1);
                s += fld->sizeDecoded;
            }
        }
    }
    return dst;
}

// Default destroy does nothing (but returns a pointer to the byte after the data).
void * UMLRTObject_destroy(  const UMLRTObject_class * desc, void * data )
{
    return (void *)((uint8_t *)data + desc->getSize(desc));
}

// Assumes 'decoded' data (not 'encoded' data).
int UMLRTObject_fprintf( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;

    // Debug
    if (nest == 0)
    {
        globalSrc = (uint8_t *)data;
    }
    for (int ai = 0; ai < arraySize; ++ai)
    {
        uint8_t * ai_base = (uint8_t *)data + (desc->sizeDecoded * ai);
        if (desc->fields != NULL)
        {
            BDEBUG(BD_SERIALIZE, "\n");
        }
        for (int n = 0; n < nest; ++n)
        {
            BDEBUG(BD_SERIALIZE, "    ");
        }
        nchar += fprintf(ostream, "{%s", desc->name);
        if (arraySize > 1)
        {
            nchar += fprintf(ostream, "[%d]", ai);
        }
        nchar += fprintf(ostream, ":");
        BDEBUG(BD_SERIALIZE, "\n");
        if (desc->fields != NULL)
        {
            for (int fi = 0; fi < desc->size; ++fi)
            {
                const UMLRTObject_field * fld = &desc->fields[fi];
                uint8_t * fi_base = ai_base + fld->offset;
                //nchar += fprintf(ostream, "\n");
                for (int n = 0; n < nest; ++n)
                {
                    BDEBUG(BD_SERIALIZE, "    ");
                }
                nchar += fprintf(ostream, "{%s:", fld->name);
                BDEBUG(BD_SERIALIZE, "<desc(%s) arraySize %d elem %d field %d gs %ld>",
                        desc->name, arraySize, ai, fi, fi_base - globalSrc);
                for (int n = 0; n < nest; ++n)
                {
                    BDEBUG(BD_SERIALIZE, "    ");
                }
                nchar += fld->desc->fprintf(ostream, fld->desc, fi_base, nest+1, fld->arraySize);
                BDEBUG(BD_SERIALIZE, "  ");
                for (int n = 0; n < nest; ++n)
                {
                    BDEBUG(BD_SERIALIZE, "    ");
                }
                nchar += fprintf(ostream, "}");
                BDEBUG(BD_SERIALIZE, "\n");
            }
        }
        else if ((desc->fprintf != NULL) && (desc->fprintf != UMLRTObject_fprintf))
        {
            // Don't allow a recursive call with the same parameters.
            nchar += desc->fprintf(ostream, desc, ai_base, nest+1, arraySize);
        }
        else
        {
            nchar += fprintf(ostream, "(unable to print)"); // Either no fprintf defined or malformed descriptor.
            BDEBUG(BD_SERIALIZE, "\n");
        }
        for (int n = 0; n < nest; ++n)
        {
            BDEBUG(BD_SERIALIZE, "    ");
        }
        nchar += fprintf(ostream, "}");
        BDEBUG(BD_SERIALIZE, "\n");
    }
    return nchar;
}

static int UMLRTObject_fprintf_bool( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    bool b;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(b)), &b);
        nchar += fprintf(ostream, "{bool %s}", b ? "true" : "false");
    }
    return nchar;
}

static int UMLRTObject_fprintf_char( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    nchar += fprintf(ostream, "{char ");
    for (int i = 0; i < arraySize; ++i)
    {
        nchar += fprintf(ostream, "0x%02X ", *((char*)data + i));
    }
    nchar += fprintf(ostream, " '");
    for (int i = 0; i < arraySize; ++i)
    {
        char c = *((char*)data + i);
        nchar += fprintf(ostream, "%c", (c > 0x1F) && (c < 0x7F) ? c : '.');
    }
    nchar += fprintf(ostream, "'}");
    return nchar;
}

static int UMLRTObject_fprintf_double( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    double d;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(d)), &d);
        nchar += fprintf(ostream, "{double %f}", d);
    }
    return nchar;
}

static int UMLRTObject_fprintf_float( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    float f;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(f)), &f);
        nchar += fprintf(ostream, "{float %f}", f);
    }
    return nchar;
}

static int UMLRTObject_fprintf_int( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    int iv;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(iv)), &iv);
        nchar += fprintf(ostream, "{int %d}", iv);
    }
    return nchar;
}

static int UMLRTObject_fprintf_long( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    long l;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(l)), &l);
        nchar += fprintf(ostream, "{long %ld}", l);
    }
    return nchar;
}

static int UMLRTObject_fprintf_longdouble( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    long double ld;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(ld)), &ld);
        nchar += fprintf(ostream, "{longdouble %Lf}", ld);
    }
    return nchar;
}

static int UMLRTObject_fprintf_longlong( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    long long ll;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(ll)), &ll);
        nchar += fprintf(ostream, "{longlong %lld}", ll);
    }
    return nchar;
}

static int UMLRTObject_fprintf_ptr( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    void * p;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(p)), &p);
        // Pointer de-referencing tracked by Bug 468512.
        nchar += fprintf(ostream, "{ptr %p}", p);
    }
    return nchar;
}

static int UMLRTObject_fprintf_short( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    short sh;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(sh)), &sh);
        nchar += fprintf(ostream, "{short %d}", sh);
    }
    return nchar;
}

static int UMLRTObject_fprintf_uchar( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    unsigned char uc;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i), &uc);
        nchar += fprintf(ostream, "{uchar %u}", uc);
    }
    return nchar;
}

static int UMLRTObject_fprintf_uint( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    unsigned int ui;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(ui)), &ui);
        nchar += fprintf(ostream, "{uint %u}", ui);
    }
    return nchar;
}

static int UMLRTObject_fprintf_ulong( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    unsigned long ul;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(ul)), &ul);
        nchar += fprintf(ostream, "{ulong %lu}", ul);
    }
    return nchar;
}

static int UMLRTObject_fprintf_ulonglong( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    unsigned long long ll;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(ll)), &ll);
        nchar += fprintf(ostream, "{ulonglong %llu}", ll);
    }
    return nchar;
}

static int UMLRTObject_fprintf_ushort( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    unsigned short ush;
    for (int i = 0; i < arraySize; ++i)
    {
        desc->copy(desc, ((uint8_t*)data + i*sizeof(ush)), &ush);
        nchar += fprintf(ostream, "{ushort %u}", ush);
    }
    return nchar;
}

static const UMLRTObject_class UMLRTType_bool_
= {
        "bool",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_bool,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(bool),
        sizeof(bool),
        NULL // fields
};

static const UMLRTObject_class UMLRTType_char_
= {
        "char",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_char,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(char),
        sizeof(char),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_double_
= {
        "double",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_double,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(double),
        sizeof(double),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_float_
= {
        "float",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_float,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(float),
        sizeof(float),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_int_
= {
        "int",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_int,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(int),
        sizeof(int),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_long_
= {
        "long",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_long,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(long),
        sizeof(long),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_longdouble_
= {
        "longdouble",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_longdouble,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(long double),
        sizeof(long double),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_longlong_
= {
        "longlong",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_longlong,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(long long),
        sizeof(long long),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_ptr_
= {
        "ptr",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_ptr,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(void *),
        sizeof(void *),
        NULL // fields
};

static const UMLRTObject_class UMLRTType_short_
= {
        "short",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_short,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(short),
        sizeof(short),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_uchar_
= {
        "uchar",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_uchar,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(unsigned char),
        sizeof(unsigned char),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_uint_
= {
        "uint",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_uint,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(unsigned int),
        sizeof(unsigned int),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_ulong_
= {
        "ulong",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_ulong,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(unsigned long),
        sizeof(unsigned long),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_ulonglong_
= {
        "ulonglong",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_ulonglong,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(unsigned long long),
        sizeof(unsigned long long),
        NULL, // fields
};

static const UMLRTObject_class UMLRTType_ushort_
= {
        "ushort",
        UMLRTObject_initialize,
        UMLRTObject_copy,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObject_destroy,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_ushort,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(unsigned short),
        sizeof(unsigned short),
        NULL, // fields
};

const UMLRTObject_class * const UMLRTType_bool = &UMLRTType_bool_;
const UMLRTObject_class * const UMLRTType_char = &UMLRTType_char_;
const UMLRTObject_class * const UMLRTType_double = &UMLRTType_double_;
const UMLRTObject_class * const UMLRTType_float = &UMLRTType_float_;
const UMLRTObject_class * const UMLRTType_int = &UMLRTType_int_;
const UMLRTObject_class * const UMLRTType_long = &UMLRTType_long_;
const UMLRTObject_class * const UMLRTType_longdouble = &UMLRTType_longdouble_;
const UMLRTObject_class * const UMLRTType_longlong = &UMLRTType_longlong_;
const UMLRTObject_class * const UMLRTType_ptr = &UMLRTType_ptr_;
const UMLRTObject_class * const UMLRTType_short = &UMLRTType_short_;
const UMLRTObject_class * const UMLRTType_uchar = &UMLRTType_uchar_;
const UMLRTObject_class * const UMLRTType_uint = &UMLRTType_uint_;
const UMLRTObject_class * const UMLRTType_ulong = &UMLRTType_ulong_;
const UMLRTObject_class * const UMLRTType_ulonglong = &UMLRTType_ulonglong_;
const UMLRTObject_class * const UMLRTType_ushort = &UMLRTType_ushort_;

