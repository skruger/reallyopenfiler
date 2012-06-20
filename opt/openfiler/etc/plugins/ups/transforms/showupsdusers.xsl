<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="xml" omit-xml-declaration="yes" encoding="UTF-8" indent="yes"/>
    
    
    
    <xsl:template match="/">
        <form id="upsduserentryform" name="upsduserentryform" action="/admin/services_ups.html" method="post">
            <table id="upsduserentrytable" name="upsduserentrytable" cellpadding="8" cellspacing="2" border="0">
                <tr><td class="color_table_heading">Username</td><td class="color_table_heading">Delete</td></tr>
                <xsl:for-each select="userentries/userentry">
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
                    <tr><td><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><xsl:value-of select="@id"/></td><td><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><input type="checkbox" name="{@id}" id="{@id}"/></td></tr>
                </xsl:for-each>
                <tr><td align="right"><input type="button" value="Submit" onclick="tinker_deleteUPSDUsers(tinkerAjax.getFormValues('upsduserentryform')); Effect.BlindUp('configdiv') "/></td><td align="left"><input type="button" value="Cancel" onclick="Effect.BlindUp('configdiv');"/></td></tr>
            </table>
        </form>
    </xsl:template>
    
    
    
   
    
</xsl:stylesheet>
