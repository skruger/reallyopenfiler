<?php



$ajax->registerFunction("/admin/services_ups.html", "buildACLConfigForm");
$ajax->registerFunction("/admin/services_ups.html", "updateUPSACL");
$ajax->registerFunction("/admin/services_ups.html", "buildUserManagerForm");
$ajax->registerFunction("/admin/services_ups.html", "addUPSDUser");
$ajax->registerFunction("/admin/services_ups.html", "buildLocalUPSMonForm");
$ajax->registerFunction("/admin/services_ups.html", "addLocalUPSMonEntry");
$ajax->registerFunction("/admin/services_ups.html", "deleteUPSDUserForm");
$ajax->registerFunction("/admin/services_ups.html", "deleteUPSDUsers");
$ajax->registerFunction("/admin/services_ups.html", "deleteLocalUPSMonForm");
$ajax->registerFunction("/admin/services_ups.html", "deleteLocalUPSMonEntries");
$ajax->registerFunction("/admin/services_ups.html", "buildServiceModeForm");
$ajax->registerFunction("/admin/services_ups.html", "buildServerModeForm");
$ajax->registerFunction("/admin/services_ups.html", "removeServerModeForm");
$ajax->registerFunction("/admin/services_ups.html", "setServiceMode");

function setServiceMode($formvalues)
{

	$objResponse = new tinkerAjaxResponse();

	 $mode;
	 $deviceentry;
	 $device;

	if (count($formvalues) == 1) // client mode
	{
		$mode = $formvalues[mode];
		$device = "null";
	}

	else if (count($formvalues) == 2) // servermode
	{

		$mode = $formvalues[mode];
		$deviceentry = $formvalues[upslist];

		// find device entry in ups.xml

		$upsfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";
		$upsdom = DOMDocument::load($upsfile);
		$xpathquery = "//upsdevices/ups[@id='$deviceentry']";
		$xpath = new DOMXpath($upsdom);
		$nodelist = $xpath->query($xpathquery);
		$device = $nodelist->item(0)->getAttribute('port');


	}

	// configure sysconfig_ups.xml

	$sysconfigupsfile = "/opt/openfiler/etc/plugins/ups/xml/sysconfig_ups.xml";
	$sysconfigupsdom = DOMDocument::load($sysconfigupsfile);
	$xpathquery = "//sysconfig/mode";
	$xpath = new DOMXpath($sysconfigupsdom);
	$nodelist = $xpath->query($xpathquery);
	$node = $nodelist->item(0);
	$node->setAttribute('server', $mode);

	$xpathquery = "//sysconfig/device";
	$nodelist = $xpath->query($xpathquery);
	$node = $nodelist->item(0);
	$node->setAttribute('id', $device);

	$sysconfigupsdom->save($sysconfigupsfile);

	// transform sysconfig_ups.xml

	$sysconfigupsdom = DOMDocument::load($sysconfigupsfile);
	$transformfile = "/opt/openfiler/etc/plugins/ups/transforms/sysconfigupsconf.xsl";
	$outfile = "/opt/openfiler/etc/plugins/ups/configs/generated_sysconfig_ups.conf";
	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($transformfile));
	$output = $xsl->transformToDoc($sysconfigupsdom);

	$output->save($outfile);

	$removeXMLDecl = new line_delete($outfile);
	$removeXMLDecl->rundelete();

	exec("/usr/bin/sudo /bin/cp $outfile /etc/sysconfig/ups", $output, $ret);
	if ($ret != 0)
		return $output; 
}


function removeServerModeForm()
{
	$objResponse = new tinkerAjaxResponse();

	$objResponse->addRemove("servermodetr");
	$objResponse->addAppend("servicemodetable", "innerHTML", "<tr id=\"servermodetr\"></tr>");

	return $objResponse->getXML();
}

