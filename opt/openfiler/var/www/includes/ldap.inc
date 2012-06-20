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

define('FILE_SLAPD', "/etc/openldap/slapd.conf");
define('CONFIG_XML_HOMESPATH', "/opt/openfiler/etc/homespath.xml"); 
require_once('authconfig.inc.php'); 
require_once('service.inc'); 

global $ldap_password;
global $ldap_basedn;
global $ldap_rootdn;
$ldap_usersuffix="ou=People";
$ldap_groupsuffix="ou=Group";
getvalues();



function getvalues()
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_tls;

	/*
	$authp = popen("export LANG=C; /usr/bin/sudo /usr/sbin/authconfig --test --kickstart", "r");
	while (!feof($authp))
		$aresult[] = trim(fgets($authp, 4096));
	pclose($authp);
	*/
	$authobj = new Authconfig(); 
	
	$ldap_password=$authobj->get_nss_ldap_root_pw();
	$ldap_basedn=$authobj->get_nss_ldap_base_dn();
	$ldap_rootdn=$authobj->get_nss_ldap_root_dn();
    
    $ldap_tls = "";
    
    if ($aresult[20] == "YES") 
        $ldap_tls = " -ZZ ";
        
    
}

function testconnection()
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_tls;
	$cmd="/usr/bin/ldapwhoami $ldap_tls -x -D \"$ldap_rootdn\" -w " . escapeshellarg($ldap_password) . " 2>&1 1>&3";
	exec($cmd, $output, $ret);

	if ($ret != 0)
		return $output;
}

function ldapadd($string)
{
	global $ldap_password, $ldap_rootdn, $ldap_tls;

	$string = str_replace("\n","\\n", $string);
	$cmd="echo -e \"$string\" | /usr/bin/sudo /usr/bin/ldapadd $ldap_tls -x -D \"$ldap_rootdn\" -w " . escapeshellarg($ldap_password) . " 2>&1 1>&3";
	exec($cmd,$output, $ret);

	if ($ret != 0)
		return $output;
}

function ldapmodify($string)
{
	global $ldap_password, $ldap_rootdn;

	$string = str_replace("\n","\\n", $string);
	$cmd="echo -e \"$string\" | /usr/bin/sudo /usr/bin/ldapmodify $ldap_tls -x -D \"$ldap_rootdn\" -w " . escapeshellarg($ldap_password) . " 2>&1 1>&3";
	exec($cmd,$output, $ret);

	if ($ret != 0)
		return $output;
}


function getLDAPgroup()
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix, $ldap_tls;
	$grouparray = array();
	$memberarray = array();

	$searchstring = "objectclass=posixgroup";
	$cmd = "/usr/bin/ldapsearch $ldap_tls -x -D \"" . $ldap_rootdn . "\" -w \"$ldap_password\" -S gidNumber \"" . $searchstring . "\"";
	$groupp = popen($cmd, "r");

	while (!feof($groupp))
	{
		$line =  explode(":", fgets($groupp, 4096));
		$line[0] = trim($line[0]);
		$line[1] = trim($line[1]);

		switch ($line[0]) {
		case "dn":
			$string = explode("cn=", $line[1]);
			$ldap_cn = preg_replace('/,.*/', '', $string[1]);
			break;
		case "gidNumber":
			$ldap_gidNumber = $line[1];
			break;
		case "memberUid":
			$memberarray[] = $line[1];
			break;
		case "memberUid":
			$memberarray[] = $line[1];
			break;
		case "objectClass":
			if ($line[1] == "posixGroup")
				$isgroup = "true";
			break;
		case "":
			if ($isgroup == "true"){
				$grouparray[] = array("name" => $ldap_cn, "gid" => $ldap_gidNumber, "members" => $memberarray);
				unset($memberarray);
				$isgroup = "false";
			}
			break;
		}
	}

	pclose($groupp);
	return $grouparray;
}

