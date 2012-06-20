<?php


/*
 * --------------------------------------------------------------------
 * Copyright (c) Nathan Jones (najones AT vt.edu)
 * --------------------------------------------------------------------
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


/*
 * Provides necessary classes and functions for handling and manulating network
 * configurations
 *
 */

define('FILE_NETWORKHOSTS', "/opt/openfiler/etc/networks.xml");
define('FILE_RESOLV', "/etc/resolv.conf");
define('FILE_NETWORK', "/etc/sysconfig/network");
define('FILE_HOSTS', "/etc/hosts");
define('FILE_MODPROBE', "/etc/modprobe.conf");
define('CMD_ETHTOOL', "export LANG=C; /usr/bin/sudo /sbin/ethtool 2>&1 ");
define('CMD_MIITOOL', "export LANG=C; /usr/bin/sudo /sbin/mii-tool 2>&1 ");
define('CMD_IFCONFIG', "export LANG=C; /usr/bin/sudo /sbin/ifconfig 2>&1 ");
define('CMD_VCONFIG', "export LANG=C; /usr/bin/sudo /sbin/vconfig 2>&1 ");
define('CMD_IFUP', "export LANG=C; /usr/bin/sudo /sbin/ifup 2>&1 ");
define('CMD_IFDOWN', "export LANG=C; /usr/bin/sudo /sbin/ifdown 2>&1 ");
define('CMD_HOSTNAME', "export LANG=C; /usr/bin/sudo /bin/hostname 2>&1 ");


