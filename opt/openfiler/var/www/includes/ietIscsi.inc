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


define('CMD_IETADM', "export LANG=C; /usr/bin/sudo /usr/sbin/ietadm 2>&1");
define('CMD_ISCSI_TARGET_RESTART', "export LANG=C; /usr/bin/sudo /etc/init.d/iscsi-target restart 2>&1");
define('CMD_MV', "export LANG=C; /usr/bin/sudo /bin/mv 2>&1");
define('CMD_CP', "export LANG=C; /usr/bin/sudo /bin/cp 2>&1"); 
define('CMD_RM', "export LANG=C; /usr/bin/sudo /bin/rm 2>&1");
define('CMD_UUIDGEN', "export LANG=C; /usr/bin/sudo /usr/bin/uuidgen 2>&1");
define('FILE_IETD_CONF', "/etc/ietd.conf");
define('FILE_IETD_CONF_TMP',"/opt/openfiler/etc/iscsi/ietd.conf.tmp");
define('FILE_ISCSI_SETTINGS_XSL', "/opt/openfiler/etc/iscsi/transforms/iscsi_settings.xsl");
define('FILE_IET_SESSION_XML', "/proc/net/iet/session.xml");
define('FILE_IET_VOLUME_XML', "/proc/net/iet/volume.xml");
define('FILE_ISCSI_SETTINGS_XML', "/opt/openfiler/etc/iscsi/targets/iscsi_settings.xml");


require_once("service.inc");
require_once("lvm.inc"); 

class SerializableDomNode extends DOMDocument {
    
    private $domElement; 
    
    public function __construct() {
        parent::__construct('1.0', 'iso-8859-1');
        $this->preserveWhiteSpace = false;
        $this->formatOutput = true; 
    }
    
    public function createDomElement($elementName, $nodeAttrKeyValArray) {
        
        $domElement = $this->createElement($elementName);
        if (count($nodeAttrKeyValArray) > 0)
            foreach($nodeAttrKeyValArray as $key => $value) {
                $domElement->setAttribute($key, $value); 
        }
        
        return $domElement; 
    }
    
    public function getDomElement() {
        
        return $this->domElement; 
    }
}




class IetUser extends SerializableDomNode {
    
    protected $username = "";
    protected $password = "";
    protected $type = ""; 
    
    public function IetUser($username, $password, $type) {
        
        $this->username = $username;
        $this->password = $password;
        $this->type = $type; 
    }
    
    public function getCmdParams() {
        
        $params = "$this->type=$this->username,Password=$this->password";
        return $params; 
    }
    
    public function getUsername() {
        
        return $this->username; 
    }
    
    public function getPassword() {
        
        return $this->password;
    }
    
    public function getUserType() {
        
        return $this->type; 
    }
    
    public function getVars() {
        
        return get_object_vars($this); 
    }
    
    public function serialize() {
        
        parent::__construct();
        
        $rootElement = $this->createElement($this->type);
        $this->appendChild($rootElement); 
        
        foreach ($this->getVars() as $key => $value) {
            if ($key != "type") {
                $rootElement->setAttribute($key, $value);  
            }
        }
    
        return $rootElement; 
    }
}

class IetOutgoingUser extends IetUser {
    
    public function IetOutgoingUser($username, $password) {
        
        parent::IetUser($username, $password, "OutgoingUser"); 
        
    }
    
}

class IetIncomingUser extends IetUser {
    
    public function IetIncomingUser($username, $password) {
        
        parent::IetUser($username, $password, "IncomingUser");
        
    }
    
}



class IetLun extends SerializableDomNode {
    
    private $Id = "";
    private $Path = "";
    private $Type = "";
    private $ScsiId = "";
    private $ScsiSN = "";
    private $IOMode = "wt";
    
    public function IetLun($attrArray) {
        
        foreach ($attrArray as $attrName => $attr)
            $this->setAttr($attrName, $attr);
    }
    
    
    public function getAttr($AttrName) {
        
        return $this->$AttrName;
    
    }
    
    public function setAttr($AttrName, $Attr) {
        
        $this->$AttrName = $Attr;
        
    }
    
    public function getVars() {
        
        return get_object_vars($this); 
        
    }
    
    public function getCmdParams() {
        
        $attrArray = $this->getVars();
        $params = ""; 
        $commacounter = count($attrArray); 
        
        foreach($attrArray as $key => $value) {
            
            if ($key != "Id") {
                $params .= "$key=$value";
            
                if ($commacounter > 2) {
                   $params .= ",";
                   $commacounter--; 
                }
            }
            
        }
        
        return $params; 
    }
    
    public function serialize() {
        
        parent::__construct();
        
        $rootElement = $this->createElement("lun");
        
        foreach ($this->getVars() as $key => $value)
            $rootElement->setAttribute($key, $value); 

        return $rootElement; 
    
    }
    
}


