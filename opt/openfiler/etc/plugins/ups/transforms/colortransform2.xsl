<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="xml" omit-xml-declaration="yes" encoding="UTF-8" indent="no"/>
    
    
    
    <xsl:template match="html">
        <xsl:apply-templates select="tr"/>
    </xsl:template>
    
    <xsl:template match="tr">
    
   
            
            <xsl:choose>
                <xsl:when test="position() mod 2 = 0">
                    <tr  bgcolor="red" id="{@id}tr">   <xsl:call-template name="red"/></tr>   
          
                </xsl:when>
                <xsl:otherwise>
                    <tr  bgcolor="green" id="{@id}tr">   <xsl:call-template name="green"/></tr>   
                 
                </xsl:otherwise>
            </xsl:choose>     
  
    </xsl:template>
   
   
   
    <xsl:template name="green">
       
       <xsl:for-each select="td">
       
            <td style="" align="center">
                <xsl:attribute name="class">color_table_row2</xsl:attribute>
                <xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
                <xsl:choose>
                    <xsl:when test="position() = 1">
                        <a href="" onclick="addCurtain(); tinker_buildUPSEditForm('{@id}'); return false;"><xsl:attribute name="href">#</xsl:attribute><img src="/images/icons/edit_icon.png" height="16" width="16" alt="edit" /></a>
                       
                    </xsl:when>
                    <xsl:when test="position() = last()">
                        <xsl:variable name="ison" select="."/>  
                        
                        <xsl:if test="$ison = 1">
                            <img src="/images/icons/on_icon.png" height="16" width="16" alt="on" />
                        </xsl:if>
                        <xsl:if test="$ison = 0">
                            <img src="/images/icons/off_icon.png" height="16" width="16" alt="off" />
                        </xsl:if>
                        
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="."/>
                    </xsl:otherwise>
                </xsl:choose>
                </td>
               
       </xsl:for-each>
            
    </xsl:template>
 
    <xsl:template name="red">
        <xsl:for-each select="td">
           
            <td style="" align="center">
                <xsl:attribute name="class">color_table_row1</xsl:attribute>
                <xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
                <xsl:choose>
                    <xsl:when test="position() = 1">
                        <a href="" onclick="addCurtain(); tinker_buildUPSEditForm('{@id}'); return false;"><xsl:attribute name="href">#</xsl:attribute>
                       
                        <img src="/images/icons/edit_icon.png" height="16" width="16" alt="edit" /></a>
                    </xsl:when>
                    <xsl:when test="position() = last()">
                        <xsl:variable name="ison" select="."/>  
                       
                        <xsl:if test="$ison = 1">
                            <img src="/images/icons/on_icon.png" height="16" width="16" alt="on" />
                        </xsl:if>
                        <xsl:if test="$ison = 0">
                            <img src="/images/icons/off_icon.png" height="16" width="16" alt="off" />
                        </xsl:if>
                        
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="."/>
                    </xsl:otherwise>
                </xsl:choose>
            </td>
            
        </xsl:for-each>
        
    </xsl:template>
    
    
            
   
    
    
   
</xsl:stylesheet>

