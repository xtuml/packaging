/*
//=PrologueBegin============================================================
//
// File:      pt_trans.c, 1.30
// Modified:  12/21/99, 07:50:47
//
// (C) Copyright 1999-2013 Mentor Graphics Corporation  All Rights Reserved.
//
//=PrologueEnd==============================================================
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "pt_trans.h"

char *pt_Translate_And(char * p_string)
{
/*
************************************************************
Note:
The heap memory allocated by this function is not freed.
************************************************************
*/
  unsigned char *pIndex = NULL;
  unsigned char *pIndex2 = NULL;
  char *pResult = NULL;
  char buff[256]={0};
  int  len;
  
  for(pIndex = p_string,pIndex2 = p_string;*pIndex != '\0'; pIndex++)
  {
    if ( IS_SJIS1(*pIndex) )
    {
      pIndex++;
      continue;
    }
    if ( *pIndex == '&') 
    {
      *pIndex = '\0';
      strcat(buff, pIndex2);
      strcat(buff, "&amp;");
      pIndex2 = pIndex + 1;
	}
  }
  strcat(buff, pIndex2);

  /* alloc memory */
  len = strlen(buff) ;
  pResult = (unsigned char*)malloc(len * sizeof(char) + sizeof(char));
  strcpy (pResult,buff);

  return pResult;
}


char *pt_Translate_Nosplat(char * p_string)
{
/*
************************************************************
Note:
The heap memory allocated by this function is not freed.
************************************************************
*/
  char *pIndex = NULL;
  char *pResult = NULL;
  char buff[256];
  int  len;
  unsigned char* ptr1;

  pIndex = strstr(p_string, PT_TRANSLATE_POLY_WORD);

  if (pIndex != NULL) {
    *pIndex = '\0';

    strcpy(buff, p_string);
    strcat(buff, pIndex + 1);
  } else {
    strcpy(buff, p_string);
  }

  ptr1 = (unsigned char*)buff;
  while(*ptr1 != '\0') {
    if(!IS_ASCII(*ptr1)) {
      strcpy(p_string, buff);
      return NULL;
    }
    ptr1++;
  }

  /* alloc memory */
  len = strlen(buff) ;
  pResult = (char*)malloc(len * sizeof(char) + sizeof(char));
  strcpy (pResult,buff);

  return pResult;
}


char *pt_Translate_Dom_Enum( const char * p_string )
{
/*
************************************************************
Note:
The heap memory allocated by this function is not freed.
************************************************************
*/
  char *pFindLoc = NULL;
  char *pResult = NULL;
  char buff[256];
  int  len;

  /* Check to see if the enumeration contains a value */
  pFindLoc = strstr( p_string, PT_TRANSLATE_ENUM_TOKEN );
  if ( pFindLoc != NULL  )
  {
    strcpy( buff, "EnumName: " );
    len = strlen( buff );
    if ( (255 -14 - len) < (pFindLoc - p_string) )
      strncat( buff, p_string, 255 -14 - len );
    else
      strncat( buff, p_string, pFindLoc - p_string );
    strcat( buff, " \nEnumValue: " );
    pFindLoc++;
    len = strlen( buff );
    strncat( buff, pFindLoc, 255 -1 - len );
    strcat( buff, "\n" );
  }else
  {
    strcpy( buff, "EnumName: " );
    len = strlen( buff );
    strncat( buff, p_string, 255 -1 - len );
    strcat( buff, "\n" );
  }

  /* alloc memory */
  len = strlen( buff ) ;
  pResult = (char*)malloc(len * sizeof(char) + sizeof(char));
  strcpy (pResult,buff);

  return pResult;
}

char* pt_Translate_skip_qauote(const char* p_string)
{
/*
************************************************************
Note:
This function deletes the very first and last char.
This will be used to delete qauotation mark in the string
as follows;
  <example>
    "string" -> string
The heap memory allocated by this function is not freed.
************************************************************
*/
  char* new_string = (char*)malloc(strlen(p_string)* sizeof(char) + sizeof(char));
  strcpy(new_string, p_string + 1);
  new_string[strlen(p_string)-2] = (char)NULL;

  return new_string;
}

