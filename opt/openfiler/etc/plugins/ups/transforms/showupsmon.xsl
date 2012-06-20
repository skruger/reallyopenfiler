<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="xml" omit-xml-declaration="yes" encoding="UTF-8" indent="yes"/>
    
    
    
    <xsl:template match="/">
        <form id="upsmonentryform" name="upsmonentryform" action="/admin/services_ups.html" method="post">
            <table id="upsmonentrytable" name="upsmonentrytable" cellpadding="8" cellspacing="2" border="0">
                <tr><td class="color_table_heading">Entry</td><td class="color_table_heading">Username</td><td class="color_table_heading">Num. PSUs</td><td class="color_table_heading">Delete</td></tr>
                <xsl:for-each select="upsmonentries/upsmonentry">
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
                    <tr><td><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><xsl:value-of select="@ups"/><xsl:text>@</xsl:text><xsl:value-of select="@host"/></td><td align="center"><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><xsl:value-of select="@username"/></td><td align="center"><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><xsl:value-of select="@numpsus"/></td><td align="center"><xsl:attribute name="class"><xsl:copy-of select="$tableclass"/></xsl:attribute><input type="checkbox" name="{position()-1}" id="{position()-1}"/></td></tr>
                </xsl:for-each>
                <tr><td align="right" colspan="2"><input type="button" value="Submit" onclick="tinker_deleteLocalUPSMonEntries(tinkerAjax.getFormValues('upsmonentryform')); Effect.BlindUp('configdiv') "/></td><td align="left" colspan="2"><input type="button" value="Cancel" onclick="Effect.BlindUp('configdiv');"/></td></tr>
            </table>
        </form>
    </xsl:template>
    
    
    
   
    
</xsl:stylesheet>