function buildServerModeForm()
{
	$objResponse = new tinkerAjaxResponse();

	$upslist = array();

	// get list of UPS'

	$upsfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";
	$upsdom = DOMDocument::load($upsfile);

	$xpathquery = "//upsdevices/ups";
	$xpath = new DOMXpath($upsdom);
	$nodelist = $xpath->query($xpathquery);

	foreach ($nodelist as $node)
	{
		array_push($upslist, $node->getAttribute('id'));
	}

	$output = "<td class=\"color_table_row2\" colspan=\"2\"><strong>Select Primary UPS: </strong></td><td class=\"color_table_row1\">";
	$output .= "<select id=\"upslist\" name=\"upslist\">\n";

	foreach ($upslist as $key => $value)
	{
		$output .= "<option id=\"$value\" value=\"$value\">$value</option>\n";
	}

	$output .= "</select>";
	$output .= "</td>";

	$objResponse->addRemove("servermodetr");
	$objResponse->addAppend("servicemodetable", "innerHTML", "<tr id=\"servermodetr\"></tr>");
	$objResponse->addAppend("servermodetr", "innerHTML", $output);
	$objResponse->addScript("var obj = document.getElementById('modeyes'); obj.checked=true;");


	return $objResponse->getXML();
}

function buildServiceModeForm()
{
	$objResponse = new tinkerAjaxResponse();




	$formbegin = "<form id=\"servicemodeform\" name=\"servicemodeform\" action=\"/admin/services_ups.html\" method=\"post\"/>\n";
	$tablebegin = "<table id=\"servicemodetable\" name=\"servicemodetable\" cellspacing=\"2\" cellpadding=\"8\" border=\"0\">\n";
	$tableheading = "<tr><td></td><td class=\"color_table_heading\">Server</td><td class=\"color_table_heading\">Client</td></tr>\n";
	$formcont = "<tr><td class=\"color_table_row2\"><strong>Service Mode</strong></td><td class=\"color_table_row1\" align=\"center\"><input type=\"radio\" id=\"modeyes\" name=\"mode\" value=\"yes\" onclick=\"tinker_buildServerModeForm();\"/></td><td class=\"color_table_row1\" align=\"center\"><input type=\"radio\" name=\"mode\" id=\"modeno\" value=\"no\" checked=\"checked\" onclick=\"tinker_removeServerModeForm();\" /></td></tr>";
	$tableend = "</table>";
	$servermode = "<tr id=\"servermodetr\"></tr>";
	$formbuttons = "<table><tr><td><input type=\"button\" value=\"Submit\" onclick=\"tinker_setServiceMode(tinkerAjax.getFormValues('servicemodeform')); blindUp();\"/></td><td><input type=\"button\" value=\"Cancel\" onclick=\"blindUp();\"/></td></tr></table>";
	$formend = "</form>";

	$output = $formbegin . $tablebegin . $tableheading . $formcont . $servermode . $tableend . $formbuttons . $formend;
	$objResponse->addRemove("innerdiv");
	$objResponse->addAppend("configdiv", "innerHTML", "<div id=\"innerdiv\"></div>");
	$objResponse->addAppend("innerdiv", "innerHTML", $output);


	return $objResponse->getXML();
}

function deleteLocalUPSMonEntries($formvalues)
{

	$upsmonfile = "/opt/openfiler/etc/plugins/ups/xml/upsmonconf.xml";
	$upsmondom = DOMDocument::load($upsmonfile);

		$xpath = new DOMXpath($upsmondom);


		$deleted = FALSE;
		$counter = 1;

		foreach ($formvalues as $key => $value)
		{

			if ($deleted)
				$key -= $counter++;  //we've deleted an entry so the new position of the next key will have moved up by counter

			$xpathquery = "//upsmonentries/upsmonentry";
			$nodelist = $xpath->query($xpathquery);
			$nodedelete = $nodelist->item($key);
			$emptynode = $nodedelete->parentNode->removeChild($nodedelete);
			$deleted = TRUE;


		}

		$upsmondom->save($upsmonfile);
		createUPSMonConfigFile();
}

