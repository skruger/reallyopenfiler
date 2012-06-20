<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="xml" omit-xml-declaration="yes" encoding="UTF-8" indent="yes"/>
    
    <xsl:template match="upsdevices">
        <xsl:apply-templates select="ups" />
    </xsl:template>
    
  
    
    <xsl:template match="ups">
        <tr><td ><xsl:value-of select="@id"/></td><td><xsl:value-of select="@devicename"/></td>
            <td><xsl:value-of select='@id'/></td><td ><xsl:value-of select="@port"/></td><td ><xsl:value-of select="@desc"/></td>
            <td ><xsl:value-of select="@sorder"/></td><td ><xsl:for-each select="extrasettings/extrasetting"><xsl:value-of select="@name"/><xsl:value-of select="@value"/><xsl:if test="not(position()=last())"></xsl:if></xsl:for-each></td></tr>
    </xsl:template>
  
    
 </xsl:stylesheet>
