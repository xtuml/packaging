#!ksh
#===========================================================================
# File: rox_callback
#
# Description:
# This file strings OOA domain callback functions together into
# a application callback table block for system level generation.
#
# Notice:
# (C) Copyright 2001-2013 Mentor Graphics Corporation
#     All rights reserved.
# Enhancements provided by TOYO Corporation.
#===========================================================================
# $1 - Path to the translation root node
#===========================================================================

set -u
set -e

exit # Callback generation is disabled.

# Load utility functions
. ${ROX_MC_BIN_DIR}/rox_functions   # This does not work.

#===========================================================================
# Function: Usage
#===========================================================================
function Usage
{
  print -u2 "Usage: ${0##*/} <root>"
  exit 1
}

#===========================================================================
# Function: GetDomainAsyncSegmentFileName
# $1 - Domain repository name
#===========================================================================
function GetDomainAsyncSegmentFileName
{
  if [ ${#} -ne 1 ] ; then
    print -u2 "Usage: GetDomainAsyncSegmentFileName <domain>"
    exit 1
  fi

  typeset domain=$1
  # Must jibe with archetype declared file name.
  print "${domain}_dom_async.seg"
}

#===========================================================================
# Function: GetDomainAsyncFileName
# $1 - Translation root node
# $2 - Domain repository name
#===========================================================================
function GetDomainAsyncFileName
{
  if [ ${#} -ne 2 ] ; then
    print -u2 "Usage: GetDomainAsyncFileName <root> <domain>"
    exit 1
  fi

  typeset root_node=$1
  typeset domain=$2
  typeset callback_file_path=${root_node}/${domain}/${ROX_APP_XLATE_DIR_NAME}/${ROX_APP_SOURCE_DIR_NAME}
  typeset callback_file_name=$( GetDomainAsyncSegmentFileName $domain )
  print "${callback_file_path}/${callback_file_name}"
}

#===========================================================================
# Main body:
#===========================================================================
if [ ${#} -ne 1 ] ; then
  Usage
fi

program=${0##*/}
root_node=$( 'cd' $1; 'rox_pwd' )

# Get the name of the system generated source directory, and hop over there.
sys_gen_src_dir=${root_node}/${ROX_APP_SYSTEM_DIR_NAME}/${ROX_APP_XLATE_DIR_NAME}/${ROX_APP_SOURCE_DIR_NAME}
'cd' $sys_gen_src_dir

# Get the OOA domain configuration file.
config_file=$( ROX_GetSystemConfigFile $root_node )

# Get the list of domains in the system.
domain_set=$( ROX_GetList Domain $config_file )

# Temporary file in which to create the system async table.
temp_callback_file=${sys_gen_src_dir}/tmp_callback_$$

# Verify that each domain has a async file generated for it.
for domain in $domain_set
  do
    callback_file=$( GetDomainAsyncFileName $root_node $domain )
    if [ ! -f $callback_file ] ; then
      print -u2 -r "${program}: ERROR: File '$callback_file' does not exist."
      exit 1
    fi
  done

# Names to be used in the generated code.
# Note: Must jibe with archetype expectations/definitions.
callback_table_file_name=async_callback.c
real_callback_file=${sys_gen_src_dir}/${callback_table_file_name}

# Create a temporary Callback table file.
print "\
/*
 * File: ${callback_table_file_name}
 *
 * Warnings:
 *   !!! THIS IS AN AUTO-GENERATED FILE. PLEASE DO NOT EDIT. !!!
 */

#include \"async_callback.h\"" > $temp_callback_file

for domain in $domain_set
  do
    callback_file=$( GetDomainAsyncFileName $root_node $domain )
    # Change "+include" to "#include" and gather them up.
    'sed' -n 's/^+include/#include/p' $callback_file >> $temp_callback_file
  done

print "" >> $temp_callback_file

# Render implementation of each domain's public Callback method(s)
# into a single system level function.
print "\
static CALLBACK_TABLE callbackTable[] =
{" >> $temp_callback_file
for domain in $domain_set
  do
    callback_file=$( GetDomainAsyncFileName $root_node $domain )
    #
    'sed' '/^+include/d' $callback_file |
    'sed' '/^ *$/d' |
    'sed' '/^ *\/\/ *$/d' >> $temp_callback_file
  done
print "  {0, 0, false}
};" >> $temp_callback_file

print "
void ASYNC_enableCallback(int enum_name)
{
  CALLBACK_TABLE* table = callbackTable;

  while(table->handler) {
    if(table->number == enum_name) {
      table->triggered = true;
      break;
    }
    table++;
  }
}

void ASYNC_disableCallback(int enum_name)
{
  CALLBACK_TABLE* table = callbackTable;

  while(table->handler) {
    if(table->number == enum_name) {
      table->triggered = false;
      break;
    }
    table++;
  }
}

bool ASYNC_getCallbackStatus(int enum_name)
{
  CALLBACK_TABLE* table = callbackTable;
  bool ret;

  ret = false;
  while(table->handler) {
    if(table->number == enum_name) {
      ret = table->triggered;
      break;
    }
    table++;
  }

  return ret;
}

void ASYNC_callback()
{
  CALLBACK_TABLE* table = callbackTable;

  while(table->handler) {
    if(table->triggered) {
      (table->handler)();
    }
    table++;
  }
}

" >> $temp_callback_file

# Update the generated system Callback file as appropriate.
if [ -f $real_callback_file ] ; then
  set +e
  'diff' $temp_callback_file $real_callback_file > /dev/null 2> /dev/null
  if [[ $? -ne 0 ]] ; then
    'rm' -f $real_callback_file
    'mv' $temp_callback_file $real_callback_file
    print "${program}: INFO: File '${callback_table_file_name}' REPLACED."
  else
    print "${program}: INFO: File '${callback_table_file_name}' UNCHANGED."
    'rm' -f $temp_callback_file
  fi
  set -e
else
  'mv' $temp_callback_file $real_callback_file
  print "${program}: INFO: File '${callback_table_file_name}' CREATED."
fi

