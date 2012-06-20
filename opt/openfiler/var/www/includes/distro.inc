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



function htmlEntitiesWrapper ($string) {

    return htmlentities($string, ENT_COMPAT, "UTF-8");
    
}

$lvm_version = 2;
// $lvm_version = 1;
if (file_exists("/sbin/lvm"))
	$lvm_command_prefix = "/sbin/lvm "; 
else
	$lvm_command_prefix = "/usr/sbin/lvm ";
// $lvm_command_prefix = "/sbin/";

$fs_info = array(
                array(
                    "type" => "xfs",
                    "name" => "XFS",
                    "fs_block_size" => 1024,
                    "mkfs_command" => "/sbin/mkfs.xfs -f",
                    "mount_options" => "defaults,usrquota,grpquota",
                    "snapshot_mount_options" => "defaults,nouuid",
                    "resize_command" => "/usr/bin/sudo /usr/sbin/xfs_growfs"
                ),
    
    
                array(
                    "type" => "ext3",
                    "name" => "Ext3",
                    "fs_block_size" => 1024,
                    "mkfs_command" => "/sbin/mke2fs -j",
                    "mount_options" => "defaults,usrquota,grpquota,acl,user_xattr",
                    "snapshot_mount_options" => "defaults,acl,user_xattr", 
                    "resize_command" => "/usr/bin/sudo /sbin/resize2fs"
                ),


                array(
                    "type" => "btrfs",
                    "name" => "btrfs",
                    "fs_block_size" => 4096,
                    "mkfs_command" => "/sbin/mkfs.btrfs",
                    "mount_options" => " ",
                    "snapshot_mount_options" => " ",
                    "resize_command" => " "
                
                ),

                array(
                    "type" => "ext4",
                    "name" => "Ext4",
                    "fs_block_size" => 4096,
                    "mkfs_command" => "/sbin/mkfs.ext4",
                    "mount_options" =>  "defaults,usrquota,grpquota,acl",
                    "snapshot_mount_options" => "defaults,acl",
                    "resize_command" => "/usr/bin/sudo /sbin/resize2fs -f"
                ),
    
                array(
                    "type" => "iscsi",
                    "name" => "block (iSCSI,FC,etc)",
                    "fs_block_size" => 0, 
                    "mkfs_command" => "/bin/true",
                    "mount_options" => "",
                    "snapshot_mount_options" => "",
                    "resize_command" => "/bin/true"
                ),


        );
?>
