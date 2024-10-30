<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:exsl="http://exslt.org/common"
  extension-element-prefixes="exsl"
>
<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>

<xsl:variable name="ldsv-version-old" select="1328"/>
<xsl:variable name="ldsv-version-new" select="2130"/>

<xsl:template match="/SaveGame">
  <xsl:variable name="ldsv-version-act" select="(@ldsv-version | loadsave_version)[1]"/>
  <xsl:element name="lc-game">
    <xsl:if test="$ldsv-version-act != $ldsv-version-old">
      <xsl:message terminate="yes">error: expected load/save version '<xsl:value-of select="$ldsv-version-old"/>' but found '<xsl:value-of select="$ldsv-version-act"/>'</xsl:message>
    </xsl:if>
    <xsl:attribute name="ldsv-version">
      <xsl:value-of select="$ldsv-version-new"/>
    </xsl:attribute>
    <xsl:element name="globals">
      <xsl:apply-templates select="GlobalVariables/*" mode="copying"/>
    </xsl:element>
    <xsl:element name="map">
      <xsl:for-each select="ConstructionSection/* | MapTileSection/*">
        <xsl:variable name="resolved">
          <xsl:choose>
            <xsl:when test="/SaveGame/TemplateSection/Template/*[name() = name(current())]">
              <xsl:variable name="name">
                <xsl:choose>
                  <xsl:when test="name(..) = 'ConstructionSection'">
                    <xsl:value-of select="'Construction'"/>
                  </xsl:when>
                  <xsl:when test="name(..) = 'MapTileSection'">
                    <xsl:value-of select="'MapTile'"/>
                  </xsl:when>
                </xsl:choose>
              </xsl:variable>
              <xsl:element name="{$name}">
                <xsl:call-template name="resolve-template">
                  <xsl:with-param name="keys" select="/SaveGame/TemplateSection/Template/*[name() = name(current())]"/>
                  <xsl:with-param name="values" select="."/>
                </xsl:call-template>
              </xsl:element>
            </xsl:when>
            <xsl:otherwise>
              <xsl:message terminate="no">warning: element '<xsl:value-of select="name()"/>' does not match a template</xsl:message>
              <xsl:apply-templates select="." mode="copying"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:variable name="resolved-one" select="exsl:node-set($resolved)/*[1]"/>
        <xsl:element name="{name($resolved-one)}">
          <xsl:attribute name="group">
            <xsl:value-of select="($resolved-one/group | $resolved-one/Group)[1]"/>
          </xsl:attribute>
          <xsl:attribute name="map-x">
            <xsl:value-of select="$resolved-one/map_x"/>
          </xsl:attribute>
          <xsl:attribute name="map-y">
            <xsl:value-of select="$resolved-one/map_y"/>
          </xsl:attribute>
          <xsl:apply-templates select="$resolved-one/*[name() != 'group' and name() != 'Group' and name() != 'map_x' and name() != 'map_y']" mode="copying"/>
        </xsl:element>
      </xsl:for-each>
    </xsl:element>
  </xsl:element>
</xsl:template>

<xsl:template name="resolve-template">
  <xsl:param name="keys" select="''"/>
  <xsl:param name="values" select="''"/>
  <xsl:param name="delim" select="'&#x09;'"/>
  <xsl:if test="$keys">
    <xsl:element name="{substring-before($keys, $delim)}">
      <xsl:value-of select="substring-before($values, $delim)"/>
    </xsl:element>
    <xsl:call-template name="resolve-template">
      <xsl:with-param name="keys" select="substring-after($keys, $delim)"/>
      <xsl:with-param name="values" select="substring-after($values, $delim)"/>
      <xsl:with-param name="delim" select="$delim"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template match="//GlobalVariables/pbar/array" mode="copying">
  <xsl:element name="data">
    <xsl:apply-templates select="node()|@*" mode="copying"/>
  </xsl:element>
</xsl:template>

<xsl:template match="//int" mode="copying">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="//GlobalVariables/binary_mode" mode="copying"/>
<xsl:template match="//GlobalVariables/seed_compression" mode="copying"/>
<xsl:template match="//GlobalVariables/altered_tiles" mode="copying"/>
<xsl:template match="//GlobalVariables/constructions" mode="copying"/>
<xsl:template match="//GlobalVariables/monthgraph_size" mode="copying"/>

<xsl:template match="node()|@*" mode="copying">
  <xsl:copy>
    <xsl:apply-templates select="node()|@*" mode="copying"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="//Construction/Food" mode="copying">
  <xsl:element name="food"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Labor" mode="copying">
  <xsl:element name="labor"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Coal" mode="copying">
  <xsl:element name="coal"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Goods" mode="copying">
  <xsl:element name="goods"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Ore" mode="copying">
  <xsl:element name="ore"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Steel" mode="copying">
  <xsl:element name="steel"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Waste" mode="copying">
  <xsl:element name="waste"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Lo-Volt" mode="copying">
  <xsl:element name="lovolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Hi-Volt" mode="copying">
  <xsl:element name="hivolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Water" mode="copying">
  <xsl:element name="water"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/Jobs" mode="copying">
  <xsl:element name="labor"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/MWh" mode="copying">
  <xsl:element name="hivolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/kWh" mode="copying">
  <xsl:element name="lovolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>

<xsl:template match="//Construction/give_Food" mode="copying">
  <xsl:element name="give_food"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Labor" mode="copying">
  <xsl:element name="give_labor"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Coal" mode="copying">
  <xsl:element name="give_coal"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Goods" mode="copying">
  <xsl:element name="give_goods"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Ore" mode="copying">
  <xsl:element name="give_ore"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Steel" mode="copying">
  <xsl:element name="give_steel"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Waste" mode="copying">
  <xsl:element name="give_waste"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Lo-Volt" mode="copying">
  <xsl:element name="give_lovolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Hi-Volt" mode="copying">
  <xsl:element name="give_hivolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Water" mode="copying">
  <xsl:element name="give_water"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_Jobs" mode="copying">
  <xsl:element name="give_labor"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_MWh" mode="copying">
  <xsl:element name="give_hivolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/give_kWh" mode="copying">
  <xsl:element name="give_lovolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>

<xsl:template match="//Construction/take_Food" mode="copying">
  <xsl:element name="take_food"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Labor" mode="copying">
  <xsl:element name="take_labor"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Coal" mode="copying">
  <xsl:element name="take_coal"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Goods" mode="copying">
  <xsl:element name="take_goods"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Ore" mode="copying">
  <xsl:element name="take_ore"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Steel" mode="copying">
  <xsl:element name="take_steel"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Waste" mode="copying">
  <xsl:element name="take_waste"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Lo-Volt" mode="copying">
  <xsl:element name="take_lovolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Hi-Volt" mode="copying">
  <xsl:element name="take_hivolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Water" mode="copying">
  <xsl:element name="take_water"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_Jobs" mode="copying">
  <xsl:element name="take_labor"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_MWh" mode="copying">
  <xsl:element name="take_hivolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
<xsl:template match="//Construction/take_kWh" mode="copying">
  <xsl:element name="take_lovolt"><xsl:apply-templates select="node()|@*" mode="copying"/></xsl:element>
</xsl:template>
</xsl:stylesheet>
