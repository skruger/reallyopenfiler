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


	if ($GLOBALS["authenticated"] == 0)
        {
		/*
                generic_header(array("title" => _("Your account")));
                dual_begin(array());
                print("<h2><a href=\"/\">" . _("Home") . "</a> / " . _("Your account") . "</h2>\n");
                print("<p>" . _("You have not authenticated yourself to the system yet with your") . 
				" <em>" . _("username") . "</em> " . _("and") . " <em>" . _("password") . "</em>. " .
                        _("You will not be able to use this section of the website without authenticating yourself first. 
			So, please continue and login using the form on the right hand side.") . "</p>\n");
                dual_middle(array());
                show_account_details(array());
                dual_end(array());
                generic_footer(array());
                exit;
		*/
		
		$redirect = "/index.html?redirect=" . $_SERVER["PHP_SELF"]; 
		header("Location: $redirect");
		exit;
        }
?>
