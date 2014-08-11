.//====================================================================
.//
.// File:      reftable.arc, 1.10
.// Modified:  12/21/99, 16:07:58
.//
.// (C) Copyright 1999-2013 Mentor Graphics Corporation  All Rights Reserved.
.//
.//====================================================================
.//
.//    Purpose:    This archetype file contains functions for generating
.//                translation table C code for the OOA elements.
.//                The generated file will contain translation table from
.//                OOA element including non-ascii code, like Japanse,
.//                to the compilation name.
.//
.//                Note. This is a example archetype. You can modify
.//                for the performance improvement and enhancement.
.//
.//    Functions:  (marker for formatting)
.//
.//===========================================================================
.//
.//
.//============================================================================
.//
.// define file name
.//
.//============================================================================
.function getFileName
  .assign attr_result = "pt_transTable.csv"
.end function
.//
.//
.//============================================================================
.//
.// add OOA element for each enumeration definition in owner domain
.//
.//============================================================================
.function addOOAElementEnumerationOwner
  .param boolean trans_flag
  .param string elm_name
  .assign attr_result = false
  .//
  .assign enum_name = "$rtENUM{elm_name}"
  .assign enum_value = "$rtENUM2{elm_name}"
  .if (trans_flag)
    .assign trans_name = "$t{enum_name}"
    .if ("${enum_name}" != trans_name)
      .assign attr_result = true
      .if ("${enum_value}" != "")
${elm_name},${trans_name}${enum_value}
${enum_name},${trans_name}
      .else
${elm_name},${trans_name}
      .end if
    .end if
  .else
    .if ("${elm_name}" != "")
      .assign attr_result = true
${elm_name},
    .end if
  .end if
.end function
.//
.//============================================================================
.//
.// add OOA element for each enumeration in user domain
.//
.//============================================================================
.function addOOAElementEnumerationUser
  .param boolean trans_flag
  .param string elm_name
  .assign attr_result = false
  .//
  .assign enum_name = "$rtENUM{elm_name}"
  .if (trans_flag)
    .assign trans_name = "$t{enum_name}"
    .if ("${enum_name}" != trans_name)
      .assign attr_result = true
${enum_name},${trans_name}
    .end if
  .else
    .if ("${enum_name}" != "")
      .assign attr_result = true
${enum_name},
    .end if
  .end if
.end function
.//
.//
.//
.//============================================================================
.//
.// check whether generation for enumuration
.//
.//============================================================================
.//
.function IsGenerateEnumeration
  .param inst_ref dt  .// S_DT
  .assign attr_result = FALSE
  .assign attr_case = ""
  .assign attr_owner_flag = TRUE
  .//
  .if ( "$ur{dt.Descrip:Enumeration}" == "TRUE" )
    .assign attr_result = TRUE
    .assign attr_case = "MIX"
  .elif ( "$ur{dt.Descrip:ENUMERATION}" == "TRUE" )
    .assign attr_result = TRUE
    .assign attr_case = "UPPER"
  .elif ( "$ur{dt.Descrip:enumeration}" == "TRUE" )
    .assign attr_result = TRUE
    .assign attr_case = "LOWER"
  .end if
  .assign owning_dom_name = "$r{dt.Descrip:Owner}"
  .if ( owning_dom_name == "" )
    .assign owning_dom_name = "$r{dt.Descrip:OWNER}"
    .if ( owning_dom_name == "" )
      .assign owning_dom_name = "$r{dt.Descrip:owner}"
    .end if
  .end if
  .if ( owning_dom_name != "" )
    .assign attr_owner_flag = FALSE
  .end if
.end function
.//
.//
.//============================================================================
.//
.// add OOA element for each enumeration in user domain
.//
.//============================================================================
.function addOOAElementEnumerationBody
  .param boolean trans_flag
  .param boolean owner_flag
  .param string elm_name
  .assign attr_result = false
  .//
  .if (owner_flag)
    .invoke enum_body = addOOAElementEnumerationOwner(trans_flag, elm_name)
${enum_body.body}\
  .else
    .invoke enum_body = addOOAElementEnumerationUser(trans_flag, elm_name)
