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


define('FILE_SERVICES_DESCRIPTION',
        "/opt/openfiler/etc/services_description.xml");
define('FILE_CLUSTER_XML', "/opt/openfiler/etc/cluster.xml");
define('CMD_SERVICE', "export LANG=C; /usr/bin/sudo /sbin/service "); 
define('CMD_CHKCONFIG', "export LANG=C; /usr/bin/sudo /sbin/chkconfig ");
define('CMD_PIDOF', "export LANG=C; /usr/bin/sudo /sbin/pidof -s ");
define('STATE_STOPPED', 0);
define('STATE_RUNNING', 1);
define('STATE_HUNG', 2);
define('STATE_PAUSED', 3);
include_once('xmlhandler.inc'); 


class ServiceManager {

    private static $clustermode = false;  // BOOL: whether clustering is enabled
    private  $allrunlevels;      // ARRAY: runlevels for all services
    private static $xmlDom; 
    public $services; 
        
    public function ServiceManager() {
        
               
        $this->xmlDom = new XmlHandler(FILE_SERVICES_DESCRIPTION);
 
        $this->services = array();
        self::tokenizeRunlevels();
        $this->getInitItems(); 
    }

    private function getInitItems() {
        
	$servicelist = array("smb","nfs","rsync","httpd","ldap",
                        "proftpd","iscsi-target","upsd",
                        "upsmon", "open-iscsi","acpid","scst","corosync");
        $handle = opendir("/etc/init.d/"); 
        while (false !== ($file = readdir($handle))) {
	
       //if ($file != "." && $file != ".." && $file != "killall" && $file != "status" && $file != "halt" && $file != "functions" && $file != "") {
        //	if ($file != "." && $file != ".." && array_search($file, $servicelist) != FALSE) { 
		if ($file != "." && $file != "..") {
			if (array_search($file, $servicelist) !== FALSE)
	       			array_push($this->services, new Service($file,$this->xmlDom,$this->allrunlevels[$file])); 
            }
        }
      
        closedir($handle); 
    }

    public function getService($name) {

        foreach($this->services as $service) {

            if($service->getName() == $name)
                return $service;  
        }
    }

    
    public static function getClusterMode() {
        
        if (is_file(FILE_CLUSTER_XML)) {
            
            $dom = new XmlHandler(FILE_CLUSTER_XML);
            $query = "//cluster/clustering";
            $element = $dom->runXpathQuery($query);
            $output = $element->item(0)->getAttribute("state");
            if (strcmp("on", $output) == 0)
                self::$clustermode = true;
            else
                self::$clustermode = false; 
        }
        
        return self::$clustermode; 
    }

    private static function getFHData($command) {
        
        $fh = popen($command . " 2>&1", "r");
        $i = 0;
        $data = array(); 
        
        while (!feof($fh))
            $data[$i++] = trim(fgets($fh, 4096));
            
        // remove EOL
        
        $EOL = array_pop($data); 
        
        pclose($fh);
        return $data;
    }



    private function tokenizeRunlevels () {
        
        $command = CMD_CHKCONFIG . " --list";
        $servicerunlevels = self::getFHData($command);
        $servicenames = array();
        $servicerl = array(); 
        
        foreach($servicerunlevels as $servicerunlevel) {
            
            // match service type (xinetd || standard)
            
            if (preg_match("/[A-Za-z0-9-_]+\s\d/", $servicerunlevel)) {
                
                $rlarray = array();
                $servicekeyval = explode("\t", $servicerunlevel);
                $servicename = trim(array_shift($servicekeyval));
                array_push($servicenames, $servicename); 
                
                foreach ($servicekeyval as $serviceval) {
                    
                    $trimmed = ltrim($serviceval, "0123456:");
                    array_push($rlarray, $trimmed); 
                }
              
                array_push($servicerl, $rlarray); 
            }     
        }
        
        $this->allrunlevels = array_combine($servicenames, $servicerl); 
    }   
    
    public static function getAllRunlevels() {
       return $this->allrunlevels; 
    }
    


}

class Service {
   
    private $name = "";         // STRING: name of service
    private $daemonname = "";   // STRING: name of daemon
    private $programpath = "";  // STRING: path to service executable binary
    public $runlevels;         // ARRAY: runlevels for the service
    private $isManaged = False; // BOOL: is this service managed by Openfiler? 
     
