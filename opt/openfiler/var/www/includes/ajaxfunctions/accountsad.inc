<?php


/*
 *
 *
 * --------------------------------------------------------------------
 * Copyright (c) 2001 - 2009 Openfiler Project.
 * --------------------------------------------------------------------
 *
 * Openfiler is an Open Source SAN/NAS Appliance Software Distribution
 *
 * This file is part of Openfiler.
 *
 * Openfiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Openfiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Openfiler.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * --------------------------------------------------------------------
 *
 *  
 */ 


$ajax->registerFunction("/admin/index.html", "addAdOption");
$ajax->registerFunction("/admin/index.html", "delAdOption"); 

function addAdOption($adoption) {

	$objResponse = new tinkerAjaxResponse(); 
	if ($adoption != "") {
		$handle = popen('/usr/bin/uuidgen', 'r'); 
		$key = trim(fgets($handle));
		fclose($handle); 
	
		$IDMAP_AD_FILE = "/opt/openfiler/etc/idmap_ad.xml"; 
		$idmap_adDoc = new XmlHandler($IDMAP_AD_FILE);
		
		$rootElement = $idmap_adDoc->getElementsByTagName("idmap_ad")->item(0); 
		$optionElement = $idmap_adDoc->createElement("option"); 
		$optionElement->setAttribute("key", $key);
		$optionElement->setAttribute("value", $adoption); 
		$rootElement->appendChild($optionElement);
		$idmap_adDoc->saveDom(); 


		$objResponse->addAppend("tabadidmapsync", "innerHTML","<tr id=\"tr-$key\"><td class=\"color_table_row1\" colspan=\"2\"><span>$adoption</span></td> " . 
				"<td class=\"color_table_row1\"><input type=\"button\" name=\"but-$adoption\" value=\"Delete\" onclick=\"tinker_delAdOption('$key')\" /></td></tr>"); 	


	}
	
	return $objResponse->getXML(); 
	
}


function delAdOption($adoptionKey) {

	$objResponse = new tinkerAjaxResponse(); 

	if ($adoptionKey != "") {

		$IDMAP_AD_FILE = "/opt/openfiler/etc/idmap_ad.xml"; 
		$idmap_adDoc = new XmlHandler($IDMAP_AD_FILE);
		$xPath = "//idmap_ad/option[@key='$adoptionKey']"; 
		$targets = $idmap_adDoc->runXpathQuery($xPath); 
		foreach ($targets as $target) {
			$nodedelete = $target->parentNode->removeChild($target); 
		}
		$idmap_adDoc->normalize();
		$idmap_adDoc->saveDom(); 
	$objResponse->addClear("tr-$adoptionKey", "innerHTML"); 	
	        
	}

	return $objResponse->getXML();



		
}



?>
