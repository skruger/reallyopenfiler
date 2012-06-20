<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
<xsl:output method="xml" omit-xml-declaration="yes" encoding="UTF-8" indent="yes"/>


    <xsl:template match="/">
      
        <xsl:for-each select="aclentries/hostentry">
            <xsl:variable name="tableclass">
                <xsl:choose>
                    <xsl:when test="position() mod 2 = 0">
                        <xsl:text>color_table_row1</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>color_table_row2</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>
            <tr><td><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><xsl:value-of select="@id"/></td><td><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><xsl:value-of select="@iphostname"/></td><td><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><xsl:value-of select="@netmask"/></td>
                
                <td><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><input id="{@id}" value="accept" name="{@name}" type="radio"><xsl:if test="contains(@mode, 'accept')"><xsl:attribute name="checked">checked</xsl:attribute></xsl:if></input></td><td><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><input id="{@id}" value="reject" name="{@name}" type="radio"><xsl:if test="contains(@mode, 'reject')"><xsl:attribute name="checked">checked</xsl:attribute></xsl:if></input></td></tr>
        </xsl:for-each>
      
    </xsl:template>
    
</xsl:stylesheet>