function deleteLocalUPSMonForm()
{

	$objResponse = new tinkerAjaxResponse();
	$upsdmonfile = "/opt/openfiler/etc/plugins/ups/xml/upsmonconf.xml";
	$upsdmondom = DOMDocument::load($upsdmonfile);
	$transformfile = "/opt/openfiler/etc/plugins/ups/transforms/showupsmon.xsl";

	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($transformfile));
	$output = $xsl->transformToXML($upsdmondom);
	$objResponse->addRemove("innerdiv");
	$objResponse->addAppend("configdiv", "innerHTML", "<div id=\"innerdiv\"></div>");
	$objResponse->addAppend("innerdiv", "innerHTML", $output);
	return $objResponse->getXML();

}

function deleteUPSDUsers($formvalues)
{
	$upsdusersfile = "/opt/openfiler/etc/plugins/ups/xml/upsdusers.xml";
	$upsdusersdom = DOMDocument::load($upsdusersfile);

		$xpath = new DOMXpath($upsdusersdom);

		foreach ($formvalues as $key => $value)
		{

			$xpathquery = "//userentries/userentry[@id='$key']";
			$nodelist = $xpath->query($xpathquery);
			$nodedelete = $nodelist->item(0);
			$emptynode = $nodedelete->parentNode->removeChild($nodedelete);

		}

		$upsdusersdom->save($upsdusersfile);
		createUPSDUserConfigFile();
}

function deleteUPSDUserForm() {

	$objResponse = new tinkerAjaxResponse();

	$upsdusersfile = "/opt/openfiler/etc/plugins/ups/xml/upsdusers.xml";
	$upsdconfdom = DOMDocument::load($upsdusersfile);
	$transformfile = "/opt/openfiler/etc/plugins/ups/transforms/showupsdusers.xsl";

	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument :: load($transformfile));
	$output = $xsl->transformToXML($upsdconfdom);
	$objResponse->addRemove("innerdiv");
	$objResponse->addAppend("configdiv", "innerHTML", "<div id=\"innerdiv\"></div>");
	$objResponse->addAppend("innerdiv", "innerHTML", $output);

	return $objResponse->getXML();

}

function createUPSMonConfigFile() {
	$upsmonconfigfile = "/opt/openfiler/etc/plugins/ups/xml/upsmonconf.xml";
	$outfile = "/opt/openfiler/etc/plugins/ups/configs/generated_upsmon_conf.conf";
	$upsmonconfdom = DOMDocument :: load($upsmonconfigfile);
	$transformfile = "/opt/openfiler/etc/plugins/ups/transforms/upsmonconf.xsl";

	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument :: load($transformfile));
	$output = $xsl->transformToDoc($upsmonconfdom);

	$output->save($outfile);

	$removeXMLDecl = new line_delete($outfile);
	$removeXMLDecl->rundelete();
	
	exec("/usr/bin/sudo /bin/cp $outfile /etc/ups/upsmon.conf", $output, $ret);
	if ($ret != 0)
		return $output; 

}

function addLocalUPSMonEntry($formvalues) {

	$upsdusersfile = "/opt/openfiler/etc/plugins/ups/xml/upsdusers.xml";
	$upsdconfdom = DOMDocument::load($upsdusersfile);

	$query = "//userentry[@id = '$formvalues[userinput]']";
	$xpath = new DOMXpath($upsdconfdom);
	$nodes = $xpath->query($query);

	$node = $nodes->item(0);
	$password = $node->getAttribute('password');

	$upsmonconfigfile = "/opt/openfiler/etc/plugins/ups/xml/upsmonconf.xml";
	$upsmonconfdom = DOMDocument :: load($upsmonconfigfile);
	$upsmonentry = $upsmonconfdom->createElement('upsmonentry');

	$upsmonentry->setAttribute('ups', "$formvalues[upsinput]");
	$upsmonentry->setAttribute('host', "localhost");
	$upsmonentry->setAttribute('username', "$formvalues[userinput]");
	$upsmonentry->setAttribute('numpsus', "$formvalues[numpsus]");
	$upsmonentry->setAttribute('password', "$password");
	$upsmonentry->setAttribute('mode', "master");

	$nodelist = $upsmonconfdom->getElementsByTagName('upsmonentries');
	$root = $nodelist->item(0);
	$root->appendChild($upsmonentry);
	$upsmonconfdom->saveXML($root);
	$upsmonconfdom->save($upsmonconfigfile);

	createUPSMonConfigFile();

}

