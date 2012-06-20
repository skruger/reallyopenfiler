<?php


/*
 *
 *
 * --------------------------------------------------------------------
 * Copyright (c) 2001 - 2008 Openfiler Project.
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


/*
 * Scheduled snapshot job
 *
 *
 */

	/* First take a lock so that this program is an exclusive process and no other
	   openfiler code such as the web-interface is also operating at the same time
	   trying to snapshot stuff.
	*/

	$flockp = fopen("/opt/openfiler/var/oflocks/snap.lock", "w+");
	
	if (!$flockp)
		exit;

	if (!flock($flockp, LOCK_EX | LOCK_NB))
	{
		fclose($flockp);
		exit;
	}
	
	require("pre.inc");

	global $cluster_enabled, $cluster_nodename, $cluster_resource;

	$cluster_enabled = false;
	$cluster_nodename = "";
	$cluster_resource = array();

	function cluster_startelement($parser, $name, $attrs)
	{
		global $cluster_enabled, $cluster_nodename, $cluster_resource;

		if ($name == "CLUSTERING")
		{
			if ($attrs["STATE"] == "on")
				$cluster_enabled = true;
			else
				$cluster_enabled = false;
		}
		else if ($name == "NODENAME")
			$cluster_nodename = $attrs["VALUE"];
		else if ($name == "RESOURCE")
			array_push($cluster_resource, $attrs["VALUE"]);
	}

	function cluster_endelement($parser, $name)
	{
	}
	
	$cluster_parser = xml_parser_create();
	xml_set_element_handler($cluster_parser, "cluster_startelement", "cluster_endelement");
	$cluster_fp = fopen("/opt/openfiler/etc/cluster.xml", "r");

	while ($cluster_data = fread($cluster_fp, 4096))
		xml_parse($cluster_parser, $cluster_data, feof($cluster_fp));
		
	fclose($cluster_fp);
	xml_parser_free($cluster_parser);


	$volumes_id = array();
	$volumes_name = array();
	$volumes_mountpoint = array();
	$volumes_vg = array();
	$volumes_fstype = array();

	function volumes_startelement($parser, $name, $attrs)
	{
		global $volumes_id, $volumes_name, $volumes_mountpoint, $volumes_vg, $volumes_fstype;

		if ($name == "VOLUME")
		{
			array_push($volumes_id, $attrs["ID"]);
			array_push($volumes_name, $attrs["NAME"]);
			array_push($volumes_mountpoint, $attrs["MOUNTPOINT"]);
			array_push($volumes_vg, $attrs["VG"]);
			array_push($volumes_fstype, $attrs["FSTYPE"]);
		}
	}

	function volumes_endelement($parser, $name)
	{
	}
	
	$volumes_parser = xml_parser_create();
	xml_set_element_handler($volumes_parser, "volumes_startelement", "volumes_endelement");
	$volumes_fp = fopen("/opt/openfiler/etc/volumes.xml", "r");

	while ($volumes_data = fread($volumes_fp, 4096))
		xml_parse($volumes_parser, $volumes_data, feof($volumes_fp));
		
	fclose($volumes_fp);
	xml_parser_free($volumes_parser);

	$umount_devices = array();
	$unlink_paths = array();
	$unlink_parent_paths = array();

	$snapshots_id = array();
	$snapshots_lvname = array();
	$snapshots_vgname = array();
	$snapshots_shared = array();
	$snapshots_rotateid = array();
	$snapshots_timestamp = array();
	
	$schedule_size = array();
	$schedule_share = array();
	$schedule_timecounter = array();
	$schedule_timemax = array();
	$schedule_rotatecounter = array();
	$schedule_rotatemax = array();
	$schedule_lvname = array();
	$schedule_vgname = array();

	function snapshots_startelement($parser, $name, $attrs)
	{
		global $snapshots_id, $snapshots_lvname, $snapshots_vgname, $snapshots_shared, $snapshots_rotateid, $snapshots_timestamp;
		global $schedule_size, $schedule_share;
		global $schedule_timecounter, $schedule_timemax;
		global $schedule_rotatecounter, $schedule_rotatemax;
		global $schedule_lvname, $schedule_vgname;

		if ($name == "SNAPSHOT")
		{
			array_push($snapshots_id, $attrs["ID"]);
			array_push($snapshots_lvname, $attrs["LVNAME"]);
			array_push($snapshots_vgname, $attrs["VGNAME"]);
			array_push($snapshots_shared, $attrs["SHARED"]);
			array_push($snapshots_rotateid, $attrs["ROTATEID"]);
			array_push($snapshots_timestamp, $attrs["TIMESTAMP"]);
		}
		else if ($name == "SCHEDULE")
		{
			array_push($schedule_size, $attrs["SIZE"]);
			array_push($schedule_share, $attrs["SHARE"]);
			array_push($schedule_timecounter, $attrs["TIMECOUNTER"]);
			array_push($schedule_timemax, $attrs["TIMEMAX"]);
			array_push($schedule_rotatecounter, $attrs["ROTATECOUNTER"]);
			array_push($schedule_rotatemax, $attrs["ROTATEMAX"]);
			array_push($schedule_lvname, $attrs["LVNAME"]);
			array_push($schedule_vgname, $attrs["VGNAME"]);
		}
	}

	function snapshots_endelement($parser, $name)
	{
	}
	
	$snapshots_parser = xml_parser_create();
	xml_set_element_handler($snapshots_parser, "snapshots_startelement", "snapshots_endelement");
	$snapshots_fp = fopen("/opt/openfiler/etc/snapshots.xml", "r");

	while ($snapshots_data = fread($snapshots_fp, 4096))
		xml_parse($snapshots_parser, $snapshots_data, feof($snapshots_fp));
		
	fclose($snapshots_fp);
	xml_parser_free($snapshots_parser);


	$reload_services = false;
		

	/* Now iterate through all the scheduled items */
	for ($i = 0; $i < count($schedule_lvname); $i++)
	{
		print("Processing " . $schedule_lvname[$i] . "...\n");
	
		if ($schedule_timecounter[$i] == 0)
		{
			$reload_services = true;
		
			/* The time is right to take scheduled snapshot for this volume */
			
			print("Time is right for " . $schedule_lvname[$i] . "...\n");

			/* First check and see if any snapshots out of rotation exist for this volume and remove them */
			if ($schedule_rotatecounter[$i] >= $schedule_rotatemax[$i])
			{
				$temp_snapshots_id = array();
				$temp_snapshots_lvname = array();
				$temp_snapshots_vgname = array();
				$temp_snapshots_shared = array();
				$temp_snapshots_rotateid = array();
				$temp_snapshots_timestamp = array();
	
				print("Obsolete scheduled snapshots may exist for " . $schedule_lvname[$i] . "...\n");

				for ($j = 0; $j < count($snapshots_id); $j++)
				{
					if (($schedule_lvname[$i] == $snapshots_lvname[$j])
						&& ($schedule_vgname[$i] == $snapshots_vgname[$j])
						&& ((intval($schedule_rotatecounter[$i]) - intval($schedule_rotatemax[$i])) >= intval($snapshots_rotateid[$j]))
						&& (substr($snapshots_id[$j], 0, 5) == "sched"))
					{

						print("Processing obsolete scheduled snapshot " . $snapshots_id[$j] . " for " . $schedule_lvname[$i] . "...\n");
						
						array_push($umount_devices, "/dev/" . escapeshellarg($snapshots_vgname[$j]) . "/of.snapshot." . escapeshellarg($snapshots_lvname[$j]) . "." . escapeshellarg($snapshots_id[$j]));
						array_push($unlink_paths, "/mnt/snapshots/" . escapeshellarg($snapshots_vgname[$j]) . "/" . escapeshellarg($snapshots_lvname[$j]) . "/" . escapeshellarg($snapshots_id[$j]));
						array_push($unlink_parent_paths, "/mnt/snapshots/" . escapeshellarg($snapshots_vgname[$j]) . "/" . escapeshellarg($snapshots_lvname[$j]));

						if (!$cluster_enabled)
						{
							$fstab = array();
							$fstabp = popen("/usr/bin/sudo /bin/cat /etc/fstab", "r");
							$ii = 0;
							while (!feof($fstabp))
							{
								$jj = 0;
								$fstabstr = trim(fgets($fstabp, 4096));

								if (strlen($fstabstr) <= 0)
									continue;
		
								if (substr($fstabstr, 0, 1) == "#")
									continue;

								$foo = explode(" ", $fstabstr);
		
								foreach ($foo as $fresultitem)
								{
									$fresultitem = trim($fresultitem);
									if (strlen($fresultitem) > 0)
									{
										$fstab[$ii][$jj] = $fresultitem;
										$jj++;
									}
								}

								$ii++;
							}
	
							$fstabcount = $ii;
							
							pclose($fstabp);

							$fstabp = popen("/usr/bin/sudo /usr/bin/tee /etc/fstab", "w");
		
							for ($ii = 0; $ii < $fstabcount; $ii++)
							{
								$str = "";
				
								if ($fstab[$ii][0] == "/dev/" . $snapshots_vgname[$j] . "/of.snapshot." . $snapshots_lvname[$j] . "." . $snapshots_id[$j])
									continue;
			
								for ($jj = 0; $jj < count($fstab[$ii]); $jj++)
								{
									if ($jj > 0)
										$str .= " ";
									$str .= $fstab[$ii][$jj];
								}
			
								fputs($fstabp, ($str . "\n"));
							}
		
							pclose($fstabp);
						}
					}
					else
					{
						array_push($temp_snapshots_id, $snapshots_id[$j]);
						array_push($temp_snapshots_lvname, $snapshots_lvname[$j]);
						array_push($temp_snapshots_vgname, $snapshots_vgname[$j]);
						array_push($temp_snapshots_shared, $snapshots_shared[$j]);
						array_push($temp_snapshots_rotateid, $snapshots_rotateid[$j]);
						array_push($temp_snapshots_timestamp, $snapshots_timestamp[$j]);
					}
				}
				
				$snapshots_id = $temp_snapshots_id;
				$snapshots_lvname = $temp_snapshots_lvname;
				$snapshots_vgname = $temp_snapshots_vgname;
				$snapshots_shared = $temp_snapshots_shared;
				$snapshots_rotateid = $temp_snapshots_rotateid;
				$snapshots_timestamp = $temp_snapshots_timestamp;
			}

			/* Now take a snapshot for this volume */

			$snapname = "sched" . $schedule_rotatecounter[$i];
			$snapsize = $schedule_size[$i];
			$snapshare = $schedule_share[$i];
			$vgname = $schedule_vgname[$i];
			$volume = $schedule_lvname[$i];

			$mountpath = "/mnt/snapshots/" . $vgname . "/" . $volume . "/" . $snapname;

			print("Now taking snapshot " . $mountpath . "...\n");

			if ((!is_dir($mountpath)) && (!is_file($mountpath)))
			{
				$fstype = "";

				for ($fsj = 0; $fsj < count($volumes_id); $fsj++)
				{
					if (($vgname == $volumes_vg[$fsj]) && ($volume == $volumes_id[$fsj]))
					{
						$fstype = $volumes_fstype[$fsj];
						break;
					}
				}

				$fs = 0;
				$fsfound = false;
			
				for ($fsi = 0; $fsi < count($fs_info); $fsi++)
				{
					if ($fs_info[$fsi]["type"] == $fstype)
					{
						$fs = $fsi;
						$fsfound = true;
						break;
					}
				}
			
				if ($fsfound == false)
					$fs = 0;
			

				exec("/usr/bin/sudo " . $GLOBALS["lvm_command_prefix"] . "lvcreate -s -L " . escapeshellarg($snapsize) . " -n of.snapshot." . escapeshellarg($volume) . "." . escapeshellarg($snapname) . " /dev/" . escapeshellarg($vgname) . "/" . escapeshellarg($volume));


				$lvp = popen("/usr/bin/sudo " . $GLOBALS["lvm_command_prefix"] . "lvdisplay -c /dev/" . $vgname . "/of.snapshot." . escapeshellarg($volume) . "." . escapeshellarg($snapname), "r");
				$ii = 0;
				while (!feof($lvp))
					$lvds[$ii++] = fgets($lvp, 4096);
				pclose($lvp);
			
				$lvinfo = explode(":", trim($lvds[0], " "));
			
				if ($lvinfo[0] == ("/dev/" . $vgname . "/of.snapshot." . $volume . "." . $snapname))
				{
					array_push($snapshots_id, $snapname);
					array_push($snapshots_lvname, $volume);
					array_push($snapshots_vgname, $vgname);
					array_push($snapshots_shared, $snapshare);
					array_push($snapshots_rotateid, $schedule_rotatecounter[$i]);
					array_push($snapshots_timestamp, gmdate("Y-m-d H:i:s") . " GMT");
				
					if (strcmp($fs_info[$fs]["type"] , "iscsi") != 0)	
						exec("/usr/bin/sudo /bin/mkdir -p " . escapeshellarg($mountpath));

					if (!$cluster_enabled)
					{
						$fstabp = popen("/usr/bin/sudo /usr/bin/tee -a /etc/fstab", "w");
						if (strcmp($fs_info[$fs]["type"] , "iscsi") != 0)  
							fputs($fstabp, "/dev/" . $vgname . "/of.snapshot." . $volume . "." . $snapname . " " . $mountpath . " " . $fs_info[$fs]["type"] . " " . $fs_info[$fs]["snapshot_mount_options"] . " 0 0\n");
						pclose($fstabp);

						exec("/usr/bin/sudo /bin/mount -a");
					}
					else
					{
                        			if (strcmp($fs_info[$fs]["type"] , "iscsi") != 0)  
							exec("/usr/bin/sudo /bin/mount /dev/" . $vgname . "/of.snapshot." . $volume . "." . $snapname . " " . $mountpath . " -o " . $fs_info[$fs]["snapshot_mount_options"]);
					}
				}
			}

			$schedule_rotatecounter[$i] = $schedule_rotatecounter[$i] + 1;
			$schedule_timecounter[$i] = $schedule_timemax[$i] - 1;
		}
		else
		{
			/* Wait it's not time yet for this volume */

			print("It is NOT time yet for " . $schedule_lvname[$i] . "...\n");

			$schedule_timecounter[$i] = $schedule_timecounter[$i] - 1;
		}
	}


	$snapshotsp = popen("/usr/bin/sudo /usr/bin/tee /opt/openfiler/etc/snapshots.xml", "w");
	fputs($snapshotsp, "<?xml version=\"1.0\" ?>\n");
	fputs($snapshotsp, "<snapshots>\n");

	for ($i = 0; $i < count($snapshots_id); $i++)
		fputs($snapshotsp, "\t<snapshot id=\"" . htmlentities($snapshots_id[$i])
					. "\" lvname=\"" . htmlentities($snapshots_lvname[$i])
					. "\" vgname=\"" . htmlentities($snapshots_vgname[$i])
					. "\" shared=\"" . htmlentities($snapshots_shared[$i])
					. "\" rotateid=\"" . htmlentities($snapshots_rotateid[$i])
					. "\" timestamp=\"" . htmlentities($snapshots_timestamp[$i])
					. "\" />\n");

	for ($i = 0; $i < count($schedule_vgname); $i++)
		fputs($snapshotsp, "\t<schedule"
					. " size=\"" . htmlentities($schedule_size[$i])
					. "\" share=\"" . htmlentities($schedule_share[$i])
					. "\" timecounter=\"" . htmlentities($schedule_timecounter[$i])
					. "\" timemax=\"" . htmlentities($schedule_timemax[$i])
					. "\" rotatecounter=\"" . htmlentities($schedule_rotatecounter[$i])
					. "\" rotatemax=\"" . htmlentities($schedule_rotatemax[$i])
					. "\" lvname=\"" . htmlentities($schedule_lvname[$i])
					. "\" vgname=\"" . htmlentities($schedule_vgname[$i])
					. "\" />\n");

	fputs($snapshotsp, "</snapshots>\n\n");
	pclose($snapshotsp);

	if ($cluster_enabled && $reload_services)
	{

	/* The following won't work because services are controlled by heartbeat in cluster mode */
	/*	$service_winbind = (strstr(exec("/usr/bin/sudo /sbin/chkconfig --list winbind"), "3:on") ? 1 : 0);
		$service_smb = (strstr(exec("/usr/bin/sudo /sbin/chkconfig --list smb"), "3:on") ? 1 : 0);
		$service_nfsv3 = (strstr(exec("/usr/bin/sudo /sbin/chkconfig --list nfs"), "3:on") ? 1 : 0);
		$service_http = (strstr(exec("/usr/bin/sudo /sbin/chkconfig --list httpd"), "3:on") ? 1 : 0);
        	$service_iscsi = (strstr(exec("/usr/bin/sudo /sbin/chkconfig --list iscsi-target"), "3:on") ? 1 : 0);
        	$service_ftp = (strstr(exec("/usr/bin/sudo /sbin/chkconfig --list proftpd"), "3:on") ? 1 : 0);
	*/

		$service_winbind = (strstr(exec("/usr/bin/sudo /sbin/service winbind status"), "running") ? 1 : 0);
		$service_smb = (strstr(exec("/usr/bin/sudo /sbin/service smb status"), "running") ? 1 : 0);
		$service_nfsv3 = (strstr(exec("/usr/bin/sudo /sbin/service nfs status"), "running") ? 1 : 0);
		$service_http = (strstr(exec("/usr/bin/sudo /sbin/service httpd status"), "running") ? 1 : 0);
		$service_ftp = (strstr(exec("/usr/bin/sudo /sbin/service proftpd status"), "running") ? 1 : 0);
		$service_iscsi = (strstr(exec("/usr/bin/sudo /sbin/service iscsi-target status"), "running") ? 1 : 0);
                $service_rsync = (strstr(exec("/usr/bin/sudo /sbin/service rsync status"), "running") ? 1 : 0); 
		
		$clusterp = popen("/usr/bin/sudo /usr/bin/tee /etc/ha.d/haresources", "w");

		$cluster_resource_str = "";

		for ($i = 0; $i < count($cluster_resource); $i++)
			$cluster_resource_str .= " " . $cluster_resource[$i];

		for ($i = 0; $i < count($volumes_id); $i++)
		{
			$fstype = $volumes_fstype[$i];
			if ($fstype != "iscsi")
			{
			$fs = 0;
			$fsfound = false;
			
			for ($fsi = 0; $fsi < count($fs_info); $fsi++)
			{
				if ($fs_info[$fsi]["type"] == $fstype)
				{
					$fs = $fsi;
					$fsfound = true;
					break;
				}
			}
			
			if ($fsfound == false)
				$fs = 0;

			$cluster_resource_str .= " Filesystem::/dev/" . $volumes_vg[$i] . "/" . $volumes_id[$i] . "::" . "/mnt/" . $volumes_vg[$i] . "/" . $volumes_id[$i] . "::" . $fs_info[$fs]["type"] . "::" . $fs_info[$fs]["mount_options"];
			}
		}

		for ($i = 0; $i < count($snapshots_id); $i++)
		{
			$fstype = "";
	
			for ($fsj = 0; $fsj < count($volumes_id); $fsj++)
			{
				if (($snapshots_vgname[$i] == $volumes_vg[$fsj]) && ($snapshots_lvname[$i] == $volumes_id[$fsj]))
				{
					$fstype = $volumes_fstype[$fsj];
					break;
				}
			}

			$fs = 0;
			$fsfound = false;
			
			for ($fsi = 0; $fsi < count($fs_info); $fsi++)
			{
				if ($fs_info[$fsi]["type"] == $fstype)
				{
					$fs = $fsi;
					$fsfound = true;
					break;
				}
			}
			
			if ($fsfound == false)
				$fs = 0;
			
			if ($fstype != "iscsi")
				$cluster_resource_str .= " Filesystem::/dev/" . $snapshots_vgname[$i] . "/of.snapshot." . $snapshots_lvname[$i] . "." . $snapshots_id[$i] . "::" . "/mnt/snapshots/" . $snapshots_vgname[$i] . "/" . $snapshots_lvname[$i] . "/" . $snapshots_id[$i] . "::" . $fs_info[$fs]["type"] . "::" . $fs_info[$fs]["snapshot_mount_options"];
		}

		if ($service_winbind > 0)
			$cluster_resource_str .= " winbind";
		if ($service_http > 0)
			$cluster_resource_str .= " httpd";
		if ($service_nfsv3 > 0)
			$cluster_resource_str .= " nfs";
		if ($service_smb > 0)
			$cluster_resource_str .= " smb";
                if ($service_ftp > 0)
                        $cluster_resource_str .= " proftpd";
                if ($service_iscsi > 0)
                        $cluster_resource_str .= " iscsi-target";
                if ($service_rsync > 0)
                        $cluster_resource_str .= " rsync";

		$cluster_resource_str .= " openfiler";

		fputs($clusterp, $cluster_nodename . $cluster_resource_str . "\n");
		pclose($clusterp);
	}

	if ($reload_services)
		apply_configuration(array("services" => "reload", "chmod" => "no", "chmod_path" => ""));


	/* Now unmount the volumes to be removed, delete them and their mountpaths */
	
	for ($i = 0; $i < count($umount_devices); $i++)
	{
		/* Unmount the snapshot volume and remove it */
		exec("/usr/bin/sudo /bin/umount -f " . $umount_devices[$i], $umount_output, $umount_result);
	
		if ($umount_result == 0)
		{
			exec("/usr/bin/sudo " . $GLOBALS["lvm_command_prefix"] . "lvremove -f " . $umount_devices[$i]);
			exec("/usr/bin/sudo /bin/rmdir --ignore-fail-on-non-empty " . $unlink_paths[$i]);
			exec("/usr/bin/sudo /bin/rmdir -p --ignore-fail-on-non-empty " . $unlink_parent_paths[$i]);
		}

        else /* TODO: confirm whether we're snapshot of iscsi volume */ 
        {
            /* umount failed, so we're snapshot of an iscsi volume */

			exec("/usr/bin/sudo " . $GLOBALS["lvm_command_prefix"] . "lvremove -f " . $umount_devices[$i]);
      
        }
	}

	/* Release the lock */
	
	flock($flockp, LOCK_UN);
	fclose($flockp);
	
?>