char* pt_Translate_trim_enum(const char* p_string)
{
/*
************************************************************
Note:
This function deletes the initial value from enum definition
as follows;
  <example>
    enum_name1 = 1 -> 'enum_name1'
    enum_name2 = 2 -> 'enum_name2'
The trimed string might contains space. You should trim
space by yourself.
The heap memory allocated by this function is not freed.
************************************************************
*/
  char* new_string = (char *)p_string;
  char* ret;

  ret = strstr(new_string, PT_TRANSLATE_ENUM_TOKEN);
  if (ret) {
    new_string = (char*)malloc(strlen(p_string) * sizeof(char) + sizeof(char));
    strcpy(new_string, p_string);
    ret = strstr(new_string, PT_TRANSLATE_ENUM_TOKEN);
    *ret = (char)NULL;  /* trim initial value */
  }
  return new_string;
}

char* pt_Translate_trim_enum2(const char* p_string)
{
/*
************************************************************
Note:
This function deletes the initial value from enum definition
as follows;
  <example>
    enum_name1 = 1 -> '= 1'
    enum_name2 = 2 -> '= 2'
The trimed string might contains space. You should trim
space by yourself.
The heap memory allocated by this function is not freed.
************************************************************
*/
  char* new_string = (char *)p_string;
  char* ret;

  ret = strstr(new_string, PT_TRANSLATE_ENUM_TOKEN);
  if (ret) {
    new_string = (char*)malloc(strlen(p_string) * sizeof(char) + sizeof(char));
    ret = strstr(p_string, PT_TRANSLATE_ENUM_TOKEN);
    strcpy(new_string, ret);
  }
  else {
    *new_string = (char)NULL;
  }

  return new_string;
}

char* pt_Translate_jp_unique(const char* p_string)
{
/*
************************************************************
Note:
The heap memory allocated by this function is not freed.
************************************************************
*/
  char temp[20];
  char* new_string;
  unsigned char* ptr;
  char is_japanese = 0;
  unsigned int unique_id;
  FILE *stream;

  char init_txt[256];
  char *domain_root_dir;
  unsigned char in_creating_code = 0;

  ptr = (unsigned char*)p_string;
  while(*ptr != 0) {
    if(!IS_ASCII(*ptr)) {
      is_japanese = !is_japanese;
      break;
    }
    ptr++;
  }

  if(is_japanese) {
    // pt_transInit.txt のパスを得る
    domain_root_dir = getenv("DOMAIN_ROOT_DIR");
	if (domain_root_dir == NULL) {
		sprintf(init_txt, "pt_transInit.txt");
	}
	else {
		sprintf(init_txt
				, "%s/translate/pt_transInit.txt"
				, domain_root_dir);
    }

	if (strcmp(getenv("IN_CREATING_TRANSLATE_TABLE"), "TRUE") != 0) {
		// ソースコード生成フェーズならば(対応表作成フェーズではないならば)
		// 後で Warning を出すためにフラグを立てておく
		in_creating_code = 1;
	}

    if( (stream = fopen( init_txt, "r+" )) == NULL ) {
      printf("Error : pt_transInit.txt does not exist!\n", init_txt);
      exit(-1);
    }

    fscanf( stream, "%d", &unique_id );

    sprintf(temp, "%d", unique_id);
    new_string = (char*)malloc(strlen(temp) * sizeof(char) +
           strlen(PT_TRANSLATE_JP_PREFIX) * sizeof(char) + /* for prefix string */
           sizeof(char));                 /* for null string */

    strcpy(new_string, PT_TRANSLATE_JP_PREFIX);
    sprintf(new_string + strlen(PT_TRANSLATE_JP_PREFIX), "%d", unique_id);

    unique_id++;
    rewind(stream);
    fprintf(stream, "%d", unique_id);
    fclose( stream );

    if (in_creating_code) {
      // 本処理がソースコード生成フェーズで行われた場合、
      // 本来ありえない処理である旨を、Warning Message として表示する。
      printf("Warning : '%s' が pt_transTable.csv に定義されていません。\n文字列 '%s' を割り当てました。\n"
             , p_string
             , new_string);
    }

    return new_string;
  }
  else {
    return (char*)p_string;
  }
}

