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


$ajax = new tinkerAjax();


if (strstr("$PHP_SELF", "quota.html")) 
    include("ajaxfunctions/quota.inc");
if (strstr("$PHP_SELF", "quota_user.html")) 
    include("ajaxfunctions/quota_user.inc");
if (strstr("$PHP_SELF", "quota_guest.html"))
    include("ajaxfunctions/quota_guest.inc");
if (strstr("$PHP_SELF", "services_ups.html"))
    include("ajaxfunctions/services_ups.inc"); 
if (strstr("$PHP_SELF", "system_ups.html")) 
    include("ajaxfunctions/system_ups.inc"); 
if (strstr("$PHP_SELF", "index.html"))
    include("ajaxfunctions/accountsad.inc");
$ajax->processRequests(); 
?>
