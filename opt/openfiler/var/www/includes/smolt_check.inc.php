<?php 



define('SMOLT_CHECK_FILE', '/opt/openfiler/etc/.smolt_checked'); 
define('CMD_SEND_PROFILE', '/usr/bin/smoltSendProfile -a -t 1');

class Smolt {

	var $debug = false; 

	function Smolt() {	
		if ($this->debug)
			syslog(LOG_INFO, 'smolt class created');
        if (is_file(SMOLT_CHECK_FILE)) 
			return true;  
		$this->sendProfile(); 
	}

	private function sendProfile() {
		if ($this->debug)
			syslog(LOG_INFO, 'sendProfile() called'); 

		if (!$this->isSent()) {
			exec(CMD_SEND_PROFILE, $output, $ret); 
			
			if ($ret != 0) {
				syslog(LOG_INFO, "unable to send smolt profile"); 
				$this->smoltSent();
            } 
		}

	}

	private function isSent() {
		if ($this->debug)
			syslog(LOG_INFO, 'isSent() called');

		if (is_file(SMOLT_CHECK_FILE)) 
			return true; 
		return false; 
	}

	private function smoltSent() {
		if ($this->debug)
			syslog (LOG_INFO, 'smoltSent() called'); 
		exec('touch ' . SMOLT_CHECK_FILE, $output, $ret);
		foreach ($output as $line)
			$outstring .= $line . '\n';
		if ($ret != 0) {
			syslog (LOG_INFO, $outstring);
			return false;
		}

		else
			return true;
		
	}
}

$smolt_sender = new Smolt(); 

?>
