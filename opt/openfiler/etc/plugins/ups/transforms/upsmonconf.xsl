<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:output method="text" omit-xml-declaration="yes" encoding="UTF-8" indent="no"/>
    
    
<xsl:template match="/">
<xsl:for-each select="upsmonentries/upsmonentry">
<xsl:text>
MONITOR </xsl:text><xsl:value-of select="@ups"/><xsl:text>@</xsl:text><xsl:value-of select="@host"/><xsl:text> </xsl:text><xsl:value-of select="@numpsus"/><xsl:text> </xsl:text><xsl:value-of select="@username"/><xsl:text> </xsl:text><xsl:value-of select="@password"/><xsl:text> </xsl:text><xsl:value-of select="@mode"/>
<xsl:text>               
</xsl:text>
</xsl:for-each>
    <xsl:text>
MINSUPPLIES 1
SHUTDOWNCMD "/sbin/shutdown -h +0"
NOTIFYCMD /usr/sbin/upssched
POLLFREQ 5
POLLFREQALERT 5
HOSTSYNC 15
DEADTIME 15
POWERDOWNFLAG /etc/killpower
NOTIFYFLAG ONLINE SYSLOG+WALL+EXEC      
NOTIFYFLAG ONBATT SYSLOG+WALL+EXEC     
NOTIFYFLAG LOWBATT SYSLOG+WALL+EXEC     
NOTIFYFLAG COMMOK SYSLOG+WALL+EXEC      
NOTIFYFLAG COMMBAD SYSLOG+WALL+EXEC     
NOTIFYFLAG SHUTDOWN SYSLOG+WALL+EXEC    
NOTIFYFLAG REPLBATT SYSLOG+WALL+EXEC    
NOTIFYFLAG NOCOMM SYSLOG+WALL+EXEC      
NOTIFYFLAG FSD SYSLOG+WALL+EXEC         
RBWARNTIME 43200
NOCOMMWARNTIME 300
FINALDELAY 5      
    </xsl:text>
</xsl:template>
</xsl:stylesheet>