function getLDAPuser()
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix, $ldap_tls;
	$searchstring = "objectclass=posixaccount";
	$userarray = array();
	$userp = popen("/usr/bin/ldapsearch $ldap_tls -x -D \"" . $ldap_rootdn . "\" -w \"$ldap_password\" -b \"" . $ldap_usersuffix . "," . $ldap_basedn . "\" \"" . $searchstring . "\" uid uidNumber objectClass", "r");

	$i = 0;
	while (!feof($userp))
	{
		$line =  explode(":", fgets($userp, 4096));
		$line[0] = trim($line[0]);
		$line[1] = trim($line[1]);

		switch ($line[0]) {
		case "uid":
			$ldap_uid  = $line[1];
			break;
		case "uidNumber":
			$ldap_uidNumber = $line[1];
			break;
		case "objectClass":
			if ($line[1] == "posixAccount")
				$isuser = "true";
			break;
		case "":
			if ($isuser == "true"){
				$userarray[] = array("name" => $ldap_uid, "uid" => $ldap_uidNumber);
				$isuser = "false";
			}
		}
	}
	pclose($userp);
	return $userarray;
}

function getLDAPgroupuser($gidNumber)
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix, $ldap_tls;
	if (!$gidNumber)
		return;

	$groupuserarray = array();

	$groupuserp = popen("/usr/bin/ldapsearch $ldap_tls -x -D \"$ldap_rootdn\" -w " . escapeshellarg($ldap_password) . " 'gidNumber=$gidNumber'", "r");

	while (!feof($groupuserp))
	{
		$line =  explode(":", fgets($groupuserp, 4096));

		$line[0] = trim($line[0]);
		$line[1] = trim($line[1]);

		if ($line[0] == "memberUid"){
			$groupuserarray[] = $line[1];
		}
	}
	pclose($groupuserp);
	return $groupuserarray;
}

function newLDAPuser($username, $gid_number, $uid_number)
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix, $global_min_uid;
	if ((!$username) or (strtolower($username) == "root"))
		return;
	if (!$gid_number)
		$gid_number=500;
	if ($uid_number >= $global_min_uid)
		$next_uid=$uid_number;
	else{
		//get next uid number
		$next_uid=500;
		$uid_list= explode("\n", `getent passwd | cut -d ':' -f 3`);
		foreach ($uid_list as $uid)
			if ($uid >= $next_uid)
				$next_uid = $uid + 1;
	}

	$homeDirectory = uGetHomesDir();
	(($homeDirectory != "") ? ($homeDirectory = $homeDirectory . $username) : $homeDirectory = "/dev/null");


	$LDIF="dn: uid=$username,$ldap_usersuffix,$ldap_basedn
	objectClass: inetOrgPerson
	objectClass: posixAccount
	homeDirectory: $homeDirectory
	loginShell: /bin/false
	cn: $username
	givenName: $username
	sn: $username
	uid: $username
	uidNumber: $next_uid
	gidNumber: $gid_number";

	$ret = ldapadd($LDIF);
	if ($ret)
		return $ret;
	
	foreach (getLDAPgroup() as $LDAPgroup)
		if ($LDAPgroup[gid] == $gid_number){
			addLDAPgroupuser($LDAPgroup[name], $username);
			break;
		}
}

function addLDAPgroup($group, $gid_number)
{
	global $ldap_basedn, $ldap_usersuffix, $ldap_groupsuffix, $global_min_gid;
	
	if ($gid_number >= $global_min_gid)
		$next_gid=$gid_number;
	else{
		//get next GID number
		$next_gid=500;
		$gid_list= explode("\n", `getent group | cut -d ':' -f 3`);
		foreach ($gid_list as $gid)
			if ($gid >= $next_gid)
				$next_gid = $gid + 1;
	}
	

	$LDIF="dn: cn=$group, $ldap_groupsuffix, $ldap_basedn
	gidNumber: $next_gid
	objectClass: posixGroup
	cn: $group";

	$ret = ldapadd($LDIF);
	if ($ret) 
		return $ret;
	
}

function editLDAPgroup($group, $newname)
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix;
	$LDIF="dn: cn=$group,$ldap_groupsuffix,$ldap_basedn
	changetype:moddn
	newrdn:cn=$newname";
	
	$ret = ldapmodify($LDIF);
	if ($ret)
		return $ret;
}

