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


define('CONFIG_SMB', "/etc/samba/smb.conf");
define('CONFIG_NFS', "/etc/exports");
define('CONFIG_FTP', "/etc/proftpd/proftpd.conf");
define('CONFIG_FTP_SHARES', "/etc/proftpd/openfiler-shares.conf");
define('CONFIG_HTTP_SHARES', "/etc/httpd/conf.d/openfiler-shares.conf");
define('CONFIG_RSYNC', "/etc/rsyncd.conf");
define('CONFIG_IETD', "/etc/ietd.conf");
define('CONFIG_INIT_ALLOW', "/etc/initiators.allow");
define('CONFIG_INIT_DENY', "/etc/initiators.deny");
define('SETTING_XML_CLUSTER', "/opt/openfiler/etc/cluster.xml");
define('SETTING_XML_RSYNC', "/opt/openfiler/etc/rsync_settings.xml");
define('SETTING_XML_SMB', "/opt/openfiler/etc/smb_settings.xml");
define('SETTING_XML_FTP', "/opt/openfiler/etc/ftp_settings.xml");
define('SETTING_XML_NETWORK', "/opt/openfiler/etc/networks.xml");
define('SETTING_XML_IET_TARGETS', "/opt/openfiler/etc/iscsi/targets/iscsi_settings.xml");
define('SETTING_XML_VOLUME', "/opt/openfiler/etc/volumes.xml");
define('SETTING_XML_SNAPSHOT', "/opt/openfiler/etc/snapshots.xml");
define('SETTING_XML_CLUSTER_RSYNC', "/opt/openfiler/etc/rsync.xml");
define('SETTING_XML_IDMAPADSYNC', "/opt/openfiler/etc/idmap_ad.xml"); 
require_once('authconfig.inc.php'); 


function convert_netmask($netmask) {
	$intmask = ip2long($netmask);
					
	if ($intmask == 0)
		$network_suffix = "/0";
	else if ($intmask == (intval(pow(2, 32)) - 1))
		$network_suffix = "";
	else {
		$pos = -1;
		for ($pt = 31; $pt >= 0; $pt--)
		if (($intmask & intval(pow(2, $pt))) == 0) {
			$pos = $pt;
			break;
		}
						
		if ($pos != -1)
			$network_suffix = "/" . (32 - $pos - 1);
		else
			$network_suffix = "";
	}
	return $network_suffix;
}