function buildLocalUPSMonForm() {
	$objResponse = new tinkerAjaxResponse();

	//get list of local ups

	$upslist = array ();
	$upsfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";
	$upsfiledom = DOMDocument::load($upsfile);

	$query = "//ups[@enabled = '1']";
	$xpath = new DOMXpath($upsfiledom);
	$nodelist = $xpath->query($query);

	foreach ($nodelist as $node) {
		array_push($upslist, $node->getAttribute('id'));
	}

	// get list of upsd users

	$userlist = array ();
	$upsdusersfile = "/opt/openfiler/etc/plugins/ups/xml/upsdusers.xml";
	$upsdusersfiledom = DOMDocument::load($upsdusersfile);

	$query = "//userentry[@mode='master']";
	$xpath = new DOMXpath($upsdusersfiledom);
	$nodelist = $xpath->query($query);

	foreach ($nodelist as $node) {
		array_push($userlist, $node->getAttribute('id'));
	}

	$formbegin = "<form id=\"localupsmonform\" name=\"localupsmonform\" action=\"/admin/services_ups.html\" method=\"post\">\n";
	$formcont = "<tr><td class=\"color_table_row1\"><select id=\"upsinput\" name=\"upsinput\">\n";

	foreach ($upslist as $ups) {
		$formcont .= "<option id=\"$ups\" name=\"$ups\" value=\"$ups\">$ups</option>";
	}

	$formcont .= "</select></td><td class=\"color_table_row1\">Localhost</td><td class=\"color_table_row1\"><select id=\"userinput\" name=\"userinput\">";

	foreach ($userlist as $user) {
		$formcont .= "<option id=\"$user\" name=\"$user\" value=\"$user\">$user</option>";
	}

	$formcont .= "</select></td>";
	$formcont .= "<td class=\"color_table_row1\"><select name=\"numpsus\" id=\"numpsus\">";

	for ($i = 1; $i <= 4; $i ++) {
		$formcont .= "<option id=\"psu$i\" name=\"psu$i\" value=\"$i\">$i</option>";
	}

	$formcont .= "</select></td></tr>";
	$formcont .= "<tr><td colspan=\"2\" align=\"right\"><input type=\"button\" onclick=\"tinker_addLocalUPSMonEntry(tinkerAjax.getFormValues('localupsmonform')); blindUp();\" value=\"submit\"/></td><td colspan=\"2\" align=\"left\"><input type=\"button\" value=\"Cancel\" onclick=\"blindUp();\"/></td></tr>";
	$formend = "</form>";
	$tablebegin = "<table id=\"localupsmontable\" cellpadding=\"8\" cellspacing=\"2\" border=\"0\">";
	$tableheading = "<tr><td class=\"color_table_heading\" align=\"center\">Select UPS</td>";
	$tableheading .= "<td class=\"color_table_heading\">Hostname</td>";
	$tableheading .= "<td class=\"color_table_heading\">Select Username</td>";
	$tableheading .= "<td class=\"color_table_heading\">Select Num. PSUs</td>";
	$tableend = "</table>";

	$output = $formbegin.$tablebegin.$tableheading.$formcont.$tableend.$formend;

	$objResponse->addRemove("innerdiv");
	$objResponse->addAppend("configdiv", "innerHTML", "<div id=\"innerdiv\"></div>");
	$objResponse->addAppend("innerdiv", "innerHTML", $output);

	return $objResponse->getXML();
}