function setLDAPpasswd($user, $password)
{	
	global $ldap_password, $ldap_password, $ldap_rootdn, $ldap_usersuffix, $ldap_basedn;

	if ((!$user) or (strtolower($user) == "root"))
		return 2;

	//login smb to ldap
	exec("sudo smbpasswd -w " . escapeshellarg($ldap_password));

	$cmd="echo -e \"$password\n$password\" | sudo /usr/bin/smbpasswd -a " . escapeshellarg($user) . " -s 2>&1 1>&3";
	exec($cmd, $output, $ret);
	if ($ret)
		return $ret;
	
	$cmd = "export LANG=C; /usr/bin/sudo /usr/bin/ldappasswd $ldap_tls -x -w " . escapeshellarg($ldap_password) . " -D \"$ldap_rootdn\" -s " . escapeshellarg($password) . "  uid=".escapeshellarg($user).",\"$ldap_usersuffix\",\"$ldap_basedn\" 2>&1 1>&3";
	
	exec($cmd, $output, $ret);
	if ($ret)
		return $ret;
	
}

function addLDAPgroupuser($group, $user)
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix;
	$LDIF="dn: cn=$group,$ldap_groupsuffix,$ldap_basedn
	add: memberuid
	memberuid: $user";
	
	$ret = ldapmodify($LDIF);
	if ($ret)
		return $ret;
}

function delLDAPgroupuser($group, $user)
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix;
	$LDIF="dn: cn=$group,$ldap_groupsuffix,$ldap_basedn
	delete: memberuid
	memberuid: $user";
	
	$ret = ldapmodify($LDIF);
	if ($ret)
		return $ret;
}

function delLDAPuser($user)
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix;
	$LDIF="dn: uid=$user,$ldap_usersuffix,$ldap_basedn
	changetype: delete";

	$ret = ldapmodify($LDIF);
	if ($ret)
		return $ret;
}

function delLDAPgroup($group)
{
	global $ldap_password, $ldap_basedn, $ldap_rootdn, $ldap_usersuffix, $ldap_groupsuffix;
	$LDIF="dn: cn=$group,$ldap_groupsuffix,$ldap_basedn
	changetype: delete";
	
	$ret = ldapmodify($LDIF);
	if ($ret)
		return $ret;
}


class LDAPServer {
	var $basedn;
	var $rootdn;
	var $rootpw;
	var $directory = "/var/lib/ldap";
	var $authobj;
    	var $tls = ""; 
	var $sm;
	var $srv; 

	function LDAPServer($basedn, $rootdn, $rootpw){

		/*
		$authp = popen("export LANG=C; /usr/bin/sudo /usr/sbin/authconfig --test --kickstart", "r");
		while (!feof($authp))
			$aresult[] = trim(fgets($authp, 4096));
		pclose($authp);
		*/

		$this->authobj = new Authconfig(); 
		$this->sm = new ServiceManager(); 

		$this->srv = $this->sm->getService('ldap');

		$this->basedn = ($basedn)? $basedn : $this->authobj->get_nss_ldap_base_dn();
		$this->rootdn = ($rootdn)? $rootdn : $this->authobj->get_nss_ldap_root_dn(); 
		$this->rootpw = ($rootpw)? $rootpw : $this->authobj->get_nss_ldap_root_pw();
        
		if ($aresult[20] == "YES")
            	$this->tls = " -ZZ "; 
	}

	function Start()
	{

		if (!file_exists($this->directory)) {
			exec("export LANG=C; /usr/bin/sudo /bin/mkdir -p $this->directory");
		}

		exec("export LANG=C; /usr/bin/sudo /bin/chown -R ldap:ldap $this->directory");
		exec("export LANG=C; /usr/bin/sudo /bin/chmod 700 $this->directory");
		if (!file_exists('/etc/openldap/cacerts'))
			exec("export LANG=C; /usr/bin/sudo /bin/mkdir /etc/openldap/cacerts"); //this is need to start ldap

		$this->srv->enable(); 
		$return = $this->srv->start(True); 
		//exec("export LANG=C; /usr/bin/sudo /sbin/chkconfig ldap on");
		//exec("export LANG=C; /usr/bin/sudo /sbin/service ldap start", $output, $retval);

		if ($return['ret'] != 0)
			return $return['output'];
	}

