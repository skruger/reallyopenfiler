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


if (file_exists('/sbin/lvm'))
	$lvm_cmd = '/sbin/lvm '; 
else
	$lvm_cmd = '/usr/sbin/lvm ';

define('CMD_LVM', "export LANG=C; /usr/bin/sudo $lvm_cmd 2>&1");
define('CMD_LVS', CMD_LVM . " lvs ");
define('CMD_VGS', CMD_LVM . " vgs ");
define('CMD_PVS', CMD_LVM . " pvs ");
define('CMD_PVCREATE', CMD_LVM . " pvcreate ");
define('CMD_PVREMOVE', CMD_LVM . " pvremove ");
define('CMD_PVRESIZE', CMD_LVM . " pvresize ");
define('CMD_PVEXTEND', CMD_LVM . " pvextend ");
define('CMD_LVCREATE', CMD_LVM . " lvcreate ");
define('CMD_LVREMOVE', CMD_LVM . " lvremove ");
define('CMD_LVRESIZE', CMD_LVM . " lvresize ");
define('CMD_LVEXTEND', CMD_LVM . " lvextend ");
define('CMD_VGREMOVE', CMD_LVM . " vgremove ");
define('CMD_VGEXTEND', CMD_LVM . " vgextend ");
define('CMD_VGCREATE', CMD_LVM . " vgcreate ");


class LVMManager extends XmlHandler {

/* utility class for managing  /etc/lvm/* settings */

    public function LVMManager() {
        parent::__construct();
    }


}

class LVM extends LVMManager {

    protected $lvs = array();
    protected $vgs = array();
    protected $pvs = array();
    protected $volumePath;
    protected $volumeType;
    protected $mtabmounts = array();
    protected $procmounts = array();
    public $volumeConfigDom;
    public $snapshotConfigDom; 
    protected $volXML = array();


    public function LVM() {

        $this->resetLVM();
    }

    public function resetLVM() {
        $pathToFile = "/opt/openfiler/etc/volumes.xml";
        $this->volumeConfigDom = new XmlHandler($pathToFile);
        $pathToFile = "/opt/openfiler/etc/snapshots.xml"; 
        $this->snapshotConfigDom = new XmlHandler($pathToFile); 
        $this->resetPVs();
        $this->resetVGs();
        $this->resetLVs();
        $this->resetMounts();
        $this->resetVolXML();
    }

    public function resetLVs() {
       $this->lvs = $this->getLVs();

    }

    public function resetVGs() {
       $this->vgs = $this->getVGs();

    }

    public function resetPVs() {
       $this->vgs = $this->getPVs();

    }

    public function resetMounts() {
       $this->mtabmounts = $this->getMtabMounts();
       $this->procmounts = $this->getProcMounts();
    }

    public function getterLvs() {

       return $this->lvs;

    }

    public function getterPvs() {

       return $this->pvs;
    }

    public function getterVgs() {

       return $this->vgs;
    }

    private function resetVolXML() {
       $keys = array();
       $values = array();
       $xPath = "//volume";
       $domNodes = $this->volumeConfigDom->runXpathQuery($xPath);
       if (count($domNodes > 0))
           foreach($domNodes as $domNode) {
              $string = "/dev/";
              $string .= $domNode->getAttribute("vg");
              $string .= "/";
              $string .= $domNode->getAttribute("id");
              array_push($keys, $string);
              array_push($values,$domNode->getAttribute("mountpoint"));
           }

       $this->volXML = array_combine($keys,$values);

    }

    public function getiSCSILvs() {

        $xPath = "//volume[@fstype='iscsi']";
        $iscsiLvs = $this->volumeConfigDom->runXpathQuery($xPath);

        return $iscsiLvs;

    }

    public function getFHData($command) {
        $data = "";
        $fh = popen($command . " 2>&1", "r");
        while (!feof($fh))
            $data .= fgets($fh, 8192);
        pclose($fh);
        return $data;
    }

    public function getVGs() {

        /*
         * Returns array (<seqkey> => <vgname>) : M
         *
         */

        $command = "export LANG=C; /usr/bin/sudo vgs  --noheadings | cut -f3 -d \" \"";
        $vgs = explode("\n", $this->getFHData($command));
        $data = array_pop($vgs);
        return $vgs;
    }