Class Network {
	var $devices = array();
	var $bonds = array();
	var $vinterfaces = array();
	var $vlans = array();

	function Network() {
		//Network constructor
		$this->GetNICs();
		$this->GetVIFs();
		$this->GetBonds();
		$this->GetVLANs();
		return 0;
	}

	function ListDevices() {
		$list = array_merge($this->ListBonds(), $this->ListNICs(), $this->ListVIFs(), $this->ListVLANs());
		sort($list);
		return $list;
		
	}

	function ListNICs() {
		return $this->devices;
	}

	function ListBonds() {
		return $this->bonds;
	}

	function ListVIFs() {
		return $this->vinterfaces;
	}

	function ListVLANs() {
		return $this->vlans;
	}

	function GetNICs() {
		$sys_device_paths = glob('/sys/class/net/eth*');
		$this->devices = array_map(@basename, $sys_device_paths);
		$this->devices = array_unique($this->devices);
		sort($this->devices);
	}

	function GetVIFs() {
		$devlist = array();
		$d = dir("/etc/sysconfig/network-scripts/");
		$needle = "ifcfg-eth[0-9]+:";
		while ($entry = $d->read()){
			if (preg_match("/^" . $needle . "/i", $entry))
				$devlist[] = preg_replace("/ifcfg-/","",$entry);
		}
		//limit Virtual Devices to real interfaces with following code
		if (empty($this->devices)) $this->GetNICs();
		foreach ($this->devices as $device){
			foreach($devlist as $dev){
				if (preg_match("/" . $device . ":[0-9]/", $dev, $match)){
					$this->vinterfaces[] = $match[0];
				}
			}
		}
	}

	function GetVLANs() {
		$devlist = array();
		$d = dir("/etc/sysconfig/network-scripts/");
		$needle = "ifcfg-eth[0-9]+\.";
		while ($entry = $d->read()){
			if (preg_match("/^" . $needle . "/i", $entry))
				$devlist[] = preg_replace("/ifcfg-/","",$entry);
		}
		//limit VLANS to real interfaces with following code
		if (empty($this->devices)) $this->GetNICs();
		foreach ($this->devices as $device){
			foreach($devlist as $dev){
				if (preg_match("/" . $device . "\.[0-9]+/", $dev, $match)){
					$this->vlans[] = $match[0];
				}
			}
		}
	}

	function GetBonds(){
		$d = dir("/etc/sysconfig/network-scripts/");
		$needle = "ifcfg-bond";
		while ($entry = $d->read()){
			if (preg_match("/^" . $needle . "/i", $entry))
				$this->bonds[] = preg_replace("/ifcfg-/","",$entry);
		}
		
		sort($this->bonds);	
	}

	function GetDNS() {
		$file = new File(FILE_RESOLV);
		if(!$file->Load())
			return false; //couldn't open file

		$entries = array();
		$needle = "nameserver ";
		while (!$file->EOF()){
			if (preg_match("/" . $needle . "[^ ][^ ]*/i", $file->GetLine(), $match)){
				$entries[] = trim(preg_replace("/" . $needle ."/i", "", $match[0]));
			}
		}
		return $entries;
	}

	function IsDHCP() {
		foreach ($this->ListNICs() as $device){
			$eth = new NetworkCard($device);
			if ($eth->IsDHCP())
				return true;
		}
		return false;
	}

	function SetDNS($dns1, $dns2) {
		$file = new File(FILE_RESOLV);

		$file->AddLine("search " . $this->GetHostname());
		if ($dns1)
			$file->AddLine("nameserver " . $dns1);
		if ($dns2)
			$file->AddLine("nameserver " . $dns2);
		$file->Save();
	}

	function GetHostname() {
		exec(CMD_HOSTNAME, $output, $retval);
		$output = implode(" ", $output);
		if ($retval)
			$error = $output; //capturuing error for future development
		return $output;
	}

	function GetDomain() {
		exec(CMD_HOSTNAME . " -d", $output, $retval);
		$output = implode(" ", $output);
		if ($retval)
			$error = $output; //capturuing error for future development
		return $output;
	}

	function GetGatewayDevice() {
		$file = new File(FILE_NETWORK);
		if(!$file->Load())
			return false; //couldn't open file

		$needle = "gatewaydev=";
		while (!$file->EOF()){
			if (preg_match("/" . $needle . "[^ ][^ ]*/i", $file->GetLine(), $match)){
				return trim(preg_replace("/" . $needle . "/i", "", $match[0]));
			}
		}
	}

	function SetGatewayDevice($device) {
		$file = new File(FILE_NETWORK);
		$file->Load();
		$file->EditLine("GATEWAYDEV=", "GATEWAYDEV=" . $device);
		$file->Save();
	}

	function GetGateway() {
		$file = new File(FILE_NETWORK);
		if(!$file->Load())
			return false; //couldn't open file

		$needle = "gateway=";
		while (!$file->EOF()){
			if (preg_match("/" . $needle . "[^ ][^ ]*/i", $file->GetLine(), $match)){
				return trim(preg_replace("/" . $needle . "/i", "", $match[0]));
			}
		}
	}

	function SetGateway($gateway) {
		$file = new File(FILE_NETWORK);
		$file->Load();
		$file->EditLine("GATEWAY=", "GATEWAY=" . $gateway);
		$file->Save();
	}

	function SetHostname($hostname) {
		$hostname_split = split("\.", $hostname);
		$file = new File(FILE_RESOLV);
		$file->Load();
		$file->EditLine("search ", "search " . $hostname);
		$file->Save();

		$file = new File(FILE_NETWORK);
		$file->Load();
		$file->EditLine("HOSTNAME=", "HOSTNAME=" . $hostname);
		$file->Save();

		$file = new File(FILE_HOSTS);
		$file->AddLine("# Do not remove the following line, or various programs");
		$file->AddLine("# that require network functionality will fail.");
		$file->AddLine("127.0.0.1		" . $hostname . " " . $hostname_split[0] . " localhost.localdomain localhost");
		$file->Save();

		exec("export LANG=C; /usr/bin/sudo /bin/hostname " . $hostname);
	}

	function UpdatePrompt(){
		exec("export LANG=C; /usr/bin/sudo /etc/init.d/openfiler-appliance restart > /dev/null 2> /dev/null");
	}

	function Restart() {
		exec("export LANG=C; /usr/bin/sudo /sbin/service network restart 2>&1", $output, $retval);
		if ($retval) //error occured
			$error = implode(" ", $output); //future
		return $retval;
	}

	function IsValidIP($ip) { 
		if (preg_match("/^[0-9]{1,3}(.[0-9]{1,3}){3}$/",$ip)) {
			foreach(explode(".", $ip) as $octet) {
				if ($octet<1 || $octet>255)
					return false;
			}
		}
		else
			return false;
		return true;
	}

	//this function is to generate all the possible netmasks
	function GenerateNetmasks() {
		$values = array();
		for ($i = 31; $i >= 0; $i--) {
			$values[] = (long2ip($intmask));
			$intmask += intval(pow(2, $i));
		}
		$values[] = long2ip(pow(2, 32) - 1);
		return $values;
	}
}