char* pt_Translate_jp_code(const char* p_string)
{
/*
************************************************************
Note:
The heap memory allocated by this function is not freed.
************************************************************
*/
  unsigned char* ptr1;
  char* ptr2;
  char* new_string;

  new_string = (char*)malloc(strlen(p_string) * 2 * sizeof(char) +
    strlen(PT_TRANSLATE_JP_PREFIX) * sizeof(char) +  /* for prefix string */
    sizeof(char));                /* for null string */
  ptr1 = (unsigned char*)p_string;
  ptr2 = new_string;

  if(!IS_ASCII(*ptr1)) {
    strcpy(new_string, PT_TRANSLATE_JP_PREFIX);
    ptr2 += strlen(PT_TRANSLATE_JP_PREFIX);
  }

  while(*ptr1 != 0) {
#if defined(__EUC_JP)
    if(!IS_ASCII(*ptr1)) {
      sprintf(ptr2, "%2X", *ptr1);
      ptr1 ++ ;
      ptr2 += 2;
    }
#elif defined(__SHIFT_JIS)
    if (IS_SJIS1(*ptr1)) {
      sprintf(ptr2, "%2X%2X", *ptr1, *(ptr1+1));
      ptr1 += 2;
      ptr2 += 4;
    }
    else if(IS_HANKATA(*ptr1)) {
      sprintf(ptr2, "%2X", *ptr1);
      ptr1 ++ ;
      ptr2 += 2;
    }
#endif
    else {
      *ptr2 = *ptr1;
      ptr1 ++ ;
      ptr2 ++;
    }
  }

  *ptr2 = (char)NULL;
  return new_string;
}

char* pt_Translate_jp_token(const char* p_string)
{
/*
************************************************************
Note:
The heap memory allocated by this function is not freed.
************************************************************
*/
#if defined(__EUC_JP)
  char* new_string;
  char* ret;

  ret = strstr(p_string, PT_TRANSLATE_JP_DELIMITOR);
  if (ret) {
    ret += strlen(PT_TRANSLATE_JP_DELIMITOR);    /* skip delimitor string */
    new_string = (char*)malloc(strlen(ret)* sizeof(char) + sizeof(char));
    strcpy(new_string, ret);
    return new_string;
  }
  else
    return NULL;

#elif defined(__SHIFT_JIS)
  char* new_string;
  char* ret;
  unsigned char* ptr1;

  ptr1 = (unsigned char*)p_string;

  while(*ptr1 != 0) {
    if (IS_SJIS1(*ptr1)) {
      ptr1 += 2;
    }
    else if(IS_HANKATA(*ptr1)) {
      ptr1 ++ ;
    }
    else {  /* non Japanese character */
      ret = strstr(ptr1, PT_TRANSLATE_JP_DELIMITOR);
      if (ret == ptr1) {
    ret += strlen(PT_TRANSLATE_JP_DELIMITOR); /* skip delimitor string */
    new_string = (char*)malloc(strlen(ret)* sizeof(char) + sizeof(char));
    strcpy(new_string, ret);
    return new_string;
      }
      else {
    ptr1 ++ ;
      }
    }
  }

  return NULL;

#endif
}

char* pt_Translate_jp_table(const char* p_string)
{
  FILE *fp;
  char line[256];
  char *new_string, *index;

  char table_csv[256];
  char *domain_root_dir;

  // pt_transTable.csv のパスを得る
  domain_root_dir = getenv("DOMAIN_ROOT_DIR");
  if (domain_root_dir == NULL) {
    sprintf(table_csv, "pt_transTable.csv");
  }
  else {
    sprintf(table_csv
            , "%s/translate/pt_transTable.csv"
            , domain_root_dir);
  }

  // csv ファイルが存在していなければ、NULLを返却
  if ((fp = fopen(table_csv, "r")) == NULL) {
    return NULL;
  }

  // csv ファイルが存在している場合
  while ((fgets(line, sizeof(line), fp)) != NULL) {
    index = strstr(line, ",");
    if (index == NULL) {
      continue;
    }

    *index = '\0';
    if (strcmp(line, p_string) == 0) {
      fclose(fp);

      // "," の後から改行の前までをとりだす
      new_string = index + 1;
      new_string[ strlen(new_string) - 1 ] = '\0';

      if (strlen(new_string) > 0) {
        return new_string;
      }
      else {
        return NULL;
      }
    }
  }

  fclose(fp);
  return NULL;

}