    public function getVGLVs($VG) {

        /*
         * Returns array (<seqkey> => <lvname>) : M
         *
         */

        $lvs = array();
        $command = "export LANG=C; /usr/bin/sudo lvs --noheadings --separator \" \" | cut -f3,4 -d \" \"";
        $lvsvgs = explode("\n", $this->getFHData($command));
        $data = array_pop($lvsvgs);
        foreach($lvsvgs as $key => $value) {
                $lvvg = explode(" ", $value);
                foreach($lvvg as $key2 => $value2)
                        if ($VG == $value2) {
                                array_push($lvs, $lvvg[0]);
                        }
        }

        return $lvs;

    }

    public function getLVs() {

        /*
         * Returns array (</dev/vgname/lvname> => [</dev/vgname/snapsource>]) : M
         * lvnames are unique only to the containing vg, so we must append
         * the full path to the lv ; /dev/<vgname>/
         *
         */

        $lvs = array();
        $array_keys = array();
        $array_values = array();
        $command = "export LANG=C; /usr/bin/sudo lvs --noheadings --separator \" \" | cut -f3,4,7 -d \" \"";
        $lvsdata = explode("\n", $this->getFHData($command));
        $rubbish = array_pop($lvsdata);
        foreach($lvsdata as $lvsdatum) {
            $localarr = explode(" ", $lvsdatum);
            $key = "/dev/" . $localarr[1] . "/" . $localarr[0];
            if ($localarr[2] != " ")
                    $value = "/dev/" . $localarr[1] . "/" . $localarr[2];
            else
                    $value = " ";
            array_push($array_keys, $key);
            array_push($array_values,$value);
        }

        $lvs = array_combine($array_keys, $array_values);

        return $lvs;

     }

     public function getPVs() {

        /*
        *
        * Returns array ( uuid => pvpath ) : M
        * pv=>vg mappings can be derived
        *
        */

        $pvs = array();
        $array_keys = array();
        $array_values = array();
        $command = "export LANG=C; /usr/bin/sudo pvs --noheadings --separator \" \" -o +pv_uuid | cut -f 3,9 -d \" \"";
        $pvsdata = explode("\n", $this->getFHData($command));
        $rubbish = array_pop($pvsdata);
        foreach ($pvsdata as $pvsdatum) {

           $localarr = explode(" ", $pvsdatum);
           $key = $localarr[1];
           $value = $localarr[0];
           array_push($array_keys, $key);
           array_push($array_values, $value);

        }

        $pvs = array_combine($array_keys, $array_values);
        return $pvs;
     }

    public function getVGPVs($VG) {

        /*
         * Returns array (<seqkey> => <pvname>) : M
         *
         */

        $pvs = array();
        $command = "export LANG=C; /usr/bin/sudo pvs --noheadings --separator \" \" | cut -f3,4 -d \" \"";
        $pvsvgs = explode("\n", $this->getFHData($command));
        $data = array_pop($pvsvgs);
        foreach($pvsvgs as $key => $value) {
                $pvvg = explode(" ", $value);
                foreach($pvvg as $key2 => $value2)
                        if ($VG == $value2) {
                                array_push($pvs, $pvvg[0]);
                        }
        }

        return $pvs;
    }

    public function getPVVG($PV) {

        /*
         * Returns array (<seqkey> => <pvname>) : 1
         *
         */

        $pvvgs = array();
        $command = "export LANG=C; /usr/bin/sudo pvs --noheadings --nosuffix --units m --separator \" \" | cut -f3,4 -d \" \"";
        $vgspvs = explode("\n", $this->getFHData($command));
        $data = array_pop($vgspvs);
        foreach($vgspvs as $key => $value) {
                $vgpv = explode(" ", $value);
                foreach($vgpv as $key2 => $value2)
                        if($PV == $value2) {
                                array_push($pvvgs, $vgpv[1]);
                        }
        }

        return $pvvgs;
    }

    public function getPartitionLV($part) {

    }


