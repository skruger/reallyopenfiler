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


class line_delete {

    var $myfilepath;
   
    function line_delete($filepath) {
           
        $this->myfilepath = $filepath;
    }
   
    function rundelete() {
           
        $separator = "\n";
        
        $data = file_get_contents($this->myfilepath);

        $eachline = explode($separator,$data);
        $newdata = array_shift($eachline);
        $outdata = implode($separator, $eachline);
        file_put_contents($this->myfilepath, $outdata);
    }

}


class fs_utility {

       var $mymountpoint="";
       
       // constructor
       function fs_utility($fsmountpoint) {
            
               $this->mymountpoint = $fsmountpoint;
       }

       // get size of FS according to df command

       function get_dfcommand_fssize() {

            $type = "blocks";
            $data = $this->df_command($type);

            return ($data[1] / 1024);
       
       }

       //get max inodes allocatable for FS according to df command

        function get_dfcommand_inodes() {
                       
            $type = "inodes";
            $data =	$this->df_command($type);
           
            return $data[1];


        }

        function df_command($type){  

            $blocktype = "blocks";
            $inodetype = "inodes";
    
            if (strcmp($blocktype, $type) == 0){
                
                    $dfp = popen("export LANG=C; /usr/bin/sudo /bin/df -P  -l " . $this->mymountpoint, "r");
            }
            
            else if (strcmp($inodetype, $type) == 0){
                
                    $dfp = popen("export LANG=C; /usr/bin/sudo /bin/df -P  -i " . $this->mymountpoint, "r");
            }
       
            $dfstr = trim(fgets($dfp, 4096)); // Skip table header
            $i = 0;
            
            while (!feof($dfp)) {
     
                $dfstr = trim(fgets($dfp, 4096));
    
                if (strlen($dfstr) <= 0)
                        continue;
    
                $foo = explode(" ", $dfstr);
    
                foreach($foo as $fresultitem) {
                    
                    $fresultitem = trim($fresultitem);
                    if (strlen($fresultitem) > 0) {
    
                            $df[$i] = $fresultitem;
                            $i++;
                    }
    
                }
            }
     
            pclose($dfp);
                return $df;
        }

}


class ups_utility {


    function ups_utility() {
        
        function upsdevices_startelement($parser, $name, $attrs) {
            
            global $upsdevices_id, $upsdevices_name, $upsdevices_id, $upsdevices_manufacturer, $upsdevices_driver;

            if ($name == "DEVICE") {
                    array_push($upsdevices_id, $attrs["ID"]);
                    array_push($upsdevices_name, $attrs["NAME"]);
                    array_push($upsdevices_manufacturer, $attrs["MANUFACTURER"]);
                    array_push($upsdevices_driver, $attrs["DRIVER"]);
            }
        }

        function upsdevices_endelement($parser, $name)
        {

        }




         $upsdevices_parser = xml_parser_create();
         xml_set_element_handler($upsdevices_parser, "upsdevices_startelement", "upsdevices_endelement");
         $upsdevices_fp = fopen("/opt/openfiler/etc/plugins/ups/devices/devices.xml", "r");

         while ($upsdevices_data = fread($upsdevices_fp, 4096))
                 xml_parse($upsdevices_parser, $upsdevices_data, feof($upsdevices_fp));

         fclose($upsdevices_fp);
         xml_parser_free($upsdevices_parser);
   }
       
}


function uGetHomesDir() {
       
    $homespath = ""; 
    
    if ($homespathDom = new XmlHandler(CONFIG_XML_HOMESPATH)) {
        
         if ($homespathNode = $homespathDom->getElementsByTagName("homespath")->item(0)) {
            
                 $homespath = $homespathNode->getAttribute("value"); 
            }
    }
    
    return $homespath; 
       
}



?>