char* pt_Translate_default(pt_Translate_e trans_type, const char* p_string)
{
  char* ret = 0;

  if (trans_type != PT_TRANSLATE_JP_TABLE_E)
    ret = pt_Translate_jp_table(p_string);

  if ((!ret) && (trans_type != PT_TRANSLATE_JP_TOKEN_E))
    ret = pt_Translate_jp_token(p_string);

  if ((!ret) && (trans_type != PT_TRANSLATE_JP_UNIQUE_E))
    ret = pt_Translate_jp_unique(p_string);

  if ((!ret) && (trans_type != PT_TRANSLATE_JP_CODE_E))
    ret = pt_Translate_jp_code(p_string);

  if (ret != 0)
    return ret;
  else
    return (char*)p_string;
}

pt_Translate_e pt_Translate_getType(const char* p_switch)
{
  if (strcmp(p_switch, PT_TRANSLATE_JP_CODE_STR) == 0)
    return PT_TRANSLATE_JP_CODE_E;
  else if (strcmp(p_switch, PT_TRANSLATE_JP_TOKEN_STR) == 0)
    return PT_TRANSLATE_JP_TOKEN_E;
  else if (strcmp(p_switch, PT_TRANSLATE_JP_TABLE_STR) == 0)
    return PT_TRANSLATE_JP_TABLE_E;
  else if (strcmp(p_switch, PT_TRANSLATE_JP_UNIQUE_STR) == 0)
    return PT_TRANSLATE_JP_UNIQUE_E;
  else if (strcmp(p_switch, PT_TRANSLATE_ENUM_STR) == 0)
    return PT_TRANSLATE_ENUM_E;
  else if (strcmp(p_switch, PT_TRANSLATE_ENUM2_STR) == 0)
    return PT_TRANSLATE_ENUM2_E;
  else if (strcmp(p_switch, PT_TRANSLATE_QAUOTE_STR) == 0)
    return PT_TRANSLATE_SKIP_QAUOTE;
  else if (strcmp(p_switch, PT_TRANSLATE_DOM_ENUM_STR) == 0)
    return PT_TRANSLATE_DOM_ENUM_E;
  else if (strcmp(p_switch, PT_TRANSLATE_NOSPLAT_STR) == 0)
    return PT_TRANSLATE_NOSPLAT_E;
  else if (strcmp(p_switch, PT_TRANSLATE_AND_STR) == 0)
    return PT_TRANSLATE_AND_E;
  else
    return PT_TRANSLATE_DEFAULT_E;
}

const char *pt_TranslateString( void *p_parm,
                const char *p_switch,
                const char *p_string)
{
  char* ret = 0;
  char str[256];
  pt_Translate_e trans_type;

  if (strlen(p_string) != 0) {
    strcpy(str, p_string);

    trans_type = pt_Translate_getType(p_switch);
    switch (trans_type) {
    case PT_TRANSLATE_JP_CODE_E:
      ret = pt_Translate_jp_code(p_string);
      break;
    case PT_TRANSLATE_JP_TOKEN_E:
      ret = pt_Translate_jp_token(p_string);
      break;
    case PT_TRANSLATE_JP_TABLE_E:
      ret = pt_Translate_jp_table(p_string);
      break;
    case PT_TRANSLATE_JP_UNIQUE_E:
      ret = pt_Translate_jp_unique(p_string);
      break;
    case PT_TRANSLATE_ENUM_E:
      ret = pt_Translate_trim_enum(p_string);
      break;
    case PT_TRANSLATE_ENUM2_E:
      ret = pt_Translate_trim_enum2(p_string);
      break;
    case PT_TRANSLATE_SKIP_QAUOTE:
      ret = pt_Translate_skip_qauote(p_string);
      break;
    case PT_TRANSLATE_DOM_ENUM_E:
      ret = pt_Translate_Dom_Enum(p_string);
      break;
    case PT_TRANSLATE_NOSPLAT_E:
      ret = pt_Translate_Nosplat(str);
      break;
    case PT_TRANSLATE_AND_E:
      ret = pt_Translate_And(str);
      break;
    default:
      break;
    }

    if (!ret) {
      ret = pt_Translate_default(trans_type, str);
    }
    return ret;

  }
  else
    return (char*)p_string;
}

void *pt_TranslateInit()
{
  void *parm = 0;
  return parm;
}

void pt_TranslateCleanup( void *p_parm )
{
  /* do nothing */
}