function createUPSDUserConfigFile() {
	$upsdusersconfigfile = "/opt/openfiler/etc/plugins/ups/xml/upsdusers.xml";
	$outfile = "/opt/openfiler/etc/plugins/ups/configs/generated_upsd_users_conf.conf";
	$upsdusersconfdom = DOMDocument::load($upsdusersconfigfile);
	$transformfile = "/opt/openfiler/etc/plugins/ups/transforms/addupsduser.xsl";

	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($transformfile));
	$output = $xsl->transformToDoc($upsdusersconfdom);

	$output->save($outfile);

	$removeXMLDecl = new line_delete($outfile);
	$removeXMLDecl->rundelete();

	exec("/usr/bin/sudo /bin/cp $outfile /etc/ups/upsd.users", $output, $ret);
	if ($ret != 0)
		return $output; 
}

function addUPSDUser($formvalues) {

	$unamepassmode = array_slice($formvalues, 0, 3);
	$allowedhosts = array_slice($formvalues, 3);

	$upsdusersconfigfile = "/opt/openfiler/etc/plugins/ups/xml/upsdusers.xml";

	$upsdomdocument = DOMDocument::load($upsdusersconfigfile);

	$userentry = $upsdomdocument->createElement('userentry');

	foreach ($unamepassmode as $key => $value) {
		switch ($key) {
			case "unameinput" :
				$val = "id";
				break;

			case "passwdinput" :
				$val = "password";
				break;

			case "mode" :
				$val = "mode";
				break;

		}

		$userentry->setAttribute($val, $value);
	}

	$hostaccess = $upsdomdocument->createElement('hostaccess');
	$userentry->appendChild($hostaccess);

	foreach ($allowedhosts as $key => $values) {
		$allowfrom = $upsdomdocument->createElement('allowfrom');
		$allowfrom->setAttribute("host", $key);
		$hostaccess->appendChild($allowfrom);

	}

	$nodelist = $upsdomdocument->getElementsByTagName('userentries');
	$root = $nodelist->item(0);
	$root->appendChild($userentry);
	$upsdomdocument->saveXML($root);
	$upsdomdocument->save($upsdusersconfigfile);

	createUPSDUserConfigFile();

}

function buildUserManagerForm() {

	$objResponse = new tinkerAjaxResponse();

	$formbegin = "<form id=\"upsusersform\" name=\"upsusersform\" action=\"/admin/services_ups.html\" method=\"post\">\n";
	$formcont = "<tr><td>Username: </td><td><input id=\"unameinput\" name=\"unameinput\" type=\"text\"/></td></tr>\n";
	$formcont .= "<tr><td>Password: </td><td><input id=\"passwdinput\" name=\"passwdinput\" type=\"text\"/></td></tr>\n";
	$formcont .= "<tr><td>UPSMon Mode: </td><td><select id=\"mode\" name=\"mode\"><option id=\"master\" value=\"master\">Master</option><option id=\"slave\" value=\"slave\">Slave</option></select></td></tr>\n";
	$formcont .= "<tr><td colspan=\"2\"><strong>Allow From Hosts</strong></td></tr>";
	$formend = "<tr><td align=\"right\"><input type=\"button\" onclick=\"tinker_addUPSDUser(tinkerAjax.getFormValues('upsusersform')); blindUp();\" value=\"submit\"/></td><td align=\"left\"><input type=\"button\" value=\"Cancel\" onclick=\"Effect.BlindUp('configdiv');\"/></td></tr>";
	$tablebegin = "<table id=\"upsuserstable\" cellpadding=\"8\" cellspacing=\"2\" border=\"0\">";

	$tableend = "</table>";

	// get list of configured ACL hosts

	$upsdconffile = "/opt/openfiler/etc/plugins/ups/xml/upsdconf.xml";
	$upsdconfdom = DOMDocument::load($upsdconffile);

	$query = "//hostentry[@mode = 'accept']"; // only show hosts that have accept ACL mode
	$xpath = new DOMXpath($upsdconfdom);
	$nodes = $xpath->query($query);

	foreach ($nodes as $node) {
		$name = $node->getAttribute("id");
		$formcont .= "<tr><td align=\"right\"><input type=\"checkbox\" name=\"$name\"/></td><td>$name</td></tr>";
	}

	$output = $formbegin.$tablebegin.$formcont.$tableend.$formend;
	$objResponse->addRemove("innerdiv");
	$objResponse->addAppend("configdiv", "innerHTML", "<div id=\"innerdiv\"></div>");
	$objResponse->addAppend("innerdiv", "innerHTML", $output);

	return $objResponse->getXML();

}