${enum_body.body}\
  .end if
.end function
.//
.//
.//============================================================================
.//
.// add OOA element enumeration
.//
.//============================================================================
.function addOOAElementEnumeration
  .param string elm_name
  .param inst_ref_set dt_set
  .param boolean trans_flag
  .//

***** ${elm_name} Name *****
  .for each dt in dt_set
    .invoke is_enum = IsGenerateEnumeration(dt)
    .if (is_enum.result)
      .assign owner_flag = is_enum.owner_flag
      .if (is_enum.case == "MIX")
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator0}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator1}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator2}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator3}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator4}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator5}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator6}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator7}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator8}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator9}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator10}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator11}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator12}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator13}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator14}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator15}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator16}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator17}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator18}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator19}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator20}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator21}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator22}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator23}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator24}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator25}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator26}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator27}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator28}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator29}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator30}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator31}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator32}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator33}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator34}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator35}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator36}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator37}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator38}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator39}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator40}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator41}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator42}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator43}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator44}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator45}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator46}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator47}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator48}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator49}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator50}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator51}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator52}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator53}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator54}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator55}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator56}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator57}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator58}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:Enumerator59}")
${enum_body.body}\
      .elif (is_enum.case == "LOWER")
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator0}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator1}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator2}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator3}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator4}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator5}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator6}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator7}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator8}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator9}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator10}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator11}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator12}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator13}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator14}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator15}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator16}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator17}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator18}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator19}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator20}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator21}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator22}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator23}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator24}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator25}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator26}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator27}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator28}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator29}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator30}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator31}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator32}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator33}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator34}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator35}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator36}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator37}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator38}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator39}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator40}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator41}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator42}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator43}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator44}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator45}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator46}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator47}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator48}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator49}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator50}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator51}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator52}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator53}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator54}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator55}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator56}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator57}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator58}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:enumerator59}")
${enum_body.body}\
      .else
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR0}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR1}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR2}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR3}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR4}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR5}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR6}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR7}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR8}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR9}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR10}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR11}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR12}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR13}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR14}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR15}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR16}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR17}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR18}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR19}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR20}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR21}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR22}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR23}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR24}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR25}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR26}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR27}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR28}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR29}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR30}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR31}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR32}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR33}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR34}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR35}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR36}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR37}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR38}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR39}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR40}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR41}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR42}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR43}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR44}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR45}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR46}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR47}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR48}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR49}")
${enum_body.body}\
        .//
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR50}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR51}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR52}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR53}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR54}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR55}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR56}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR57}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR58}")
${enum_body.body}\
        .invoke enum_body = addOOAElementEnumerationBody(trans_flag, owner_flag, "${dt.Descrip:ENUMERATOR59}")
${enum_body.body}\
      .end if
    .end if
  .end for
  .//
.end function
.//
.//
.//============================================================================
.//
.// add OOA element name
.//
.//============================================================================
.function addOOAElementName
  .param string elm_name
  .param inst_ref_set elm_set
  .param boolean trans_flag
  .//

***** ${elm_name} Name *****
  .for each elm in elm_set
    .if (trans_flag)
      .assign trans_name = "$t{elm.Name}"
      .if ("${elm.Name}" != trans_name)
${elm.Name},${trans_name}
      .end if
    .else
      .if ("${elm.Name}" != "")
${elm.Name},
      .end if
    .end if
  .end for
  .//
.end function
.//
.//
.//============================================================================
.//
.// add OOA element prefix
.//
.//============================================================================
.function addOOAElementPrefix
  .param string elm_name
  .param inst_ref_set elm_set
  .param boolean trans_flag
  .//

***** ${elm_name} Prefix *****
  .for each elm in elm_set
    .if (trans_flag)
      .assign trans_name = "$t{elm.Prefix}"
      .if ("${elm.Prefix}" != trans_name)