Class NetworkCard extends Network{

	var $device;
	var $ifconfig;
	var $ethtool;

	function NetworkCard($dev){
		$this->device=$dev;
		exec(CMD_IFCONFIG . $this->device, $this->ifconfig);
	}

	function GetDevice(){
		return $this->device;
	}

	function IsEnabled() {
		exec(CMD_IFCONFIG, $output);
		foreach($output as $line){
			if (preg_match("/^" . $this->device ."/", $line))
				return true;
		}
		return false;
	}

	function IsConnected() {
		$ph = popen(CMD_ETHTOOL . $this->device, "r");
		while (!feof($ph)){
			if (preg_match("/detected: [yesno]*/i", fgets($ph, 4096), $match)){
				if (preg_match("/yes/i", $match[0]))
					return true;
			}
		}
		return false;
	}

	//check the Mii compatiblity of the network card
	function Mii() {
		//exec(CMD_MIITOOL . $this->device, $output, $retval);
		//return $retval;
		return TRUE;
	}

	function GetSpeed() {
		$ph = popen(CMD_ETHTOOL . $this->device, "r");
		$needle = "speed: ";
		while (!feof($ph)){
			if (preg_match("/" . $needle . "[^ ][^ ]*/i", fgets($ph, 4096), $match)){
				return trim(preg_replace("/" . $needle . "/i", "", $match[0]));
			}
		}
		return false;
	}

	function IsDHCP() {
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->device);
		if(!$file->Load())
			return false;  //can't open file, exit

		while (!$file->EOF()){
			$line = split("=", $file->GetLine());
			if ((trim(strtoupper($line[0]))=="BOOTPROTO") && (trim(strtoupper($line[1]))=="DHCP"))
				return true;
		}
		return false;
	}

	function IsSlave() {
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->device);
		if(!$file->Load())
			return false;  //can't open file, exit

		while (!$file->EOF()){
			$line = split("=", $file->GetLine());
			if ((trim(strtoupper($line[0]))=="SLAVE") && (trim(strtoupper($line[1]))=="YES"))
				return true;
		}
		return false;
	}

	function IsVirtual() {
		$needle = "eth[0-9]+:";
		if (preg_match("/^" . $needle . "/i", $this->GetDevice()))
			return true;
		return false;
	}

	function IsVLAN() {
		$needle = "eth[0-9]+\.[0-9]";
		if (preg_match("/^" . $needle . "/i", $this->GetDevice()))
			return true;
		return false;
	}

	function IsMaster() {
		$needle = "bond[0-9]";
		if (preg_match("/^" . $needle . "/i", $this->GetDevice()))
			return true;
		return false;
	}

	function GetMaster() {
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->device);
		if(!$file->Load())
			return false; //couldn't open file

		$needle = "master=";
		while (!$file->EOF()){
			if (preg_match("/" . $needle . "[^ ][^ ]*/i", $file->GetLine(), $match)){
				return trim(preg_replace("/" . $needle . "/i", "", $match[0]));
			}
		}
		return false;
	}

	function GetIP() {
		foreach($this->ifconfig as $line){
			if (preg_match("/inet addr:[0-9\.]*/i", $line, $match)){
				$match = split(":", $match[0]);
				return $match[1];
			}
		}
		return false;
	}

	function GetMask() {
		foreach($this->ifconfig as $line){
			if (preg_match("/mask:[0-9\.]*/i", $line, $match)){
				$match = split(":", $match[0]);
				return $match[1];
			}
		}
		return false;
	}

	function GetMTU() {
		foreach($this->ifconfig as $line){
			if (preg_match("/mtu:[0-9]*/i", $line, $match)){
				$match = split(":", $match[0]);
				return $match[1];
			}
		}
		return false;
	}

	function GetMAC() {
		foreach($this->ifconfig as $line){
			if (preg_match("/hwaddr [0-9A-F:]*/i", $line, $match)){
				$match = split(" ", $match[0]);
				return $match[1];
			}
		}
		return false;
	}

	function GetTxBytes() {
		foreach($this->ifconfig as $line){
			if (preg_match("/TX bytes:[^)]*\)/i", $line, $match)){
				$match = split(":", $match[0]);
				return $match[1];
			}
		}
		return false;
	}

	function GetRxBytes() {
		foreach($this->ifconfig as $line){
			if (preg_match("/RX bytes:[^)]*\)/i", $line, $match)){
				$match = split(":", $match[0]);
				return $match[1];
			}
		}
		return false;
	}

	function GetGateway() {
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->device);
		if(!$file->Load())
			return false; //couldn't open file

		$needle = "gateway=";
		while (!$file->EOF()){
			if (preg_match("/" . $needle . "[^ ][^ ]*/i", $file->GetLine(), $match)){
				return trim(preg_replace("/" . $needle . "/i", "", $match[0]));
			}
		}
		return false;
	}

	function RemoveSlave(){
		exec("export LANG=C; /usr/bin/sudo /sbin/ifenslave -d " . $this->GetMaster() . " " . $this->GetDevice(), $output, $retval);

		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->GetDevice());
		$file->Load();
		$file->DeleteLine("master", false);
		$file->DeleteLine("slave", false);
		$file->Save();
	}

	function AddVIF(){
		$highest = 1;
		$this->getVIFs();
		foreach ($this->ListVIFs() as $vinterface){
			if (preg_match("/" . $this->device . ":([0-9])/", $vinterface, $match))
				if ($highest <= $match[1])
					$highest=$match[1] + 1;
		}
		//create a new empty configuration file
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->GetDevice() . ":" . $highest);
		$file->Save();
	}

	function AddVLAN($id){

		//create a new empty configuration file
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->GetDevice() . "." . $id);
		$file->Save();
	}

	function Save($dhcp, $ip, $netmask, $mtu) {
		//first remove from any bond
                $isSlave = false;
                $isVLAN = false;
		$isMaster = false;
			
		if ($this->IsMaster()) {
			$isMaster = true;
		}

		if ($this->IsSlave()) {
			$this->RemoveSlave();
                        $isSlave = true;
                }

		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->device);
		$file->AddLine("DEVICE=" . $this->device);
		if ($this->IsVLAN()) {
			$file->AddLine("VLAN=yes");
                        $isVLAN = true; 
                }
		if ($mtu)
			$file->AddLine("MTU=" . $mtu);
		$file->AddLine("USERCTL=no");
		$file->AddLine("ONBOOT=yes");
		if ($dhcp)
			$file->AddLine("BOOTPROTO=dhcp");
		else{
			$file->AddLine("BOOTPROTO=static");
			$file->AddLine("IPADDR=" . $ip);
			$file->AddLine("NETMASK=" . $netmask);
		}
                
                // Add mac address
                if (!$isMaster && !$isSlave && !$isVLAN && !$this->isVirtual()) {
                	$file->AddLine("HWADDR=" . $this->GetMAC()); 
		}
		$file->Save();
	}

	function Start() {
		exec(CMD_IFUP . $this->device, $output, $retval);
		if ($retval) //error occured
			$error = implode(" ", $output); //future
		$this->UpdatePrompt();
		return $retval;
	}

	function Stop() {
		exec(CMD_IFDOWN . $this->device, $output, $retval);
		if ($retval) //error occured
			$error = implode(" ", $output); //future
		return $retval;
	}

	function Remove() {
		$this->Stop();
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->device);
		$file->Delete();
	}

	function Restart() {
		$this->Stop();
		$this->Start();
	}
}

