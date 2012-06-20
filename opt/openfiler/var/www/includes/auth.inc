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

if (isset($GLOBALS["authenticated"]))
	unset($GLOBALS["authenticated"]);

if (isset($GLOBALS["authuser"]))
	unset($GLOBALS["authuser"]);

function is_valid_foldername($foldername)
{
	if ((strlen($foldername) > 0) && (!strstr($foldername, '/')) && ($foldername != ".") && ($foldername != ".."))
		return true;
	else
		return false;
}

function is_valid_user($username)
{
	return true;
}

function is_valid_group($groupname)
{
	if (strspn($groupname, "abcdefghijklmnopqrstuvwxyz0123456789") == strlen($groupname))
		return true;
	else
		return false;
}

function check_authenticated($username, $password)
{
	if (pam_auth($username, $password))
	{
		if ($username == "openfiler")
		{
			$GLOBALS["authenticated"] = 1;
			$GLOBALS["authuser"] = $username;
		}
		else
		{
			$GLOBALS["authenticated"] = 0;
			$GLOBALS["userauthenticated"] = 1;
			$GLOBALS["authuser"] = $username;
		}
		
		setcookie("usercookie", $username, 0, "/", "", 1);
		setcookie("passcookie", $password, 0, "/", "", 1);
		
		return true;
	}
	else
	{
		$GLOBALS["authenticated"] = 0;
		$GLOBALS["authuser"] = "";

		setcookie("usercookie", $username, 0, "/", "", 1);
		setcookie("passcookie", "", 0, "/", "", 1);
		
		return false;
	}
}

function makehash($password)
{
	return md5($password);
}

function show_account_details($params)
{
	if (((!isset($GLOBALS["authenticated"])) || ($GLOBALS["authenticated"] == 0))
		&& ((!isset($GLOBALS["userauthenticated"])) || ($GLOBALS["userauthenticated"] == 0)))
	{
		if (isset($_GET['redirect'])) {
			if ($_GET['redirect'] != "/index.html")
				printMessageBlock("error", "Please authenticate with your credentials");
			else if ($_GET['redirect'] == "/index.html")
				printMessageBlock("error", "Invalid username or password");
		}
		
		print("<form action=\"/account/login.html\" method=\"post\">\n");
		print("<table border=\"0\" align=\"center\" cellspacing=\"2\" cellpadding=\"8\">\n"); 
		print("<tr><td bgcolor=\"" . $GLOBALS["color_table_heading"] . "\" align=\"right\"><strong>" . _("Username:") . "</strong></td>\n");
		print("<td bgcolor=\"" . $GLOBALS["color_table_row1"] . "\" align=\"left\"><input type=\"text\" name=\"username\" size=\"15\" /></td></tr>\n");
		print("<tr><td bgcolor=\"" . $GLOBALS["color_table_heading"] . "\" align=\"right\"><strong>" . _("Password:") . "</strong></td>\n");
		print("<td bgcolor=\"" . $GLOBALS["color_table_row2"] . "\" align=\"left\"><input type=\"password\" name=\"password\" size=\"15\" /></td></tr>\n");
		print("<tr><td align=\"center\" colspan=\"2\"><input type=\"submit\" value=\"Log In\" size=\"15\" /></td></tr>\n");
		print("</table>");
		if (isset($_GET['redirect'])) {
                        include("filterpaths.inc");
                        if (in_array($_GET['redirect'],$webRootFilter)) {
				print("<input type=\"hidden\" name=\"redirect\" value=\"" . $_GET['redirect']. "\">"); 
                        }
                	else {
				print("<input type=\"hidden\" name=\"redirect\" value=\"/admin/status.html\">");
                        }

		}
		print("</form>\n");
	}
	else if (strcmp($GLOBALS["authuser"], "openfiler") == 0)
	{
		
		print("<div style=\"text-align: center;\" ><p>Welcome Administrator! <br/>You can <a href=\"/admin/status.html\">administer the storage device from here.</a></p>\n");
		print("<p><a href=\"/account/logout.html\">Log out</a>.</p></div>\n");
	}
	else
	{
		print("<p>Welcome " . $GLOBALS["authuser"] ."! You can <a href=\"/account/\">access your account information and preferences.</a> Your account expires in 15 days.</p>\n");
		print("<p><a href=\"/account/logout.html\">Click here to logout</a>.</p>\n");
	}
}