class IetTarget extends SerializableDomNode {
    
    
    /*
     *
     * NB: if you add any non ietd.conf paramaters, be sure to check for
     * them below in addTarget and handle correctly since addTarget
     * uses get_object_vars to list ietd.conf parameters
     *
     */
    
    public $Name = "";
    public $Alias = "";
    public $HeaderDigest = "None";
    public $DataDigest = "None";
    public $MaxConnections = "1";
    public $InitialR2T = "Yes";
    public $ImmediateData = "No";
    public $MaxRecvDataSegmentLength = "131072";
    public $MaxXmitDataSegmentLength = "131072";
    public $MaxBurstLength = "262144";
    public $FirstBurstLength = "262144";
    public $DefaultTime2Wait = "2";
    public $DefaultTime2Retain = "20";
    public $MaxOutstandingR2T = "8";
    public $DataPDUInOrder = "Yes"; 
    public $DataSequenceInOrder = "Yes";
    public $ErrorRecoveryLevel = "0";
    public $Wthreads = "16";
    public $QueuedCommands = "32"; 

    public function IetTarget($attrArray) {
     
        foreach ($attrArray as $attrName => $attr)
            $this->setAttr($attrName, $attr); 
        
    }
 
    public function getAttr($AttrName) {
        
        return $this->$AttrName;
    
    }
    
    public function setAttr($AttrName, $Attr) {
        
        $this->$AttrName = $Attr;        
    }
    
    public function getCmdParams() {
        
        $attrArray = $this->getVars(); 
        $params = "";
        $commacounter = count($attrArray);
        
        foreach ($attrArray as $key => $value) {
            
            if ($key != "Name" && $key != "Alias") {
                $params .= "$key=$value";
            
                if ($commacounter > 3) {
                        $params .= ",";
                        $commacounter--; 
                }
            }
            
        }
        
        return $params; 
    }
    
    public function getLuns($IetIscsiObj) {
        
        /*
         *
         * Returns the luns for this target
         * TODO: Implement this and get rid of addTarget
         * function in IetIscsi and just use updateTarget
         * for addition/updating. 
         *
         */
        
    }
    
    public function addLun($ietLun) {
        
        /*
         * Adds an ietlun to this target.
         * TODO: Implement this and get rid
         * of addLunToTarget in IetIscsi.
         *
         */
        
    }
    
    public function getVars() {
        
        return get_object_vars($this); 
        
    }
    
    public function serialize() {
        
        $rootElement; 
        
        parent::__construct();
        
        foreach ($this->getVars() as $key => $value) {
            
            /* target (Name) is the root element */
            
            if ($key == "Name")
                $rootElement = $this->createDomElement("target", array("Name"=>$value));
            
            else {
                
                $element = $this->createDomElement($key, array("value"=>$value));
                $rootElement->appendChild($element); 
                 
            }
                    
        }
        
        $this->appendChild($rootElement);
        
        return $rootElement; 
    }

}

class IetIscsi {

    private $procSessionDom;
    public  $procVolumeDom;
    private $configDom;
    private $sessionTids =  array();
    private $sessionTnames = array(); 
    private $sessionTidsToTnames = array();
    private $sessionTargetDomNodeList;
    private $offlinemode = false;
    private $clustermode = false;
    
    public function writeConfigFile() {
        
        $transform = FILE_ISCSI_SETTINGS_XSL; 
        $domDoc = new DOMDocument(); 
        $xsl = new XSLTProcessor();
        
        $xsl->importStyleSheet(DOMDocument::load($transform));
        $outputXml = $xsl->transformToXML($this->configDom);
        $file = fopen(FILE_IETD_CONF_TMP, w);
        fwrite($file, $outputXml);
        fclose($file);
        
        $cmd = CMD_CP . " -f " . FILE_IETD_CONF_TMP . " " . FILE_IETD_CONF;
        exec($cmd, $output, $ret);
      
        
        if ($ret != 0)
            return $ret;
        
        
    }
    
    public function reset() {
        
        /*
         *
         * the next two statements won't work if iscsi-target is not running
         * TODO: do something about that ;)
         *
         */
    
        
        if (is_file(FILE_IET_SESSION_XML))          
            $this->procSessionDom = new XmlHandler(FILE_IET_SESSION_XML);
            
        if (is_file(FILE_IET_VOLUME_XML))
            $this->procVolumeDom = new XmlHandler(FILE_IET_VOLUME_XML);
            
        if (is_file(FILE_ISCSI_SETTINGS_XML))
            $this->configDom = new XmlHandler(FILE_ISCSI_SETTINGS_XML);
  
        if ($this->procSessionDom != NULL &&
            $this->procVolumeDom != NULL && $this->configDom != NULL) {
            
            if ($this->sessionTargetDomNodeList =
                $this->procSessionDom->getElementsByTagName("target")) {
            
                if ($this->sessionTargetDomNodeList !=
                    NULL & $this->sessionTargetDomNodeList->length > 0) {
                    foreach ($this->sessionTargetDomNodeList as
                             $sessionTargetDomNode) {
                        array_push($this->sessionTids,
                                   $sessionTargetDomNode->getAttribute("id"));
                        array_push($this->sessionTnames,
                                   $sessionTargetDomNode->getAttribute("name")); 
                    }
                
                    $this->sessionTidsToTnames =
                      array_combine($this->sessionTids, $this->sessionTnames);
                }
            
            }
         
            /* Write the config file out to ietd.conf */
            
            $this->writeConfigFile();
        }
        
    }
    
