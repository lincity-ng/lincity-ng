<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ppl="https://github.com/lincity-ng/lincity-ng" version="1.0">
  <xsl:output indent="yes" encoding="UTF-8"/>
  <xsl:strip-space elements="*"/>

  <xsl:param name="version"/>
  <xsl:param name="date"/>

  <xsl:template match="/">
    <xsl:if test="not($version) or not($date)">
      <xsl:message terminate="yes">ERROR: Missing a version and/or date parameter; use --stringparam to supply them.</xsl:message>
    </xsl:if>
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="/component/releases">
    <xsl:copy>
      <!-- Only insert a new release if not already present -->
      <xsl:if test="not(release[@version=$version])">
        <release version="{$version}" date="{$date}"/>
      </xsl:if>
      <xsl:copy-of select="@*"/>
      <xsl:copy-of select="node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