    public function getPVMetadata($PV) {

        /*
         * Returns array (<attribute> => <value>) : M
         *
         */

        $items = array("pv_fmt", "pv_uuid", "pv_size", "dev_size", "pv_free",
                       "pv_used", "pv_start", "pv_name", "pv_attr", "pv_pe_count",
                       "pv_pe_alloc_count");

        $outputheadings = $items;

        $cmdoptions = "";

        for ($i=0; $i<sizeof($items); $i++) {

            $cmdoptions .= $items[$i];
            if (sizeof($items) - 1 > $i)
                $cmdoptions .= ",";
        }

        $cmd = CMD_PVS . "$PV --noheadings --nosuffix --units m " .
                          "--separator :  -o  $cmdoptions ";



        $data = explode(":", trim($this->getFHData($cmd)));

        $pvmetadata = array_combine($outputheadings, $data);

        return $pvmetadata;

    }


    public function getLVMetadata($LV) {


        /*
         * Returns array (<attribute> => <value>) : M
         *
         */

        $items = array("lv_uuid","lv_name", "lv_attr", "lv_major", "lv_minor",
                       "lv_read_ahead", "lv_kernel_major", "lv_kernel_minor",
                       "lv_kernel_read_ahead", "lv_size", "seg_count", "origin",
                       "snap_percent", "move_pv", "convert_lv");

        $outputheadings = $items;

        $cmdoptions = "";

        for ($i=0; $i<sizeof($items); $i++) {

            $cmdoptions .= $items[$i];
            if (sizeof($items) - 1 > $i)
                $cmdoptions .= ",";
        }

        $cmd = CMD_LVS . "$LV --noheadings --nosuffix --units m " .
                          "--separator :  -o  $cmdoptions ";



        $data = explode(":", trim($this->getFHData($cmd)));

        $lvmetadata = array_combine($outputheadings, $data);

        return $lvmetadata;

    }


    public function getVGMetadata($VG) {

         /*
         * Returns array (<attribute> => <value>) : M
         *
         */

        $items = array("vg_uuid","vg_name", "vg_attr", "vg_free", "vg_size",
                       "vg_sysid", "vg_extent_size", "vg_extent_count",
                       "vg_free_count", "pv_count", "lv_count", "snap_count");

        $outputheadings = $items;

        $cmdoptions = "";

        for ($i=0; $i<sizeof($items); $i++) {

            $cmdoptions .= $items[$i];
            if (sizeof($items) - 1 > $i)
                $cmdoptions .= ",";
        }

        $cmd = CMD_VGS . "$VG --noheadings --nosuffix --units m " .
                          "--separator :  -o  $cmdoptions ";



        $data = explode(":", trim($this->getFHData($cmd)));

        $vgmetadata = array_combine($outputheadings, $data);

        return $vgmetadata;

    }



    public function getLVSnapshots($LV) {

        /*
         * Returns array (<seqkey> => <snapshotname>) : M
         *
         */

        $array = array();
        $lvs = $this->getLVs();

        foreach($lvs as $key => $value) {
            if($LV == $value) {
                    array_push($array, $key);
            }
        }

        return $array;
    }

    public function getiSCSISnapshots() {

        $snapShotLvs = $this->getiSCSILvs();
        $iScsiSnaps = array();

        if($snapShotLvs)
            foreach($snapShotLvs as $snapShotLv) {

                $vg = $snapShotLv->getAttribute("vg");
                $lv = $snapShotLv->getAttribute("id");
                $path = "/dev/$vg/$lv";
                foreach ($this->getLVSnapshots($path) as $key=>$value)
                        array_push($iScsiSnaps, $value);
            }

        return $iScsiSnaps;
    }


    public function getLVMObjectAttr($LVMObject, $LVMObjectType, $LVMObjectAttr) {

        /*
         * Returns String: LVMObject Attribute Value
         *
         */

        $array = array();

        switch($LVMObjectType) {

            case "VG":
                    $array = $this->getVGMetadata($LVMObject);
                    break;
            case "LV":
                    $array = $this->getLVMetadata($LVMObject);
                    break;
            case "PV":
                    $array = $this->getPVMetadata($LVMObject);
                    break;
            default:
                    break;

        }

        return $array[$LVMObjectAttr];

    }

