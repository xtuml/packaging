<?xml version="1.0"?>
<xsl:stylesheet version = "1.0"
xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"/>

<!--~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\>
<| (C) Copyright Mentor Graphics Corporation (2002-2005)             |>
<| All Rights Reserved.                                              |>
<\___________________________________________________________________/>
</~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\>
<| Title:               Instance Subsystem Extraction                |>
<| File Name:           ooa_xml_sql.xsl                              |>
<| Owner:               Cortland D. Starrett                         |>
<|                                                                   |>
<| Abstract -                                                        |>
<|   This stylesheet extracts and converts to SQL INSERTs the        |>
<|   objects in the OOA Instance subsystem.                          |>
<|                                                                   |>
<| Notes -                                                           |>
<|                                                                   |>
<|   Date       Modifier              Info                Change ID  |>
<|                                                                   |>
<|   02-Nov-21  Timothy Flechtner     create                 create  |>
<|     initial creation                                              |>
<|   02-Dec-16  Cortland Starrett                                    |>
<|     Slim down for use only with I_INS, I_LNK, I_AVL and I_SMC.    |>
<\__________________________________________________________________-->

<!-- Create the I_INS instance inserts -->
<xsl:template match = "instances/object">
	<xsl:text>INSERT INTO I_INS&#10;&#9;VALUES (</xsl:text>
	<xsl:value-of select = "@obj-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select = "@obj-inst-id"/>
	<xsl:text>);&#10;</xsl:text>

	<xsl:apply-templates/>
</xsl:template>

<!-- Create the I_AVL instance inserts -->
<xsl:template match = "attribute-value">
	<xsl:text>INSERT INTO I_AVL&#10;&#9;VALUES (</xsl:text>
	<xsl:value-of select = "@obj-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select = "@obj-inst-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select = "@attr-id"/>
	<xsl:text>,&#10;&#9;&#39;</xsl:text>
	<xsl:value-of select = "@value"/>
	<xsl:text>&#39;);&#10;</xsl:text>
</xsl:template>

<!-- Create the I_LNK instance inserts -->
<xsl:template match = "link">
	<xsl:text>INSERT INTO I_LNK&#10;&#9;VALUES (</xsl:text>
	<xsl:value-of select="@link-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@rel-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@from-obj-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@from-obj-inst-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@to-obj-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@to-obj-inst-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@assoc-obj-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@assoc-obj-inst-id"/>
	<xsl:text>,&#10;&#9;&#39;</xsl:text>
	<xsl:value-of select="@txt-phrs"/>
	<xsl:text>&#39;);&#10;</xsl:text>
</xsl:template>

<!-- Create the I_SMC instance inserts -->
<xsl:template match = "object-state-machine">
	<xsl:text>INSERT INTO I_SMC&#10;&#9;VALUES (</xsl:text>
	<xsl:value-of select="@obj-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@obj-inst-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@sm-id"/>
	<xsl:text>,&#10;&#9;</xsl:text>
	<xsl:value-of select="@smstt-id"/>
	<xsl:text>);&#10;</xsl:text>
</xsl:template>

<xsl:template match = "text() | @*">
</xsl:template>


</xsl:stylesheet>
