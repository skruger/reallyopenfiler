<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="xml" omit-xml-declaration="yes" encoding="UTF-8" indent="no"/>
    
    
    
    <xsl:template match="html">
        <xsl:apply-templates select="tr"/>
    </xsl:template>
    
    <xsl:template match="tr">
     
            <tr id="{@id}"><xsl:apply-templates select="td"/></tr>        
  
    </xsl:template>
   
    <xsl:template match="td">
       
       <xsl:choose>
           <xsl:when test="position() mod 2 = 0">
               <td><xsl:attribute name="bgcolor">red</xsl:attribute>
                   <xsl:value-of select="."/></td>
           </xsl:when>
           <xsl:otherwise>
               <td><xsl:attribute name="bgcolor">green</xsl:attribute>
               <xsl:value-of select="."/></td>
           </xsl:otherwise>
       </xsl:choose>
            
           
      
    </xsl:template>
   
    
    
   
</xsl:stylesheet>

