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

include_once('pre.inc');
include_once('authenticated.inc');
require_once('service.inc');
define(CMD_CRM_MON, "/usr/bin/sudo /usr/sbin/crm_mon 2>&1");
define(CMD_CIBADMIN,"/usr/bin/sudo /usr/sbin/cibadmin 2>&1");
define(FILE_CIB_STATUS_XSL, "/opt/openfiler/etc/cluster/transforms/cib_status.xsl"); 

class ClusterManager {

	public $isRunning = FALSE;
	private $domdoc;
	private $xpath;
	
	function ClusterManager() {
		$sm = new ServiceManager();
		if ($sm->getService('corosync')->getStatus() === STATE_RUNNING) {
			$this->isRunning = TRUE;
		}
		
		if ($this->isRunning) {
			$this->load(); 
		}
		
	}

	
	private function readXMLConfig() {
		
		$content = "";
	        $retval = "";	
		ob_start();
		
		system(CMD_CIBADMIN . " -Ql", $retval);
		$content = ob_get_contents();
		ob_end_clean();
		if ($retval == 0)
			return $content;
		else 
			return NULL;
	}

	private function load() {
		
		$this->domdoc = new DOMDocument;
		$this->domdoc->ValidateOnParse = true;
		$this->domdoc->loadXML($this->readXMLConfig());	
		$this->xpath = new DOMXpath($this->domdoc); 
	}


	public function printStatus() {
	    	if ($this->isRunning) {
			$xsl = new XSLTProcessor(); 
			$xsl->importStyleSheet(DOMDocument::load(FILE_CIB_STATUS_XSL)); 
			$output = $xsl->transformToXML($this->domdoc);
			return $output;
		}
		else 
			return NULL;
	}

	

}

?>
