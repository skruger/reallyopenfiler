<?php 

$ajax->registerFunction("/admin/quota.html", "setQuota");
  $ajax->registerFunction("/admin/quota.html", "setQuotaBulk");


        function setQuota($blocks, $inodes, $savedfsblocksize, $savedvolumemountpoint, $blockval, $inodeval, $groupid)
        {



                $objResponse = new tinkerAjaxResponse();

                        if(intval($blockval) != intval($blocks)){
                                $objResponse->addScript("Fat.fade_element(\"gblocksvaltd" . $groupid . "\")");
                                $objResponse->addScript("var elem = document.getElementById('gblocksvaltd" . $groupid . "'); elem.removeChild(elem.childNodes.item(1));");

                        }


                        if(intval($inodeval) != intval($inodes)){
                                $objResponse->addScript("Fat.fade_element(\"ginodesvaltd" . $groupid . "\")");
                                $objResponse->addScript("var elem = document.getElementById('ginodesvaltd" . $groupid . "'); elem.removeChild(elem.childNodes.item(1));");

                        }



                 exec("/usr/bin/sudo /usr/sbin/setquota -g " . escapeshellarg($groupid) . " " . escapeshellarg(intval(round($blocks * $savedfsblocksize))) . " " . escapeshellarg(intval(round($blocks * $savedfsblocksize))) . " " . escapeshellarg($inodes) . " " . escapeshellarg($inodes) . " " . escapeshellarg($savedvolumemountpoint));



                $objResponse->addAssign("gblocksval".$groupid, "innerHTML", $blocks);
                $objResponse->addAssign("ginodesval".$groupid, "innerHTML", $inodes);
                $objResponse->addAssign("gbval".$groupid, "value", $blocks);
                $objResponse->addAssign("gival".$groupid, "value", $inodes);
                return $objResponse->getXML();



        }



    function setQuotaBulk($blocks, $inodes, $savedfsblocksize, $savedvolumemountpoint, $blockval, $inodeval, $groupid){

                // difference between setQuotaBulk and setQuota is that $blockval, $inodeval and $groupid are arrays

                $objResponse = new tinkerAjaxResponse();

                        if (intval($blocks) < 0 || !ctype_digit($blocks))
                                $blocks = 0;
                        if (intval($inodes) < 0 || !ctype_digit($inodes))
                                $inodes = 0;

                        for ($j = 0; $j < count($groupid); $j++){

                        if(intval($blockval[$j]) != intval($blocks)){
                                $objResponse->addScript("Fat.fade_element(\"gblocksvaltd" . $groupid[$j] . "\")");
                                $objResponse->addScript("var elem = document.getElementById('gblocksvaltd" .$groupid[$j] . "'); elem.removeChild(elem.childNodes.item(1));");

                        }

                        if(intval($inodeval[$j]) != intval($inodes)){
                                $objResponse->addScript("Fat.fade_element(\"ginodesvaltd" . $groupid[$j] . "\")");
                                $objResponse->addScript("var elem = document.getElementById('ginodesvaltd" .$groupid[$j] . "'); elem.removeChild(elem.childNodes.item(1));");
                        }
                        $objResponse->addScript("document.groupquotaform.check". $groupid[$j] . ".checked = false");


                exec("/usr/bin/sudo /usr/sbin/setquota -g " . escapeshellarg($groupid[$j]) . " " . escapeshellarg(intval(round($blocks * $savedfsblocksize))) . " " . escapeshellarg(intval(round($blocks * $savedfsblocksize))) . " " . escapeshellarg($inodes) . " " . escapeshellarg($inodes) . " " . escapeshellarg($savedvolumemountpoint));

                $objResponse->addAssign("gblocksval".$groupid[$j], "innerHTML", $blocks);
                $objResponse->addAssign("ginodesval".$groupid[$j], "innerHTML", $inodes);
                $objResponse->addAssign("gbval".$groupid[$j], "value", $blocks);
                $objResponse->addAssign("gival".$groupid[$j], "value", $inodes);

                $objResponse->addScript("s" . $groupid[$j] . ".setValue(parseInt($blocks))");
                $objResponse->addScript("s2_" . $groupid[$j] . ".setValue(parseInt($inodes))");
                $objResponse->addScript("disableButtons(document.getElementById('request" . $groupid[$j] . "'), document.getElementById('reset" . $groupid[$j] . "'))");
                $objResponse->addScript("disableButton(document.getElementById('requestgroups'));");
                        }
                return $objResponse->getXML();


        }



?>
