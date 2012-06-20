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
 * Provides necessary classes and functions for handling and manulating files 
 *
 */


Class File {
	var $filename;
	var $fh;
	var $pos = 0;
	var $contents = array();

	function File($filename) {
		//constructor
		$this->filename = $filename;
	}

	function Load() {
		if (!$this->fh = fopen($this->filename, "r"))
			return false; //couldn't open file
		$this->Clear();

		while(!feof($this->fh)){
			$this->contents[] = str_replace("\n", "", fgets($this->fh, 4096));
		}
		fclose($this->fh);
		$this->contents=array_slice($this->contents, 0, count($this->contents)-1); //remove the last empty line

		return true;
	}

	function Save() {
		$fp = popen("export LANG=C; /usr/bin/sudo /usr/bin/tee " . $this->filename, "w");
		foreach ($this->contents as $line){
			fwrite($fp, $line . "\n");
		}

		pclose($fp);
	}

	function GetLine() {
		if ($this->EOF())
			return false;
		$line = $this->contents[$this->pos];
		$this->pos++;
		return $line;
	}

	function EOF() {
		if ($this->pos == count($this->contents)){
			return true;
		}
	}

	function Clear() {
		$this->pos = 0;
		unset($this->contents);
		$this->contents = array();
	}

	function Top() {
		$this->pos = 0;
	}

	function AddLine($line) {
		$this->contents[] = $line;
	}

	function EditLine($search, $replace) {
		for ($i=0; $i < count($this->contents); $i++){
			if (preg_match("/" . $search . "/i", $this->contents[$i])){
				$this->contents[$i] = preg_replace("/" . $search . ".*/i", $replace, $this->contents[$i]);
				return true;
			}
		}
		$this->AddLine($replace); //line not found, add it
	}

	function FindLine($search) {
		for ($i=0; $i < count($this->contents); $i++){
			if (preg_match("/" . $search . "/i", $this->contents[$i])){
				return $this->contents[$i];
			}
		}
		return false;
	}

	function DeleteLine($search, $all){
		$match = false;
		for ($i=0; $i < count($this->contents); $i++){
			if (preg_match("/" . $search . "/i", $this->contents[$i])){
				$this->contents = array_merge(array_slice($this->contents, 0,$i), array_slice($this->contents, $i+1));
				$match = true;
				$i--; //fix the counter because the array shunk
				if (!$all) return true;  //only return if not removing all occurances
			}
		}
		return $match;
	}

	function Delete(){
		exec("export LANG=C; /usr/bin/sudo /bin/rm -f " . $this->filename . " 2>&1", $retval, $output);
		if ($retval){
			//chown error occured
			$error = implode(" ", $output);
			return false;
		}
	}

	function chmod($val){
		if ($val){
			exec("export LANG=C; /usr/bin/sudo /bin/chmod " . $val . " " . $this->filename . " 2>&1", $retval, $output);
			if ($retval){
				//chown error occured
				$error = implode(" ", $output);
				return false;
			}
		}
	}

	function chown($user, $group){
		if ($user){
			exec("export LANG=C; /usr/bin/sudo /bin/chown " . $user . " " . $this->filename . " 2>&1", $retval, $output);
			if ($retval){
				//chown error occured
				$error = implode(" ", $output);
				return false;
			}
		}
		return $this->chgrp($group);
	}
	function chgrp($group){
		if ($group){
			exec("export LANG=C; /usr/bin/sudo /bin/chgrp " . $group . " " . $this->filename . " 2>&1", $retval, $output);
			if ($retval){
				//chgrp error occured
				$error = implode(" ", $output);
				return false;
			}
		}
	}
}
?>