    public function IetIscsi() {
        $this->procSessionDom = NULL;
        $this->procVolumeDom = NULL;
        $this->configDom = NULL;
        $this->reset(); 
    
    }

    
    
    
    public function getNextTid() {
        
        /*
         *
         *  Returns int, next tid for use
         *
         */
        
        $nextTid = 1;
        
        foreach ($this->sessionTids as $key => $value) {
            
            if(intval($value) >= $nextTid)
                $nextTid = intval($value) + 1;
        }
        
        return $nextTid; 
        
    }
    
    public function getTargetLuns($targetIqn) {
        
        
        /*
         * Returns DOMNodeList containing all lun nodes
         * for $targetIqn
         *
         */
        
        $xPath = "//target[@name='$targetIqn']/lun";
        $targetLuns = $this->procVolumeDom->runXpathQuery($xPath);
        
        return $targetLuns; 
        
    }
    
    public function getTargetConfigLuns($targetIqn) {
        
        $xPath = "//target[@Name='$targetIqn']/lun";
        $targetConfigLuns = $this->configDom->runXpathQuery($xPath); 
        
        
        return $targetConfigLuns;
        
        
    }
    
    public function getLunTargets($lunPath) {
        
        
        /*
         * returns DomNodeList targets and child lun with
         * path=$lunPath
         *
         */
        
        $dom = $this->procVolumeDom->cloneNode(TRUE);
        
      
        $targets = $dom->getElementsByTagName("target");
        
        foreach($targets as $target) {
        
            $found = FALSE;
            $luns = $target->getElementsByTagName("lun");
            
            foreach ($luns as $lun) {
            
                if ($lun->getAttribute("path") == $lunPath) 
                    $found = TRUE;
            }
            
            if ($found)
                $newnode = $dom->getElementsByTagName("info")->item(0)->removeChild($target); 
        }
        
        return $dom->getElementsByTagName("info")->item(0)->childNodes;    
    }
    
    public function getTidtoLidMap($lunPath) {
        
        /*
         *
         *  returns array $tid=>$lid
         *
         */
        
        $mapping = array();
        
        $targetNodeList = $this->getLunTargets($lunPath);
        
        foreach ($targetNodeList as $targetNode) {
            
            
            $tid = $targetNode->getAttribute("id");
            
            if (count($targetNode->childNodes) > 1) {
            
                $lunNodeList = $targetNode->childNodes;
                
                
                
                foreach($lunNodeList as $lunNode) {
                    
                    if ($lunNode->getAttribute("path") == $lunPath) {
                        
                        $lid = $lunNode->getAttribute("number");
                        break;
                    }
                }
            
                $mapping[] = array($tid=>$lid); 
            
            }
        }
        
        return $mapping; 
    
    }
    
    
    
    
    public function getTargetConfigIncomingUsers($targetIqn) {
        
        $xPathIncomingUsers = "//target[@Name='$targetIqn']/IncomingUser";
        $incomingUsers = $this->configDom->runXpathQuery($xPathIncomingUsers);
        
        return $incomingUsers; 
        
    }
    
    public function getTargetConfigOutgoingUsers($targetIqn) {
        
        $xPathOutgoingUsers = "//target[@Name='$targetIqn']/OutgoingUser";
        $outgoingUsers = $this->configDom->runXpathQuery($xPathOutgoingUsers);
        
        return $outgoingUsers; 
        
    }
    
    public function getGlobalConfigIncomingUsers() {
        
        $xPath = "//globalsettings/IncomingUser";
        $incomingUsers = $this->configDom->runXpathQuery($xPath);
        
        return $incomingUsers; 
        
    }
    
    public function getGlobalConfigOutgoingUsers() {
        
        $xPath = "//globalsettings/OutgoingUser";
        $outgoingUsers = $this->configDom->runXpathQuery($xPath);
        
        return $outgoingUsers; 
        
    }
    
    
    public function getNextLunId($targetIqn) {
        
        /*
         * Returns int, next available lun id for
         * $targetIqn.
         *
         */
        
        
        $nextLunId = 0; 
        
        $targetLuns = $this->getTargetLuns($targetIqn); 
        
        if ($targetLuns) {
            foreach ($targetLuns as $targetLun) {
                $lunId = intval($targetLun->getAttribute("number"));
                if ($lunId >= $nextLunId)
                    $nextLunId = $lunId + 1;
            }
        }
    
        return $nextLunId;

    }
    
    public function getSessionTids() {
        
        return $this->sessionTids; 
        
    }
    
    public function getSessionTnames() {
        
        return $this->sessionTnames; 
        
    }
    
