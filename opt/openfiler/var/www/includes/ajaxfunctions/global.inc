<?php

/* ajax function for uptime */
$ajax->registerFunction($PHP_SELF, "getUptime"); 

function getUptime() {

        $objResponse = new tinkerAjaxResponse();
        $handle = popen("/usr/bin/sudo /usr/bin/uptime", "r");
        $data = fread($handle, 4096);
        $uptime = trim($data);
        pclose($handle);
        
        /* update via ajax */
        
  
        $objResponse->addRemove("uptimeData");
        $objResponse->addAppend("uptimeDiv", "innerHTML", "<span id=\"uptimeData\">$uptime</span>");
        return $objResponse->getXML(); 
        
}


/* end ajax function for uptime */


?>