${elm.Prefix},${trans_name}
      .end if
    .else
      .if ("${elm.Prefix}" != "")
${elm.Prefix},
      .end if
    .end if
  .end for
  .//
.end function
.//
.//
.//============================================================================
.//
.// add OOA element key letter
.//
.//============================================================================
.function addOOAElementKeyLetter
  .param string elm_name
  .param inst_ref_set elm_set
  .param boolean trans_flag
  .//

***** ${elm_name} KeyLetter *****
  .for each elm in elm_set
    .if (trans_flag)
      .assign trans_name = "$t{elm.Key_Lett}"
      .if ("${elm.Key_Lett}" != trans_name)
${elm.Key_Lett},${trans_name}
      .end if
    .else
      .if ("${elm.Key_Lett}" != "")
${elm.Key_Lett},
      .end if
    .end if
  .end for
  .//
.end function
.//
.//
.//============================================================================
.//
.// add OOA element event label
.//
.//============================================================================
.function addOOAElementEventLabel
  .param string elm_name
  .param inst_ref_set elm_set
  .param boolean trans_flag
  .//

***** ${elm_name} Event Label *****
  .for each elm in elm_set
    .if (trans_flag)
      .assign trans_name = "$t{elm.Drv_Lbl}"
      .if ("${elm.Drv_Lbl}" != trans_name)
${elm.Drv_Lbl},${trans_name}
      .end if
    .else
      .if ("${elm.Drv_Lbl}" != "")
${elm.Drv_Lbl},
      .end if
    .end if
  .end for
  .//
.end function
.//
.//
.//============================================================================
.//
.// add OOA element meaning
.//
.//============================================================================
.function addOOAElementMeaning
  .param string elm_name
  .param inst_ref_set elm_set
  .param boolean trans_flag
  .//

***** ${elm_name} Meaning *****
  .for each elm in elm_set
    .if (trans_flag)
      .assign trans_name = "$t{elm.Mning}"
      .if ("${elm.Mning}" != trans_name)
${elm.Mning},${trans_name}
      .end if
    .else
      .if ("${elm.Mning}" != "")
${elm.Mning},
      .end if
    .end if
  .end for
  .//
.end function
.//
.//
.//============================================================================
.//
.// add OOA element text
.//
.//============================================================================
.function addOOAElementText
  .param string elm_name
  .param inst_ref_set elm_set
  .param boolean trans_flag
  .//

***** ${elm_name} Text Phrase *****
  .for each elm in elm_set
    .if (trans_flag)
      .assign trans_name = "$t{elm.Txt_Phrs}"
      .if ("${elm.Txt_Phrs}" != trans_name)
${elm.Txt_Phrs},${trans_name}
      .end if
    .else
      .if ("${elm.Txt_Phrs}" != "")
${elm.Txt_Phrs},
      .end if
    .end if
  .end for
  .//
.end function
.//
.//
.//============================================================================
.//
.// Add table contents. The trans_flag(boolean) can control output string.
.// If 'tfans_flag = TRUE', this is default, tabel contains translated string.
.// If 'trans_flag = FALSE', table does not contain translated string.
.//
.//============================================================================
.function addTableContents
  .assign trans_flag = TRUE
  .//
  .//--- Function ---
  .select many sync_set from instances of S_SYNC
  .invoke elm_table = addOOAElementName("Function", sync_set, trans_flag)