    public function updateTarget($targetObject) {
        
        $tname = $targetObject->getAttr("Name"); 
        $tid = $this->getTargetTid($tname); 
        
        $params = $targetObject->getCmdParams();             
        
        $cmd = CMD_IETADM . "  --op update --tid=$tid --params=$params";
        
        exec($cmd, $output, $ret);
        
        if ($ret != 0 )
            return $output;
        
        else {
            
            /*
             *
             * ietadm success, so add to iscsi_settings.xml
             *
             */
            
            
            /* Get serializable configuration object for this target */
            
            $targetNode = $targetObject->serialize();     
    
        
            /* Import the domNode into the current iscsi_settings.xml domDocument */
        
            $importedNode = $this->configDom->importNode($targetNode, true);
        
            /* Append the luns for this target to the importedNode */
        
            if ($targetConfigLuns = $this->getTargetConfigLuns($tname))
                foreach ($targetConfigLuns as $targetConfigLun)
                    $importedNode->appendChild($targetConfigLun);
                    
            /* Apppend the users for this target */
            

            /*  Append outgoing users for this target */
            
            if ($targetConfigUsers = $this->getTargetConfigOutgoingUsers($tname))
                foreach ($targetConfigUsers as $targetConfigUser)
                    $importedNode->appendChild($targetConfigUser); 
                    
            /* Append incoming users for this target */
            
            if ($targetConfigUsers = $this->getTargetConfigIncomingUsers($tname))
                foreach ($targetConfigUsers as $targetConfigUser)
                    $importedNode->appendChild($targetConfigUser);      
            
        
            /* Get the targetNode to replace */
            
            $oldNode = $this->getConfigTarget($tname); 
            
            /* Replace the existing targetNode with the new target */
            
            $oldTarget = $oldNode->parentNode->replaceChild($importedNode, $oldNode); 
        
            /* Save the iscsi_settings.xml configuration file */
            
            $this->configDom->saveDom();
            $this->reset(); 
            
            
        }
    }
    
    
    public function addTarget($ietTargetObject) {

        /*
         *
         * Runs ietadm and adds the target to ietd.conf if successfull
         *
         */
         
        $params = "";
        $tid = $this->getNextTid(); 
        
    
        $attrArray = get_object_vars($ietTargetObject);
        
        $targetNode = $this->configDom->createElement("target");
        $targetNode->setAttribute("Name", $attrArray["Name"]); 
        $commacounter = count($attrArray); 
        
        foreach($attrArray as $key => $value) {
            
            if($key == "Name") { 
                $targetNode->setAttribute("Name", $attrArray["Name"]);
            }
            
            else if($key != "Alias") {
                
                // Alias attribute doesn't seem to work currently
            
                $params .= $key . "=" . $value;
            
                if ($commacounter > 3) {
                    $params .= ",";
                    $commacounter--; 
                }
            
                $child = $this->configDom->createElement($key);
                $child->setAttribute("value", $value);
                $targetNode->appendChild($child); 
            }
        }
        
        
        $name = $attrArray["Name"];
        
        $cmd = CMD_IETADM . "  --op new --tid=$tid --params Name=$name";
        exec($cmd, $output, $ret);

        if ($ret != 0)
            return $output;          
        
        else {
            
            // add target to config file
            
            $targetsNode = $this->configDom->getElementsByTagName("targets")->item(0);
            $targetsNode->appendChild($targetNode);
    
            $this->configDom->saveDom();
            $this->reset(); 
            
            // target created so now lets udpate its params
            
            /* TODO: Think about using updateTarget instead
              (caveat, it rewrites the config file) */
            
            $cmd = CMD_IETADM . "  --op update --tid=$tid --params=$params";
            exec($cmd, $output, $ret);
            
            if ($ret != 0)
                return $output;
            
            
        }
        
    }
    
    public function getTargets() {
        
        /*
         *
         * Returns DOMNodeList of existing targets
         *
         */
        
        $xPath = "//target";
        $targetList = $this->procVolumeDom->runXpathQuery($xPath); 
        
        return $targetList; 
        
    }
    
    public function getTarget($targetIqn) {
        
        /*
         *
         * Returns a DOMNode target referenced by targetIqn
         *
         */
        
        $xPath = "//target[@name='$targetIqn']";
        $target = $this->procVolumeDom->runXpathQuery($xPath); 
        
        return $target->item(0); 
    }
    
    public function getConfigTargets() {
        
        /*
         *
         * Returns DOMNodeList of existing targets in iscsi_settings.xml
         *
         */
        
        $xPath = "//target";
        $targetList = $this->configDom->runXpathQuery($xPath); 
        
        return $targetList; 
        
    }
    
    public function getConfigTarget($targetIqn) {
        
        /*
         *
         * Returns a DOMNode target referenced by targetIqn
         * from iscsi_settings.xml
         *
         */
        
        $xPath = "//target[@Name='$targetIqn']";
        $target = $this->configDom->runXpathQuery($xPath)->item(0);
        
        return $target; 
    }
    
