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

function getOpenfilerVersion($versionQuery) {

	
	if ($versionQuery == "distro") {
	
		$distrofile = "/etc/distro-release";
		$dhandle = fopen($distrofile, "r");
        $of_distro_version = fread($dhandle, filesize($distrofile));
        fclose($dhandle);
        return $of_distro_version; 
	}
	
    else if ($versionQuery == "gui") {
	
	
        $of_versionp = popen('/usr/bin/conary q openfiler | /bin/cut -d "=" -f2', 'r');
        $of_info_version = trim(fread($of_versionp, 1024));
        pclose($of_versionp);
        return $of_info_version;
    }
	
}

$of_info_version = getOpenfilerVersion("gui"); 

?>
