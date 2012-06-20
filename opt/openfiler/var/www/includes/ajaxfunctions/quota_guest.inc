<?php

 $ajax->registerFunction("/admin/quota_guest.html","setQuotaGuest");


        function setQuotaGuest($gblocks96, $ginodes96, $savedfsblocksize, $savedvolumemountpoint, $gbval96, $gival96)
        {

                $objResponse = new tinkerAjaxResponse();


                        if(intval($gbval96) != intval($gblocks96)){

                                $objResponse->addScript("Fat.fade_element(\"gblocksvaltd96\");");
                                $objResponse->addScript("var elem = document.getElementById('gblocksvaltd96'); elem.removeChild(elem.childNodes.item(1));");

                        }
                        if(intval($gival96) != intval($ginodes96)){
                                $objResponse->addScript("Fat.fade_element(\"ginodesvaltd96\");");
                                $objResponse->addScript("var elem = document.getElementById('ginodesvaltd96'); elem.removeChild(elem.childNodes.item(1));");

                        }

                 exec("/usr/bin/sudo /usr/sbin/setquota -g 96 " . escapeshellarg(intval(round($gblocks96 * $savedfsblocksize))) . " " . escapeshellarg(intval(round($gblocks96 * $savedfsblocksize))) . " " . escapeshellarg($ginodes96) . " " . escapeshellarg($ginodes96) . " " . escapeshellarg($savedvolumemountpoint));



                $objResponse->addAssign("gblocksval96", "innerHTML", $gblocks96);
                $objResponse->addAssign("ginodes96val", "innerHTML", $ginodes96);
                $objResponse->addAssign("gbval96", "value", $gblocks96);
                $objResponse->addAssign("gival96", "value", $ginodes96);

                return $objResponse->getXML();


        }

?>