    public function getTargetTid($targetIqn) {
        
        return array_search($targetIqn, $this->sessionTidsToTnames); 
        
    }
    
    public function getSessionConnections($sessionId) {
        
        /*
         * Returns DOMNodelist containing all connection DOMNodes
         * for sessionId
         *
         * it's easier to use xpath for this
         *
         */
        
        $sessionConnections = NULL; 
        
        $xPath = "//session[@id='$sessionId']";
        $sessionNodeList = $this->procSessionDom->runXpathQuery($xPath);
        $sessionNode = $sessionNodeList->item(0); 
        $sessionConnections = $sessionNode->getElementsByTagName("connection");
        
        return $sessionConnections; 
        
    }
    
    public function getTargetSessions($targetName) {
        
        /*
         *
         * Returns DOMNodeList containing all session DOMNodes
         * for targetName or NULL if no sessions were found
         * for targetName 
         *
         */
        
        
        $node = NULL;   
        $sessions = NULL; 
        
        foreach ($this->sessionTargetDomNodeList as $sessionTargetDomNode)
            if ($sessionTargetDomNode->getAttribute("name") == $targetName) {
                $node = $sessionTargetDomNode;
                break;
            }
        
        if ($node)
            $sessions = $node->getElementsByTagName("session"); 
        

        return $sessions;
        
    }
    
    public function delSessionsFromTarget($targetName) {
        
        /*
         *
         * Delete all sessions from target
         *
         */
        

        $targetSessions = $this->getTargetSessions($targetName);
        
        /*
         *
         * Session is automatically deleted after there are no more connections.
         * IET currently supports only a single connection per session
         * 
         *
         */
        
       
        /* Search for the tid using the targetName as needle */
        
        $tid = array_search($targetName,$this->sessionTidsToTnames); 
        
        if ($targetSessions) {
        
            foreach ($targetSessions as $targetSession) {
                
                $sid = $targetSession->getAttribute("id");
                
                /* Get connections for this session */
                
                $sessionConnections = $this->getSessionConnections($sid);
                
                if ($sessionConnections) {
                
                    foreach($sessionConnections as $sessionConnection) {
                        $cid = $sessionConnection->getAttribute("id"); 
                        $cmd = CMD_IETADM . "  --op delete --tid=$tid --sid=$sid --cid=$cid"; 
                    
                        exec($cmd, $output, $ret);
                        
                        if ($ret != 0)
                            return $output; 
                        
                    }
                }
                
                else {
                
                    /* No connections found for this session, so delete the session */
                    
                    $cmd = CMD_IETADM . "  --op delete --tid=$tid --sid=$sid";
                    
                    exec($cmd, $output, $ret);
                    
                    if ($ret != 0)
                        return $output;
                }
            }
        }
    }
    
    
             
    public function delLunsFromTarget($targetIqn) {  
        
        /*
         *  Delete from config file after successfully removing
         *  with ietadm.
         *
         *  TODO: add exception handling
         */
         
        
        /* Run xpath query for target name in iscsi_settings.xml */
        
        $domNodeConfig = $this->getConfigTarget($targetIqn);             
       
        
        /* Run xpath query for target name in /proc/net/iet/volumes.xml */
        
        $domNodeProc = $this->getTarget($targetIqn); 
        
        
        /* Get targetId for use with ietadm command */
        
        $tid = $domNodeProc->getAttribute("id");

        /* Get target luns from config file and from proc/net/iet/volumes.xml */
        
        $lunConfig = $domNodeConfig->getElementsByTagName("lun");
        $lunProc = $domNodeProc->getElementsByTagName("lun");
        
        /* Loop through all luns for this target and remove */
        
        foreach ($lunProc as $lunProcElement) {
            
            $lunProcId = $lunProcElement->getAttribute("number");
            $lunPath = $lunProcElement->getAttribute("path");

            /* Delete lun with ietadm */
            
            $cmd = CMD_IETADM . "  --op delete --tid=$tid --lun=$lunProcId"; 

            exec($cmd, $output, $ret);
        
            if ($ret != 0)
                return $output;
            
            else {
               
               /*
                * Successfully deleted with ietadm, so now remove from iscsi_settings.xml
                * using Path/path as matching attribute (path is as unique as any other attr)
                */
               
                foreach ($lunConfig as $lunConfigElement) 
                    if ($lunConfigElement->getAttribute("Path") == $lunPath)  
                        $nodedelete = $lunConfigElement->parentNode->removeChild($lunConfigElement);
            }
        }
    
        /* Save the iscsi_settings.xml document */
    
        $this->configDom->saveDom();
        $this->reset(); 
    
    }
    