    public function getLVMObjectSize($LVMObject, $LVMObjectType) {

        /*
         * Returns String: LVMObject Size, eg PVSize, LVSize
         *
         */

        $array = array();
        $keyname = "";
        switch ($LVMObjectType) {

            case "VG":
                    $array = $this->getVGMetadata($LVMObject);
                    $keyname = "vg_size";
                    break;
            case "LV":
                    $array = $this->getLVMetadata($LVMObject);
                    $keyname = "lv_size";
                    break;
            case "PV":
                    $array = $this->getPVMetadata($LVMObject);
                    $keyname = "pv_size";
                    break;
            default:
                    break;
        }


        return $array[$keyname];
    }


    public function resizeLV($LVPath, $newLVSize) {

        $cmd = "export LANG=C; /usr/bin/sudo lvresize -L " . escapeshellarg($newLVSize) .
               " " . escapeshellarg($LVPath);
        exec($cmd, $output, $ret);

        if ($ret != 0)
            return $output;

        return true;

    }

    protected function getExec($command) {

        $returnData = array();

        exec($command, $output, $ret);

        $returnData["output"] = $output;
        $returnData["ret"] = $ret;

        return $returnData;

    }

    protected function getMtabMounts() {

       //realpath to volume is stored in mtab

       // returns array numerickey=>mountentry

       $mtab = array();
       $mntfile = "/etc/mtab";
       $file = new File($mntfile);
       $file->Load();
       return $file->contents;

    }

    protected function getProcMounts() {
       $procmounts = array();
       $procfile = "/proc/mounts";
       $file = new File($procfile);
       $file->Load();
       return $file->contents;

    }
}




class PhysicalVolume extends LVM {

    function PhysicalVolume($volumePath) {
        parent::__construct();
        $this->volumePath = $volumePath;
        $this->volumeType = "PV";
    }

    function pvcreate() {

        $cmd = CMD_PVCREATE . $this->volumePath;
        $run = $this->getExec($cmd);

        if ($run["ret"] != 0)
            return $run["output"];

        return true;
    }

    function pvremove() {

        $cmd = CMD_PVREMOVE . $this->volumePath;
        $run = $this->getExec($cmd);

        if ($run["ret"] != 0)
            return $run["output"];

        return true;

     }

     function pvresize($newSizeMB) {

        $cmd = CMD_PVRESIZE . "--setphysicalvolumesize +" .
            escapeshellarg($newSizeMB) . " $this->volumePath";
        $run = $this->getExec($cmd);

        if ($run["ret"] != 0)
            return $run["output"];

        return true;
     }
}

class VolumeGroup extends LVM {

    function VolumeGroup() {

       parent::__construct();
       $this->volumeType = "VG";

    }

}

class LogicalVolume extends LVM {

   public $name = ""; // name of LV
   public $snapname = ""; //name of snapshot 
   public $vgname = ""; // volume group for this lv
   public $sizeInMB = ""; // size of volume when creating or extending


   function LogicalVolume($volumePath) {

      parent::__construct();
      $this->volumePath = $volumePath;
      $this->volumeType = "LV";
   }

   public function lvcreate() {

      if (empty($this->snapname)) {
         $cmd = CMD_LVCREATE . "-n " . escapeshellarg($this->name) . " -L " . 
         escapeshellarg($this->sizeInMB) . " " . escapeshellarg($this->vgname);
      }

      else {

         $cmd = CMD_LVCREATE . "-L -s -n " . escapeshellarg($this->snapname) . 
         " /dev/" . escapeshellarg($this->vgname) . " " .
         escapeshellarg($this->name);  
      }


      $run = $this->getExec($cmd);
      if ($run["ret"] != 0)
          return $run["output"];

      return true;

   }

   public function lvremove() {

      $cmd = CMD_LVREMOVE  . "-f  $this->volumePath";
      $run = $this->getExec($cmd);

      if ($run["ret"] != 0)
          return $run["output"];

      return true;
   }

   public function lvextend($sizeInMB) {

      $cmd = CMD_LVEXTEND . "-L +" . $sizeInMB . "  $this->volumePath";
      $run = $this->getExec($cmd);

      if ($run["ret"] != 0)
        return $run["output"];

      return true;

   }

   public function lvrename($newName) {

      $cmd = CMD_LVRENAME . $this->volumePath . " $newName";
      $run = $this->getExec($cmd);

      if ($run["ret"] != 0)
        return $run["output"];

      return true;
   }

