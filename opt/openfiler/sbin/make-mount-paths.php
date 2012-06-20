<?php

/*
 * Make mount paths from volumes.xml
 *
 * Copyright (C) 2001-2005 Voluna Software. All rights reserved.
 *
 */

	$specialpaths_filename = array();

	function specialpaths_startelement($parser, $name, $attrs)
	{
		global $specialpaths_filename;

		if ($name == "DIRECTORY")
			array_push($specialpaths_filename, $attrs["FILENAME"]);
	}

	function specialpaths_endelement($parser, $name)
	{
	}
	
	$specialpaths_parser = xml_parser_create();
	xml_set_element_handler($specialpaths_parser, "specialpaths_startelement", "specialpaths_endelement");
	$specialpaths_fp = fopen("/opt/openfiler/etc/specialpaths.xml", "r");

	while ($specialpaths_data = fread($specialpaths_fp, 4096))
		xml_parse($specialpaths_parser, $specialpaths_data, feof($specialpaths_fp));
		
	fclose($specialpaths_fp);
	xml_parser_free($specialpaths_parser);


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


	$snapshots_id = array();
	$snapshots_lvname = array();
	$snapshots_vgname = array();
	$snapshots_shared = array();
	$snapshots_rotateid = array();
	
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
		global $snapshots_id, $snapshots_lvname, $snapshots_vgname, $snapshots_shared, $snapshots_rotateid;
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
	

	/* First remove any mountpaths which already exist on the secondary (when it becomes primary) */
	
	exec("/usr/bin/sudo /bin/rmdir /mnt/snapshots/*/*/*");
	exec("/usr/bin/sudo /bin/rmdir /mnt/snapshots/*/*");
	exec("/usr/bin/sudo /bin/rmdir /mnt/snapshots/*");
	exec("/usr/bin/sudo /bin/rmdir /mnt/snapshots");

	$handle = opendir("/mnt/");
	
	while ($file = readdir($handle))
	{
		if (($file == ".") || ($file == ".."))
			continue;
		
		$ignore_file = false;
		
		for ($i = 0; $i < count($specialpaths_filename); $i++)
			if ($specialpaths_filename[$i] == $file)
			{
				$ignore_file = true;
				break;
			}
		
		if ($ignore_file)
			continue;
		
		exec("/usr/bin/sudo /bin/rmdir /mnt/" . escapeshellarg($file) . "/*");
		exec("/usr/bin/sudo /bin/rmdir /mnt/" . escapeshellarg($file));
	}

	closedir($handle);
	

	/* Now make the mountpath directories for the volumes */
	
	for ($i = 0; $i < count($volumes_id); $i++)
		if (!is_dir($volumes_mountpoint[$i]))
			exec("/usr/bin/sudo /bin/mkdir -p " . escapeshellarg($volumes_mountpoint[$i]));

	for ($i = 0; $i < count($snapshots_id); $i++)
		if (!is_dir("/mnt/snapshots/" . $snapshots_vgname[$i] . "/" . $snapshots_lvname[$i] . "/" . $snapshots_id[$i]))
			exec("/usr/bin/sudo /bin/mkdir -p " . escapeshellarg("/mnt/snapshots/" . $snapshots_vgname[$i] . "/" . $snapshots_lvname[$i] . "/" . $snapshots_id[$i]));

?>
