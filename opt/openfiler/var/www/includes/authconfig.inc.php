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

require_once('pstools.inc.php'); 
define('CMD_AUTHCONFIG', "export LANG=C; /usr/bin/sudo /usr/sbin/authconfig ");


class Authconfig {
	
	public $winbindadmin = ""; 	
	public $winbindpassd = "";	
	private $keys = array('caching','nss_compat','nss_db','nss_hesiod','nss_ldap',
                                'nss_nis', 'nss_nisplus', 'nss_winbind', 'nss_wins', 'pam_unix',
                                'pam_krb5', 'pam_ldap', 'pam_pkcs11', 'pam_smb_auth', 'pam_winbind',
                                'pam_passwdqc', 'authorize_local', 'authenticate_network','pam_cracklib');
	public $globalSettings;
	private $dom; 
	private $domxpath; 
	
	public $command_string = "";

	
	public function Authconfig() {

		$this->dom = new DOMDocument();
		
	  	#$ret = PsExecute(CMD_AUTHCONFIG . " --enablelocalauthorize"); 
		$ph = popen(CMD_AUTHCONFIG . " --update --enablelocauthorize --testxml", "r");
		$string = fread($ph, 16384);
		pclose($ph); 

		$this->dom->loadXML($string); 
		$this->domxpath = new DOMXPath($this->dom); 
		$this->reset();
	}

	public function test_ldap_connection($ldap_rootdn, $ldap_basedn ,$ldap_password, $ldap_tls=False) {

        	$cmd="/usr/bin/ldapwhoami $ldap_tls -x -D \"$ldap_rootdn\" -w " . escapeshellarg($ldap_password) . " 2>&1 1>&3";
        	exec($cmd, $output, $ret);

        	if ($ret != 0)
			return False;
		return True;
	}




	public function reset() {
		
		$this->initAuthConfig(); 

	}

	private function initAuthconfig() {
	
		foreach ($this->keys as $key) {

			$xpath = "//authconfig/globals/key[@name='$key']";
			//$domxpath = new DOMXPath($this->dom);
			$value = $this->domxpath->query($xpath)->item(0)->getAttribute('value');
			$this->globalSettings[$key] = $value;
		}

	}

	public function get_wins_server() {

		$winsserver = ""; 
		
		$winsserver = $this->get_key_settings("", "");
		return $winsserver; 
	}

	public function get_ad_realm() {

		$adrealm = $this->get_key_settings("pam_winbind","realm"); 
		return $adrealm; 
	}

	public function get_krb5_realm() {
		$krb5realm = $this->get_key_settings("pam_krb5", "krb5_realm"); 
		return $krb5realm; 
	}

	public function get_krb5_kdc() {
		$krb5kdc = $this->get_key_settings("pam_krb5", "krb5_kdc");
		return $krb5kdc; 

	}

	public function get_krb5_admin_server() {
		$krb5adminserver = $this->get_key_settings("pam_krb5", "krb5_admin_server"); 
		return $krb5adminserver; 
	}

	public function get_smb_password_server() {

		$smbpasswdserver = $this->get_key_settings("pam_winbind", "servers"); 
		return $smbpasswdserver; 
	}

	public function get_smb_security_mode() {
		
		$smbsecuritymode = $this->get_key_settings("pam_winbind", "security");
		return $smbsecuritymode; 

	}

	public function get_nss_ldap_base_dn() {

		$nssldapbasedn = $this->get_key_settings("nss_ldap", "base_dn"); 
		return $nssldapbasedn; 
	}

	public function get_nss_ldap_server() {
		
		$nssldapserver = $this->get_key_settings("nss_ldap", "server"); 
		return $nssldapserver; 
	}

	public function get_nss_ldap_binddn() {
		$nssldapbinddn = $this->get_key_settings("nss_ldap", "binddn");
		return $nssldapbinddn; 
	}

	public function get_nss_ldap_bindpw() {
		$nssldapbindpw = $this->get_key_settings("nss_ldap", "bindpw"); 
		return $nssldapbindpw; 
	}

	public function get_nss_ldap_root_dn() {
		$nssldapauthdn = $this->get_key_settings("nss_ldap", "rootbinddn");
		return $nssldapauthdn;
	}

	public function get_nss_ldap_root_pw() {
		$nssldaprootpw = $this->get_key_settings("nss_ldap", "rootbindpw"); 
		return $nssldaprootpw; 
	}

	public function get_nss_nis_domain()  {

		$nssnisdomain = $this->get_key_settings("nss_nis", "domain");
		return $nssnisdomain; 
	}

	public function get_nss_nis_server() {
		
		$nssnisserver = $this->get_key_settings("nss_nis", "server"); 
		return $nssnisserver; 
	}