function apply_configuration($params = array()) {
	
	/* begin variables */
	
	/* -------------------------- */
	
	global 	$ac_request_chmod, $ac_request_chmod_path, $ac_unique_descriptions,
		$ac_unique_description_counts, $acx_groups_gid, $acx_groups_name,
		$global_min_gid, $ac_aresult;
	
	$ac_unique_descriptions = array();
	$ac_unique_description_counts = array();
	$acx_groups_gid = array();
	$acx_groups_name = array();
	
	
	/* -------------------------- */
	
	global $ac_cluster_enabled, $ac_cluster_nodename, $ac_cluster_resource;
    
	$ac_cluster_enabled = false;
	$ac_cluster_nodename = "";
	$ac_cluster_resource = array();
	
	/* -------------------------- */
	
	global 	$ac_volumes_id, $ac_volumes_name, $ac_volumes_mountpoint,
		$ac_volumes_vg, $ac_volumes_fstype;
	
	$ac_volumes_id = array();
	$ac_volumes_name = array();
	$ac_volumes_mountpoint = array();
	$ac_volumes_vg = array();
	$ac_volumes_fstype = array();
	
	/* -------------------------- */
	
	global	$ac_targets_name;
	
	$ac_targets_name = array();
	
	/* -------------------------- */

	global 	$ac_snapshots_suffix, $ac_snapshots_id, $ac_snapshots_lvname,
		$ac_snapshots_vgname, $ac_snapshots_shared, $ac_snapshots_rotateid,
		$ac_snapshots_timestamp;

	$ac_snapshots_id = array();
	$ac_snapshots_lvname = array();
	$ac_snapshots_vgname = array();
	$ac_snapshots_shared = array();
	$ac_snapshots_rotateid = array();
	$ac_snapshots_timestamp = array();
	
	/* -------------------------- */
	
	global	$ac_serverstring, $ac_netbiosname, $ac_winsserver, $ac_idmapsync,
		$ac_ldapidmaptls, $ac_ldapidmapserver, $ac_ldapidmapsuffix,
		$ac_ldapusersuffix, $ac_ldapgroupsuffix, $ac_homesvolume_vg, $ac_homesvolume_lv,
		$ac_homesvolume_mountpoint, $ac_snapshot_homes,
		$ac_winbind_encryptedpasswords, $ac_winbindpolicy, $ac_unixcharset,
		$ac_doscharset, $ac_displaycharset, $ac_smbstoredosattributes, $ac_smbmapaclinherit,
        $ac_winbindseparator;
			
			
	$ac_serverstring = "";
	$ac_netbiosname = "";
	$ac_winsserver = "";
	$ac_idmapsync = "nosync";
	$ac_ldapidmaptls = FALSE;
	$ac_ldapidmapserver = "";
	$ac_ldapidmapsuffix = "ou=Idmap";
	$ac_ldapusersuffix = "ou=People"; 
        $ac_ldapgroupsuffix = "ou=Group";
	$ac_homesvolume_vg = "";
	$ac_homesvolume_lv = "";
	$ac_homesvolume_mountpoint = "";
	$ac_snapshot_homes = FALSE; 
	$ac_winbind_encryptedpasswords = 1;
	$ac_winbindpolicy = "yes";
	$ac_unixcharset = "UTF8";
	$ac_doscharset = "850";
	$ac_displaycharset = "UTF8";
	$ac_ldapidmapserver = "127.0.0.1";
    	$ac_smbstoredosattributes = "yes";
    	$ac_smbmapaclinherit = "yes";
    	$ac_winbindseparator = "\\";
	
	/* -------------------------- */
	
	global 	$ac_networks_count, $ac_networks_name, $ac_networks_network,
                $ac_networks_netmask, $ac_networks_access, $ac_groups_access,
                $ac_groups_primary, $ac_share_accesspublic, $ac_smb_oplocks,
                $ac_smb_sharename, $ac_smb_force_security_mode, $ac_smb_dos_filemode,
                $ac_smb_dos_filetime_resolution, $ac_smb_dos_filetimes,
                $ac_smb_csc_policy, $ac_smb_browseable,
                $ac_smb_fake_directory_create_times; 

	$ac_share_accesspublic = 0;
	$ac_networks_count = 0;
	$ac_networks_name = array();
	$ac_networks_network = array();
	$ac_networks_netmask = array();
	$ac_networks_access = array();
	
	/* -------------------------- */
	
	global	$ac_smb_fp, $ac_nfsv3_fp, $ac_ftp_fp,
		$ac_ftp_conf_fp, $ac_http_fp, $ac_iscsi_fp, $ac_rsync_fp;
			
	/* -------------------------- */
	
	global $ac_rsync_port, $ac_rsync_motd_file, $ac_rsync_address;
	
	$ac_rsync_address = ""; 

	/* -------------------------- */

	global $ac_homespathdefined;
	global $ac_ishomepath;
	global $ac_homespath;
	$ac_homespathdefined = FALSE;
	$ac_ishomepath = FALSE;
	$ac_homespath = ""; 

	
	/* end variables */
	
	define('CONFIG_XML_HOMESPATH', "/opt/openfiler/etc/homespath.xml");
	
	if ($homespathDom = new XmlHandler(CONFIG_XML_HOMESPATH)) {
		if ($homespathNode = $homespathDom->getElementsByTagName("homespath")->item(0)) {
			$homespath = $homespathNode->getAttribute("value");
			if (strlen($homespath) > 0) {
				$ac_homespath = $homespath; 
				$ac_homespathdefined = TRUE;			 
			}
		}
	}

	if ($params["chmod"] == "yes") {
		$ac_request_chmod = true;
		$ac_request_chmod_path = $params["chmod_path"];
	}
	
	else {
		$ac_request_chmod = false;
		$ac_request_chmod_path = "";
	}
	
	endgrent();
	
	while ($acx_group_info = getgrent()) {
		if  (($acx_group_info["gr_gid"] >= $global_min_gid)) {
			array_push($acx_groups_gid, $acx_group_info["gr_gid"]);
			array_push($acx_groups_name, $acx_group_info["gr_name"]);
		}
	}
	
	endgrent();

	
   	$authcfg_obj = new Authconfig();  


	

    if ($clusterDom = new XmlHandler(SETTING_XML_CLUSTER)) {
        
        /* check whether clustering is enabled */
        
        $xPathState = "//clustering";
        if ($node = $clusterDom->runXpathQuery($xPathState)->item(0)) {
            if ($node->getAttribute("state") == "on") {
                $ac_cluster_enabled = true; 
            }
        }
        
        /* if cluster enabled is true, continue getting settings */
        
        if ($ac_cluster_enabled) {
            
            /* get cluster node name */
            
            $xPathNodeName = "//nodename";
            if ($node = $clusterDom->runXpathQuery($xPathNodeName)->item(0)) {
                if (($attr = $node->getAttribute("value")) != "") {
                    $ac_cluster_nodename = $attr; 
                }                
            }
            
            /* end get cluster node name */
            
            /* get cluster resources */
            
            
            $xPathResources = "//resources";
            if ($nodes = $clusterDom->runXpathQuery($xPathResources)) {        
                foreach($nodes as $node) {   
                    if ($value = $node->getAttribute("value"))
                        array_push($ac_cluster_resource, $value);   
                }
            }
            
            /* end get cluster resources */
            
            
            /* get cluster rsync paths */
            
            global $ac_cluster_rsync_paths, $ac_cluster_rsync_hosts;
            
            $ac_cluster_rsync_paths = array();
            $ac_cluster_rsync_hosts = array();
            
            if ($clusterRsyncDom = new XmlHandler(SETTING_XML_CLUSTER_RSYNC)) {
                
                $xPathHosts = "//remote";
                if ($remoteNodes = $clusterRsyncDom->runXpathQuery($xPathHosts)) {                
                    foreach($remoteNodes as $remoteNode) {
                        if($attr = $remoteNode->getAttribute("hostname")) {
                            array_push($ac_cluster_rsync_hosts, $attr); 
                        }
                    }
                }
                
                $xPathItems = "//item";
                if ($itemNodes = $clusterRsyncDom->runXpathQuery($xPathItems)) {
                    foreach($itemNodes as $itemNode) {
                        if ($attr = $itemNode->getAttribute("path")) {
                            array_push($ac_cluster_rsync_paths, $attr); 
                        }
                    }
                }
            }
        
            /* end get cluster rsync paths */
        }
    }
	
	

	
	/* end check for cluster settings */
	
	
	
	if ($volumesDom = new XmlHandler(SETTING_XML_VOLUME)) {
		$xPathVolume = "//volume";
		if ($volumeNodes = $volumesDom->runXpathQuery($xPathVolume)) {
			foreach($volumeNodes as $volumeNode) {
				array_push($ac_volumes_id, $volumeNode->getAttribute("id"));
				array_push($ac_volumes_name, $volumeNode->getAttribute("name"));
				array_push($ac_volumes_mountpoint, $volumeNode->getAttribute("mountpoint"));
				array_push($ac_volumes_vg, $volumeNode->getAttribute("vg"));
				array_push($ac_volumes_fstype, $volumeNode->getAttribute("fstype")); 
			}
		}
	}
	
	

	
	
	if ($ietDom = new XmlHandler(SETTING_XML_IET_TARGETS)) {
		$xPathTargets = "//target";
		if ($targetNodes = $ietDom->runXpathQuery($xPathTargets)) {
			foreach($targetNodes as $targetNode) {
				if ($targetNode->hasAttribute("Name"))
					array_push($ac_targets_name, $targetNode->getAttribute("Name")); 
			}
		}
	}


	
	
	if ($snapshotsDom = new XmlHandler(SETTING_XML_SNAPSHOT)) {
		$xPathSnapshot = "//snapshot";
		if ($snapshotNodes = $snapshotsDom->runXpathQuery($xPathSnapshot)) {
			foreach($snapshotNodes as $snapshotNode) {
				array_push($ac_snapshots_id, $snapshotNode->getAttribute("id"));
				array_push($ac_snapshots_lvname, $snapshotNode->getAttribute("lvname"));
				array_push($ac_snapshots_vgname, $snapshotNode->getAttribute("vgname"));
				array_push($ac_snapshots_shared, $snapshotNode->getAttribute("shared"));
				array_push($ac_snapshots_rotateid, $snapshotNode->getAttribute("rotateid"));
				array_push($ac_snapshots_timestamp, $snapshotNode->getAttribute("timestamp")); 
				
			}
		}
	}




	if ($rsyncXmlDom = new XmlHandler(SETTING_XML_RSYNC)) {
		
		if ($node = $rsyncXmlDom->getElementsByTagName("port")->item(0)) {
			$ac_rsync_port = $node->getAttribute("number"); 
		}
		
		if ($node = $rsyncXmlDom->getElementsByTagName("motd")->item(0)) {
			$ac_rsync_motd_file = $node->getAttribute("file"); 
		}
		
		if ($node = $rsyncXmlDom->getElementsByTagName("address")->item(0)) {
			$attr = $node->getAttribute("ip");
			if (strlen($attr) > 0) {
				$ac_rsync_address = $attr; 
			}
		}
		
	}
	
	if ($smbXmlDom = new XmlHandler(SETTING_XML_SMB)) {
		if ($node = $smbXmlDom->getElementsByTagName("serverstring")->item(0)) {
			$ac_serverstring = $node->getAttribute("value"); 
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("netbiosname")->item(0)) {
			$ac_netbiosname = $node->getAttribute("value"); 
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("winsserver")->item(0)) {
			$ac_winsserver = $node->getAttribute("value"); 
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("idmapsync")->item(0)) {
			$attr = $node->getAttribute("value");
			$ac_idmapsync = $attr;
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("ldapidmaptls")->item(0)) {
			$attr = $node->getAttribute("value");
			$ac_ldapidmaptls = ($attr == "on") ? TRUE : FALSE; 
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("ldapidmapserver")->item(0)) {
			$attr = $node->getAttribute("value");
			if (strlen($attr) != 0)
				$ac_ldapidmapserver = $attr; 
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("ldapidmapsuffix")->item(0)) {
			$attr = $node->getAttribute("value");
			if (strlen($attr) != 0)
				$ac_ldapidmapsuffix = $attr; 
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("ldapusersuffix")->item(0)) {
			$attr = $node->getAttribute("value");
			if (strlen($attr) > 0)
				$ac_ldapusersuffix = $attr; 
		}

                if ($node = $smbXmlDom->getElementsByTagName("ldapgroupsuffix")->item(0)) {
                        $attr = $node->getAttribute("value");
                        if (strlen($attr) > 0)
                                $ac_ldapgroupsuffix = $attr;
                                                                                                                                }
		
		if ($node = $smbXmlDom->getElementsByTagName("homesvolume")->item(0)) {
			$ac_homesvolume_vg = $node->getAttribute("vg");
			$ac_homesvolume_lv = $node->getAttribute("lv");
			$ac_homesvolume_mountpoint = $node->getAttribute("mountpoint"); 
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("snapshothomes")->item(0)) {
			if ($node->getAttribute("value") == "on") {
				$ac_snapshot_homes = TRUE; 
			}
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("encryptedpasswords")->item(0)) {
			if ($node->getAttribute("value") == "no") {
				$ac_winbind_encryptedpasswords = FALSE; 
			}
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("doscharset")->item(0)) {
			$attr = $node->getAttribute("value");
			if (strlen($attr) > 0)
				$ac_doscharset = $attr; 
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("unixcharset")->item(0)) {
			$attr = $node->getAttribute("value");
			if (strlen($attr) > 0)
				$ac_unixcharset = $attr;
		}
		
		if ($node = $smbXmlDom->getElementsByTagName("displaycharset")->item(0)) {
			$attr = $node->getAttribute("value");
			if (strlen($attr) > 0)
				$ac_displaycharset = $attr; 
		}

                if ($node = $smbXmlDom->getElementsByTagName("winbindpolicy")->item(0)) {
                        $attr = $node->getAttribute("value");
                        if (strlen($attr) > 0)
                                $ac_winbindpolicy = $attr; 
                }
        
                if ($node = $smbXmlDom->getElementsByTagName("winbindseparator")->item(0)) {
			$attr = $node->getAttribute("value");
			if (strlen($attr) > 0)
			    $ac_winbindseparator = $attr;
                        else
                            $ac_winbindseparator = "\\";
		}
        
	
    }


	if ($networksDom = new XmlHandler(SETTING_XML_NETWORK)) {
		$xPathNetwork = "//network";
		if ($networkNodes = $networksDom->runXpathQuery($xPathNetwork)) {
			foreach($networkNodes as $networkNode) {
				$ac_networks_name[$ac_networks_count] = $networkNode->getAttribute("name");
				$ac_networks_network[$ac_networks_count] = $networkNode->getAttribute("network");
				$ac_networks_netmask[$ac_networks_count] = $networkNode->getAttribute("netmask");
				$ac_networks_access[$ac_networks_count] = "n          ";
				$ac_networks_count++; 
			}
		}
	}



	function ac_recurse_dir($ac_mountpoint, $ac_lv, $ac_vg, $ac_is_snapshot,
				$ac_chmod, $ac_chmod_path, $ac_share_path) {
		
		
		global 	$ac_dir_description, $ac_dir_type, $ac_dir_count, $ac_groups_access,
			$ac_groups_primary, $ac_share_accesspublic;
				
		global 	$ac_smb_oplocks, $ac_smb_sharename, $ac_smb_force_security_mode,
			$ac_smb_dos_filemode, $ac_smb_dos_filetime_resolution,
			$ac_smb_dos_filetimes, $ac_smb_csc_policy, $ac_smb_browseable,
			$ac_smb_fake_directory_create_times;
				
		global	$ac_networks_count, $ac_networks_name, $ac_networks_network,
			$ac_networks_netmask, $ac_networks_access;
				
		global 	$ac_nfsv3_fp, $ac_smb_fp, $ac_afp_fp, $ac_ftp_fp, $ac_ftp_conf_fp,
			$ac_http_fp, $ac_rsync_fp;
				
		global 	$acx_groups_gid, $acx_groups_name,$ac_unique_descriptions,
			$ac_unique_description_counts, $ac_snapshots_suffix;
			
		global 	$ac_snapshots_id, $ac_snapshots_lvname, $ac_snapshots_vgname,
			$ac_snapshots_shared, $ac_snapshots_rotateid, $ac_snapshots_timestamp;
				
		global 	$ac_rsync_max_connections, $ac_rsync_list, $ac_rsync_readonly,
			$ac_rsync_write_only, $ac_rsync_use_chroot, $ac_rsync_comment, $ac_rsync_fake_super;
				
		global 	$ac_homespath, $ac_homespathdefined, $ac_ishomespath;

		global $ac_smbmapaclinherit, $ac_smbstoredosattributes; 
		
		$ac_handle = opendir($ac_mountpoint); 
		
		while (false !== ($ac_file = readdir($ac_handle))) {
			if (($ac_file == ".") || ($ac_file == "..") || (!is_dir($ac_mountpoint . $ac_file)))
				continue;
			if (!is_file($ac_mountpoint . $ac_file . ".info.xml"))
				continue;
			
			$ac_dir_type = "";
			$ac_dir_description = "";
			$ac_dir_count = 0;
			$ac_groups_access = array();
			$ac_groups_primary = 0;
            		$ac_share_accesspublic = 0; // NB: fix for #661
            		$ac_networks_nfs = array();
            		$nfsAttrList = array("anonuid"=>"99", "anongid"=>"99", "insecure"=>"no",
                         "secure"=>"yes",
                         "no_root_squash"=>"no", "all_squash"=>"no",
                         "root_squash"=>"yes",
                         "no_wdelay"=>"yes", "wdelay"=>"no", "sync"=>"yes",
                         "async"=>"no");
            
            		$nfs_share_entry = "";
            		$nfsAttrString = "";
			
			for ($i = 0; $i < count($ac_networks_network); $i++)
				$ac_networks_access[$i] = "n          ";
			
			$shareXml = $ac_mountpoint . $ac_file . ".info.xml"; 
			
		
			
			if ($ac_homespath == $ac_mountpoint . $ac_file . "/")
				$ac_ishomepath = TRUE;
			
			if ($shareDom = new XmlHandler($shareXml)) {  //begin check for share xml
				
				$keyNodes = $shareDom->getElementsByTagName("key");
				foreach ($keyNodes as $keyNode) {
					if ($keyNode->getAttribute("name") == "dirtype")
						$ac_dir_type = $keyNode->getAttribute("value");
					else if ($keyNode->getAttribute("name") == "description")
						$ac_dir_description = $keyNode->getAttribute("value");
					else if ($keyNode->getAttribute("name") == "dircount")
						$ac_dir_count = $keyNode->getAttribute("value"); 
				}
				
				$xPathGroups = "//group";
				if ($groupNodes = $shareDom->runXpathQuery($xPathGroups)) {
					
					foreach ($groupNodes as $groupNode) {
						$ac_gid = $groupNode->getAttribute("id");
						$ac_str = ""; 
						
						if ($groupNode->getAttribute("read") == "yes")
							$ac_str .= "r";
						else
							$ac_str .= " ";
						
						if ($groupNode->getAttribute("write") == "yes")
							$ac_str .= "w";
						else
							$ac_str .= " ";
						
						if ($groupNode->getAttribute("access") == "yes")
							$ac_str .= "a";
						else
							$ac_str .= " ";
							
						$ac_groups_access[$ac_gid] = $ac_str;
					}
				}
				
				if ($primaryGroup = $shareDom->getElementsByTagName("primary")->item(0))
					$ac_groups_primary = $primaryGroup->getAttribute("id");
				
				if ($accessType = $shareDom->getElementsByTagName("access")->item(0)) {
					$attr = $accessType->getAttribute("public");
					if ($attr == "yes")
						$ac_share_accesspublic = 1; 
				}
				
				if ($rsyncNode = $shareDom->getElementsByTagName("rsync")->item(0)) {
					if ($rsync_read_only = $rsyncNode->getAttribute("read_only")) {
						if ($rsync_read_only == "yes")
							$ac_rsync_read_only = "yes";
						else
							$ac_rsync_read_only = "no"; 
					}
					
					if ($rsync_write_only = $rsyncNode->getAttribute("write_only")) {
						if ($rsync_write_only == "yes")
							$ac_rsync_write_only = "yes";
						else
							$ac_rsync_write_only = "no"; 
					}
					
					if ($rsync_list = $rsyncNode->getAttribute("list")) {
						if ($rsync_list == "yes")
							$ac_rsync_list = "yes";
						else
							$ac_rsync_list = "no";
					}
					
					if ($rsync_use_chroot = $rsyncNode->getAttribute("use_chroot")) {
						if ($rsync_use_chroot == "yes")
							$ac_rsync_use_chroot = "yes";
						else
							$ac_rsync_use_chroot = "no";
					}
					
					if ($rsync_comment = $rsyncNode->getAttribute("comment")) {
						$ac_rsync_comment = $rsync_comment;
					}
					
					
					if ($rsyncNode->hasAttribute("max_connections")) {
						$attr = $rsyncNode->getAttribute("max_connections"); 
						if ($attr != "0")
							$ac_rsync_max_connections = $attr;
						else
							$ac_rsync_max_connections = "0"; 
					}

					if ($rsync_fake_super = $rsyncNode->getAttribute("fake_super")) {
						if ($rsync_fake_super == "yes")
							$ac_rsync_fake_super = "yes";
						else
							$ac_rsync_fake_super = "no"; 
					}
					
				}
				
				if ($smbNode = $shareDom->getElementsByTagName("smb")->item(0)) {
					
					$ac_smb_oplocks = FALSE;
					$ac_smb_dos_filemode = FALSE;
					$ac_smb_dos_filetime_resolution = FALSE;
					$ac_smb_dos_filetimes = FALSE;
					$ac_smb_fake_directory_create_times = FALSE;
					$ac_smb_browseable = FALSE;
					
					
					if ($smb_sharename = $smbNode->getAttribute("sharename"))
						$ac_smb_sharename = $smb_sharename;
					else
						$ac_smb_sharename = "";
					
					if ($smb_oplocks = $smbNode->getAttribute("smb_oplocks")) {
						$ac_smb_oplocks = (strcmp($smb_oplocks,"yes") == 0 ? TRUE : FALSE);   
				
					}
					
					if ($smbNode->hasAttribute("smb_force_security_mode"))
						$ac_smb_force_security_mode = $smbNode->getAttribute("smb_force_security_mode");
					
					if ($smb_dos_filemode = $smbNode->getAttribute("dos_filemode"))
						$ac_smb_dos_filemode = (strcmp($smb_dos_filemode,"yes") == 0 ? TRUE : FALSE);
						
					if ($smb_dos_filetime_resolution = $smbNode->getAttribute("dos_filetime_resolution"))
						$ac_smb_dos_filetime_resolution = (strcmp($smb_dos_filetime_resolution, "yes") == 0 ? TRUE : FALSE);
					
					if ($smb_dos_filetimes = $smbNode->getAttribute("dos_filetimes"))
						if ($smb_dos_filetimes != "no")
							$ac_smb_dos_filetimes = (strcmp($smb_dos_filetimes,"yes") == 0 ? TRUE : FALSE);
					
					if ($smb_csc_policy = $smbNode->getAttribute("csc_policy"))
						if ($smb_csc_policy != "manual")
							$ac_smb_csc_policy = $smb_csc_policy;
						else
							$ac_smb_csc_policy = "manual"; 
					
							
					if ($smb_fake_directory_create_times = $smbNode->getAttribute("fake_directory_create_times"))
						$ac_smb_fake_directory_create_times = (strcmp($smb_fake_directory_create_times, "yes") == 0 ? TRUE : FALSE);
					
						
					if ($smb_browseable = $smbNode->getAttribute("browseable")) {
						$ac_smb_browseable = (strcmp($smb_browseable, "yes") == 0 ? TRUE : FALSE);
					}
				}
				
				if ($networkNodes = $shareDom->getElementsByTagName("network")) {
                    
					foreach($networkNodes as $networkNode) {
						$ac_str = "";
						if ($dnNetworkName = $networkNode->getAttribute("network"))
							true;
						if ($dnAccess = $networkNode->getAttribute("access"))
							true;
						if ($dnSmb = $networkNode->getAttribute("smb"))
							true;
						if ($dnNfs = $networkNode->getAttribute("nfs"))
							true;
						if ($dnAfp = $networkNode->getAttribute("afp"))
							true;
						if ($dnNoSquash = $networkNode->getAttribute("nosquash"))
							true;
                        if ($dnInsecure = $networkNode->getAttribute("insecure"))
                            true;
						if ($dnHttp = $networkNode->getAttribute("http"))
							true;
						if ($dnFtp = $networkNode->getAttribute("ftp"))
							true;
						if ($dnRsync = $networkNode->getAttribute("rsync"))
							true;
						
						if ($dnAccess == "rw")
							$ac_str .= "w";
						else if ($dnAccess == "ro")
							$ac_str .= "o";
						else
							$ac_str .= "n";
						
						if ($dnSmb == "yes")
							$ac_str .= "y";
						else if ($dnSmb == "ro")
							$ac_str .= "o";
						else
							$ac_str .= " ";
						
						if ($dnNfs == "yes")
							$ac_str .= "y";
						else
							$ac_str .= " "; 
							
						if ($dnAfp == "yes")
							$ac_str .= "y";
						else
							$ac_str .= " ";
						
						if ($dnNoSquash == "yes")
							$ac_str .= "y";
						else
							$ac_str .= " ";
						
						if ($dnHttp == "yes")
							$ac_str .= "y";
						else if ($dnHttp == "ro")
							$ac_str .= "o";
						else
							$ac_str .= " ";
						
						$ac_str .= "-"; // for removed NFS ACLs
						$ac_str .= "*"; // for old oplocks field
					
						if ($dnInsecure == "yes")
							$ac_str .= "y";
						else
							$ac_str .= " "; 
						
						if ($dnFtp == "yes")
							$ac_str .= "y";
						else if ($dnFtp == "ro")
							$ac_str .= "o";
						else
							$ac_str .= " ";
						
						if ($dnRsync == "yes")
							$ac_str .= "y";
						else if ($dnRsync == "ro")
							$ac_str .= "o";
						else
							$ac_str .= " ";
                            
                        
                        $nfsAttrArray = array();
        
                        foreach($nfsAttrList as $nfsAttrKey => $nfsAttrValue) {
        
                            if ($networkNode->hasAttribute($nfsAttrKey))
                                $domAttr = $networkNode->getAttribute($nfsAttrKey);
                            
                            if (!empty($domAttr))
                                $nfsAttrArray["$nfsAttrKey"] = $domAttr;     
                            else
                                $nfsAttrArray["$nfsAttrKey"] = $nfsAttrValue; 
                        }     
                            
							
							
                        for ($i = 0; $i < count($ac_networks_network); $i++) {
							if ($ac_networks_name[$i] == $dnNetworkName) {
								$ac_networks_access[$i] = $ac_str;
                                
                                $ac_networks_nfs[$i] = $nfsAttrArray;   
                                
                            }
					   }
					}
				}
				
				if ($ac_dir_type == "share") {
					
					
					$ac_dir_description_temp = str_replace("/",
                                            ".", substr($ac_mountpoint .
                                                        $ac_file, 5,
                                                        strlen($ac_mountpoint .
                                                        $ac_file) - 5)) .
                                                        $ac_snapshots_suffix;
		
					if ((strlen($ac_smb_sharename) > 0) && (!$ac_is_snapshot)) {
						$ac_tr_found = false;
						for ($ac_tr_i = 0; $ac_tr_i < count($ac_unique_descriptions); $ac_tr_i++)
							if ($ac_unique_descriptions[$ac_tr_i] == $ac_smb_sharename) {
								$ac_tr_found = true;
								$ac_unique_description_index = $ac_tr_i;
								break;
							}
		 
						if ($ac_tr_found) {
							$ac_unique_description_counts[$ac_unique_description_index]++;
							$ac_dir_description_temp = $ac_smb_sharename . " " .
							$ac_unique_description_counts[$ac_unique_description_index];
						}
						
						else {
							array_push($ac_unique_descriptions, $ac_smb_sharename);
							array_push($ac_unique_description_counts, 1);
							$ac_dir_description_temp = $ac_smb_sharename;
						}
					}
					
					if (($ac_groups_primary > 0) && ($ac_share_accesspublic == 0)) {
						if (($ac_chmod) && (($ac_chmod_path == "") ||
											($ac_chmod_path == ($ac_mountpoint . $ac_file)))) {
							/* Set FACLs */
		
							/* First, clear all ACLs on the share */
							exec("export LANG=C; /usr/bin/sudo /usr/bin/setfacl -R -b " .
								 escapeshellarg($ac_mountpoint . $ac_file));
						
							/* Then, set the primary group of the share */
							exec("export LANG=C; /usr/bin/sudo /bin/chown -R nobody:" .
								 escapeshellarg($ac_groups_primary) . " " .
								 escapeshellarg($ac_mountpoint . $ac_file));
							exec("export LANG=C; /usr/bin/sudo /bin/chmod 2770 " .
								 escapeshellarg($ac_mountpoint . $ac_file));
		
							/* Set ACLs for WebDAV to work */
						
							exec("export LANG=C; /usr/bin/sudo /usr/bin/setfacl -R -m g:apache:rwx " .
								 escapeshellarg($ac_mountpoint . $ac_file));
							exec("export LANG=C; /usr/bin/sudo /usr/bin/setfacl -R -d -m g:apache:rwx " .
								 escapeshellarg($ac_mountpoint . $ac_file));
						}
		
						/* Then, set group ACLs */
						
						reset($ac_groups_access);
						
						$http_rstr_rw = "\trequire group";
						$http_rstr_ro = "\t\trequire group";
		
						for ($g = 0; $g < count($acx_groups_gid); $g++)
							if ($acx_groups_gid[$g] == $ac_groups_primary) {
								$http_rstr_rw .= " \"" . $acx_groups_name[$g] . "\"";
							}
						$ac_http_group_readonlys = false;
		
						while (list($ac_group, $ac_access) = each($ac_groups_access))
							if ($ac_group != $ac_groups_primary) {
								$permissions = "";
								if (substr($ac_access, 0, 1) == "r")
									$permissions .= "r";
								if (substr($ac_access, 1, 1) == "w")
									$permissions .= "w";
								if (substr($ac_access, 2, 1) == "a")
									$permissions .= "x";
							
								if (strlen($permissions) > 0) {
									if (($ac_chmod) && (($ac_chmod_path == "") ||
														($ac_chmod_path == ($ac_mountpoint . $ac_file)))) {
										exec("export LANG=C; /usr/bin/sudo /usr/bin/setfacl -R -m g:" .
											 escapeshellarg($ac_group) . ":" . $permissions . " " .
											 escapeshellarg($ac_mountpoint . $ac_file));
										exec("export LANG=C; /usr/bin/sudo /usr/bin/setfacl -R -d -m g:" .
											 escapeshellarg($ac_group) . ":" . $permissions . " " .
											 escapeshellarg($ac_mountpoint . $ac_file));
									}
								}
		
								if ((substr($ac_access, 0, 1) == "r") &&
									(substr($ac_access, 2, 1) == "a") &&
									(substr($ac_access, 1, 1) == "w")) {
									for ($g = 0; $g < count($acx_groups_gid); $g++)
										if ($acx_groups_gid[$g] == $ac_group) {
											$http_rstr_rw .= " \"" . $acx_groups_name[$g] . "\"";
										}
								}
								else if ((substr($ac_access, 0, 1) == "r") &&
										 (substr($ac_access, 2, 1) == "a") &&
										 (substr($ac_access, 1, 1) != "w")) {
									for ($g = 0; $g < count($acx_groups_gid); $g++)
										if ($acx_groups_gid[$g] == $ac_group) {
											$http_rstr_ro .= " \"" . $acx_groups_name[$g] . "\"";
											$ac_http_group_readonlys = true;
										}
								}
							}
						
						$http_rstr_rw .= "\n";
						$http_rstr_ro .= "\n";
							
						$do_samba = 0;
						$do_afp = 0;
						$do_http = 0;
						$http_readonly_networks = false;
						$do_ftp = 0;
						$do_rsync = 0; 
						$ftp_readonly_networks = false;
						$samba_networks = "";
						$samba_readonly_networks = "";
						$rsync_networks = "";
						$rsync_readonly_networks = "";
					
						/* NFS section */
						
                     
					
						for ($i = 0; $i < count($ac_networks_network); $i++) {
						
							if (substr($ac_networks_access[$i], 0, 1) != "n") {
								if (substr($ac_networks_access[$i], 0, 1) != "n") {
                                    
                                    // get the list of nfs attributes
                                    
                                    $nfsAttrString = ""; 
                                    
                                    $localNFSAttrArray = $ac_networks_nfs[$i];
                                    
                                    foreach ($localNFSAttrArray as $key => $value) {
                                    
                                        if ($key != "anonuid" && $key != "anongid") {
                                        
                                            if($value == "yes")
                                                $nfsAttrString .= ",$key";
                                        }
                                        
                                        else if ($key == "anonuid")
                                            $nfsAttrString .= ",anonuid=" . $value;
                                        else if ($key == "anongid")
                                            $nfsAttrString .= ",anongid=" . $value;
                                    
                                    }
                                    
                                    /*
									$ac_nfsv3_fp->AddLine($ac_mountpoint . $ac_file . " " .
										  $ac_networks_network[$i] . "/" .
										  $ac_networks_netmask[$i] .
										  "(" . (((substr($ac_networks_access[$i], 0, 1) == "o") ||
												  ($ac_is_snapshot)) ? "ro" : "rw") . "," .
										  ((substr($ac_networks_access[$i], 4, 1) == "y") ?
										   "no_root_squash" : "root_squash") . "," .
										  ((substr($ac_networks_access[$i], 8, 1) == "y") ?
										   "insecure" : "secure") . ",sync)");
                                    */
                                    
                                    $nfs_share_entry .= $ac_networks_network[$i] . "/" .
                                    $ac_networks_netmask[$i] .
                                    "(" . (((substr($ac_networks_access[$i], 0, 1) == "o") ||
												  ($ac_is_snapshot)) ? "ro" : "rw") . 
                                    $nfsAttrString . ")  " ; 
							
                            	}
                                
                                
							}
                            
                            
		
							if ((substr($ac_networks_access[$i], 1, 1) == "y") ||
								(substr($ac_networks_access[$i], 1, 1) == "o")) {
								
								$samba_networks .= " " . $ac_networks_network[$i]  .
								convert_netmask($ac_networks_netmask[$i]);
								$do_samba = 1;
		
								if (substr($ac_networks_access[$i], 1, 1) == "o")
									$samba_readonly_networks .= " " .
									$ac_networks_network[$i] . convert_netmask($ac_networks_netmask[$i]);
							}
							
							
							if ((substr($ac_networks_access[$i], 10, 1) == "y") ||
								(substr($ac_networks_access[$i], 10, 1) == "o")) {
								
								$rsync_networks .= " " . $ac_networks_network[$i] .
								convert_netmask($ac_networks_netmask[$i]);
								$do_rsync = 1;
								
								if (substr($ac_networks_access[$i], 10, 1) == "o")
									$rsync_readonly_networks .= " " .
									$ac_networks_network[$i] . convert_netmask($ac_networks_netmask[$i]); 
							}
		
							if (substr($ac_networks_access[$i], 3, 1) == "y")
								$do_afp = 1;
							
							if (substr($ac_networks_access[$i], 5, 1) != " ")
								$do_http = 1;
		
							if (substr($ac_networks_access[$i], 5, 1) == "o")
								$http_readonly_networks = true;
		
							if (substr($ac_networks_access[$i], 9, 1) != " ")
								$do_ftp = 1;
		
							if (substr($ac_networks_access[$i], 9, 1) == "o")
								$ftp_readonly_networks = true;
							
							
						}
                        
                        if (!empty($nfs_share_entry)) {
                            $ac_nfsv3_fp->AddLine($ac_mountpoint . $ac_file . " " . $nfs_share_entry);
                            $ac_nfsv3_fp->AddLine("");
                        }
					
						/* Rsync Section */
						
						if ($do_rsync != 0) {
							
							$ac_rsync_fp->AddLine("[ $ac_dir_description_temp ]");
							$ac_rsync_fp->AddLine("\tpath = $ac_mountpoint" . "$ac_file");
							$ac_rsync_fp->AddLine("\tcomment = $ac_rsync_comment");
							$ac_rsync_fp->AddLine("\thosts allow = $rsync_networks");
							$ac_rsync_fp->AddLine("\thosts readonly allow = $rsync_readonly_networks");
							$ac_rsync_fp->AddLine("\tauth use pam = yes");
							$ac_rsync_fp->AddLine("\tread only = $ac_rsync_read_only");
							$ac_rsync_fp->AddLine("\twrite only = $ac_rsync_write_only");
							$ac_rsync_fp->AddLine("\tuse chroot = $ac_rsync_use_chroot");
							$ac_rsync_fp->AddLine("\tmax connections = $ac_rsync_max_connections");
							$ac_rsync_fp->AddLine("\tlist = $ac_rsync_list"); 
							$ac_rsync_fp->AddLine("\tfake super = $ac_rsync_fake_super");
							
						}
						
					
						/* SMB section */
						
						if ($do_samba != 0) {
							$ac_smb_fp->AddLine("[" . $ac_dir_description_temp . "]");
							$ac_smb_fp->AddLine("	comment = " . $ac_dir_description);
							$ac_smb_fp->AddLine("	path = " . $ac_mountpoint . $ac_file);
		
							if (!$ac_is_snapshot) {
								$ac_smb_fp->AddLine("	read only = no");
								$ac_smb_fp->AddLine("	writeable = yes");
							}
							else {
								$ac_smb_fp->AddLine("	read only = yes");
								$ac_smb_fp->AddLine("	writeable = no");
							}
							
							if (!$ac_smb_oplocks) {
								$ac_smb_fp->AddLine("	oplocks = no");
								$ac_smb_fp->AddLine("	level2 oplocks = no");
							}
							else {
								$ac_smb_fp->AddLine("	oplocks = yes");
								$ac_smb_fp->AddLine("	level2 oplocks = yes");
							}
							
							
							$ac_smb_fp->AddLine("	force security mode = $ac_smb_force_security_mode"); 
							
							
							if (!$ac_smb_dos_filemode) {
								$ac_smb_fp->AddLine("	dos filemode = no"); 
							}
							
							else {
								$ac_smb_fp->AddLine("	dos filemode = yes"); 
							}
							
							if (!$ac_smb_dos_filetime_resolution) {
								$ac_smb_fp->AddLine("	dos filetime resolution = no"); 
							}
							
							else {
								$ac_smb_fp->AddLine("	dos filetime resolution = yes"); 
							}
							
							if (!$ac_smb_dos_filetimes) {
								$ac_smb_fp->AddLine("	dos filetimes = no"); 
							}
							
							else {
								$ac_smb_fp->AddLine("	dos filetimes = yes"); 
							}
							
							if (!$ac_smb_fake_directory_create_times) {
								$ac_smb_fp->AddLine("	fake directory create times = no"); 
							}
							
							else {
								$ac_smb_fp->AddLine("	fake directory create times = yes"); 
							}
							
							if (!$ac_smb_browseable) {
								$ac_smb_fp->AddLine("	browseable = no");
							}
							else {
								$ac_smb_fp->AddLine("	browseable = yes"); 
							}
		

							$ac_smb_fp->AddLine("	csc policy = $ac_smb_csc_policy"); 
							$ac_smb_fp->AddLine("	share modes = yes");
							$ac_smb_fp->AddLine("	veto oplock files = /*.mdb/*.MDB/*.dbf/*.DBF/");
							$ac_smb_fp->AddLine("	veto files = /*:Zone.Identifier:*/");
							$ac_smb_fp->AddLine("	create mode = 0770");
							$ac_smb_fp->AddLine("	directory mode = 2770");
							$ac_smb_fp->AddLine("	printable = no");
							$ac_smb_fp->AddLine("	guest ok = no");
							$ac_smb_fp->AddLine("	hosts allow = " . $samba_networks);
							$ac_smb_fp->AddLine("	hosts readonly allow = " . $samba_readonly_networks);
                            				$ac_smb_fp->AddLine("	store dos attributes = " . $ac_smbstoredosattributes);
                            				$ac_smb_fp->AddLine("	map acl inherit = " . $ac_smbmapaclinherit);  // NOTE: Experimental. 
		
							if (!$ac_is_snapshot) {
								global $ac_homesvolume_mountpoint;
								global $ac_snapshot_homes;
								
								/* First clean up any existing @GMT-timestamp entries */
								exec("export LANG=C; /usr/bin/sudo /usr/bin/find " .
									 escapeshellarg($ac_mountpoint . $ac_file) .
									 " -maxdepth 1 -type l -name '@GMT-*' -delete");
								
								
								/* clean up entries for homes */
								if (strlen($ac_homesvolume_mountpoint) > 0 && $ac_snapshot_homes)
									exec("export LANG=C; /usr/bin/sudo /usr/bin/find " .
										 escapeshellarg($ac_homesvolume_mountpoint . "homes") .
										 " -maxdepth 1 -type l -name '@GMT-*' -delete");
			
								$use_shadow_copy = FALSE;
		
								for ($ac_si = 0; $ac_si < count($ac_snapshots_id); $ac_si++) {
									if (($ac_snapshots_vgname[$ac_si] == $ac_vg) &&
										($ac_snapshots_lvname[$ac_si] == $ac_lv)) {
										
										$ac_snap_mountpoint = "/mnt/snapshots/" .
										$ac_snapshots_vgname[$ac_si] . "/" .
										$ac_snapshots_lvname[$ac_si] . "/" .
										$ac_snapshots_id[$ac_si] . "/";
										
										$ac_snap_timestamp = gmdate("Y.m.d-H.i.s",
													strtotime($ac_snapshots_timestamp[$ac_si]));
		
										if (is_dir($ac_snap_mountpoint)) {
											exec("export LANG=C; /usr/bin/sudo /bin/ln -s " .
												 escapeshellarg($ac_snap_mountpoint . $ac_share_path . $ac_file) .
												 " " . escapeshellarg($ac_mountpoint .
															$ac_file . "/@GMT-" . $ac_snap_timestamp));
											
											/* Add snapshot link for smb homes */
		
											if (strlen($ac_homesvolume_mountpoint) > 0 &&
												$ac_snapshot_homes)
												
												exec("export LANG=C; /usr/bin/sudo /bin/ln -f -s " .
													 escapeshellarg($ac_snap_mountpoint .
															$ac_share_path . "homes") . " " .
													 escapeshellarg($ac_homesvolume_mountpoint .
															"homes" . "/@GMT-" . $ac_snap_timestamp));
		
											$use_shadow_copy = TRUE;
										}
									}
		
								}
								
								if ($use_shadow_copy)
									$ac_smb_fp->AddLine("vfs objects = shadow_copy");
							}
							
							$ac_smb_fp->AddLine("\n");
						}
		
						
						/* Begin FTP section */
						
						if ($do_ftp == 0) {
							$ac_ftp_conf_fp->AddLine( "<Directory  \"" .
								  $ac_mountpoint . $ac_file . "\">");
							$ac_ftp_conf_fp->AddLine( "   DenyAll");
							$ac_ftp_conf_fp->AddLine( "</Directory>\n");
						}
						
						else {
							
							$ac_ftp_conf_fp->AddLine( "<Directory  \"" .
								  $ac_mountpoint . $ac_file . "\">");
							
							if ($ftp_readonly_networks) {
								
								$ac_ftp_conf_fp->AddLine("\t<Limit LOGIN READ DIRS>");
								for ($i = 0; $i < count($ac_networks_network); $i++) {	
									if (substr($ac_networks_access[$i], 9, 1) == "o")
										$ac_ftp_conf_fp->AddLine( "\t\tAllow from " .
											  $ac_networks_network[$i] . "" .
											  convert_netmask($ac_networks_netmask[$i]));
								}
								$ac_ftp_conf_fp->AddLine( "\t</Limit>");
							}
							
							$ac_ftp_conf_fp->AddLine( "\t<Limit LOGIN DIRS READ WRITE>");
							for ($i = 0; $i < count($ac_networks_network); $i++) {
								if (substr($ac_networks_access[$i], 9, 1) == "y")
									$ac_ftp_conf_fp->AddLine( "\t\tAllow from " . $ac_networks_network[$i] .
										  "" . convert_netmask($ac_networks_netmask[$i]));
							}
							$ac_ftp_conf_fp->AddLine( "\t</Limit>");
							$ac_ftp_conf_fp->AddLine( "</Directory>");
						}
						
						/* End FTP Section*/
		
						/* HTTP / FTP section */
						
						if ($do_http == 0) {
							$ac_http_fp->AddLine("<Directory \"" . $ac_mountpoint.$ac_file . "\">");
							$ac_http_fp->AddLine("Order Allow,Deny");
							$ac_http_fp->AddLine("Deny from all\n");
							$ac_http_fp->AddLine("</Directory>\n");
							
						}
						
						else {
                            //$ac_http_fp->AddLine( "<Location $ac_mountpoint"."$ac_file >");
                            //$ac_http_fp->AddLine( "	Dav on");
                            //$ac_http_fp->AddLine( "</Location>");
							$ac_http_fp->AddLine("<Directory \"" . $ac_mountpoint . $ac_file . "\">");
							$ac_http_fp->AddLine("Options +Indexes");
							$ac_http_fp->AddLine("AllowOverride None");
							$ac_http_fp->AddLine("AuthType Basic");
                            $ac_http_fp->AddLine("AuthBasicAuthoritative off");
                            $ac_http_fp->AddLine("AuthUserFile    /dev/null");
							$ac_http_fp->AddLine("AuthName \"" . $ac_dir_description . "\"\n");
							
							$ac_http_fp->AddLine("AuthPAM_Enabled on\n");
							$ac_http_fp->AddLine("AuthPAM_FallThrough on\n");
		
							if ($ac_http_group_readonlys) {
								$ac_http_fp->AddLine("<Limit GET HEAD PROPFIND OPTIONS REPORT>");
								$ac_http_fp->AddLine( $http_rstr_ro);
								$ac_http_fp->AddLine("</Limit>\n");
							}
		
							$ac_http_fp->AddLine( $http_rstr_rw . "\n");
		
							$ac_http_fp->AddLine("Order Allow,Deny\n");
							$ac_http_fp->AddLine("# Deny from all\n\n");
		
							if ($http_readonly_networks) {
								$ac_http_fp->AddLine("<Limit GET HEAD PROPFIND OPTIONS REPORT>\n");
		
								for ($i = 0; $i < count($ac_networks_network); $i++) {
									if (substr($ac_networks_access[$i], 5, 1) == "o")
										$ac_http_fp->AddLine("\tAllow from " . $ac_networks_network[$i] . "/" .
															 $ac_networks_netmask[$i] . "\n");
								}
		
								$ac_http_fp->AddLine("</Limit>\n\n");
							}
		
							$ac_http_fp->AddLine("<Limit GET HEAD POST PUT DELETE OPTIONS CONNECT LOCK UNLOCK PROPFIND PROPPATCH COPY MOVE MKCOL CHECKIN CHECKOUT UNCHECKOUT VERSION-CONTROL REPORT UPDATE LABEL MERGE MKWORKSPACE BASELINE-CONTROL MKACTIVITY>\n");
							for ($i = 0; $i < count($ac_networks_network); $i++)
								if (substr($ac_networks_access[$i], 5, 1) == "y")
									$ac_http_fp->AddLine("Allow from " . $ac_networks_network[$i] . "/" .
														 $ac_networks_netmask[$i] . "\n");
							$ac_http_fp->AddLine("</Limit>\n\n");
		
						    $ac_http_fp->AddLine( "	Dav on");
							$ac_http_fp->AddLine("</Directory>\n");
							$ac_http_fp->AddLine("\n");
						
		
		
							$ac_http_fp->AddLine("Alias \"" . $ac_mountpoint . $ac_file . "\" \"" .
												 $ac_mountpoint . $ac_file . "\"\n\n");
						}
					}
					
					else if ($ac_share_accesspublic != 0 || ($ac_homespathdefined && $ac_ishomepath)) {
						if (($ac_chmod) && (($ac_chmod_path == "") ||
											($ac_chmod_path == ($ac_mountpoint .
																$ac_file)))) {
							/* Set FACLs */
		
							/* First, clear all ACLs on the share */
							exec("export LANG=C; /usr/bin/sudo /usr/bin/setfacl -R -b " .
								 escapeshellarg($ac_mountpoint . $ac_file));
						
							/* Then, set the primary group of the share */
							
							if ($ac_share_accesspublic != 0) {
								exec("export LANG=C; /usr/bin/sudo /bin/chown -R ofguest:ofguest " .
									 escapeshellarg($ac_mountpoint . $ac_file));
								exec("export LANG=C; /usr/bin/sudo /bin/chmod 2777 " .
									 escapeshellarg($ac_mountpoint . $ac_file));
							}
							/* Set ACLs for WebDAV to work */
						
							exec("export LANG=C; /usr/bin/sudo /usr/bin/setfacl -R -m g:apache:rwx " .
								 escapeshellarg($ac_mountpoint . $ac_file));
							exec("export LANG=C; /usr/bin/sudo /usr/bin/setfacl -R -d -m g:apache:rwx " .
								 escapeshellarg($ac_mountpoint . $ac_file));
						}
		
						$do_samba = 0;
						$do_afp = 0;
						$do_http = 0;
						$http_readonly_networks = false;
						$do_ftp = 0;
						$ftp_readonly_networks = false;
						$do_rsync = 0; 
					
						$samba_networks = "";
						$samba_readonly_networks = "";
						$rsync_networks = ""; 
						$rsync_readonly_networks = ""; 
					
						/* NFS section */
					
						for ($i = 0; $i < count($ac_networks_network); $i++) {
							if (substr($ac_networks_access[$i], 0, 1) != "n") {
								if (substr($ac_networks_access[$i], 0, 1) != "n") {
                                    
                                    // get the list of nfs attributes
                                    
                                    $nfsAttrString = ""; 
                                    
                                    $localNFSAttrArray = $ac_networks_nfs[$i];
                                    
                                    foreach ($localNFSAttrArray as $key => $value) {
                                    
                                        if ($key != "anonuid" && $key != "anongid") {
                                        
                                            if($value == "yes")
                                                $nfsAttrString .= ",$key";
                                        }
                                        
                                        else if ($key == "anonuid") {
                                            if ($ac_share_accesspublic == 0)
                                                $nfsAttrString .= ",anonuid=" . $value;
                                            else
                                                $nfsAttrString .= ",anonuid=" . "96"; 
                                        }
                                        
                                        else if ($key == "anongid") {
                                            if ($ac_share_accesspublic == 0)
                                                $nfsAttrString .= ",anongid=" . $value;
                                            else
                                                $nfsAttrString .= ",anongid=" . "96";
                                        }
                                    
                                    }
                                    
                                    
                                    
                                    
									if ($ac_share_accesspublic != 0) {
										/*$ac_nfsv3_fp->AddLine($ac_mountpoint . $ac_file . " " .
													$ac_networks_network[$i] . "/" .
													$ac_networks_netmask[$i] . "(" .
													(((substr($ac_networks_access[$i], 0, 1) == "o") ||
													      ($ac_is_snapshot)) ? "ro" : "rw") . "," .
													((substr($ac_networks_access[$i], 8, 1) == "y") ? "insecure" : "secure") .
													",sync,all_squash,anonuid=96,anongid=96)");*/
                                        
                                        $nfs_share_entry .= $ac_networks_network[$i] . "/" .
                                        $ac_networks_netmask[$i] . "(" .
                                        (((substr($ac_networks_access[$i], 0, 1) == "o") ||
													      ($ac_is_snapshot)) ? "ro" : "rw") . 
                                        $nfsAttrString . ")  ";
                                        
									}
									
                                    else if ($ac_ishomepath) {
										/*$ac_nfsv3_fp->AddLine($ac_mountpoint . $ac_file . " " .
													$ac_networks_network[$i] . "/" .
													$ac_networks_netmask[$i] . "(" .
													(((substr($ac_networks_access[$i], 0, 1) == "o") ||
													      ($ac_is_snapshot)) ? "ro" : "rw") . "," .
													((substr($ac_networks_access[$i], 8, 1) == "y") ? "insecure" : "secure") .
													",sync,root_squash)");*/
                                        $nfs_share_entry .= $ac_networks_network[$i] . "/" .
                                        $ac_networks_netmask[$i] . "(" .
                                        (((substr($ac_networks_access[$i], 0, 1) == "o") ||
													      ($ac_is_snapshot)) ? "ro" : "rw") . 
                                        $nfsAttrString . ")  ";
									}
								}
							}
		
							if ((substr($ac_networks_access[$i], 1, 1) == "y") ||
								(substr($ac_networks_access[$i], 1, 1) == "o")) {
								$samba_networks .= " " . $ac_networks_network[$i] .
								convert_netmask($ac_networks_netmask[$i]);
								$do_samba = 1;
		
								if (substr($ac_networks_access[$i], 1, 1) == "o")
									$samba_readonly_networks .= " " .
									$ac_networks_network[$i] . convert_netmask($ac_networks_netmask[$i]);
							}
							
							if ((substr($ac_networks_access[$i], 10, 1) == "y") ||
								(substr($ac_networks_access[$i], 10, 1) == "o")) {
								
								$rsync_networks .= " " . $ac_networks_network[$i] .
								convert_netmask($ac_networks_netmask[$i]);
								$do_rsync = 1;
								
								if (substr($ac_networks_access[$i], 10, 1) == "o")
									$rsync_readonly_networks .= " " .
									$ac_networks_network[$i] . convert_netmask($ac_networks_netmask[$i]); 
							}
							
		
							if (substr($ac_networks_access[$i], 3, 1) == "y")
								$do_afp = 1;
		
							if (substr($ac_networks_access[$i], 5, 1) != " ")
								$do_http = 1;
		
							if (substr($ac_networks_access[$i], 5, 1) == "o")
								$http_readonly_networks = true;
		
							if (substr($ac_networks_access[$i], 9, 1) != " ")
								$do_ftp = 1;
		
							if (substr($ac_networks_access[$i], 9, 1) == "o")
								$ftp_readonly_networks = true;
						}
                        
                        if (!empty($nfs_share_entry)) {
                            $ac_nfsv3_fp->AddLine($ac_mountpoint . $ac_file . " " . $nfs_share_entry);
                            $ac_nfsv3_fp->AddLine("");
                        }
						
						
						/* Rsync section */
						
						if ($do_rsync != 0) {
							
							$ac_rsync_fp->AddLine("[$ac_dir_description_temp]");
							$ac_rsync_fp->AddLine("\tpath = $ac_mountpoint" . "$ac_file");
							$ac_rsync_fp->AddLine("\tcomment = $ac_rsync_comment");
							$ac_rsync_fp->AddLine("\thosts allow = $rsync_networks");
							$ac_rsync_fp->AddLine("\thosts readonly allow = $rsync_readonly_networks");
							if ($ac_share_accesspublic != 0)
								$ac_rsync_fp->AddLine("\tauth use pam = no");
							else if ($ac_ishomepath)
								$ac_rsync_fp->AddLine("\tauth use pam = yes"); 
							$ac_rsync_fp->AddLine("\tread only = $ac_rsync_read_only");
							$ac_rsync_fp->AddLine("\twrite only = $ac_rsync_write_only");
							$ac_rsync_fp->AddLine("\tuse chroot = $ac_rsync_use_chroot");
							$ac_rsync_fp->AddLine("\tmax connections = $ac_rsync_max_connections");
							$ac_rsync_fp->AddLine("\tlist = $ac_rsync_list"); 
							$ac_rsync_fp->AddLine("\tfake super = $ac_rsync_fake_super"); 
							
						}
						
						
					
						/* SMB section */
						
						if ($do_samba != 0) {
							$ac_smb_fp->AddLine("[" . $ac_dir_description_temp . "]");
							$ac_smb_fp->AddLine("	comment = " . $ac_dir_description );
							
							if ($ac_share_accesspublic != 0)
								$ac_smb_fp->AddLine("	path = " . $ac_mountpoint . $ac_file);
							else if ($ac_ishomepath)
								$ac_smb_fp->AddLine("	path = " . $ac_mountpoint . $ac_file . "/%U"); 
	
							if (!$ac_is_snapshot) {
								$ac_smb_fp->AddLine("	read only = no");
								$ac_smb_fp->AddLine("	writeable = yes");
							}
							else {
								$ac_smb_fp->AddLine("	read only = yes");
								$ac_smb_fp->AddLine("	writeable = no");
							}
							
							if (!$ac_smb_oplocks) {
								$ac_smb_fp->AddLine("	oplocks = no");
								$ac_smb_fp->AddLine("	level2 oplocks = no");
							}
							else {
								$ac_smb_fp->AddLine("	oplocks = yes");
								$ac_smb_fp->AddLine("	level2 oplocks = yes");
							}
							
							
							$ac_smb_fp->AddLine("	force security mode = $ac_smb_force_security_mode"); 
							
							if (!$ac_smb_dos_filemode)
								$ac_smb_fp->AddLine("	dos filemode = no"); 
							else
								$ac_smb_fp->AddLine("	dos filemode = yes"); 
					
							if (!$ac_smb_dos_filetime_resolution)
								$ac_smb_fp->AddLine("	dos filetime resolution = no"); 
							else
								$ac_smb_fp->AddLine("	dos filetime resolution = yes"); 
							
							if (!$ac_smb_dos_filetimes)
								$ac_smb_fp->AddLine("	dos filetimes = no"); 
							else
								$ac_smb_fp->AddLine("	dos filetimes = yes"); 
							
							if (!$ac_smb_fake_directory_create_times)
								$ac_smb_fp->AddLine("	fake directory create times = no"); 
							else
								$ac_smb_fp->AddLine("	fake directory create times = yes"); 
						
							if (!$ac_smb_browseable)
								$ac_smb_fp->AddLine("	browseable = no");
							else
								$ac_smb_fp->AddLine("   browseable = yes"); 
		
							$ac_smb_fp->AddLine("   csc policy = $ac_smb_csc_policy"); 
							$ac_smb_fp->AddLine("   share modes = yes");
							$ac_smb_fp->AddLine("   veto oplock files = /*.mdb/*.MDB/*.dbf/*.DBF/");
							$ac_smb_fp->AddLine("   veto files = /*:Zone.Identifier:*/");
							$ac_smb_fp->AddLine("   store dos attributes = " . $ac_smbstoredosattributes); // NOTE: fix for #682
							$ac_smb_fp->AddLine("   map acl inherit = " . $ac_smbmapaclinherit);  // NOTE: Experimental. 
							
                            if ($ac_share_accesspublic != 0) {
								$ac_smb_fp->AddLine("	create mode = 0777");
								$ac_smb_fp->AddLine("	directory mode = 2777");
							}
							
							else if ($ac_ishomepath) {
								$ac_smb_fp->AddLine("	create mode = 0700");
								$ac_smb_fp->AddLine("	directory mode = 0700");
							}
							
							$ac_smb_fp->AddLine("	printable = no");
							if ($ac_share_accesspublic != 0)
								$ac_smb_fp->AddLine("	guest ok = yes");
							else if ($ac_ishomepath)
								$ac_smb_fp->AddLine("	guest ok = no"); 
								
							$ac_smb_fp->AddLine("	hosts allow =" . $samba_networks);
							$ac_smb_fp->AddLine("	hosts readonly allow =" . $samba_readonly_networks);
							
							if (!$ac_is_snapshot)
							{
							
								/* First clean up any existing @GMT-timestamp entries */
								exec("export LANG=C; /usr/bin/sudo /usr/bin/find " .
									 escapeshellarg($ac_mountpoint . $ac_file) . " -maxdepth 1 -type l -name '@GMT-*' -delete");
							
								$use_shadow_copy = FALSE;
		
								for ($ac_si = 0; $ac_si < count($ac_snapshots_id); $ac_si++) {
									if (($ac_snapshots_vgname[$ac_si] == $ac_vg) &&
										($ac_snapshots_lvname[$ac_si] == $ac_lv)) {
										$ac_snap_mountpoint = "/mnt/snapshots/" . $ac_snapshots_vgname[$ac_si] .
													"/" . $ac_snapshots_lvname[$ac_si] . "/" . $ac_snapshots_id[$ac_si] . "/";
										$ac_snap_timestamp = gmdate("Y.m.d-H.i.s", strtotime($ac_snapshots_timestamp[$ac_si]));
		
										if (is_dir($ac_snap_mountpoint)) {
											exec("export LANG=C; /usr/bin/sudo /bin/ln -s " . escapeshellarg($ac_snap_mountpoint .
																							  $ac_share_path . $ac_file) . " " .
												 escapeshellarg($ac_mountpoint . $ac_file . "/@GMT-" . $ac_snap_timestamp));
											$use_shadow_copy = TRUE;
										}
									}
								}
								
								if ($use_shadow_copy)
									$ac_smb_fp->AddLine("vfs objects = shadow_copy\n");
							}
							
							$ac_smb_fp->AddLine("\n");
						}
		
		
						/* Begin FTP section */
						
						if ($do_ftp == 0) {
							$ac_ftp_conf_fp->AddLine( "<Directory  \"" . $ac_mountpoint . $ac_file . "\">");
							$ac_ftp_conf_fp->AddLine( "   DenyAll");
							$ac_ftp_conf_fp->AddLine( "</Directory>\n");
						}
						
						else {
							$ac_ftp_conf_fp->AddLine( "<Directory  \"" . $ac_mountpoint . $ac_file . "\">");
							
							if ($ftp_readonly_networks) {
								$ac_ftp_conf_fp->AddLine( "\t<Limit LOGIN READ DIRS>");
								for ($i = 0; $i < count($ac_networks_network); $i++) {	
									if (substr($ac_networks_access[$i], 9, 1) == "o")
										$ac_ftp_conf_fp->AddLine("\t\tAllow from " . $ac_networks_network[$i] . "" .
																 convert_netmask($ac_networks_netmask[$i]));
								}
								
								$ac_ftp_conf_fp->AddLine( "\t</Limit>");
							}
							
							$ac_ftp_conf_fp->AddLine( "\t<Limit LOGIN DIRS READ WRITE>");
							for ($i = 0; $i < count($ac_networks_network); $i++) {
								if (substr($ac_networks_access[$i], 9, 1) == "y")
									$ac_ftp_conf_fp->AddLine("\t\tAllow from " . $ac_networks_network[$i] . "" .
															 convert_netmask($ac_networks_netmask[$i]));
							}

							$ac_ftp_conf_fp->AddLine( "\t</Limit>\n");
							$ac_ftp_conf_fp->AddLine( "</Directory>");
						}
						
						/* End FTP Section*/
		
						/* HTTP / FTP section */
						
						if ($do_http == 0) {
                            //$ac_http_fp->AddLine( "<Location " .  $ac_mountpoint . $ac_file. ">");
                            //$ac_http_fp->AddLine( "	Dav on");
                            //$ac_http_fp->AddLine( "</Location>");
							$ac_http_fp->AddLine("<Directory \"" . $ac_mountpoint . $ac_file . "\">");
							$ac_http_fp->AddLine("Order Allow,Deny");
							$ac_http_fp->AddLine("Deny from all\n");
                            $ac_http_fp->AddLine("Dav on");
							$ac_http_fp->AddLine("</Directory>\n");
						}
						
						else {
                            //$ac_http_fp->AddLine( "<Location " . $ac_mountpoint . $ac_file .">");
                            //$ac_http_fp->AddLine( "	Dav on");
                            //$ac_http_fp->AddLine( "</Location>");
							$ac_http_fp->AddLine("<Directory \"" . $ac_mountpoint . $ac_file . "\">");
							$ac_http_fp->AddLine("Options +Indexes");
							/* The following two lines no longer supported in httpd 2.2 
							$ac_http_fp->AddLine("Anonymous_Authoritative On");
							$ac_http_fp->AddLine("AuthAuthoritative Off");
							*/
							$ac_http_fp->AddLine("Anonymous_NoUserID Off");
							$ac_http_fp->AddLine("Anonymous_MustGiveEmail Off");
							$ac_http_fp->AddLine("Anonymous_VerifyEmail Off");
							$ac_http_fp->AddLine("Anonymous_LogEmail Off");
							$ac_http_fp->AddLine("Anonymous anonymous");
							$ac_http_fp->AddLine("Order Allow,Deny");
							$ac_http_fp->AddLine("# Deny from all\n");
		
							if ($http_readonly_networks) {
								$ac_http_fp->AddLine("<Limit GET HEAD PROPFIND OPTIONS REPORT>");
		
								for ($i = 0; $i < count($ac_networks_network); $i++) {
									if (substr($ac_networks_access[$i], 5, 1) == "o")
										$ac_http_fp->AddLine("\tAllow from " . $ac_networks_network[$i] . "/" .
															 $ac_networks_netmask[$i]);
								}
		
								$ac_http_fp->AddLine("</Limit>\n");
							}
		
							$ac_http_fp->AddLine("<Limit GET HEAD POST PUT DELETE OPTIONS CONNECT LOCK UNLOCK PROPFIND PROPPATCH COPY MOVE MKCOL CHECKIN CHECKOUT UNCHECKOUT VERSION-CONTROL REPORT UPDATE LABEL MERGE MKWORKSPACE BASELINE-CONTROL MKACTIVITY>\n");
							
							for ($i = 0; $i < count($ac_networks_network); $i++) {
								if (substr($ac_networks_access[$i], 5, 1) == "y")
									$ac_http_fp->AddLine("Allow from " . $ac_networks_network[$i] . "/" . $ac_networks_netmask[$i]);
							}
							
							$ac_http_fp->AddLine("</Limit>\n");
                            $ac_http_fp->AddLine("Dav on");
							$ac_http_fp->AddLine("</Directory>");
							$ac_http_fp->AddLine("");
							$ac_http_fp->AddLine("Alias \"" . $ac_mountpoint . $ac_file . "\" \"" . $ac_mountpoint . $ac_file . "\"\n");
						}
					}
				}
			}  // end check for share xml
			

                
			if ($ac_dir_type == "closed")
				ac_recurse_dir($ac_mountpoint . $ac_file . "/", $ac_lv, $ac_vg, $ac_is_snapshot, $ac_chmod, $ac_chmod_path, $ac_share_path . $ac_file . "/"); 
		}
		
		closedir($ac_handle); 
		
	} // end ac_recurse_dir


	function beginConfigFile($filepath) {
		
		$filehandle = new File($filepath);
		if ($filehandle->Load()) {
			$filehandle->Clear();
			$filehandle->AddLine("");
			$filehandle->AddLine("# PLEASE DO NOT MODIFY THIS CONFIGURATION FILE!");
			$filehandle->AddLine("#\tThis configuration file was autogenerated");
			$filehandle->AddLine("#\tby Openfiler. Any manual changes will be overwritten");
			$filehandle->AddLine("#\tGenerated at: "  . date("D M j G:i:s T Y"));
			$filehandle->AddLine(""); 
			return $filehandle; 
		}
		
	}
	
	function endConfigFile($filehandle) {
		
		$filehandle->AddLine("");
		$filehandle->AddLine("# End of Openfiler configuration");
		$filehandle->AddLine("");
		$filehandle->Save(); 
	}
	
	
	
	$ac_smb_fp = beginConfigFile(CONFIG_SMB);
	$ac_nfsv3_fp = beginConfigFile(CONFIG_NFS);
	$ac_ftp_fp = beginConfigFile(CONFIG_FTP);
	$ac_ftp_conf_fp = beginConfigFile(CONFIG_FTP_SHARES);
	$ac_http_fp = beginConfigFile(CONFIG_HTTP_SHARES);
	$ac_rsync_fp = beginConfigFile(CONFIG_RSYNC);
	$ac_iscsi_deny_fp = beginConfigFile(CONFIG_INIT_DENY);
	$ac_iscsi_allow_fp = beginConfigFile(CONFIG_INIT_ALLOW); 
	
	/*  begin smb.conf global section */
	

	$ac_result_workgroup = ""; 
	

	/* get all possible entries for smb workgroup */
	
	$smb_workgroup_entries = array(); 

		
	array_push($smb_workgroup_entries, $authcfg_obj->get_key_settings("pam_winbind", "workgroup")); 
	array_push($smb_workgroup_entries, $authcfg_obj->get_key_settings("pam_smb_auth", "workgroup"));

	foreach($smb_workgroup_entries as $entry) {
		if ($entry != "") {
			$ac_result_workgroup = $entry;  
			break; 
		}
	}	
		
	
	$ac_smb_fp->AddLine( "# Global settings");
	$ac_smb_fp->AddLine( "[global]\n");
	$ac_smb_fp->AddLine( "workgroup = " . $ac_result_workgroup);
	$ac_smb_fp->AddLine( "server string = " . $ac_serverstring);
	
	$of_uname = posix_uname();
	if (strlen($ac_netbiosname) > 0)
		$ac_smb_fp->AddLine( "netbios name = " . strtoupper(substr($ac_netbiosname, 0, 15)));
	else {
		$tmparr = preg_split('/[^a-zA-Z0-9\-]/', $of_uname["nodename"]); 
		$ac_smb_fp->AddLine( "netbios name = " . strtoupper(substr($tmparr[0], 0, 15)));
	}

	if (strlen($ac_winsserver) > 0)
		$ac_smb_fp->AddLine( "wins server = " . $ac_winsserver);

	$ac_smb_fp->AddLine( "password server = " . $authcfg_obj->get_smb_password_server());
	$ac_smb_fp->AddLine( "realm = " . $authcfg_obj->get_ad_realm());
	$ac_smb_fp->AddLine( "; interfaces = " . "192.168.12.2/24 192.168.13.2/24");
	$ac_smb_fp->AddLine( "; remote announce = " . "92.168.1.255 192.168.2.44");
	$ac_smb_fp->AddLine( "; domain logons = " . "yes");

	$ac_smb_fp->AddLine( "log file = /var/log/samba/%m.log");
	$ac_smb_fp->AddLine( "max log size = 0");
	$ac_smb_fp->AddLine( "; hosts deny = all");
	$ac_smb_fp->AddLine( "map to guest = Bad User");
	$ac_smb_fp->AddLine( "guest account = ofguest");
	$ac_smb_fp->AddLine( "display charset = " . $ac_displaycharset);
	$ac_smb_fp->AddLine( "unix charset = " . $ac_unixcharset);
	$ac_smb_fp->AddLine( "dos charset = " . $ac_doscharset);
	

	if ((strcmp($ac_idmapsync, "ldap") == 0) || ($ac_winbind_encryptedpasswords && (strncasecmp($authcfg_obj->globalSettings['nss_ldap'], "enabled", 7) == 0))) {
		if (strncasecmp($authcfg_obj->get_nss_ldap_tls(), "True", 4) == 0)
			$ac_smb_fp->AddLine( "ldap ssl = start_tls");
		else
			$ac_smb_fp->AddLine( "ldap ssl = no");

		$ac_smb_fp->AddLine( "ldap admin dn = " . trim($authcfg_obj->get_nss_ldap_root_dn()));
		$ac_smb_fp->AddLine( "ldap suffix = " . trim($authcfg_obj->get_nss_ldap_base_dn()));
	}
	
	if ($ac_winbind_encryptedpasswords) {
		$ac_smb_fp->AddLine( "encrypt passwords = yes");
			$ac_smb_fp->AddLine( "security = " . $authcfg_obj->get_smb_security_mode());

		if (strncasecmp($authcfg_obj->globalSettings['nss_ldap'], "enabled", 7) == 0) {
			$ac_smb_fp->AddLine( "passdb backend = ldapsam:" . ((strncasecmp($authcfg_obj->get_nss_ldap_tls(), "True", 4) == 0) ? "ldaps://" : "ldap://")  . trim($authcfg_obj->get_nss_ldap_server()));

			$ac_smb_fp->AddLine( "ldap user suffix = " . $ac_ldapusersuffix);
                        $ac_smb_fp->AddLine( "ldap group suffix = " . $ac_ldapgroupsuffix); 
		}

	}
	
	else {
		$ac_smb_fp->AddLine( "encrypt passwords = no");
		$ac_smb_fp->AddLine( "security = user");
	}

	$ac_smb_fp->AddLine( "smb passwd file = /etc/samba/smbpasswd");
	$ac_smb_fp->AddLine( "unix password sync = yes");
	$ac_smb_fp->AddLine( "passwd program = /usr/bin/passwd %u");
	$ac_smb_fp->AddLine( "passwd chat = *New*password* %n\\n *Retype*new*password* %n\\n *passwd:*all*authentication*tokens*updated*successfully*");
	$ac_smb_fp->AddLine( "pam password change = yes");
	$ac_smb_fp->AddLine( "; username map = /etc/samba/smbusers");
	$ac_smb_fp->AddLine( "obey pam restrictions = yes");
	$ac_smb_fp->AddLine( "load printers = no");
    	$ac_smb_fp->AddLine( "domain master = no");
    	$ac_smb_fp->AddLine( "local master = no");
    	$ac_smb_fp->AddLine( "preferred master = no");
    	$ac_smb_fp->AddLine( "os level = 0");

	if (strncasecmp($authcfg_obj->globalSettings['nss_winbind'], "enabled", 7) == 0) {
		$ac_smb_fp->AddLine( "\n");
        	if ($ac_winbindseparator != "\\")
            		$ac_smb_fp->AddLine( "winbind separator = +");
		
		if ((strcmp($ac_idmapsync, "ldap") == 0) && (strlen(trim($ac_ldapidmapserver)) > 0) && (strlen(trim($ac_ldapidmapsuffix)) > 0)) {
			$ac_smb_fp->AddLine( "idmap backend = ldap:ldap://" . trim($authcfg_obj->get_nss_ldap_server()));
			$ac_smb_fp->AddLine( "ldap idmap suffix = " . trim($ac_ldapidmapsuffix));
		}

		else if (strcmp($ac_idmapsync, "ad") == 0) {

			$xPath = "//idmap_ad/option";
			$idmap_adDom = new XmlHandler(SETTING_XML_IDMAPADSYNC);
			$adOptions = $idmap_adDom->runXpathQuery($xPath); 
			foreach($adOptions as $adOption) {
				$ac_smb_fp->AddLine( "". $adOption->getAttribute('value')); 
			}

		}
		
		else {
			// required to prevent winbind from trying ldap 
			$ac_smb_fp->AddLine( "passdb backend = tdbsam\n");	
			
		}
		
		$ac_smb_fp->AddLine( "idmap uid = " . $authcfg_obj->get_idmap_uid());
		$ac_smb_fp->AddLine( "idmap gid = " . $authcfg_obj->get_idmap_gid());
		$ac_smb_fp->AddLine( "winbind cache time = 15");
		$ac_smb_fp->AddLine( "winbind enum users = yes");
		$ac_smb_fp->AddLine( "winbind enum groups = yes");

		if (strlen($ac_homespath) > 0)
			$ac_smb_fp->AddLine( "template homedir = " . $ac_homespath . "%U");
		else
			$ac_smb_fp->AddLine( "template homedir = /");

		$ac_smb_fp->AddLine( "template shell = " . $authcfg_obj->get_winbind_template_shell());
		if (strcmp($ac_winbindpolicy, "off") == 0)
			$ac_smb_fp->AddLine( "winbind use default domain = no");
		else
			$ac_smb_fp->AddLine( "winbind use default domain = yes");
	}

	$ac_smb_fp->AddLine( "\n");
	
	
	/*  end smb.conf global section */
	
	/* begin proftpd.conf global section */
	
	if ($ftpDom = new XmlHandler(SETTING_XML_FTP)) {
		if ($ftpSettings = $ftpDom->getElementsByTagName("key")) {
			foreach ($ftpSettings as $ftpSetting) {
				$name = $ftpSetting->getAttribute("name");
				$value = $ftpSetting->getAttribute("value");
				$ac_ftp_fp->AddLine($name . "			" . $value); 
			}
		}
	}
	
	else {
		
		$ac_ftp_fp->AddLine("ServerName\t\t\t\"FTP Server\"");
		$ac_ftp_fp->AddLine("ServerIdent\t\t\ton");
		$ac_ftp_fp->AddLine("Port\t\t\t\t21\n");
		$ac_ftp_fp->AddLine("PassivePorts\t\t\t55535 65534");
		$ac_ftp_fp->AddLine("MaxInstances\t\t\t500");
		$ac_ftp_fp->AddLine("TimeoutLogin\t\t\t120");
		$ac_ftp_fp->AddLine("TimeoutIdle\t\t\t600");
		$ac_ftp_fp->AddLine("TimeoutNoTransfer\t\t900");
		$ac_ftp_fp->AddLine("TimeoutStalled\t\t\t3600");
		$ac_ftp_fp->AddLine("TimesGMT\t\t\toff");
		$ac_ftp_fp->AddLine("UseReverseDNS\t\t\toff");
		$ac_ftp_fp->AddLine("IdentLookups\t\t\toff");
		
	}
	

	
	$ac_ftp_fp->AddLine("ServerType\t\t\tstandalone");
	$ac_ftp_fp->AddLine("DeferWelcome\t\t\ton");
	$ac_ftp_fp->AddLine("DefaultServer\t\t\ton");
	$ac_ftp_fp->AddLine("Umask\t\t\t\t022");
	$ac_ftp_fp->AddLine("User\t\t\t\tnobody");
	$ac_ftp_fp->AddLine("Group\t\t\t\tnobody");
	$ac_ftp_fp->AddLine("SystemLog\t\t\t/var/log/ftpd/ftp.log");
	$ac_ftp_fp->AddLine("TransferLog\t\t\t/var/log/ftpd/ftpxfer.log");
	$ac_ftp_fp->AddLine("LogFormat\t\t\tdefault\"%h %l %u %t \\\"%r\\\" %s %b\"");
	$ac_ftp_fp->AddLine("LogFormat\t\t\tauth \"%v [%p] %h %t \"\\%r\\\" %s\"");
	$ac_ftp_fp->AddLine("LogFormat\t\t\twrite \"%h %l %u %t \\\"%r\\\" %s %b\"");
	$ac_ftp_fp->AddLine("DisplayLogin\t\t\twelcome.msg");
	$ac_ftp_fp->AddLine("DisplayFirstChdir\t\t.message");
	$ac_ftp_fp->AddLine("PersistentPasswd\t\toff");
	$ac_ftp_fp->AddLine("AuthPAM\t\t\t\ton");
	$ac_ftp_fp->AddLine("AuthPAMConfig\t\t\tproftpd");
	$ac_ftp_fp->AddLine("<Global>");
	$ac_ftp_fp->AddLine("\tRequireValidShell\t\toff");
	$ac_ftp_fp->AddLine("\tAllowOverwrite\t\t\ton");
	$ac_ftp_fp->AddLine("\tAllowRetrieveRestart\t\ton");
	$ac_ftp_fp->AddLine("\tAllowStoreRestart\t\ton");
	$ac_ftp_fp->AddLine("\t<Limit LOGIN>");
	$ac_ftp_fp->AddLine("\t\tDenyGroup\t\troot");
	$ac_ftp_fp->AddLine("\t</Limit>");
	$ac_ftp_fp->AddLine("\tDefaultRoot\t\t\t/mnt");
	$ac_ftp_fp->AddLine("</Global>");
	$ac_ftp_fp->AddLine("<Directory /mnt>");
	$ac_ftp_fp->AddLine("\tHideFiles \"(\\\\.info.xml|aquota.group|aquota.user)$\"");
	$ac_ftp_fp->AddLine("\t<Limit LOGIN DIRS>");
	$ac_ftp_fp->AddLine("\t\tAllowAll");
	$ac_ftp_fp->AddLine("\t</Limit>");
	$ac_ftp_fp->AddLine("</Directory>\n");
	$ac_ftp_fp->AddLine("Include\t\t\t/etc/proftpd/openfiler-shares.conf");
	
	
	/* end proftpd.conf global section */
	
	/* begin http openfiler-shares.conf section */
	
	
	//$ac_http_fp->AddLine( "<Directory \"/mnt\">");
	
	/* The following two lines no longer supported in httpd 2.2 
	
	$ac_http_fp->AddLine( "    Anonymous_Authoritative Off");
	$ac_http_fp->AddLine( "    AuthAuthoritative Off");
	
	*/
    
    /*
	$ac_http_fp->AddLine( "    Anonymous_NoUserID Off");
	$ac_http_fp->AddLine( "    Anonymous_MustGiveEmail Off");
	$ac_http_fp->AddLine( "    Anonymous_VerifyEmail Off");
	$ac_http_fp->AddLine( "    Anonymous_LogEmail Off");
	$ac_http_fp->AddLine( "    Anonymous anonymous ");
	$ac_http_fp->AddLine( "    Options +Indexes");
	$ac_http_fp->AddLine( "    AllowOverride None");
	$ac_http_fp->AddLine( "    AuthType Basic");
    $ac_http_fp->AddLine( "    AuthBasicAuthoritative off");
    $ac_http_fp->AddLine( "    AuthUserFile  /dev/null");
	$ac_http_fp->AddLine( "    AuthName \"Openfiler share tree\"");
	$ac_http_fp->AddLine( "    AuthPAM_Enabled on");
	$ac_http_fp->AddLine( "    AuthPAM_FallThrough on");
	$ac_http_fp->AddLine( "    Order Allow,Deny");
	$ac_http_fp->AddLine( "    Allow from all");
	$ac_http_fp->AddLine( "          <LimitExcept GET HEAD PROPFIND OPTIONS REPORT>");
	$ac_http_fp->AddLine( "            Deny from all");
	$ac_http_fp->AddLine( "        </LimitExcept>");
	$ac_http_fp->AddLine( "</Directory>");
	$ac_http_fp->AddLine( "");
	$ac_http_fp->AddLine( "");
	$ac_http_fp->AddLine( "<Location /mnt>");
	$ac_http_fp->AddLine( "	Dav on");
	$ac_http_fp->AddLine( "</Location>");
	$ac_http_fp->AddLine( "");
	$ac_http_fp->AddLine( "\n Alias \"/mnt\" \"/mnt\"");
    */
	
	/* end http openfiler-shares.conf section */
	
	
	/* begin Rsync global section */
	
	$ac_rsync_fp->AddLine("port = $ac_rsync_port");
	$ac_rsync_fp->AddLine("motd file = $ac_rsync_motd_file");
	if (strlen($ac_rsync_address) > 0)
		$ac_rsync_fp->AddLine("address = $ac_rsync_address");
	$ac_rsync_fp->AddLine(""); 
	
	/* end Rsync global section */
	
	/* begin ietd initiators.allow/deny global section */
	
	for ($ac_tgt = 0; $ac_tgt < count($ac_targets_name); $ac_tgt++) {
	
		global $ac_iscsi_access, $ac_networks_name;
		$ac_iscsi_access = array();
		$localTargetFilePath = "/opt/openfiler/etc/iscsi/" . $ac_targets_name[$ac_tgt] . "/" . $ac_targets_name[$ac_tgt] . ".xml"; 
		
		for ($l = 0; $l < count($ac_networks_network); $l++)
			$ac_iscsi_access[$l] = FALSE;
			
		if (is_file($localTargetFilePath) && $networkDom = new XmlHandler($localTargetFilePath)) {
			
			$xPathNetwork = "//network"; 
			if ($networkList = $networkDom->runXpathQuery($xPathNetwork)) {
				
				if ($networkList->length > 0) {
					
					for ($counter = 0; $counter < count($ac_networks_name); $counter++) {
						foreach ($networkList as $networkItem) {
							$networkName = $networkItem->getAttribute("network");
							$accessValue = $networkItem->getAttribute("access"); 
						
							if (($ac_networks_name[$counter] == $networkName) && $accessValue == "yes") {
								$ac_iscsi_access[$counter] = TRUE;
								break; 
							}
						}
					}
					
				}
			}
			
			$ac_iscsi_deny_fp->AddLine( $ac_targets_name[$ac_tgt] . " ALL\n");
			
			$iscsi_network_list = "";
			$iscsi_first = TRUE;
			
			for ($l = 0; $l < count($ac_networks_network); $l++) {
				
				if ($ac_iscsi_access[$l]) {
					if ($iscsi_first) {
						$iscsi_network_list .= " " . $ac_networks_network[$l] . convert_netmask($ac_networks_netmask[$l]);
						$iscsi_first = FALSE;	
					}
					
					else {
						$iscsi_network_list .= ", " . $ac_networks_network[$l] . convert_netmask($ac_networks_netmask[$l]);
					}
				}
			}
			
			if (strlen($iscsi_network_list) > 0)
				$ac_iscsi_allow_fp->AddLine($ac_targets_name[$ac_tgt] . " " . $iscsi_network_list);
		}
	}
	
	/* end ietd initiators.allow/deny global section */
	
	$ac_lun = 0;

	for ($ac_vi = 0; $ac_vi < count($ac_volumes_id); $ac_vi++) {
		if ($ac_volumes_fstype[$ac_vi] != "iscsi") {
			$ac_snapshots_suffix = "";
			 
		
			ac_recurse_dir($ac_volumes_mountpoint[$ac_vi], $ac_volumes_id[$ac_vi], $ac_volumes_vg[$ac_vi], false, $ac_request_chmod, $ac_request_chmod_path, "");
			
			for ($l = 0; $l < count($ac_snapshots_id); $l++)
				if (($ac_snapshots_vgname[$l] == $ac_volumes_vg[$ac_vi]) &&
					($ac_snapshots_lvname[$l] == $ac_volumes_id[$ac_vi]) &&
					($ac_snapshots_shared[$l] == "yes")) {
					
					$ac_snapshot_mountpoint = "/mnt/snapshots/" . $ac_snapshots_vgname[$l] . "/" .
					$ac_snapshots_lvname[$l] . "/" . $ac_snapshots_id[$l] . "/";
					$ac_snapshots_suffix = " " . date("Y-m-d H.i.s", strtotime($ac_snapshots_timestamp[$l]));
					
					if (is_dir($ac_snapshot_mountpoint))
						ac_recurse_dir($ac_snapshot_mountpoint, $ac_volumes_id[$ac_vi], $ac_volumes_vg[$ac_vi], true, false, "", "");
				}
		}
	}
	
	
	/* close all opened config files */
	
	endConfigFile($ac_smb_fp);
	endConfigFile($ac_nfsv3_fp);
	endConfigFile($ac_ftp_fp);
	endConfigFile($ac_ftp_conf_fp);
	endConfigFile($ac_http_fp);
	endConfigFile($ac_rsync_fp);
	endConfigFile($ac_iscsi_deny_fp);
	endConfigFile($ac_iscsi_allow_fp); 
	
	/* Now reload the services' configuration */
	
	$ac_smb = (strstr(exec("export LANG=C; /usr/bin/sudo /sbin/service smb status"), "running") ? 1 : 0);
	$ac_nfsv3 = (strstr(exec("export LANG=C; /usr/bin/sudo /sbin/service nfs status"), "running") ? 1 : 0);
	$ac_rsync = (strstr(exec("export LANG=C; /usr/bin/sudo /sbin/service rsync status"), "running") ? 1 : 0);
	$ac_ftp = (strstr(exec("export LANG=C; /usr/bin/sudo /sbin/service proftpd status"), "running") ? 1 : 0);
	$ac_http = (strstr(exec("export LANG=C; /usr/bin/sudo /sbin/service httpd status"), "running") ? 1 : 0);
	$ac_winbind = (strstr(exec("export LANG=C; /usr/bin/sudo /sbin/service winbind status"), "running") ? 1 : 0);

	// first restart winbind

	if ($params["winbindrestart"] == true) {
		if ($ac_winbind)
			exec("export LANG=C; /usr/bin/sudo /sbin/service winbind restart");
	}
	else if ($params["winbindreload"] == true) {
		if ($ac_winbind)
			exec("export LANG=C; /usr/bin/sudo /sbin/service winbind reload");
	}


	// then everything else

	if ($params["services"] == "keep") {

		/* Do nothing */
	}
	
	else if ($params["services"] == "reload") {
		if ($ac_smb)
			exec("export LANG=C; /usr/bin/sudo /sbin/service smb reload");
		if ($ac_nfsv3)
			exec("export LANG=C; /usr/bin/sudo /sbin/service nfs reload");
		if ($ac_ftp)
			exec("export LANG=C; /usr/bin/sudo /sbin/service proftpd reload");
		if ($ac_http)
			exec("export LANG=C; /usr/bin/sudo /sbin/service httpd reload");
		if ($ac_rsync)
			exec("export LANG=C; /usr/bin/sudo /sbin/service rsync reload");
	}
	
	else {
		if ($ac_smb)
			exec("export LANG=C; /usr/bin/sudo /sbin/service smb restart");
		if ($ac_nfsv3)
			exec("export LANG=C; /usr/bin/sudo /sbin/service nfs restart");

		if ($ac_ftp)
			exec("export LANG=C; /usr/bin/sudo /sbin/service proftpd restart");

		if ($ac_http)
			exec("export LANG=C; /usr/bin/sudo /sbin/service httpd restart");
		
		if ($ac_rsync)
			exec("export LANG=C; /usr/bin/sudo /sbin/service rsync restart"); 
	}
	

	/* rsync files which are to be rsync'd to remote target machines, when cluster mode is enabled */

	if ($ac_cluster_enabled) {
		if ((count($ac_cluster_rsync_paths) > 0) && (count($ac_cluster_rsync_hosts) > 0)) {
			for ($i = 0; $i < count($ac_cluster_rsync_hosts); $i++)
				for ($j = 0; $j < count($ac_cluster_rsync_paths); $j++)
					exec("export LANG=C; /usr/bin/sudo /usr/bin/rsync -a --rsh=/usr/bin/ssh " . $ac_cluster_rsync_paths[$j] . " root@" . $ac_cluster_rsync_hosts[$i] . ":" . $ac_cluster_rsync_paths[$j]);
		}
	}
	
} // end apply configuration 


?>