    public function delTarget($targetIqn) {
        
        $this->delSessionsFromTarget($targetIqn);
        
        $domNodeConfig = $this->getConfigTarget($targetIqn);
        $domNodeProc = $this->getTarget($targetIqn); 
        
        
        $tid = $domNodeProc->getAttribute("id");
        
        $cmd = CMD_IETADM . "  --op delete --tid=$tid";
        
        exec($cmd, $output, $ret);
        
        if ($ret != 0)
            return $output;
        
        else {
            
            /* ietadm succeeded, delete from iscsi_settings.xml */
            
            $nodedelete = $domNodeConfig->parentNode->removeChild($domNodeConfig);
            
        }
        
        $this->configDom->saveDom();
        $this->reset(); 
        
        
        /* check whether acls exist and delete */
        
        if (is_dir("/opt/openfiler/etc/iscsi/$targetIqn")) {
            $cmd = "export LANG=C; /usr/bin/sudo /bin/rm -rf /opt/openfiler/etc/iscsi/$targetIqn"; 
            exec($cmd, $output, $ret);
            
            if ($ret != 0)
                return $output; 
        }
        
    }
    
    
    
    public function addLunToTarget($lunObject, $targetIqn, $updateConfig = TRUE) {
    
        $params = $lunObject->getCmdParams();
        $tid = array_search($targetIqn, $this->sessionTidsToTnames);
        $lunId = $lunObject->getAttr("Id"); 
        
        $cmd = CMD_IETADM . "  --op new --tid=$tid --lun=$lunId --params $params";
        
        exec($cmd, $output, $ret);
        
        if ($ret != 0)
            return $output;
        
        else if ($updateConfig) {
            
            /* successfully added lun to target, serialize. */
            
            $target; 
            
            $lun = $lunObject->serialize();
            $configTargets = $this->getConfigTargets();
            
            foreach ($configTargets as $configTarget) {
                
                $foundTarget = $configTarget->getAttribute("Name");
                if ($foundTarget == $targetIqn)
                    $target = $configTarget;
                
            }
            
            /* Import the lun into the dom document */
            
            $importedLunNode = $this->configDom->importNode($lun, true);
            
            /* Add lun to target */
            
            $target->appendChild($importedLunNode);
            
            /* Save the file */
            
            $this->configDom->saveDom();
            $this->reset(); 
            
        }
    }
    
    public function delLunFromTarget($lunPath, $targetIqn, $resize=false) {
        
        /* resize is false by default. We only need it if we're calling
           in order to resize a lun and not actually change the lun
           configuration */
        
        $configTarget = $this->getConfigTarget($targetIqn);
        $procTarget = $this->getTarget($targetIqn);
        
        $tid = $procTarget->getAttribute("id");
        $targetLuns = $this->getTargetLuns($targetIqn);
        
        foreach ($targetLuns as $targetLun)
            if ($targetLun->getAttribute("path") == $lunPath)
                $lunId = $targetLun->getAttribute("number"); 
    
        
        $cmd = CMD_IETADM . "  --op delete --tid=$tid --lun=$lunId";
        
        exec($cmd, $output, $ret);
        
        if($ret != 0)
            return $output;
        
        else if (!$resize) {  /* we're just resizing so no need
                                to change config */
            
            /* ietadm success. Remove from iscsi_settings.xml */
            
            $targetConfigLuns = $this->getTargetConfigLuns($targetIqn);
            foreach ($targetConfigLuns as $targetConfigLun)
                if ($targetConfigLun->getAttribute("Path") == $lunPath)
                    $oldnode = $configTarget->removeChild($targetConfigLun); 
        
            
            /* Save the config file */
        
            $this->configDom->saveDom();
            $this->reset(); 
        }
    }
    
