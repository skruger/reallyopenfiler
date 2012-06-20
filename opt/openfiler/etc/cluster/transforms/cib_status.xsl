<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Edited with XML Spy v2007 (http://www.altova.com) -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method='html' version='1.0' encoding='UTF-8' indent='yes'/>

<xsl:template match="status">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="lrm_resources/lrm_resource">
	<div style="margin-top: 2em; text-align: left;">
		<form action="">
			<fieldset>
				<legend><strong>Node cluster resource: 
					<xsl:value-of select="@id"/>
				</strong></legend>
				<div style="padding: 1em;">
				<p style="border-bottom: 3px solid #e8e8e8;"><strong>Resource Attributes</strong></p>
				<table cellpadding="8" cellspacing="2" border="0">
				<xsl:call-template name="resourceAttributes"/>
				</table>
 				<p style="border-bottom: 3px solid #e8e8e8;"><strong>Resource Operations</strong></p>
                                <xsl:call-template name="resourceOps"/>		
				</div>
			</fieldset>
		</form>
	</div>
</xsl:template>

<xsl:template name="resourceAttributes">
	<tr><td align="left" bgcolor="#e8e8e8" width="50%"><strong>Resource type</strong></td><td><xsl:value-of select="@type"/></td></tr>
        <tr><td align="left" bgcolor="#e8e8e8" width="50%"><strong>Resource class</strong></td><td><xsl:value-of select="@class"/></td></tr>
        <tr><td align="left" bgcolor="#e8e8e8" width="50%"><strong>Resource provider</strong></td><td><xsl:value-of select="@provider"/></td></tr>

</xsl:template>

<xsl:template name="resourceOps">
	<xsl:for-each select="lrm_rsc_op">
	<div class="sysinfo" style="margin-top: 1em;">
	<table class="box" cellpadding="8" cellspacing="2" border="0">
	<tr class="boxheader"><td class="boxheader" colspan="2">Resource operation ID: <xsl:value-of select="@id"/></td></tr>
	<tr class="boxbody"><td><strong>Operation key</strong></td><td><strong>Operation value</strong></td></tr>
	<tr class="boxbody"><td>operation</td><td><xsl:value-of select="@operation"/></td></tr>
        <tr class="boxbody"><td>op-status</td><td><xsl:value-of select="@op-status"/></td></tr>
        <tr class="boxbody"><td>rc-code</td><td><xsl:value-of select="@rc-code"/></td></tr>
        <tr class="boxbody"><td>last-run</td><td><xsl:value-of select="@last-run"/></td></tr>
        <tr class="boxbody"><td>last-rc-change </td><td><xsl:value-of select="@last-rc-change"/></td></tr>

	</table>
	</div>
	</xsl:for-each>
</xsl:template>

</xsl:stylesheet>

