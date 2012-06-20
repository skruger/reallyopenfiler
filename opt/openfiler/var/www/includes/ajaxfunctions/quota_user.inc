<?php

$ajax->registerFunction("/admin/quota_user.html",  "setQuotaUser");

        function setQuotaUser($blocks, $inodes, $savedfsblocksize, $savedvolumemountpoint, $blockval, $inodeval, $userid)
        {



                $objResponse = new tinkerAjaxResponse();

			if(intval($blockval) != intval($blocks)){
                                $objResponse->addScript("Fat.fade_element(\"ublocksvaltd" . $userid . "\")");
		                $objResponse->addScript("var elem = document.getElementById('ublocksvaltd" . $userid . "'); elem.removeChild(elem.childNodes.item(1));");

                        }


			if(intval($inodeval) != intval($inodes)){
                                $objResponse->addScript("Fat.fade_element(\"uinodesvaltd" . $userid . "\")");
                                $objResponse->addScript("var elem = document.getElementById('uinodesvaltd" . $userid . "'); elem.removeChild(elem.childNodes.item(1));");

			}



                 exec("/usr/bin/sudo /usr/sbin/setquota -u " . escapeshellarg($userid) . " " . escapeshellarg(intval(round($blocks * $savedfsblocksize))) . " " . escapeshellarg(intval(round($blocks * $savedfsblocksize))) . " " . escapeshellarg($inodes) . " " . escapeshellarg($inodes) . " " . escapeshellarg($savedvolumemountpoint));



                $objResponse->addAssign("ublocksval".$userid, "innerHTML", $blocks);
                $objResponse->addAssign("uinodesval".$userid, "innerHTML", $inodes);
                $objResponse->addAssign("ubval".$userid, "value", $blocks);
                $objResponse->addAssign("uival".$userid, "value", $inodes);
                return $objResponse->getXML();



        }

  $ajax->registerFunction("/admin/quota_user.html", "setQuotaUserBulk");

	function setQuotaUserBulk($blocks, $inodes, $savedfsblocksize, $savedvolumemountpoint, $blockval, $inodeval, $userid){

		// difference between setQuotaUserBulk and setQuotaUser is that $blockval, $inodeval and $userid are arrays

                $objResponse = new tinkerAjaxResponse();

			if ((intval($blocks) < 0) || !ctype_digit($blocks))
				$blocks = 0;
			if ((intval($inodes) < 0) || !ctype_digit($inodes))
				$inodes = 0;

                        for ($j = 0; $j < count($userid); $j++){
			if(intval($blockval[$j]) != intval($blocks)){
                                $objResponse->addScript("Fat.fade_element(\"ublocksvaltd" . $userid[$j] . "\")");
		                $objResponse->addScript("var elem = document.getElementById('ublocksvaltd" .$userid[$j] . "'); elem.removeChild(elem.childNodes.item(1));");

			}

			if(intval($inodeval[$j]) != intval($inodes)){
				$objResponse->addScript("Fat.fade_element(\"uinodesvaltd" . $userid[$j] . "\")");
                                $objResponse->addScript("var elem = document.getElementById('uinodesvaltd" .$userid[$j] . "'); elem.removeChild(elem.childNodes.item(1));");

			}

		$objResponse->addScript("document.userquotaform.check". $userid[$j] . ".checked = false");

		exec("/usr/bin/sudo /usr/sbin/setquota -u " . escapeshellarg($userid[$j]) . " " . escapeshellarg(intval(round($blocks * $savedfsblocksize))) . " " . escapeshellarg(intval(round($blocks * $savedfsblocksize))) . " " . escapeshellarg($inodes) . " " . escapeshellarg($inodes) . " " . escapeshellarg($savedvolumemountpoint));

		$objResponse->addAssign("ublocksval".$userid[$j], "innerHTML", $blocks);
		$objResponse->addAssign("uinodesval".$userid[$j], "innerHTML", $inodes);
		$objResponse->addAssign("ubval".$userid[$j], "value", $blocks);
                $objResponse->addAssign("uival".$userid[$j], "value", $inodes);

		$objResponse->addScript("s" . $userid[$j] . ".setValue(parseInt($blocks))");
                $objResponse->addScript("s2_" . $userid[$j] . ".setValue(parseInt($inodes))");
		$objResponse->addScript("disableButtons(document.getElementById('request" . $userid[$j] . "'), document.getElementById('reset" . $userid[$j] . "'))");
		$objResponse->addScript("disableButton(document.getElementById('requestusers'));");
                        }
                return $objResponse->getXML();


        }





?>