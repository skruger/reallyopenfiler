<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="xml" omit-xml-declaration="yes" encoding="UTF-8" indent="no"/>
	
	<xsl:template match="formfields">
		
		<xsl:apply-templates select="field" />
		
	</xsl:template>
		
	<xsl:template match="field">
		
		<xsl:if test="contains(@inputtype, 'radio')">
			<tr><td id="{@id}td" name="{@name}td"><xsl:value-of select="@label"/></td><td id="{@id}inputtd" name="{@name}inputtd">
				<xsl:for-each select="option">
					
					<input id="{@id}input" type="radio" name="{@name}input" value="{@value}"><xsl:if test="contains(@checked, 'true')"><xsl:attribute name="checked">true</xsl:attribute></xsl:if>
					<xsl:value-of select="@label"/></input>
				</xsl:for-each>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="contains(@inputtype, 'text')">
		<tr><td id="{@id}td" name="{@name}td"><xsl:value-of select="@label"/></td>
			<td id="{@id}inputtd" name="{@name}inputtd"><input id="{@id}input" name="{@name}input" value="{@value}" type="{@inputtype}"><xsl:attribute name="value"><xsl:value-of select="."/></xsl:attribute></input></td></tr>
		</xsl:if>
		<xsl:if test="contains(@inputtype, 'select')">
			<tr><td id="{@id}td" name="{@name}td"><xsl:value-of select="@label"/></td>
				<td id="{@id}inputtd" name="{@name}inputtd"><select id="{@id}input" name="{@name}input" type="{@inputtype}">
					<xsl:for-each select="option">
						<option name="{@name}" value="{@value}">
							<xsl:if test="contains(@disabled, 'true')">
								<xsl:attribute name="disabled">disabled</xsl:attribute>
								<xsl:attribute name="disabled" />
								<xsl:attribute name="style">background-color: pink;</xsl:attribute>
							</xsl:if>
							<xsl:if test="contains(@selected, 'selected')">
								<xsl:attribute name="selected">selected</xsl:attribute>
							</xsl:if>
						<xsl:value-of select="."/>
						</option>
					</xsl:for-each>
				</select></td></tr>	
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>

