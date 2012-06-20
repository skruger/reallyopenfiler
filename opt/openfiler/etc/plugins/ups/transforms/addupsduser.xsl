<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="text" omit-xml-declaration="yes" encoding="UTF-8" indent="no"/>
    
    
<xsl:template match="/">
<xsl:for-each select="userentries/userentry">
<xsl:text>
[</xsl:text><xsl:value-of select="@id"/><xsl:text>]
</xsl:text>
    <xsl:text>    password = </xsl:text><xsl:value-of select="@password"/>
    <xsl:text> 
    </xsl:text>
    <xsl:text>allowfrom = </xsl:text><xsl:for-each select="hostaccess/allowfrom"><xsl:value-of select="@host"/><xsl:text> </xsl:text></xsl:for-each>
    <xsl:text> 
    </xsl:text>
    <xsl:text>instcmds = all</xsl:text>
    <xsl:text> 
    </xsl:text>
    <xsl:text>actions = all</xsl:text>
    <xsl:text>
    </xsl:text>
    <xsl:text>upsmon </xsl:text><xsl:value-of select="@mode"/>
    <xsl:text> 
                
    </xsl:text>
</xsl:for-each>
</xsl:template>
</xsl:stylesheet>