	function Authconfig(){ 
		//$authconfigcommand = "export LANG=C; /usr/bin/sudo /usr/sbin/authconfig --kickstart --disablecache";
		
		$authconfigcommand .= " --enableldap --enableldapauth --ldapserver=\"127.0.0.1\" --ldapbasedn=\"" . 
					$this->basedn . "\" --ldaprootbinddn=\"" . $this->rootdn . "\" --ldaprootbindpw='" . 
					$this->rootpw . "' --ldapbinddn=\"\" --ldapbindpw=\"\"";
		//exec($authconfigcommand);
		$this->authobj->command_string=$authconfigcommand;
		$this->authobj->execute(); 
	}

	function Stop()
	{
		$this->srv->stop();
		$this->srv->disable(); 
	}

	function Restart()
	{
		$this->srv->restart(); 
	}

	function Rebuild()
	{
		$this->Stop();
		exec("export LANG=C; /usr/bin/sudo /usr/bin/db_recover -h $this->directory");
		$this->Start();
		sleep(2);
		$this->srv->restart(); //needs to be started twice
	}

	function Create(&$error)
	{
		$this->create_slapd();
		$status = $this->Start();


		if (!is_null($status) && is_array($status)) {
			if (preg_grep("/stale lock file/", $status)){
			//found stale lock file
				$error[] = "Stale lock files found in LDAP directory, attepting to correct";
				$this->Rebuild();
			}
		}

		$this->Authconfig();
		$this->create_base();
	}

	function Backup()
	{
		exec("/usr/bin/ldapsearch " . $this->tls . " -x -D \"" . $this->rootdn . "\" -w \"" . $this->rootpw . "\" -L", $output, $ret);
		return $output;
	}

	function Restore($input)
	{
		$this->Delete();
		$this->Start();
		sleep(1);

		$descriptorspec = array(
		   0 => array("pipe", "r"),
		   1 => array("pipe", "w"),
		   2 => array("pipe", "w"));

		$process = proc_open("export LANG=C; /usr/bin/sudo /usr/bin/ldapadd " . $this->tls . " -x -D \"" . $this->rootdn . "\" -w \"" . $this->rootpw . "\"", $descriptorspec, $pipes);
		foreach ($input as $line)
			fwrite($pipes[0], $line);
		fclose($pipes[0]);
		fclose($pipes[1]);

		$error = fgets($pipes[2]);
		fclose($pipes[2]);

		$retval = proc_close($process);
		if ($retval)
			return "[" . $retval . "] " . $error;
	}

	function Delete()
	{
		exec("export LANG=C; /usr/bin/sudo /bin/rm -rf " . $this->directory);
		$this->Stop();
	}

