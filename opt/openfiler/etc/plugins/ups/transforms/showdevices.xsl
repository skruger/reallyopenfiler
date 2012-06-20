<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="xml" omit-xml-declaration="yes" encoding="UTF-8" indent="yes"/>
    
    <xsl:template match="upsdevices">
        <html>
        <xsl:apply-templates select="ups" />
        </html>
    </xsl:template>
    
  
    
    <xsl:template match="ups">
        <tr id="{@id}" name="{@id}"><td align="center"><xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute><xsl:value-of select="@id"/></td><td><xsl:value-of select="@id"/></td>
            <td bgcolor="" align="center"><xsl:value-of select="@devicename"/></td><td bgcolor="" align="center"><xsl:value-of select="@port"/></td><td bgcolor="" align="center"><xsl:value-of select="@desc"/></td>
            <td bgcolor="" align="center"><xsl:value-of select="@sorder"/></td><td bgcolor="" align="center"><xsl:for-each select="extrasettings/extrasetting"><xsl:value-of select="@name"/><xsl:text> = </xsl:text><xsl:value-of select="@value"/><xsl:if test="not(position()=last())"><xsl:text>, </xsl:text></xsl:if></xsl:for-each></td><td bgcolor="" align="center"><xsl:value-of select="@enabled"/></td></tr>
    </xsl:template>
  
    
 </xsl:stylesheet>