   public function lvreduce() {}

}


class Snapshot extends LogicalVolume {



}

class OFLogicalVolume extends LogicalVolume {

   protected $realVolPath = "";
   protected $xmlVolAttrs = array("id"=>"","name"=>"", 
                                  "mountpoint"=>"", "vg"=>"", "fstype"=>"");
   protected $xmlMountPath = "";

   function OFLogicalVolume($volumePath) {
      parent::__construct($volumePath);
      $this->setRealPath();
      $this->initXMLMountPath();
      if (!empty($this->xmlMountPath)) {
         $this->initXMLVolAttrs();
      }
   }

   protected function initXMLMountPath() {
      if (array_key_exists($this->volumePath, $this->volXML)) {
         $this->xmlMountPath = $this->volXML["$this->volumePath"];
      }

   }

   public function getXMLVolAttrs() {
        return $this->xmlVolAttrs;
   }

   private function initXMLVolAttrs() {
      $xPath = "//volume[@mountpoint='$this->xmlMountPath']";
      if ($domNode = $this->volumeConfigDom->runXPathQuery($xPath)->item(0)) {
         foreach($this->xmlVolAttrs as $volAttrKey=>$volAttrVal) {
            $this->xmlVolAttrs["$volAttrKey"] = $domNode->getAttribute("$volAttrKey");
         }
      }
   }


   private function setRealPath() {
         $cmd = "sudo readlink $this->volumePath";
         $fhdata = explode("\n", $this->getFHData($cmd));
         $this->realVolPath = $fhdata[0];
   }

   public function delete() {

     if ($this->xmlVolAttrs["fstype"] != "iscsi" &&
     $this->xmlVolAttrs["fstype"] != "iblock") {
        

         $foundInMtab = false;
         // check if we're mounted. mtab contains link target for LVM volumes
         // so use realVolPath
         $string = addcslashes($this->realVolPath, '/');

         foreach($this->mtabmounts as $value) {

            if (preg_match("/$string\s/", $value)) {
               $foundInMtab = true;
               break;
            }
         }

         if ($foundInMtab) {
            $cmd = "/usr/bin/sudo umount -f $this->realVolPath 2>&1";
            $run = $this->getExec($cmd);
            if ($run["ret" != 0]) {
               return $run["output"];
            }

            else {
                $this->resetMounts();
            }
         }
     }



     //remove LV
     /*
     * Check if LV exists. If it doesn't it means it was removed in a previous
     * operation but for some reason the entry is still in volumes.xml
     */

     if (array_key_exists($this->volumePath, $this->getLVs())) {

        $output = $this->lvremove();
        if ($output !== true)
            return $output;
     }

     //delete from volumes.xml

     $xPath = "//volume[@mountpoint='$this->xmlMountPath']";
     if ($node = $this->volumeConfigDom->runXpathQuery($xPath)->item(0)) {
        $nodedelete = $node->parentNode->removeChild($node);
        $this->volumeConfigDom->saveDom();
     }


     // remove fspath

     /*
      * Check whether cluster mode is enabled (fstab does not contain mount
      * entries when clustering is enabled)
      */

      $clusterfile = "/opt/openfiler/etc/cluster.xml";
      $domDoc = new XmlHandler($clusterfile);
      $xPath = "//clustering";
      $clusterdomnode = $domDoc->runXpathQuery($xPath)->item(0);

      if ($clusterdomnode->getAttribute("state") != "on") {
 
         if (!empty($this->volumePath)) {

            $file = new File("/etc/fstab");
            $file->Load();
            $cleanSearch = addcslashes($this->volumePath,'/');
            $cleanSearch .= "\s"; //for regular expression


            $file->DeleteLine($cleanSearch, false);

            $file->Save();

        }

      }


      // remove filesystem mountpoint

         if (is_dir($this->xmlMountPath)) {
            $options = " --ignore-fail-on-non-empty $this->xmlMountPath";
            $cmd = "/usr/bin/sudo rmdir $options 2>&1";
            $run = $this->getExec($cmd);
            if ($run["ret"] != 0) {
               return $run["output"];
            }
            
            else {
               return true;
            }
         }
      }

      public function create() {}

}

class OFSnapshot extends OFLogicalVolume {



}

?>