function updateUPSACL($formdata) {

	// open upsdconf.xml

	$upsdconffile = "/opt/openfiler/etc/plugins/ups/xml/upsdconf.xml";
	$upsdconfdom = DOMDocument::load($upsdconffile);

	foreach ($formdata as $key => $value) {
		$xpath = new DOMXpath($upsdconfdom);
		$query = "//hostentry[@id='$key']";
		$nodelist = $xpath->query($query);

		$node = $nodelist->item(0);
		$node->setAttribute("mode", $value);

	}

	$upsdconfdom->save($upsdconffile);
	createConfigFile();

}

function createConfigFile() {

	// open upsdconf.xml

	$upsdconffile = "/opt/openfiler/etc/plugins/ups/xml/upsdconf.xml";
	$upsdconfdom = DOMDocument::load($upsdconffile);

	$upsdconftransfile = "/opt/openfiler/etc/plugins/ups/transforms/upsdconf.xsl";
	$outfile = "/opt/openfiler/etc/plugins/ups/configs/generated_upsd_conf.conf";

	// run xsl transform and save config file

	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($upsdconftransfile));
	$config = $xsl->transformToDoc($upsdconfdom);
	$config->save($outfile);

	$removeXMLDecl = new line_delete($outfile);
	$removeXMLDecl->rundelete();

	exec("/usr/bin/sudo /bin/cp $outfile /etc/ups/upsd.conf", $output, $ret);
	if ($ret != 0)
		return $output; 

}

function buildACLConfigForm() {

	$objResponse = new tinkerAjaxResponse();

	$tablebegin = "<table id=\"devicesidtable\" cellpadding=\"8\" cellspacing=\"2\" border=\"0\">";
	$tableheading = "<tr><td class=\"color_table_heading\" align=\"center\">Name</td>";
	$tableheading .= "<td class=\"color_table_heading\">IP/Hostname</td>";
	$tableheading .= "<td class=\"color_table_heading\">Netmask</td>";
	$tableheading .= "<td class=\"color_table_heading\">Accept</td>";
	$tableheading .= "<td class=\"color_table_heading\">Reject</td></tr>";

	$tableend = "</table>";

	$upsdconffile = "/opt/openfiler/etc/plugins/ups/xml/upsdconf.xml";
	$transform = "/opt/openfiler/etc/plugins/ups/transforms/upsdconfform.xsl";
	$upsdconfdom = DOMDocument :: load($upsdconffile);

	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($transform));
	$output = $xsl->transformToDoc($upsdconfdom);

	//syslog(LOG_INFO, "".$output->saveXML());

	$output = $output->saveXML();
	$output2 = $tablebegin.$tableheading.$output.$tableend;

	$objResponse->addRemove("innerdiv");
	$objResponse->addAppend("configdiv", "innerHTML", "<div id=\"innerdiv\"></div>");

	$objResponse->addAppend("innerdiv", "innerHTML", "<form id=\"upsaclconfigform\" action=\"/admin/services_ups.html\" method=\"post\">".$output2."</form>");
	$objResponse->addAppend("devicesidtable", "innerHTML", "<tr><td align=\"right\" colspan=\"2\"><input type=\"button\" value=\"Submit\" onclick=\"Effect.BlindUp('configdiv'); tinker_updateUPSACL(tinkerAjax.getFormValues('upsaclconfigform'));\"/></td><td colspan=\"3\"><input type=\"button\" value=\"Cancel\" onclick=\"Effect.BlindUp('configdiv'); \"/></td></tr>");

	return $objResponse->getXML();

}



?>