Class Bond extends NetworkCard{
	var $slaves;
	var $macaddress;
	var $bonding_opts;

	function Bond($bond){
		$this->device = $bond;
		$this->FindSlaves();
	}

	function FindSlaves(){
		$this->GetNICs();
		foreach ($this->ListNICs() as $device){
			$slave = new NetworkCard($device);
			if ($slave->GetMaster() == $this->device){
				$this->slaves[] = $device;
			}
		}
		$this->GetVIFs();
		foreach ($this->ListVIFs() as $device){
			$slave = new NetworkCard($device);
			if ($slave->GetMaster() == $this->device){
				$this->slaves[] = $device;
			}
		}
	}

	function ListSlaves(){
		sort($this->slaves);
		return $this->slaves;
	}

	function GetSlaveStatus($device){
		$file = new File("/proc/net/bonding/" . $this->device);
		if(!$file->Load())
			return false; //couldn't open file

		$needle = "Slave interface: " . $device;
		while (!$file->EOF()){
			if (preg_match("/" . $needle . "/i", $file->GetLine(), $match)){
				$temp = $file->GetLine();
				$needle = "mii status: ";
				while ($temp != ""){ //this prevents us from reading past the correct stanza
					if (preg_match("/" . $needle . "[^ ][^ ]*/i", $temp, $match)){
						return trim(preg_replace("/" . $needle . "/i", "", $match[0]));
					}
					$temp = $file->GetLine();
				}
			}
		}
		return false;
	}

	function GetSlaveFailureCount($device){
		$file = new File("/proc/net/bonding/" . $this->device);
		if(!$file->Load())
			return false; //couldn't open file

		$needle = "Slave interface: " . $device;
		while (!$file->EOF()){
			if (preg_match("/" . $needle . "/i", $file->GetLine(), $match)){
				$temp = $file->GetLine();
				$needle = "link failure count: ";
				while ($temp != ""){ //this prevents us from reading past the correct stanza
					if (preg_match("/" . $needle . "[^ ][^ ]*/i", $temp, $match)){
						return trim(preg_replace("/" . $needle . "/i", "", $match[0]));
					}
					$temp = $file->GetLine();
				}
			}
		}
		return false;
		
	}

	function AddSlave($slave){
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $slave);
		$file->AddLine("DEVICE=" . $slave);
		$file->AddLine("USERCTL=no");
		$file->AddLine("ONBOOT=yes");
		$file->AddLine("BOOTPROTO=static");
		$file->AddLine("SLAVE=yes");
		$file->AddLine("MASTER=" . $this->device);
		$file->Save();

		$slave = new NetworkCard($slave);
		$slave->Restart();
	}

	function RemoveSlave($slave){
		exec("export LANG=C; /usr/bin/sudo /sbin/ifenslave -d " . $this->device . " " . $slave, $output, $retval);
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $slave);
		$file->Load();
		$file->DeleteLine("master", false);
		$file->DeleteLine("slave", false);
		$file->Save();

		$slave = new NetworkCard($slave);
		$slave->Stop();
	}

	function Create($mode, $primary, $alternate, $miimon, $downdelay, $updelay){
		$options = "BONDING_OPTS=\"";
		$options .= "mode=" . $mode;
		$options .= " miimon=" . $miimon;
		$options .= " downdelay=" . $downdelay;
		$options .= " updelay=" . $updelay;
		if ($primary != "false")
			$options .= " primary=" . $primary;
		if ($alternate!="1")
			$options .= " use_carrier=0";
		$options .= "\"";
		
		$this->bonding_opts = $options;
	
		$file = new File(FILE_MODPROBE);
		$file->Load();
		if ($file->FindLine($this->device)) {
			$file->DeleteLine($this->device,true);
		}
		$file->AddLine("alias " . $this->device . " bonding");
		$file->Save();
	}

	function Save($bool, $IP, $netmask, $mtu) {
	
		parent::Save($bool,$IP, $netmask, $mtu);
                $file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->device);
		$file->Load();
		if ($file->FindLine("BONDING_OPTS")) {
			$file->DeleteLine("BONDING_OPTS"); 
		}
		$file->AddLine($this->bonding_opts);
		$file->Save();

	}

	function Disable(){
		//unload module bonding which takes all bonds down
		$bondlist = array();
		$d = dir("/proc/net/bonding/");
		while ($entry = $d->read()){
			$bondlist[] = $entry;
		}

		exec("export LANG=C; /usr/bin/sudo /sbin/rmmod bonding ", $output, $retval);

		foreach ($bondlist as $bond){
			if ($bond != $this->device){
				//restart bond
				exec("export LANG=C; /usr/bin/sudo /sbin/modprobe " . $bond, $output, $retval);
			}
		}
		
	}

	//delete the current bond from the system
	function Remove(){
		foreach ($this->ListSlaves() as $slave){
			$this->RemoveSlave($slave);
		}
		$this->Disable();

		$file = new File(FILE_MODPROBE);
		$file->Load();
		$file->DeleteLine($this->device, true); //delete all occurances
		$file->Save();
		$file = new File("/etc/sysconfig/network-scripts/ifcfg-" . $this->device);
		$file->Delete(); //delete files
	}

}
Class VLAN extends NetworkCard{
	var $eth;
	var $id;

	function VLAN($vlan){
		$this->device = $vlan;

		$temp = split('.', $vlan);
		$this->eth = $temp[0];
		$this->id = $temp[1];
	}

	function Create(){
		exec(CMD_VLAN . "add " . $this->eth . " " . $this->id, $output, $retval);
		if ($retval)
			$error = implode(" ", $output);
		
	}

	function Delete(){
		$this->Remove();

		exec(CMD_VLAN . "rem " . $this->device, $output, $retval);
		if ($retval)
			$error = implode(" ", $output);

	}
}

