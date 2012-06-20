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


// urls and stuff

$sys_adminemail = "admin@server.com";

$support_report = array("title"=>"Openfiler Bug Report",
                        "url"=>"https://dev.openfiler.com/projects/oflegacy/issues/new",
                        "message"=>"");

$support_register = array("title"=>"Register Openfiler System",
                        "url"=>"https://support.openfiler.com/register.php",
                        "message"=>"");

$support_get = array("title"=>"Openfiler Support",
                        "url"=>"http://www.openfiler.com/support/support-ticket",
                        "message"=>"");

$support_forums = array("title"=>"Openfiler Forums",
                        "url"=>"https://forums.openfiler.com",
                        "message"=>"");

$support_knowledgebase = array("title"=>"Openfiler Knowledgebase",
                        "url"=>"https://support.openfiler.com/index.php?x=&mod_id=2",
                        "message"=>""); 

$support_admin_guide = array("title"=>"Openfiler Administrator Guide",
                        "url"=>"https://www.openfiler.com/buy/administrator-guide",
                        "message"=>""); 

// number of list items per page

$list_count = 10;

// colour styles

$color_tab1_regular_background = "#7494d4";
$color_tab1_highlighted_background = "#4265ad";
$color_page_tab1_regular_background = "#7a8388";
$color_page_tab1_highlighted_background = "#4c555a";
$color_tab1_regular = "white";
$color_tab1_highlighted = "white";
$color_tab2_regular_background = "#7494d4";
$color_tab2_highlighted_background = "#4265ad";
$color_tab2_regular = "white";
$color_tab2_highlighted = "white";


$color_outer_body = "#f5f5f5";
$color_inner_body = "#ffffff";

$color_inner_body_r = 0xff;
$color_inner_body_g = 0xff;
$color_inner_body_b = 0xff;

$color_table_red = "#f48383";
$color_table_amber = "#f8d057";
$color_table_green = "#e3f6b9";


$color_table_heading = "#ebebeb";
$color_table_heading2 = "#c6d5a8"; 
$color_table_heading_text = "#404040";



$color_table_row_selected = "#e3f6b9"; 
$color_table_row1 = "#edf6fd";
$color_table_row1_colstr = "e2e1f7";
$color_table_row2 = "#f7efff";  
//$color_table_row2 = "#ebccff";
$color_table_row2_colstr = "f7e9e1";
$color_table_alert = "#ffa4a4";

$color_applet_background = "246,246,222";
$color_popup_background = "#ffffff";

$LINK_ICON_DIR="/images/icons/links";


function printMessageBlock($type, $message) {

    print("<div class=\"messageblock messageblock-$type\">\n\t");
    print("<div class=\"messageblock-internal\">\n\t\t");
    print($message . "\n");
    print("\t\t</div></div>\n\n");
    
}


?>
