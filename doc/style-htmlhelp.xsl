<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
  <xsl:import href="/usr/share/sgml/docbook/stylesheet/xsl/nwalsh/htmlhelp/htmlhelp.xsl" />
  
  <xsl:param name="admon.graphics.extension" select="'.png'"></xsl:param>
  <xsl:param name="admon.graphics" select="1"></xsl:param>
  <xsl:param name="admon.textlabel" select="0"></xsl:param>
  <xsl:param name="html.stylesheet">style.css</xsl:param>
  <xsl:param name="refentry.generate.title" select="1"></xsl:param>  
  <xsl:param name="refentry.generate.name" select="0"></xsl:param>
  
  <xsl:param name="navig.graphics" select="1"></xsl:param>
  <xsl:param name="navig.graphics.extension" select="'.png'"></xsl:param>
  <xsl:param name="navig.graphics.path">images/</xsl:param>
  
  <xsl:param name="htmlhelp.use.hhk" select="1"></xsl:param>

  <xsl:param name="htmlhelp.chm" select="'dbgen.chm'"></xsl:param>  
  
</xsl:stylesheet>

