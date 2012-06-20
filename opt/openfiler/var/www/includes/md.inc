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


function get_md()
{
	$mdstat = file("/proc/mdstat");
	$md = array();

	$c = 0;

	for ($i = 0; $i < count($mdstat); $i++)
	{
		if (preg_match("/^(md\d+)/", $mdstat[$i], $matches) > 0)
		{
			$md_device = "/dev/" . $matches[1];
			
			$mdp = popen("export LANG=C; /usr/bin/sudo /sbin/mdadm --detail --brief --openfiler " . escapeshellarg($md_device), "r");

			$j = 0;

			while (!feof($mdp))
				$md_result[$j++] = trim(fgets($mdp, 4096));

			pclose($mdp);
			
			if (count($md_result) <= 0)
				continue;

			if ($md_device != $md_result[0])
				continue;

			$tmp = split(" ", $md_result[1]);

			$md_major_version = intval(trim($tmp[0]));
			$md_minor_version = intval(trim($tmp[1]));
			$md_patch_version = intval(trim($tmp[2]));

			$md_ctime = strtotime($md_result[2]);

			$tmp = split(" ", $md_result[3]);

			$md_level = intval(trim($tmp[0]));
			$md_layout = intval(trim($tmp[1]));
			$md_disks = intval(trim($tmp[2]));

			$tmp = split(" ", $md_result[4]);

			$md_chunk_size = trim($tmp[0]);
			$md_array_size = trim($tmp[1]);
			$md_device_size = trim($tmp[2]);

			$md_raid_disks = intval($md_result[5]);
			$md_total_disks = intval($md_result[6]);
			$md_active_disks = intval($md_result[7]);
			$md_working_disks = intval($md_result[8]);
			$md_failed_disks = intval($md_result[9]);
			$md_spare_disks = intval($md_result[10]);

			$md_is_not_persistent = intval($md_result[11]);

			$tmp = split(" ", $md_result[12]);

			$md_state = intval(trim($tmp[0]));
			$md_resync = intval(trim($tmp[1]));

			$md_rebuild_percent = intval($md_result[13]);

			$k = 14;
			$dev_i = 0;
			$md_members = array();

			while (strncmp($md_result[$k], "DISK ", 5) == 0)
			{
				$tmp = split(" ", substr($md_result[$k], 5, strlen($md_result[$k]) - 5));

				$md_disk_number = intval(trim($tmp[0]));
				$md_disk_member = intval(trim($tmp[1]));
				$md_disk_state = intval(trim($tmp[2]));

				if (isset($tmp[3]))
					$md_disk_device = trim($tmp[3]);
				else
					$md_disk_device = "";

				$md_members[$dev_i] = array("device" => $md_disk_device, "number" => $md_disk_number, "member" => $md_disk_member, "state" => $md_disk_state);

				$k = $k + 1;
				$dev_i = $dev_i + 1;
			}
			
			$md[$c] = array(
					"device" => $md_device,
					"major_version" => $md_major_version,
					"minor_version" => $md_minor_version,
					"patch_version" => $md_patch_version,
					"ctime" => $md_ctime,

					"level" => $md_level,
					"layout" => $md_layout,
					"disks" => $md_disks,

					"chunk_size" => $md_chunk_size,
					"array_size" => $md_array_size,
					"device_size" => $md_device_size,

					"raid_disks" => $md_raid_disks,
					"total_disks" => $md_total_disks,
					"active_disks" => $md_active_disks,
					"working_disks" => $md_working_disks,
					"failed_disks" => $md_failed_disks,
					"spare_disks" => $md_spare_disks,

					"state" => $md_state,
					"resync" => $md_resync,
					"is_not_persistent" => $md_is_not_persistent,
					"rebuild_percent" => $md_rebuild_percent,

					"members" => $md_members
				);

			$c = $c + 1;

		}
	}
	return $md;
}

function update_mdadm_conf()
{
	global $md_info_notify_email;
	global $md_info_sender_email;
	global $md_info_audible_alert;

	$md_info_notify_email = "";
	$md_info_sender_email = "";
	$md_info_audible_alert = "";

	function md_info_startelement($md_parser, $md_name, $md_attrs)
	{
		global $md_info_notify_email;
		global $md_info_sender_email;
		global $md_info_audible_alert;

		if ($md_name == "NOTIFY-EMAIL")
			$md_info_notify_email = $md_attrs["ADDRESS"];
		if ($md_name == "SENDER-EMAIL")
			$md_info_sender_email = $md_attrs["ADDRESS"];
		if ($md_name == "AUDIBLE-ALERT")
			$md_info_audible_alert = $md_attrs["STATUS"];
	}

	function md_info_endelement($md_parser, $md_name)
	{
	}
	
	$md_info_parser = xml_parser_create();
	xml_set_element_handler($md_info_parser, "md_info_startelement", "md_info_endelement");
	$md_info_fp = fopen("/opt/openfiler/etc/info.xml", "r");

	while ($md_info_data = fread($md_info_fp, 4096))
		xml_parse($md_info_parser, $md_info_data, feof($md_info_fp));
		
	fclose($md_info_fp);
	xml_parser_free($md_info_parser);

	$md = get_md();


	$mdadm_fp = popen("export LANG=C; /usr/bin/sudo /usr/bin/tee /etc/mdadm.conf", "w");

	fputs($mdadm_fp, "\n");
	fputs($mdadm_fp, "#\n");
	fputs($mdadm_fp, "# PLEASE DO NOT MODIFY THIS CONFIGURATION FILE!\n");
	fputs($mdadm_fp, "#   This configuration file was auto-generated\n");
	fputs($mdadm_fp, "#   by Openfiler. Please do not modify it.\n");
	fputs($mdadm_fp, "#\n");
	fputs($mdadm_fp, "# Generated at: " . date("D M j G:i:s T Y") . "\n");
	fputs($mdadm_fp, "#\n");
	fputs($mdadm_fp, "\n");

	fputs($mdadm_fp, "DEVICE partitions\n");

	//if (strlen($md_info_notify_email) > 0)
	//	fputs($mdadm_fp, "MAILADDR " . $md_info_notify_email . "\n");

	for ($i = 0; $i < count($md); $i++) {

                $uuidcmd =  "export LANG=C; /usr/bin/sudo mdadm -D " . $md[$i]["device"] . " | grep UUID | cut -f 2-5 -d \":\"";
                $uuidfd = popen($uuidcmd, r);
                $uuid = fgets($uuidfd, 4096);
                pclose($uuidfd);
                        
                
		fputs($mdadm_fp, "ARRAY " . $md[$i]["device"] . " UUID=" . trim($uuid) . "\n");

        }
		
	fputs($mdadm_fp, "PROGRAM /opt/openfiler/bin/mdalert \n");
	
	pclose($mdadm_fp);
	exec("export LANG=C; /usr/bin/sudo /sbin/service mdmonitor restart 2>&1");

	
}

?>