class NetworkAccessList {
	var $xpath;
	var $domfile;

	function NetworkAccessList(){
		$this->Load();
		if(!$this->domfile->haschildnodes()) {//look for nodes, if none found, make nodes
			$this->Save();

			$file = new File(FILE_NETWORKHOSTS);
			$file->Load();
			$file->AddLine("<networks>");
			$file->AddLine("</networks>");
			$file->Save();

			$this->Load(); //file changed, reload
		}

		return true;
	}

	function Load(){
		$this->domfile = new DOMDocument;
		$this->dom->ValidateOnParse = true;
		$this->domfile->load(FILE_NETWORKHOSTS);
		$this->xpath = new DOMXpath($this->domfile);
	}

	function ListItems()
	{
		$query = "//network";
		$nodelist = $this->xpath->query($query);

		$nodearray = array();
		foreach ($nodelist as $node)
			$nodearray[]=array("name"=>$node->getAttribute("name"), "network"=>$node->getAttribute("network"),
				"netmask"=>$node->getAttribute("netmask"), "type"=>$node->getAttribute("type"));
		return $nodearray;
	}

	function AddItem($name, $network, $netmask, $type)
	{
		if (!$this->GetItem($name)){
			$entry = $this->domfile->createElement('network');
			$entry->setAttribute("name", $name);
			$entry->setAttribute("network", $network);
			$entry->setAttribute("netmask", $netmask);
			$entry->setAttribute("type", $type);

			$this->domfile->firstChild->appendChild($entry);
			return true;
		}
		return false;
	}

	function RemoveItem($name)
	{
		$query = "//network[@name='$name']";
		$nodelist = $this->xpath->query($query);
		$this->domfile->documentElement->removeChild($nodelist->item(0));
		return true;
	}

	function Save()
	{
		$this->domfile->save(FILE_NETWORKHOSTS);

		return true;
	}

	function GetItem($name)
	{
		$query = "//network[@name='$name']";
		$nodelist = $this->xpath->query($query);
		foreach ($nodelist as $node){
			return array("name"=>$node->getAttribute("name"),
				"network"=>$node->getAttribute("network"),
				"netmask"=>$node->getAttribute("netmask"),
				"type"=>$node->getAttribute("type"));
		}
		return false;
	}
}
?>
