<?php


define('VOLUMES_XML', "/opt/openfiler/etc/volumes.xml");

class SharesManager extends XmlHandler {

    public $volumes = array(); // names=>path of non-iscsi/non-FC volumes
    public $validFSTypes = array("ext3", "jfs", "xfs", "reiserfs");
    public $volumesPathCount = 0; 
    
    public function SharesManager() {
        
        parent::__construct(VOLUMES_XML);
        $this->getVolumes();
        $this->volumesPathCount = count($this->volumes); 
    
    }
    
    public function getVolumes() {
    
        $query = "//volume";
        $ids = array();
        $paths = array();
        
        if ($nodes = $this->runXpathQuery($query)) {
            
            foreach ($nodes as $node) {
                
                if (in_array($node->getAttribute("fstype"),
                             $this->validFSTypes)) {
                    
                    $id = $node->getAttribute("id");
                    $path = $node->getAttribute("mountpoint");
                    $vg = $node->getAttribute("vg");
                    $lvvg = $id."-*-".$vg;
                    
                    array_push($ids, $lvvg);
                    array_push($paths, $path); 
                }
            }
            
            if (count($ids) > 0 && count($paths) > 0)
                $this->volumes = array_combine($ids, $paths);
                
           
                
        }
    
        return $this->volumes;     
    }
    
}


class Shares extends SharesManager {

    public $shares = array(); //array of share objects
    public $shareConfigs = array();
    public $sharesPathCount = 0;
    public $shareTypes = array("nfs", "http", "rsync", "ftp","smb"); 
    
    public function Shares() {
     
        parent::__construct();
        $this->findShareConfigs();
        $this->sharesPathCount = count($this->shareConfigs); 

        //TODO: open *.info.xml and getShares[NFS,CIFS...]

    }
    
    
    public function recurseDirs($mountPoint) {
    
    // open mountPoint and find all <share>.info.xml
    
    /* TODO: switch to RecursiveDirectoryIterator from
     *        php SPL
     */
    

        $dirhandle = opendir($mountPoint);
        
        while (false !== ($file = readdir($dirhandle))) {
            
            if (($file == ".") || ($file == "..") ||
                (!is_dir($mountPoint . $file))) 
                    continue;
    
                
            if (!is_file($mountPoint . $file . ".info.xml"))
                    continue;
                
            $dirtype = "";
    
            $shareConfig = $mountPoint . $file . ".info.xml";
            $shareDom = new XmlHandler($shareConfig);
            
            $query = "//key[@name='dirtype']";
            $node = $shareDom->runXPathQuery($query)->item(0);
            $dirtype = $node->getAttribute("value"); 
            
            if ($dirtype == "share") {
                array_push($this->shareConfigs, $shareConfig);
            }
            
            if ($dirtype == "closed")
                $this->recurseDirs($mountPoint . $file . "/");
                
        }
        
        closedir($dirhandle); 
        
    }
    
    public function findShareConfigs() {
    
        if (count($this->volumes) > 0) {
            foreach ($this->volumes as $mountPoint) {
  
                $this->recurseDirs($mountPoint);              
            }
        }
    }
    
    function getShareConfigs() {
    
        return $this->shareConfigs;     
    }
    
    
    function getNFSShares() {
        
    }
    
    function getCIFSShares() {
        
    }
    
    function getFTPShares() {
        
    }
    
    function getRsyncShares() {
        
    }
    
    function getHTTPDAVShares() {
        
    }
    
}

class Share extends Shares {
    
     

    public function Share() {
        
        parent::__construct();
         
    }
    
}

class NFSShare extends Share {
    
}

class CIFSShare extends Share {
    
}

class FTPShare extends Share {
    
}

class RsyncShare extends Share {
    
}

class HTTPDAVShare extends Share {
    
}


?>
