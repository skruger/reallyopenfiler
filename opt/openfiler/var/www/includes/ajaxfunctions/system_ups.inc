<?php




    $upsdevices_id = array();
    $upsdevices_name = array();
    $upsdevices_manufacturer = array();
    $upsdevices_driver = array();
    $upsdevicelist = array();
    $upsdevicedrivers = array();

    $lastupsdevice="";  			// the last device in the list of ups devices
    $numupsdevices = 0; 				// number of ups devices configured

    $myupsobject = new ups_utility();


    //find out whether we have ups devices and add the edit div

    $upsconfigfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";

    $upsdomdocument = DOMDocument::load($upsconfigfile);


    $upslistofdevs = $upsdomdocument->getElementsByTagName('ups');

    foreach($upslistofdevs as $upsdev)
    {
	$lastupsdevice = $upsdev->getAttribute('id');
	$numupsdevices++;
    }



    for ($i=0; $i < count($upsdevices_id); $i++)
    {
	$upsdevicename = '' . $upsdevices_manufacturer[$i] . ' - ' . $upsdevices_name[$i];
	array_push($upsdevicelist, $upsdevicename);

	$upsdevicedriver = $upsdevices_driver[$i];
	array_push($upsdevicedrivers, $upsdevicedriver);
    }



    $upsdevicelist_lowercase = array_map('strtolower', $upsdevicelist);
    array_multisort($upsdevicelist_lowercase, SORT_STRING, $upsdevicelist, $upsdevices_id);



    $ajax->registerFunction("/admin/system_ups.html", "buildConfigForm");
    $ajax->registerFunction("/admin/system_ups.html", "deleteForm");
    $ajax->registerFunction("/admin/system_ups.html", "addUPSDevice");
    $ajax->registerFunction("/admin/system_ups.html", "onLoadPrintTable");
    $ajax->registerFunction("/admin/system_ups.html", "buildUPSEditForm");
    $ajax->registerFunction("/admin/system_ups.html", "clearUPSConfigForm");
    $ajax->registerFunction("/admin/system_ups.html", "updateOrDeleteUPS");
    $ajax->registerFunction("/admin/system_ups.html", "clearCurtain");



    function clearCurtain()
    {

	$objResponse = new tinkerAjaxResponse();

	$objResponse->addRemove("curtain");

	return $objResponse->getXML();

    }

    function deleteForm($uireference)
    {

	$objResponse = new tinkerAjaxResponse();

	//replace the form with an empty div of the same ID
	$objResponse->addReplace("$uireference", "innerHTML", "" );
	return $objResponse->getXML();
    }



    function createUpsConfigFile()
    {

	$upsconfigfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";
	$transform = "/opt/openfiler/etc/plugins/ups/transforms/upsconffile.xsl";
	$outfile = "/opt/openfiler/etc/plugins/ups/configs/generated_ups_conf.conf";
	$upsdomdocument = DOMDocument::load($upsconfigfile);

	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($transform));
	$output = $xsl->transformToDoc(DOMDocument::load($upsconfigfile));


	$output->save($outfile);

	$removeXMLDecl = new line_delete($outfile);
	$removeXMLDecl->rundelete();
	exec("/usr/bin/sudo /bin/cp $outfile /etc/ups/ups.conf", $output, $ret);
	if ($ret != 0)
	    return $output; 


    }

    function addUPSDeviceEntry($upsdevicesformvalues)
    {

	$removebutton = array_pop($upsdevicesformvalues);  // remove the button from the array so we're left with data values
	$removecancelbutton = array_pop($upsdevicesformvalues);


	//xmlFromArray($upsdevicesformvalues);

	// Slice array to get default values {status, driver, configname/id, port, description, shutdown order}

	$defaultvalues = array_slice($upsdevicesformvalues, 0, 7);

	// Open the UPS devices file and append our data

	$upsconfigfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";

	$upsdomdocument = DOMDocument::load($upsconfigfile);

	$tmpdomdocument = new DOMDocument();
	// TBC, check to see whether there is an existing entry in the ups config file that clashes with the new entry

	// Create DOM element to add to ups config file

	$ups = $upsdomdocument->createElement('ups');

	foreach($defaultvalues as $key => $value)
	{
	    $val="";
	    switch ($key)
	    {
		case "devicenameinput":
			$val = "devicename";
			break;
		case "driverinput":
			$val = "driver";
			break;
		case "confignameinput":
			$val = "id";
			break;
		case "portinput":
			$val = "port";
			break;
		case "sorderinput":
			$val = "sorder";
			break;
		case "descinput":
			$val = "desc";
			break;
		case "upsstatusinput":
			$val = "enabled";
			break;
	    }

	    $ups->setAttribute($val, $value);
	}

	// set the device to enabled by default

	//$ups->setAttribute("enabled", "1");

	$extrasettings = $upsdomdocument->createElement('extrasettings');
	$ups->appendChild($extrasettings);

	// Create the extrasettings DOM elements

	$remainingvalues = array_slice($upsdevicesformvalues, 7);

	foreach($remainingvalues as $key => $value) // at least what's left of you :)
	{
	    $extrasetting = $upsdomdocument->createElement('extrasetting');
	    $extrasetting->setAttribute("name", substr($key, 0, -5)); 	  //substr removes the trailing "input" from the id/name
	    $extrasetting->setAttribute("value", $value);
	    $extrasettings->appendChild($extrasetting);

	}

	$nodelist = $upsdomdocument->getElementsByTagName('upsdevices');
	$root = $nodelist->item(0);
	$root->appendChild($ups);
	//$tmpdomdocument->appendChild($ups);
	$upsdomdocument->normalize();
	$upsdomdocument->saveXML($root);




	// find out the id of the last ups device

	global $lastupsdevice;
	global $numupsdevices;

	$upslistofdevs = $upsdomdocument->getElementsByTagName('ups');

	foreach($upslistofdevs as $upsdev)
	{
	    $lastupsdevice = $upsdev->getAttribute('id');
	    $numupsdevices++;
	}


	$upsdomdocument->save($upsconfigfile);

	createUpsConfigFile();

    }


    function openXMLDOMDocument($documentRef)
    {
	$tmpXMLDOMDocument = DOMDocument::load($documentRef);
	return $tmpXMLDOMDocument;
    }

    function checkValidity($upsdevicesformvalues)
    {
	// this function only needs to check that the ID and/or the COM port have not already been entered.
	// search for Port and ID in

	return TRUE;
    }



    function printTable()
    {

	$objResponse = new tinkerAjaxResponse();
// perhaps tableheading should move to the xsl stylesheet
	$tablebegin = "<table id=\"devicesidtable\" cellpadding=\"8\" cellspacing=\"2\" border=\"0\">" ;
	$tableheading = "<tr><td class=\"color_table_heading\" align=\"center\">Edit</td>" ;
	$tableheading .= "<td class=\"color_table_heading\">Config Name</td>";
	$tableheading .= "<td class=\"color_table_heading\">Device</td>";
	$tableheading .= "<td class=\"color_table_heading\">Port</td>";
	$tableheading .= "<td class=\"color_table_heading\">Description</td>";
	$tableheading .= "<td class=\"color_table_heading\">Shutdown Order</td>";
	$tableheading .= "<td class=\"color_table_heading\">Extra Settings</td>";
	$tableheading .= "<td class=\"color_table_heading\">Status</td>";
	$tableend = "</table>";

	// getHTML for table

	$output = "";
	//$output2 = new DOMDocument();
	$upsfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";
	$transform = "/opt/openfiler/etc/plugins/ups/transforms/showdevices.xsl";
	$colortransform = "/opt/openfiler/etc/plugins/ups/transforms/colortransform2.xsl";

	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($transform));
	$output = $xsl->transformToDoc(DOMDocument::load($upsfile));

	// apply table style information (bgcolor)

	$xsl2 = new XSLTProcessor();
	$xsl2->importStyleSheet(DOMDocument::load($colortransform));

	//$output2 = DOMDocument::loadXML($output);

	$output2 = $xsl2->transformToXML($output);





	$returndata = $tablebegin . $tableheading . $output2 . $tableend;

	return $returndata;
    }

    function onLoadPrintTable()
    {
	$objResponse = new tinkerAjaxResponse();
	$objResponse->addAppend("devicesiddiv", "innerHTML", "" . printTable());
	return $objResponse->getXML();
    }


    function clearUPSConfigForm()
    {

	$objResponse = new tinkerAjaxResponse();
	$objResponse->addRemove("upsconfigformform");
	$objResponse->addRemove("addupseditformdiv");
	//$objResponse->addRemove("upsconfigformdiv");
	$objResponse->addScript("document.getElementById('addupsdevicebutton').disabled = false;  document.getElementById('upsnameselect').disabled = false;");
	return $objResponse->getXML();

    }

    function addUPSDevice($upsdevicesformvalues)
    {


	$objResponse = new tinkerAjaxResponse();
	$devicearray = array();

	//Validate Entry (TBC)


	if (checkValidity($upsdevicesformvalues))
	{
	    // Clear the form
	    $objResponse->addRemove("upsconfigformform");
	    $objResponse->addRemove("addupseditformdiv");
	    $objResponse->addClear("upsconfigformdiv", "innerHTML");

	    $objResponse->addScript("document.getElementById('addupsdevicebutton').disabled = false;  document.getElementById('upsnameselect').disabled = false;");

	    // open ups config file



	    addUPSDeviceEntry($upsdevicesformvalues);
	    $objResponse->addRemove("devicesidtable");
	    $objResponse->addAppend("devicesiddiv", "innerHTML" , "" . printTable());

	    global $lastupsdevice;
	    $string = $lastupsdevice . "tr";

	    $objResponse->addScript("Fat.fade_element('$string')");

	}

	return $objResponse->getXML();


    }

    function deleteUPSNode($deviceid)
    {



	// this function deletes a UPS node from ups.xml
	$upsfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";
	$upsdomdocument = DOMDocument::load($upsfile);

	// find the node with an xpath query

	$xpath = new DOMXpath($upsdomdocument);
	$xpathquery = "//upsdevices/ups[@id='$deviceid']";
	$nodelist = $xpath->query($xpathquery);
	$nodedelete = $nodelist->item(0);
	$emptynode = $nodedelete->parentNode->removeChild($nodedelete);
	$upsdomdocument->save($upsfile);


    }


    function updateUPSNode($editformvalues, $deviceid)
    {

	$upsfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";
	$upsdomdocument = DOMDocument::load($upsfile);

	$xpath = new DOMXPath($upsdomdocument);
	$xpathquery = "//upsdevices/ups[@id='$deviceid']";
	$node = $xpath->query($xpathquery)->item(0);



	// remove the buttons from the array

	for ($i=0; $i<=2; $i++)
		$button = array_pop($editformvalues);

	foreach($editformvalues as $key => $value)
	{
	    $val="";
	    switch ($key)
	    {
		case "portinput":
		    $val = "port";
		    break;
		case "sorderinput":
		    $val = "sorder";
		    break;
		case "descinput":
		    $val = "desc";
		    break;
		case "upsstatusinput":
		    $val = "enabled";
		    break;
	    }

	    $node->setAttribute($val, $value);
	}

	$upsdomdocument->save($upsfile);

    }

    function updateOrDeleteUPS($editformvalues, $deviceid, $operation)
    {

	$objResponse = new tinkerAjaxResponse();

	$val = intval($operation);

	if ($val == 2)
	{
	    // update operation

	    updateUPSNode($editformvalues, $deviceid);
	    createUpsConfigFile();

	    $objResponse->addRemove("devicesidtable");
	    $objResponse->addAppend("devicesiddiv", "innerHTML", "" . printTable());

	}

	else if ($val == 1)
	{
		// delete operation

	    deleteUPSNode($deviceid);
	    createUpsConfigFile();

	    $objResponse->addRemove("devicesidtable");
	    $objResponse->addAppend("devicesiddiv", "innerHTML" , "" . printTable());
	}

	return $objResponse->getXML();
    }



    function buildUPSEditForm($deviceconfigid)
    {
    
    
	$objResponse = new tinkerAjaxResponse();
    
	$upstoeditdriverfile = "";
	$listocpis = array();
	$listofconfiguredids = array();
	$myport = "";
	$myid = $deviceconfigid;
    
	// find which driver configuration file to load. Search ups.xml for the device
    
	$upsfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";
	$localupsdevicelist = DOMDocument::load($upsfile);
    
	$founddriver = FALSE;
    
	// TODO: change the following construct to use xpath - much cleaner
    
	$localupslist = $localupsdevicelist->getElementsByTagName('ups');
    
	foreach($localupslist as $localups)
	{
	    $attribute_id = $localups->getAttribute('id');
	    if($attribute_id = $deviceconfigid)
	    {
		$upstoeditdriverfile = $localups->getAttribute('driver');
		break;
	    }
	}
    
	// find my id and port
    
	$xpath = new DOMXpath($localupsdevicelist);
	$xpathquery = "//upsdevices/ups[@id='$deviceconfigid']";
	$node = $xpath->query($xpathquery)->item(0);
    
	$myport = $node->getAttribute("port");
	$myname = $node->getAttribute("devicename");
	$mydesc = $node->getAttribute("desc");
	$mysorder = $node->getAttribute("sorder");
    
    
    
	foreach($localupslist as $localups)
	{
    
	    $attribute_id = $localups->getAttribute('id');
	    $attribute_port = $localups->getAttribute('port');


	    if($myid != $attribute_id)
	    array_push($listocpis, $attribute_id);

	    if($myport != $attribute_port)
	    array_push($listocpis, $attribute_port);

	}
    
    
    
	// read the form xml description file NOTE: move to utility class
    
    
    
    
	$output = "";
	$defaultform = "/opt/openfiler/etc/plugins/ups/xml/editdefaultform.xml";
	$defaultformxsl = "/opt/openfiler/etc/plugins/ups/transforms/defaultform.xsl";
	$driverform = "/opt/openfiler/etc/plugins/ups/devices/" . $upstoeditdriverfile . ".xml";
    
	//search for configured ports and id's and disable them from the list
    
    
    
    
    
    
     // find all configured ports and ids and disable them
	$defaultformdoc = DOMDocument::load($defaultform);
	$editxpath = DOMDocument::load($defaultform);
    
    
	$xpath = new DOMXpath($editxpath);
    
	foreach ($listocpis as $listocpi)
	{
		$xpathquery = "//option[@name='$listocpi']" ;
		$entry = $xpath->query($xpathquery);
		foreach($entry as $ent)
		{
			$ent->setAttribute("disabled", "true");
		}
	}
    
    
	// set enabled/disabled in the form based on the ups.xml setting
    
	$enabledordisabled = $node->getAttribute("enabled");
    
    
    
	if ($enabledordisabled == 1)
	{
		$endisquery = "//option[@id='upsenabled']";
		$entry = $xpath->query($endisquery);
		$entry->item(0)->setAttribute("checked", "true");
	}
    
	else if ($enabledordisabled == 0)
	{
		$endisquery = "//option[@id='upsdisabled']";
		$entry = $xpath->query($endisquery);
		$entry->item(0)->setAttribute("checked", "true");
	}
    
	// set the selected port in the form based on the ups.xml setting
    
	$portquery = "//option[@id='$myport']";
	$entry = $xpath->query($portquery);
	$entry->item(0)->setAttribute("selected", "selected");
    
	// set the description for the device in the form based on the ups.xml setting
    
    
	$descquery = "//field[@id='desc']";
	$entry = $xpath->query($descquery);
	$entry->item(0)->nodeValue = $mydesc;
    
	// set the shutdown order for the device in the form based on the ups.xml setting
    
	$sorderstring = "sorder" . $mysorder;
    
    
	$sorderquery = "//option[@id='$sorderstring']";
	$entry = $xpath->query($sorderquery);
	$entry->item(0)->setAttribute("selected", "selected");
    
    
	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($defaultformxsl));
    
	$default = $xsl->transformToXML($editxpath);
    
    
	$output = $default;
    
    
	$objResponse->addAppend("paneldata", "innerHTML", "<div id=\"addupseditformdiv\"><h4>" . $myname. "</h4>");
    
	$objResponse->addAppend("paneldata", "innerHTML", "<form id=\"upsconfigformform\" action=\"system_ups.html\" method=\"post\"></form>");
	$objResponse->addAppend("upsconfigformform", "innerHTML", "<h4>Editing: " . $myid . "</h4>");
	$objResponse->addAppend("upsconfigformform", "innerHTML", "<table id=\"configform-table\" name=\"configform-table\" cellspacing=\"2\" cellpadding=\"2\">" .  $output . "<tr><td id=\"configformsubmittd\"><input type=\"button\" id=\"configformsubmitbutton\" name=\"configformsubmitbutton\" value=\"Update\" onclick=\"clearScreen(document.getElementById('panelinfo')); tinker_updateOrDeleteUPS(tinkerAjax.getFormValues('upsconfigformform'),'$deviceconfigid', '2'); removeCurtain(); tinker_clearUPSConfigForm();\" /></td><td><input type=\"button\" id=\"configformdeletebutton\" name=\"configformdeletebutton\" value=\"Delete\" onclick=\"clearScreen(document.getElementById('panelinfo')); removeCurtain(); tinker_updateOrDeleteUPS(tinkerAjax.getFormValues('upsconfigformform'),'$deviceconfigid', 1); tinker_clearUPSConfigForm();\"/></td><td><input type=\"button\" id=\"configformcancelbutton\" name=\"configformcancelbutton\" value=\"Cancel\" onclick=\"clearScreen(document.getElementById('panelinfo')); tinker_clearUPSConfigForm(); removeCurtain();\"/></td></tr>");
	$objResponse->addAppend("upsconfigformform", "innerHTML", "</table></div> ");
	$objResponse->addScript("clearScreen(document.getElementById('panelinfo')); ");
    
    
    
	return $objResponse->getXML();
    
    }
    
    
    function buildConfigForm($deviceid, $mode)
    {
    
	$listocpis = array();
	$myid = $deviceid;
	$myport = "";
	global $upsdevicelist, $upsdevices_id, $upsdevicedrivers;

	$objResponse = new tinkerAjaxResponse();


	// find which driver configuration file to load

	$key = array_search($deviceid, $upsdevices_id);
	$driverfile = $upsdevicedrivers[$key];

	$devicekey = array_search($deviceid, $upsdevices_id);


	// read the form xml description file NOTE: move to utility class


	$output = "";
	$defaultform = "/opt/openfiler/etc/plugins/ups/xml/defaultform.xml";
	$defaultformxsl = "/opt/openfiler/etc/plugins/ups/transforms/defaultform.xsl";
	$driverform = "/opt/openfiler/etc/plugins/ups/devices/" . $driverfile . ".xml";
	$upsfile = "/opt/openfiler/etc/plugins/ups/devices/ups.xml";

	$localupsdevicelist = DOMDocument::load($upsfile);
	$localupslist = $localupsdevicelist->getElementsByTagName('ups');



	foreach($localupslist as $localups)
	{



	    $attribute_id = $localups->getAttribute('id');
	    $attribute_port = $localups->getAttribute('port');

	    if($myid != $attribute_id)
	    array_push($listocpis, $attribute_id);

	    if($myport != $attribute_port)
	    array_push($listocpis, $attribute_port);


	}

	$defaultformdoc = DOMDocument::load($defaultform);
	$editxpath = DOMDocument::load($defaultform);


	$xpath = new DOMXpath($editxpath);

	foreach ($listocpis as $listocpi)
	{
		$xpathquery = "//option[@name='$listocpi']" ;
		$entry = $xpath->query($xpathquery);
		foreach($entry as $ent)
		{
				$ent->setAttribute("disabled", "true");
		}
	}



	$xsl = new XSLTProcessor();
	$xsl->importStyleSheet(DOMDocument::load($defaultformxsl));
	$extra = $xsl->transformToXML(DOMDocument::load($driverform));
	$default = $xsl->transformToXML($editxpath);



	$output = $default . $extra;



	if ($mode = "build")
	{


	$objResponse->addAppend("paneldata", "innerHTML", "<div id=\"addupseditformdiv\"><h4>" . $upsdevicelist[$devicekey] . "</h4>");
	$objResponse->addAppend("paneldata", "innerHTML", "<form id=\"upsconfigformform\" action=\"system_ups.html\" method=\"post\"></form>");
	$objResponse->addAppend("upsconfigformform", "innerHTML","<input id=\"devicenameinput\" name=\"devicenameinput\" value=\"$upsdevicelist[$devicekey]\" type=\"hidden\"/>");
	$objResponse->addAppend("upsconfigformform", "innerHTML","<input id=\"driverinput\" name=\"driverinput\" value=\"$driverfile\" type=\"hidden\"/>");
	$objResponse->addAppend("upsconfigformform", "innerHTML", "<table id=\"configform-table\" name=\"configform-table\" cellspacing=\"2\" cellpadding=\"2\">" .  $output . "<tr><td id=\"configformsubmittd\"><input type=\"button\" id=\"configformsubmitbutton\" name=\"configformsubmitbutton\" value=\"Add Device\" onclick=\"clearScreen(document.getElementById('panelinfo')); removeCurtain(); tinker_addUPSDevice(tinkerAjax.getFormValues('upsconfigformform'))\"/></td><td><input type=\"button\" id=\"configformcancelbutton\" name=\"configformcancelbutton\" value=\"Cancel\" onclick=\"clearScreen(document.getElementById('panelinfo')); tinker_clearUPSConfigForm(); removeCurtain();\"/></td></tr>");
	$objResponse->addAppend("upsconfigformform", "innerHTML", "</table></div> ");
	$objResponse->addScript("clearScreen(document.getElementById('panelinfo')); ");


	}
	return $objResponse->getXML();
    
    }


?>