	function create_slapd()
	{
		$rootpw=exec("/usr/sbin/slappasswd -s \"" . $this->rootpw . "\"");

		$file = new File(FILE_SLAPD);
		$file->AddLine("# This configuration file was generated by Openfiler");
		$file->AddLine("");
		$file->AddLine("#");
		$file->AddLine("# See slapd.conf(5) for details on configuration options.");
		$file->AddLine("# This file should NOT be world readable.");
		$file->AddLine("#");
		$file->AddLine("include		/etc/openldap/schema/core.schema");
		$file->AddLine("include         /etc/openldap/schema/cosine.schema");
		$file->AddLine("include         /etc/openldap/schema/inetorgperson.schema");
		$file->AddLine("include         /etc/openldap/schema/nis.schema");
		$file->AddLine("include		/etc/openldap/schema/samba.schema");
		$file->AddLine("");
		$file->AddLine("# Allow LDAPv2 client connections.  This is NOT the default.");
		$file->AddLine("allow bind_v2");
		$file->AddLine("");
		$file->AddLine("pidfile		/var/run/openldap/slapd.pid");
		$file->AddLine("argsfile	/var/run/openldap/slapd.args");
		$file->AddLine("");
		$file->AddLine("access to dn.base=\"\"");
		$file->AddLine("	by * read");
		$file->AddLine("");
		$file->AddLine("access to attrs=sambaPasswordHistory");
		$file->AddLine("	by self write");
		$file->AddLine("	by * auth");
		$file->AddLine("");
		$file->AddLine("access to attrs=sambaLMPassword");
		$file->AddLine("	by self write");
		$file->AddLine("	by * auth");
		$file->AddLine("");
		$file->AddLine("access to attrs=sambaNTPassword");
		$file->AddLine("	by self write");
		$file->AddLine("	by * auth");
		$file->AddLine("");
		$file->AddLine("access to attrs=userPassword");
		$file->AddLine("	by self write");
		$file->AddLine("	by * auth");
		$file->AddLine("");
		$file->AddLine("access to attrs=shadowLastChange");
		$file->AddLine("	by self write");
		$file->AddLine("	by * auth");
		$file->AddLine("");
		$file->AddLine("access to *");
		$file->AddLine("	by * read");
		$file->AddLine("");
		$file->AddLine("");
		$file->AddLine("#######################################################################");
		$file->AddLine("# BDB database definitions");
		$file->AddLine("#######################################################################");
		$file->AddLine("");
		$file->AddLine("database	bdb");
		$file->AddLine("suffix		\"" . $this->basedn . "\"");
		$file->AddLine("rootdn		\"" . $this->rootdn . "\"");
		$file->AddLine("");
		$file->AddLine("# Cleartext passwords, especially for the rootdn, should");
		$file->AddLine("# be avoid.  See slappasswd(8) and slapd.conf(5) for details.");
		$file->AddLine("# Use of strong authentication encouraged.");
		$file->AddLine("rootpw		" . $rootpw);
		$file->AddLine("");
		$file->AddLine("# The database directory MUST exist prior to running slapd AND");
		$file->AddLine("# should only be accessible by the slapd and slap tools.");
		$file->AddLine("# Mode 700 recommended.");
		$file->AddLine("directory	" . $this->directory);
		$file->AddLine("");
		$file->AddLine("# Indices to maintain");
		$file->AddLine("index	objectClass	eq");

		$file->Save();
		$file->chown("ldap","ldap");
		$file->chmod("600");
	}


	function create_base()
	{
		global $ldap_groupsuffix;
		global $ldap_usersuffix;

		$ldap_group=str_replace("=",":",$ldap_groupsuffix);
		$ldap_user=str_replace("=",":",$ldap_usersuffix);
		$ldap_name=preg_replace("/.*=/","",preg_replace("/,.*/","", $this->basedn));

		sleep(1);

		$LDIF="# create base
		dn: $this->basedn
		objectClass: top
		objectClass: dcObject
		objectClass: organization
		dc: $ldap_name
		o: $ldap_name";

		exec("echo -e \"" . str_replace("\n","\\n",$LDIF) . "\" | /usr/bin/sudo /usr/bin/ldapadd " . $this->tls . " -x -D \"" . $this->rootdn. "\" -w " . $this->rootpw);

		$LDIF="#create user
		dn: $ldap_usersuffix, $this->basedn
		$ldap_user
		objectClass: top
		objectClass: organizationalUnit";

		exec("echo -e \"" . str_replace("\n","\\n",$LDIF) . "\" | /usr/bin/sudo /usr/bin/ldapadd " . $this->tls . " -x -D \"" . $this->rootdn . "\" -w " . $this->rootpw);

		$LDIF="#create group
		dn: $ldap_groupsuffix, $this->basedn
		$ldap_group
		objectClass: top
		objectClass: organizationalUnit";

		exec("echo -e \"" . str_replace("\n","\\n",$LDIF) . "\" | /usr/bin/sudo /usr/bin/ldapadd " . $this->tls . " -x -D \"" . $this->rootdn . "\" -w " . $this->rootpw);
	}

}


?>
