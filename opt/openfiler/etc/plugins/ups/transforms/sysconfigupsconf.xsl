<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="text" omit-xml-declaration="yes" encoding="UTF-8" indent="no"/>
    
    
<xsl:template match="/">
<xsl:for-each select="sysconfig/mode">
<xsl:text>SERVER=</xsl:text><xsl:value-of select="@server"/>
</xsl:for-each>
<xsl:text>
</xsl:text>
<xsl:text>MODEL=upsdrvctl
</xsl:text>
<xsl:for-each select="sysconfig/device">
    <xsl:choose>
      
    <xsl:when test="contains(@id, 'null')">
<xsl:text>#DEVICE=/dev/</xsl:text><xsl:value-of select="@id"/>
    </xsl:when>
        <xsl:otherwise>
<xsl:text>DEVICE=/dev/</xsl:text><xsl:value-of select="@id"/>
        </xsl:otherwise>
    </xsl:choose>

</xsl:for-each>
</xsl:template>
</xsl:stylesheet>