    private $pidfile;           // STRING: pid file for the service process
                                // (not all processes have a pid file)                        
    private $status = 0;        // INT: current state (running/stopped/paused
    private $ppid;              // STRING: parent pid for the process
    private $pids;              // ARRAY: all pids for the process
    private $subsysfile = "";   // STRING: var/lock/subsys entry
    private $functionlist;      // ARRAY: list of functions the service supports
    public $xmlDom; 
    private $metadescription = ""; 


    public function Service($servicename,$xmlDomDoc,$allrunlevels) {
    
        $this->runlevels = $allrunlevels;        
        $this->name = $servicename;
        $this->xmlDom = $xmlDomDoc; 
        $this->pids = array();
        $this->functionlist = array();
        $this->setMetaDescription(); 
         // TODO: write exception handlers

    }


    public function onBoot() {
        if ($this->runlevels[3] == "off")
            return false; 
        
        return true; 

    }

    
    public function getRunLevels() {
        return $this->runlevels;    
         
    }



    public function getMetaDescription(){

        return $this->metadescription; 
    }
    
    public function enable() {
  
        $cmd = CMD_CHKCONFIG . " --level 3 " . $this->name . " on";
        $run = $this->getExec($cmd); 
        
        if ($run["ret"] != 0)
            return $run["output"]; 

        return true; 
          
    
    }

    public function disable() {

        $cmd = CMD_CHKCONFIG . " --level 3  " . $this->name . " off";
        $run = $this->getExec($cmd); 
        
        if ($run["ret"] != 0)
            return $run["output"]; 

        return true; 


    }


    private function setMetaDescription() {
   	
        $xPath = "/services/service[@name='$this->name']/meta[@name='desc']";
        $domNodes = $this->xmlDom->runXpathQuery($xPath);
        $this->metadescription = $domNodes->item(0)->nodeValue; 
    }

    public function getDaemonName() {
        
        $xPath = "/services/service[@name='$this->name']/daemon";
        $domNodes = $this->xmlDom->runXpathQuery($xPath);

        foreach($domNodes as $item)
            $this->daemonname = $item->getAttribute('name');
        
        
        if(!empty($this->daemonname)) {
            $this->isManaged = True; 
            return $this->daemonname;
        }
        
        return $this->name; 
    }
    
    
    public function pause() {
        
        return $this->suspend(); 
    }
    
    
    public function suspend() {
    
        $command = CMD_SERVICE . " " . $this->name . " pause";
        $response = $this->getExec($command);
        
        if ($response["ret"] != 0) {
        
            return false;    
        
        }
        
        return true;
        
    }
    
    public function resume() {
        
        $command = CMD_SERVICE . " " . $this->name . " resume";
        $response = $this->getExec($command);
        
        if ($response["ret"] != 0) {
        
            return false;
            
        }
        
        return true;
        
    }
    
    public function start($output=False) {
        
        $command = CMD_SERVICE . " " . $this->name . " start";
        $response = $this->getExec($command);
        
        if ($response["ret"] != 0) {
            
            //TODO : unable to start, do something about it.
            /*foreach (($response["output"]) as $printout) // debug
                print $printout . "<br/>"; */

			if ($output) {
				return $response;
			}	 
			
            return false; 
        }
        
        /*foreach (($response["output"]) as $printout) // debug
            print $printout . "<br/>";*/
		if ($output) {
			return $response;
		}

        return true; 
    }
    
    public function stop() {
        
        $command = CMD_SERVICE . " " . $this->name . " stop";
        $response = $this->getExec($command);
        
        if ($response["ret"] != 0) {
            
            // TODO : unable to stop, do something about it.
            /*foreach (($response["output"]) as $printout) // debug
                print $printout . "<br/>"; */
            return false;
        }
             
        /*foreach (($response["output"]) as $printout) // debug
            print $printout . "<br/>"; */
        return true; 
    }
    
    public function restart() {
        
        if (in_array($this->functions, "restart")) {
            
            $command = CMD_SERVICE . " " . $this->name . " restart";
            $response = $this->getExec($command);
            
            if ($response["ret"] != 0) {
                
                /*foreach($response["output"] as $printout) // debug
                    print $printout . "<br/>"; */
                return false; 
            }
        }
        
        if (!$this->stop() || !$this->start())
            return false;
            
        return true; 
    }
    
