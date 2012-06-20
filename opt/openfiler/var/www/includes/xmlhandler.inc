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


    
class XmlHandler extends DOMDocument {
    
    private $pathToFile;
    
    public function XmlHandler($pathToFile) {
	
	/*
	 *
	 * Creates DOMDocument object from xml file path
	 *
	 */
	
	parent::__construct(); 
	
	$this->pathToFile = $pathToFile;     
	$this->preserveWhiteSpace = false;
	
	if (is_file($pathToFile)) {
        
	    $this->load($pathToFile); 
	    $this->formatOutput = true;
	}
	
	else
	    return false; 
    }
    
    public function getDomObject() {
	
	return $this; 
	
    }
    
    public function saveDom() {
	
	$this->save($this->pathToFile);
	
    }
    
    
    public function getPathToFile() {
	
	return $this->pathToFile; 
	
    }
    
    public function runXpathQuery($xPath) {
	
	/*
	 * Returns DOMNodeList
	 */
	
	$xPathObj = new DOMXPath($this);
	return $xPathObj->query($xPath); 
	
    }
    
}

?>