    public function updateChapSetting($ietUserObj, $globalOrTarget) {
        
        /**
         *
         * This function either adds or updates a chapsetting to a target
         * or globally depending on whether the entry exists or not
         *
         **/

        $params = $ietUserObj->getCmdParams(); 

        
        if ($globalOrTarget != "globalsetting") {
            
            $targetIqn = $globalOrTarget; 
            
            $targetConfig = $this->getConfigTarget($targetIqn);
            $targetProc = $this->getTarget($targetIqn);
            
            $tid = $targetProc->getAttribute("id");
            
            $cmd = CMD_IETADM . "  --op new --tid=$tid --user --params=$params";
            
            
            exec($cmd, $output, $ret);
            
            if ($ret != 0) 
                return $output;
                           
            else {
                
                
                /* ietadm success. Add to iscsi_settings.xml */
                
                $userExists = false; 
                $userType = $ietUserObj->getUserType();
                $userName = $ietUserObj->getUsername();
                $userPassword = $ietUserObj->getPassword();
                
                /* itemCounter keeps track of which usernode we're currently looping over */
                $itemCounter = 0;
                
                /* Check whether we're adding a new user or updating an existing one */
                
                $userTypeList = $targetConfig->getElementsByTagName($userType);
               
                if ($userTypeList) {
                    
                    /* $userTypeList is not null so an entry of $userType has been found */
                       
                    foreach ($userTypeList as $userTypeItem) { 
                    
                        if ($userTypeItem->getAttribute("username") == $userName) {
                        
                            /* found the user */
                            $userExists = true;
                            break;
                        }
                    
                        else {
                        
                            /* This node is not the user, increment counter */
                        
                            $itemCounter++;
                        }
                    }
                
                }
                
                if (!$userExists) {
                    
                    /* We're adding a new user */
                    
                    $importedNode = $this->configDom->importNode($ietUserObj->serialize(), true); 
                    $targetConfig->appendChild($importedNode); 
                
                }
                
                else {
                    
                    /* User exists so we're updating */
                    
                    $node = $userTypeList->item($itemCounter);
                    $node->setAttribute("password", $userPassword);
                }
                
                $this->configDom->saveDom();
                $this->reset(); 
            }   
        }
        
        else {
            
            $userCounter = 0; 
            
            $cmd = CMD_IETADM . "  --op new --user --params=$params";
            exec($cmd, $output, $ret);
            
            if ($ret != 0) 
                return $output;
            
            else {
                
                $userExists = false; 
                $userType = $ietUserObj->getUserType();
                $userName = $ietUserObj->getUsername();
                $userPassword = $ietUserObj->getPassword();
            
                $globalSettings = $this->configDom->getElementsByTagName("globalsettings")->item(0);
                
                /* Check whether the user exists */
                
                $userList = $globalSettings->getElementsByTagName($userType);
               
                if ($userList) {
                    
                /* $userList is not null so an entry exists */    
                    
                    foreach ($userList as $userNode) {
                    
                        if ($userNode->getAttribute("username") == $userName) {
                        
                            $userExists = true;
                            break; 
                        
                        }
                    
                        else
                            $userCounter++; 
                    
                    }
                }
                
                if (!$userExists) {
                    
                    /* $userList is null or a user has not been found */
                
                   $importedNode = $this->configDom->importNode($ietUserObj->serialize(), true);
                   $globalSettings->appendChild($importedNode);
                
                  
                }
                
                else {
                    
                    $userItem = $userList->item($userCounter);
                    $userItem->setAttribute("password", $userPassword);        
                    
                }
                
                $this->configDom->saveDom();
                $this->reset(); 
            }
        }
    }
    
    public function delChapSetting($ietUserName, $UserType, $globalOrTarget) {
        
        if ($globalOrTarget != "globalsetting") {

            /* Get the tid for the target */
            
            $tid = $this->getTarget($globalOrTarget)->getAttribute("id");
            
            $cmd = CMD_IETADM . "  --op delete --tid=$tid --user --params=$UserType=$ietUserName";
            
            exec($cmd, $output, $ret);
            
            if ($ret != 0)
                return $output;
            
            else {
                
                $target = $this->getConfigTarget($globalOrTarget);
                
                $xPath = "//target[@Name='$globalOrTarget']/$UserType";
                $nodelist = $this->configDom->runXpathQuery($xPath);
                
                foreach ($nodelist as $node) {
                    if ($node->getAttribute("username") == $ietUserName)
                        $deleted = $node->parentNode->removeChild($node);
                }   
            }       
        }
        
        else {
            
            $cmd = CMD_IETADM . "  --op delete --user --params=$UserType=$ietUserName";
            
            exec($cmd, $output, $ret); 
            
            if ($ret != 0)
                return $output;
            
            else {
                
                /* Find the entry in globalsettings */
                
                
                
                $xPath = "//globalsettings/".$UserType."[@username='".$ietUserName."']";
                $userNode = $this->configDom->runXpathQuery($xPath)->item(0);
                
                /* Delete the entry */
                
                $deletedNode = $userNode->parentNode->removeChild($userNode);
                
                
            }    
        }
        
        $this->configDom->saveDom();
        $this->reset(); 
    }
    
    public function getFHData($command) {
        $fh = popen($command . " 2>&1", "r");
        while (!feof($fh))
            $data .= fgets($fh, 8192);
        pclose($fh);
        
        return $data;
    }
    
    public function getTargetSN() {
        
        $cmd = CMD_UUIDGEN . " | cut -f5 -d \"-\"";
        $targetSN = $this->getFHData($cmd);
        return $targetSN; 
    }
    
    public function getiSNSServer() {
        
        /*
         *
         * Returns isns server DOMNode
         *
         */
        
        $isnsDomNode = $this->configDom->getElementsByTagName("isns")->item(0);
        
        return $isnsDomNode; 
    }
    
    public function updateiSNSServer($serverIP, $delete=false, $restart=false) {
        
        
        $isnsDomNode = $this->getiSNSServer(); 
        
        if (!$delete) {
         
            $isnsDomNode->setAttribute("server", $serverIP);
        }
        
        else {
            
            $isnsDomNode->removeAttribute("server"); 
        }
        
        
        $this->configDom->saveDom();
        $this->reset();
        
        if ($restart) {
            
            $cmd = CMD_ISCSI_TARGET_RESTART;
            exec($cmd, $output, $ret);
            
            if ($ret != 0)
                return $output; 
        }
    }
    
