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



class Rsync extends XmlHandler {
    
    private $configfile = "/opt/openfiler/etc/rsync_settings.xml";
    private $rsyncDom;
    private $motdfilepath; 
    
    function Rsync() {
        
        $this->rsyncDom = new XmlHandler($this->configfile);
        $this->motdfilepath = $this->getPathToMotd(); 
    }
    
    
    public function getPathToMotd() {
        
        $xPath = "//rsync/motd";
        $motd = $this->rsyncDom->runXpathQuery($xPath)->item(0);
        return $motd->getAttribute('file');
        
    }
    
    public function getMotd() {
        
        $stringdata = ""; 
        $file = new File($this->motdfilepath);
    
        if ($file->Load()) {
            while (!$file->EOF()) {
                $stringdata .= $file->GetLine(); 
            }
        }
        
        $file->Save();
        
        return $stringdata; 
    }
    
    public function setMotd($motd) {
        
        $content = explode("\n", $motd);
        $file = new File($this->motdfilepath);
        
        $file->Clear();
        foreach($content as $key => $value) {
            
            $file->AddLine($value); 
            
        }
        
        $file->Save(); 
    }
    
    public function getPort() {
        
        $xPath = "//rsync/port";
        $port = $this->rsyncDom->runXpathQuery($xPath)->item(0);
        return $port->getAttribute('number'); 
        
    }
    
    public function setIPAddr($ipAddr) {
        
        $xPath = "//rsync/address";
        $ip = $this->rsyncDom->runXpathQuery($xPath)->item(0);
        return $ip->setAttribute('ip', $ipAddr); 
    }
    
    public function getIPAddr() {
        
        $xPath = "//rsync/address";
        $ip = $this->rsyncDom->runXpathQuery($xPath)->item(0);
        return $ip->getAttribute('ip'); 
        
    }
    
    public function setPort($portnumber) {
        
        $xPath = "//rsync/port";
        $port = $this->rsyncDom->runXpathQuery($xPath)->item(0);
        $port->setAttribute('number', $portnumber);
        $this->rsyncDom->saveDom(); 
    }
    
}

?> 