    public function reload() {
    
        if(in_array($this->functions, "reload")) {
            
            $command = CMD_SERVICE . " ". $this->name . " reload";
            $response = $this->getExec($command);
            
            if ($response["ret"] != 0) {
                
                /*foreach ($response["output"] as $printout)
                    print($printout . "<br/>"); // debug */
                return false;
            }
        }
            
        if($this->restart())
            return true;
        
        return false;
    }
    
    public function setRunLevels($levels = array("3")) {
        
        $levelsstring = ""; 
        
        foreach($levels as $level)
            $levelsstring .= $level;
        
        if (!$this->clustermode) {
            
            $command = CHKCONFIG_COMMAND . " " . $this->name . " on";
            $response = $this->getExec($command);
            
            if ($response["ret"] != 0) {
                
                /*foreach ($response["output"] as $printout)
                    print($printout . "<br/>"); // debug */
                return false;
            }
            
            return true;
        }
        
        return true;
    }
    
    public function isRunning() {
        
        if($this->status == STATE_RUNNING || $this->status == STATE_PAUSED)
            return true;
        
        return false;
    }
    
    private static function getFHData($command) {
        
        $fh = popen($command . " 2>&1", "r");
        $i = 0;
        $data = array(); 
        
        while (!feof($fh))
            $data[$i++] = trim(fgets($fh, 4096));
            
        // remove EOL
        
        $EOL = array_pop($data); 
        
        pclose($fh);
        return $data;
    }
    
    private function getExec($command) {
        
        $returnData = array();
        
        exec($command, $output, $ret);
        
        $returnData["output"] = $output;
        $returnData["ret"] = $ret;
        
        return $returnData; 
    
    }
    
    public function getName() {
        
        return $this->name;
    }
    
    
    public function getPidFile() {
        
        $query = "//service[@name = '$this->name']/pidfile";
        if ($queryNode = $this->runXpathQuery($query))
            $this->pidfile = $queryNode->item(0)->getAttribute("path");
    
        return $this->pidfile;    
    }
    
    public function getStatus() {
    
        $cmd = CMD_SERVICE . " " . $this->name . " status";  
        $output = $this->getFHData($cmd);
        
        foreach($output as $line) {
            
            if (preg_match("/running/", $line) || preg_match("/status: OK/",$line)) {
                
                /* all your base are belong to us */
                
                $this->status = STATE_RUNNING;
                
                /* at least one component of the service is running
                   we now need to know whether other components of the
                   service have b0rked or if it is paused -- so continue
                   to the next loop item */
                
                continue;
               
            }
            
            if (preg_match("/paused/", $line)) {
                
                if ($this->status == STATE_RUNNING) {
                    $this->status = STATE_PAUSED;
                    break;   
                }
                
                /* hmm, we're paused but not running?
                  TODO: might want to combine this with the check for
                 "running" above */
            }
            
            if (preg_match("/but pid file exists/", $line) ||
                preg_match("/subsys locked/", $line)) {
                
                /* somebody set us up the bomb */
                
                $this->status = STATE_HUNG;
                
                /* game over, break out and return */
                
                break;
            }
            
            if (preg_match("/stopped/", $line)) {
                
                /* make your time */
                
                $this->status = STATE_STOPPED;
                
                /* game over, break out and return */
                
                break;
            }

	    if (preg_match("/Not loaded/", $line)) {
		// e.g qla2x00tgt scst etc

		$this->status = STATE_STOPPED;
		break;
            }
            
        }
        
        return $this->status;    
    }
    
    public function getPpid() {
        
        return $this->ppid;
    }
    
    public function getPids() {
    
        return $this->pids;    
    }
    
    public function getSubsysFile() {
    
        return $this->subsysfile;    
    }

    public function getPid() {
    
        return $this->ppid;     
    }
    
    
    public function getFunctions() {
        
        $query = "//services/service[@name='$this->name']";
        $serviceExists = $this->xmlDom->runXpathQuery($query);
        $query = "//services/service[@name='$this->name']/function";
        $serviceFunctions = $this->xmlDom->runXpathQuery($query);
       
               
        if ($serviceExists->length == 0 || $serviceFunctions->length == 0) {
            
            return false;
        }
        
        for ($i = 0; $i < $serviceFunctions->length; $i++) {
            $this->functionlist[] =
                   $serviceFunctions->item($i)->getAttribute("name"); 
        }
    
        //print_r($this->functionlist); //debug
        //print "<br/>"; //debug
        return $this->functionlist; 
    }
    
        
    
}






?>
