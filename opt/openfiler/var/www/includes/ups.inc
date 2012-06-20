<?php

/*
 *
 *
 * --------------------------------------------------------------------
 * Copyright (c) 2001 - 2011 Openfiler Project.
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


define('FILE_UPSDCONF', "/opt/openfiler/etc/plugins/ups/xml/upsdconf.xml");

class UPS {

	var $nodes;

	function UPS()
	{
		//UPS constructor
		return true;
	}

	function AddEntry($name, $network, $netmask)
	{
		$this->nodes[] = array("name"=>$name, "network"=>$network, "netmask"=>$netmask);
		return true;
	}

	function Save()
	{
		$file = new File(FILE_UPSDCONF);
		$file->AddLine("<?xml version=\"1.0\"?>");
		$file->AddLine("<aclentries>");

		foreach($this->nodes as $node)
		{
			$file->Addline(
				"\t<hostentry id=\"" . $node["name"] . "\" " .
				"name=\"" . $node["name"] . "\" " .
				"iphostname=\"" . $node["network"] . "\" " .
				"netmask=\"" . $node["netmask"] . "\" " .
				"mode=\"reject\"/>");
		}
		$file->AddLine("</aclentries>");
		$file->Save();
		return true;
	}
}


?>