	public function get_pam_ldap_base_dn() { 
		$pam_ldap_basedn = $this->get_key_settings("pam_ldap", "base_dn"); 
		return $pam_ldap_basedn; 

	}

	public function get_idmap_uid() {
		
		$idmapuid = $this->get_key_settings("nss_winbind", "idmap_uid"); 
		return $idmapuid; 

	}

	public function get_idmap_gid() {

		$idmapgid = $this->get_key_settings("nss_winbind", "idmap_gid"); 
		return $idmapgid;
	}


	public function get_pam_winbind_workgroup() {
	
		$pamwinbindworkgroup = $this->get_key_settings("pam_winbind", "workgroup");
		return $pamwinbindworkgroup; 

	}
	
	public function get_winbind_template_shell() {
		$winbind_template_shell = $this->get_key_settings("nss_winbind", "winbind_template_shell");
		return $winbind_template_shell; 
	}

	public function get_winbind_domain_controllers() {

		$winbinddc = $this->get_key_settings("nss_winbind", "servers"); 
		return $winbinddc; 
	
	}
	
	public function get_nss_ldap_tls() {

		$nss_ldap_tls = $this->get_key_settings("nss_ldap", "tls");
		return $nss_ldap_tls;

	}

	public function get_pam_ldap_tls() {
		$pam_ldap_tls = $this->get_key_settings("pam_ldap", "tls"); 
		return $pam_ldap_tls; 
	}


	public function get_pam_ldap_server() {
		
		$pam_ldap_server = $this->get_key_settings("pam_ldap", "server"); 
		return $pam_ldap_server;
	}

	public function get_key_settings($keyname, $settingsname) {

		$value = ""; 

		$xpath = "//authconfig/globals/key[@name='$keyname']/settings";
		//$domxpath = new DOMXPath($this->dom);
		$output = $this->domxpath->query($xpath)->item(0)->getAttribute($settingsname);
		if ($output != false)
			$value = $output; 
		return $value;   
	
	}


	public function execute() {

		$ret = PsExecute(CMD_AUTHCONFIG . " --update " . $this->command_string);
		return $ret; 
	}
	
	public function enableNIS($nisserver, $nisdomain) {
		
	  	$cmd_args =  " --update --enablenis " . " --nisdomain=" . escapeshellarg($nisdomain) . " --nisserver=" . escapeshellarg($nisserver); 	

		$ret = PsExecute(CMD_AUTHCONFIG . $cmd_args);	
		return $ret;
	}	

	public function enableLDAP($ldapserver, $ldapbasedn, $ldapbinddin, $ldapbindpw, $ldaprootbinddn, $ldaprootbindpw)

	{

	}

	public function enableHesiod($hesiodlhs, $hesiodrhs) {
		
		$cmd_args = " --update --enablehesiod --hesiodlhs=" . escapeshellarg($hesiodlhs) . " --hesiodrhs=" . escapeshellarg($hesiodrhs); 
		$ret = PsExecute(CMD_AUTHCONFIG . "$cmd_args"); 
		return $ret; 
	}

	public function disableHesiod() {

		$cmd_args = " --update --disablehesiod";
                $ret = PsExecute(CMD_AUTHCONFIG . $cmd_args);
                return $ret;

	}

	public function useWinbind($winbindseparator, $winbindtemplateshell, $winbindtype, $winbindcontrollers, $winbindrealm, $winbinduidrange, $winbindgidrange, $winbindjoin=false) {

		$cmd_args = " --update --enablewinbind --winbindseparator=" . escapeshellarg($winbindseparator) ;
		$cmd_args .= "--winbindtemplateshell=" . escapeshellarg($winbindtemplateshell) ; 
		$cmd_args .= "--smbsecurity=" . escapeshellarg($smbsecurity);  
		$cmd_args .= "--smbservers=" . escapeshellarg($winbindcontrollers); 
		$cmd_args .= "--smbrealm=" . escapeshellarg($winbindrealm); 
		$cmd_args .= "--smbidmapuid=" . escapeshellarg($winbinduidrange); 
		$cmd_args .= "--smbidmapgid=" . escapeshellarg($winbindgidrange); 
		
		if ($winbindjoin) {
			$cmd_args .= " --winbindjoin=" . escapeshellarg($this->winbindadmin) . "%" . $winbindpasswd; 
		}
		
		$ret = PsExecute(CMD_AUTHCONFIG . $cmd_args);
		return $ret; 

	}

	public function disableWinbind() {

		$cmd_args = " --update --disablewinbind";
		$ret = PsExecute(CMD_AUTHCONFIG . $cmd_args); 
		return $ret; 
	}

		
}

?>
