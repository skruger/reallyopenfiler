<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    <xsl:output method="html" omit-xml-declaration="yes" encoding="UTF-8" indent="no"/>
   
	 
        <xsl:template match="xml">
		<xsl:call-template name="Driver"/>
        </xsl:template>
        
	<xsl:template name="Driver">
		<xsl:for-each select="drivers/driver">
			<xsl:if test="@name='qla2x00t'">
				<xsl:call-template name="driverTargets"/>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="driverTargets">
		<xsl:for-each select="targets/target">
			<div style="margin-top: 2em;">
			<form action="">
			<fieldset>
			<legend><strong>Target:
			<xsl:value-of select="@name"/></strong>
			</legend>
			<div style="padding: 1em;">
                	<p style="border-bottom: 3px solid #e8e8e8;"><strong>Target Attributes</strong></p>
			<table cellpadding="8" cellspacing="2" border="0"><tr>
			<xsl:call-template name="driverTargetAttributes"/>
			</tr></table>
			</div>
			<div style="padding: 1em;">
			<p style="border-bottom: 3px solid #e8e8e8;"><strong>Target Groups</strong></p>
			<xsl:call-template name="driverTargetGroups"/>
			</div>
			<div style="padding: 1em;">
			<p style="border-bottom: 3px solid #e8e8e8;"><strong>Target Active Sessions</strong></p>
			<xsl:call-template name="driverTargetSessions"/>
			</div>
			</fieldset>
			</form>
			</div>
		</xsl:for-each>
	</xsl:template>
	
	<xsl:template name="driverTargetGroups">
		<xsl:for-each select="groups/group">
			<div class="sysinfo">
			<table cellspacing="2" cellpadding="8" border="0" width="100%" class="box">
			<tr class="boxheader"><td class="boxheader" colspan="3"><xsl:text>GROUP: </xsl:text><xsl:value-of select="@name"/></td></tr>
			<tr class="boxbody"><td colspan="3"><p style="border-bottom: 1px solid #e8e8e8;"><strong>Group LUN Disk Map</strong></p></td></tr>
			<tr class="boxbody"><td style="background: #e8e8e8;">LUN Id.</td><td style="background: #e8e8e8;">Mapped Device</td><td style="background: #e8e8e8;">Read Only</td></tr>
			<xsl:call-template name="driverTargetGroupLuns"/>
			<tr class="boxbody"><td colspan="3">
			<p style="border-bottom: 1px solid #e8e8e8"><strong>Group Initiators</strong></p>
			<table cellspacing="2" cellpadding="8" border="0">
			<xsl:call-template name="driverTargetGroupInitiators"/>
			</table>
			</td></tr>
			</table>
			</div>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="driverTargetGroupInitiators">
		<xsl:for-each select="initiators/initiator">
			<tr><td><xsl:value-of select="@name"/></td></tr>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="driverTargetSessions">
		<xsl:for-each select="sessions/session">
			<div style="margin-top: 1em;" class="sysinfo">
			<table class="box" border="0" width="100%">
                        <tr class="boxheader"><td class="boxheader" colspan="2"><strong><xsl:text>Session: </xsl:text><xsl:value-of select="@name"/></strong></td></tr>
			<tr class="boxbody"><td colspan="2"><p style="border-bottom: 1px solid #e8e8e8;"><strong>Session LUNs</strong></p></td></tr>
			<tr class="boxbody"><td style="background: #e8e8e8;"><strong>LUN</strong></td><td style="background: #e8e8e8;"><strong>Device</strong></td></tr>
			<xsl:call-template name="driverTargetSessionLuns"/>
			</table>
			</div>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="driverTargetSessionLuns">
		<xsl:for-each select="luns/lun">
			<tr class="boxbody"><td><xsl:value-of select="@id"/></td><td><xsl:value-of select="."/></td></tr>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="driverTargetGroupLuns">
		<xsl:for-each select="luns/lun">
			<tr class="boxbody"><td><xsl:value-of select="@id"/></td><td><xsl:value-of select="@device"/></td>
			<td>
			<xsl:choose>
				<xsl:when test="attributes/attribute[@name='read_only']=0">
			     <xsl:text>No</xsl:text>
				</xsl:when>
				<xsl:otherwise>
				<xsl:text>Yes</xsl:text>
				</xsl:otherwise>
			    
	                    </xsl:choose></td></tr>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="driverTargetGroupLunsLunAttributes">
		<xsl:for-each select="attributes/attribute">
			<xsl:if test="@name='read_only'">
			<td><xsl:value-of select="."/></td>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="driverTargetAttributes">
		<xsl:for-each select="attributes/attribute">
			<xsl:if test="@name != 'abort_isp'">
			<tr><td bgcolor="#ebebeb"><xsl:value-of select="@name"/></td><td><xsl:value-of select="."/></td></tr>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="driverAttributes">
		<xsl:for-each select="attributes/attribute">
			<xsl:if test="@name = 'abort_isp'">
			<td><xsl:value-of select="@name"/></td><td><xsl:value-of select="."/></td>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>
