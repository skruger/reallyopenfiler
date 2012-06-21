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

define('CMD_SCSTADM', "export LANG=C; /usr/bin/sudo /usr/sbin/scstadmin 2>&1");
define('FILE_SCST_CONF',"/etc/scst.conf"); 
define('FILE_FC_STATUS_XSL',"/opt/openfiler/etc/scst/transforms/fc_status.xsl");
include_once('pre.inc');
require_once('service.inc'); 
require_once('lvm.inc'); 


class SCSTFC {

	private $targetDomNodeList;
	private $targetGroupDomNodeList;
	private $targetInitiatorDomNodelist;
	private $targetSessionDomNodeList; 
	private $domdoc;
	private $xpath;
	private $service_scst_status;
	public $isRunning;
	private static $configXMLDom;
	
	public function SCSTFC() {
		$sm = new ServiceManager(); 
	        $service_scst = $sm->getService("scst");
		$this->service_scst_status = $service_scst->getStatus();
		if ($this->service_scst_status  === STATE_RUNNING) {
			$this->isRunning = TRUE;
			$this->reset();	
		}
	}

	private function readXMLConfig() {
		
		$content = "";
	        $retval = "";	
		ob_start();
		
		system(CMD_SCSTADM . " -list_all -xml", $retval);
		$content = ob_get_contents();
		ob_end_clean();
		
		if ($retval == 0)
			return $content;
		else 
			return NULL;
	}

	public function printStatus() {
	    	if ($this->isRunning) {
			$xsl = new XSLTProcessor(); 
			$xsl->importStyleSheet(DOMDocument::load(FILE_FC_STATUS_XSL)); 
			$output = $xsl->transformToXML($this->domdoc);
			return $output;
		}
		else 
			return NULL;
	}
	

	private function load() {
		
		$this->domdoc = new DOMDocument;
		$this->domdoc->ValidateOnParse = true;
		$this->domdoc->loadXML($this->readXMLConfig());	
		$this->xpath = new DOMXpath($this->domdoc); 
	}


	public function reset() {
		$this->load(); 

	}

	/*

	public function getTargets() {
	
		$xquery = "//targets/target"; 
		$nodelist = $this->xpath->query($xquery);
		foreach ($nodelist as $node) {
			$childnodes = $node->childNodes;
			foreach($childnodes as $childnode) {
				
			}

		}
	
	} */


}

$x = new SCSTFC();
?>
