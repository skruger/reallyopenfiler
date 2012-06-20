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

function exit_error($title, $text, $mail = 0)
{

print("<html>\n");
print("<head>\n");
print("<title>" . _("Error!") . "</title>\n");
print("</head>\n");
print("<body>\n");

print("<p>&nbsp;</p>\n");
print("<p>&nbsp;</p>\n");

print("<p align=\"center\"><center>\n");
	
print("<font color=\"#FF8080\"><strong>\n");
print($title . "<br><br>\n");
print($text . "<br><br>\n");
print("</strong></font>\n");

print("</center></p>\n");

if ($mail)
{
	print("<p align=\"center\"><center>\n");
	
	print("<strong>\n");
	print(_("Please report this problem to") . "<br>\n");
	print("<a href=\"mailto:" . $GLOBALS["sys_adminemail"] . "\">" . $GLOBALS["sys_adminemail"] . "</a>\n");
	print("</strong>\n");

	print("</center></p>\n");
}

print("</body>\n");
print("</html>\n");
	
exit;		// IMPORTANT!! IDIOT!

}

?>