${elm_table.body}\
  .select many sparm_set from instances of S_SPARM
  .invoke elm_table = addOOAElementName("Function Parameter", sparm_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Bridge ---
  .select many brg_set from instances of S_BRG
  .invoke elm_table = addOOAElementName("Bridge", brg_set, trans_flag)
${elm_table.body}\
  .select many bparm_set from instances of S_BPARM
  .invoke elm_table = addOOAElementName("Bridge Parameter", bparm_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Data Type ---
  .select many dt_set from instances of S_DT
  .invoke elm_table = addOOAElementName("Date Type", dt_set, trans_flag)
${elm_table.body}\
  .//
  .// --- External Entity ---
  .select many ee_set from instances of S_EE
  .invoke elm_table = addOOAElementName("External Entity", ee_set, trans_flag)
${elm_table.body}\
  .invoke elm_table = addOOAElementKeyLetter("External Entity", ee_set, trans_flag)
${elm_table.body}\
  .select many eeevt_set from instances of S_EEEVT
  .invoke elm_table = addOOAElementEventLabel("External Entity", eeevt_set, trans_flag)
${elm_table.body}\
  .invoke elm_table = addOOAElementMeaning("External Entity Event", eeevt_set, trans_flag)
${elm_table.body}\
  .select many eeedi_set from instances of S_EEEDI
  .invoke elm_table = addOOAElementName("External Entity Data Item", eeedi_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Class ---
  .select many obj_set from instances of O_OBJ
  .invoke elm_table = addOOAElementName("Class", obj_set, trans_flag)
${elm_table.body}\
  .invoke elm_table = addOOAElementKeyLetter("Class", obj_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Attribute ---
  .select many attr_set from instances of O_ATTR
  .invoke elm_table = addOOAElementName("Class Attribute", attr_set, trans_flag)
${elm_table.body}\
  .invoke elm_table = addOOAElementPrefix("Class Attribute", attr_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Operation ---
  .select many tfr_set from instances of O_TFR
  .invoke elm_table = addOOAElementName("Operation", tfr_set, trans_flag)
${elm_table.body}\
  .select many tparm_set from instances of O_TPARM
  .invoke elm_table = addOOAElementName("Operation Parameter", tparm_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Relationship ---
  .select many part_set from instances of R_PART
  .invoke elm_table = addOOAElementText("Relationship(Simple Participant)", part_set, trans_flag)
${elm_table.body}\
  .select many form_set from instances of R_FORM
  .invoke elm_table = addOOAElementText("Relationship(Simple Formalizer)", form_set, trans_flag)
${elm_table.body}\
  .select many aone_set from instances of R_AONE
  .invoke elm_table = addOOAElementText("Relationship(Associated One Side)", aone_set, trans_flag)
${elm_table.body}\
  .select many aoth_set from instances of R_AOTH
  .invoke elm_table = addOOAElementText("Relationship(Associated Other Side)", aoth_set, trans_flag)
${elm_table.body}\
  .select many cone_set from instances of R_CONE
  .invoke elm_table = addOOAElementText("Relationship(Composition One Side)", cone_set, trans_flag)
${elm_table.body}\
  .select many coth_set from instances of R_COTH
  .invoke elm_table = addOOAElementText("Relationship(Composition Other Side)", coth_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Event ---
  .select many evtdi_set from instances of SM_EVTDI
  .invoke elm_table = addOOAElementName("Event Data Item", evtdi_set, trans_flag)
${elm_table.body}\
  .select many evt_set from instances of SM_EVT
  .invoke elm_table = addOOAElementEventLabel("", evt_set, trans_flag)
${elm_table.body}\
  .invoke elm_table = addOOAElementMeaning("Event", evt_set, trans_flag)
${elm_table.body}\
  .//
  .// --- State ---
  .select many state_set from instances of SM_STATE
  .invoke elm_table = addOOAElementName("State", state_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Subsystem ---
  .select many ss_set from instances of S_SS
  .invoke elm_table = addOOAElementName("Subsystem", ss_set, trans_flag)
${elm_table.body}\
  .invoke elm_table = addOOAElementPrefix("Subsystem", ss_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Enumeration ---
  .select many dt_set from instances of S_DT
  .invoke elm_table = addOOAElementEnumeration("Enumeration", dt_set, trans_flag)
${elm_table.body}\
  .//
  .// --- Enumerator ---
  .select many enum_set from instances of S_ENUM
  .invoke elm_table = addOOAElementName("Enumerator", enum_set, trans_flag)
${elm_table.body}\
  .//
.end function
.//
.//============================================================================
.//
.// Main
.//
.//============================================================================
.select any dom from instances of S_DOM
.invoke table_contents = addTableContents()
${table_contents.body}\

.invoke file_name = getFileName()
.emit to file "${file_name.result}"
