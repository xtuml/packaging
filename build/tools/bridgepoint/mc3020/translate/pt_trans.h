/*
//=PrologueBegin============================================================
//
// File:      pt_trans.h, 1.30
// Modified:  12/21/99, 07:50:47
//
// (C) Copyright 1999-2013 Mentor Graphics Corporation  All Rights Reserved.
// (C) Copyright 1999-2010 ,2000 by TOYO Corporation  All Rights Reserved.
//
//=PrologueEnd==============================================================
*/

#ifndef PT_TRANS_HH
#define PT_TRANS_HH

/* Platform dependent macro definition */
#if defined(_WINDOWS)
#define __SHIFT_JIS
#define IS_ASCII(A)   (A <= 127)
#define IS_HANKATA(A) (A >= 161 && A <=223)
#define IS_SJIS1(A)   ((A >= 129 && A <=159) || (A >= 224 && A <=239))
#define IS_SJIS2(A)   (A >= 64 && A <=252)
#else /* _WINDOWS */
#define __EUC_JP
#define IS_ASCII(A)   (A <= 127)
#endif /* _WINDOWS */

typedef struct {
  char *origName;     /* original name */
  char *compName;     /* compiler name */
} pt_TranslateReferenceRecord;

extern pt_TranslateReferenceRecord pt_TranslateRefTable[];

#define PT_TRANSLATE_JP_DELIMITOR "@"	/* Japanese-English delimitor */
#define PT_TRANSLATE_JP_PREFIX	  "JP_"	/* prefix for arbitrary string */
#define PT_TRANSLATE_JP_TOKEN	  ","	/* token in reference table */
#define PT_TRANSLATE_ENUM_TOKEN	  "="	/* token in enumeration */
#define PT_TRANSLATE_POLY_WORD    "*"	/* Japanese-English delimitor */

typedef enum {
  PT_TRANSLATE_DEFAULT_E,   /* default function */
  PT_TRANSLATE_JP_CODE_E,   /* Japanese -> character code */
  PT_TRANSLATE_JP_TOKEN_E,  /* remove Japanese string and token */
  PT_TRANSLATE_JP_TABLE_E,  /* translation table */
  PT_TRANSLATE_JP_UNIQUE_E, /* arbitrary unique name */
  PT_TRANSLATE_ENUM_E,      /* triming enum(name) */
  PT_TRANSLATE_ENUM2_E,     /* triming enum(init value) */
  PT_TRANSLATE_SKIP_QAUOTE, /* skip qauotatin */
  PT_TRANSLATE_DOM_ENUM_E,  /* use instead of dom_enum.pl */
  PT_TRANSLATE_NOSPLAT_E,   /* cut *  */
  PT_TRANSLATE_AND_E        /* &->&amp; */
} pt_Translate_e;

char* pt_Translate_default(pt_Translate_e trans_type, const char* p_string);
char* pt_Translate_jp_unique(const char* p_string);
char* pt_Translate_jp_code(const char* p_string);
char* pt_Translate_jp_token(const char* p_string);
char* pt_Translate_jp_table(const char* p_string);
char* pt_Translate_trim_enum(const char* p_string);
char* pt_Translate_trim_enum2(const char* p_string);
char* pt_Translate_skip_qauote(const char* p_string);
char* pt_Translate_Dom_Enum( const char * p_string );

#define PT_TRANSLATE_DEFAULT_STR   ""	 /* default function */
#define PT_TRANSLATE_JP_CODE_STR   "JP"  /* Japanese -> character code */
#define PT_TRANSLATE_JP_TOKEN_STR  "JP2" /* remove Japanese string and token */
#define PT_TRANSLATE_JP_TABLE_STR  "JP3" /* translation table */
#define PT_TRANSLATE_JP_UNIQUE_STR "JP4" /* arbitrary unique name */
#define PT_TRANSLATE_ENUM_STR      "ENUM" /* triming enumeration */
#define PT_TRANSLATE_ENUM2_STR     "ENUM2" /* triming enumeration(init value) */
#define PT_TRANSLATE_QAUOTE_STR    "QA"  /* skip qauotation mark */
#define PT_TRANSLATE_DOM_ENUM_STR  "DOM_ENUM" /* use instead of dom_enum.pl */
#define PT_TRANSLATE_NOSPLAT_STR   "nosplat" /* */
#define PT_TRANSLATE_AND_STR   "CUTAND" /* */

#ifdef	__cplusplus
extern "C" 
{
#endif

/* Function declarations */
void *pt_TranslateInit();
const char *pt_TranslateString( void *p_parm,
                                const char *p_switch,
                                const char *p_string);
void pt_TranslateCleanup(void *p_parm);

#ifdef	__cplusplus
}
#endif

#endif