    public function getLunUsage($lunPath) {
        
        /*
         *
         * Return an array of target names where the lun
         * $lunPath is used
         *
         */
        
        $targetList = array(); 
        
        if ($targets = $this->getTargets()) {
            foreach ($targets as $target) {
                if ($luns = $this->getTargetLuns(
                                            $target->getAttribute("name"))) {
                    foreach ($luns as $lun) {
                        if ($lun->getAttribute("path") == $lunPath)
                            array_push($targetList,
                                       $target->getAttribute("name")); 
                    }
                }
            }
        }
        
        return $targetList; 
    }
    
    public function suspendService() {
       
	$sm = new ServiceManager(); 
        $service = $sm->getService("iscsi-target");
        
        if ($service->isRunning()) {
            
            
            if ($service->getStatus() == STATE_PAUSED)
                return true;
            
            if ($service->pause())
                return true;
            
            return false;
        }
        
        // service is not running
        return false;
    }
    
    public function resumeService() {
       
	$sm = new ServiceManager(); 
        $service = $sm->getService("iscsi-target");
        
        if ($service->isRunning()) {
            
            if ($service->getStatus() == STATE_PAUSED)
                if ($service->resume())
                    return true;
                
                return false;
        }
        
        // service is not running so this effectively succeeds
        
        return true;
    }
    
    public function getTargetConfigToLunObjMap($lunIdToTName) {
        
    /*
    *
    *   Returns array $tname->$lunObject
    */
    
        $tnameToLunObjMap = array();
        $tnames = array();
        $lunObjs = array(); 
        
        foreach ($lunIdToTName as $lunId => $tname) {
            
            array_push($tnames, $tname);
            
            $target = $this->getConfigTarget($tname);
            
            $luns = $target->getElementsByTagName("lun");
            
            foreach ($luns as $lun) {
                
                if ($lun->getAttribute("Id") == $lunId) {
                        
                    // create a new lun object with the right attributes
                            
                    $lunAttrs = array();
                    $attrList = array("Id","Path", "Type", "ScsiId", "ScsiSN", "IOMode");
                            
                    foreach($attrList as $attrName) 
                        $lunAttrs[$attrName] = $lun->getAttribute("$attrName");
                            
                            
                    $lunObj = new IetLun($lunAttrs);
                    
                    array_push($lunObjs, $lunObj);
                            
                    break;
                }
            }
        }
                   
        
       /* syslog (LOG_INFO, "tnames count: " . count($tnames) .
        " lunObjs count: " . count($lunObjs));
       */
       
       
        $tnameToLunObjMap = array_combine($tnames, $lunObjs); 
        
        return $tnameToLunObjMap; 
        
    }
    
    public function resizeIscsiVolume($LVPath, $newVolumeSize) {
        
        // sanity checks
        
        $isrunning = false; 
        
        if ($this->suspendService()) {
            
            $isrunning = true; 
                        
            /*
                1. Find all targets exporting this volume
                2. For each target exporting this volume
                    1. delete sessions
                    2. store lun ID for this volume
                    3. unmap lun
                    4. remap lun
            
            */
        
            // get target list
        
            $targetList = $this->getLunUsage($LVPath);
            $lunIdToTName = array(); // lun to target mapping
       
        
            // delete all sessions from target
            
            $lids = array();
            $tnames = array();
            $lunpath = array(); 
        
            foreach($targetList as $target) {
                
                $this->delSessionsFromTarget($target);
                $targetObj = $this->getTarget($target); 
                $targetLuns = $targetObj->getElementsByTagName("lun"); 
                
                foreach($targetLuns as $targetLun) {
                    
                    if ($targetLun->getAttribute("path") == $LVPath) {
                        
                        array_push($lids, $targetLun->getAttribute("number"));
                        array_push($tnames, $targetObj->getAttribute("name"));
                        array_push($lunpath, $targetLun->getAttribute("path"));
                
                    }
                }
            }
            
            $lunIdToTName = array_combine($lids, $tnames);
            $tnameToLunPath = array_combine($tnames, $lunpath);
            
            // map tid to lid
            
            $tidToLid = $this->getTidToLidMap($LVPath);
            
            $tnameToLunObjMap = $this->getTargetConfigToLunObjMap($lunIdToTName);
            
            // remove luns
            
            foreach ($targetList as $target) {
            
                $this->delLunFromTarget($LVPath, $target, TRUE);    
                
            }
            
        }
        
        
        // do the legwork
        
        $lvmObj = new LVM();
        $output = $lvmObj->resizeLV($LVPath, $newVolumeSize);
        
        
        if ($isrunning) {
            // put the luns back in place
            
            foreach ($tnameToLunObjMap as $tname => $lunObj) {
                
                $this->addLunToTarget($lunObj, $tname, FALSE); 
            
            }
            
            // reenable I/O
            
            $this->resumeService();
        }
        
        if (is_array($output)) {
            
            //b0rked
            
            return $output;
        }
        
        return true;
    }
    
}

?>
