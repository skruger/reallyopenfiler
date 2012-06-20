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
 
// suppress errors
 
error_reporting(E_ALL ^ E_NOTICE);

$timeparts = explode(" ", microtime());
$starttime = $timeparts[1] . substr($timeparts[0], 1);

$prefix = "/opt/openfiler/var/www";
$include_path = $prefix . "/includes";

header("Cache-Control: no-cache, must-revalidate");
header("Pragma: no-cache");


// Setup the system logger

openlog("openfiler", LOG_CONS | LOG_ODELAY, LOG_USER);


// Set request information

define("TINKER_HTTP_GET", 0);
define("TINKER_HTTP_POST", 1);

define("TINKER_INHERIT", -1);
define("TINKER_HTTP", 0);
define("TINKER_HTTPS", 1);

if (isset($_SERVER["HTTPS"]) && $_SERVER["HTTPS"]) {
        $GLOBALS["http_secure"] = true;
        $GLOBALS["http_url_scheme"] = "https";
}
else {
        $GLOBALS["http_secure"] = false;
        $GLOBALS["http_url_scheme"] = "http";
}


require("$include_path/distro.inc");
require("$include_path/urls.inc");
require("$include_path/data.inc");
require("$include_path/file.inc");
require("$include_path/ajax.inc");
require("$include_path/error.inc");
require("$include_path/auth.inc");
require("$include_path/ldap.inc");
require("$include_path/i18n.inc");
require("$include_path/generate.inc");
require("$include_path/utility.inc");
require_once("$include_path/xmlhandler.inc");
require("$include_path/ajaxfunctions.inc");



srand((double) microtime() * 1000000);

$printTabs;
$sectionTitle;




// define minimum uid/gid values

$global_min_uid = 150; 
$global_min_gid = 150; 

// define user filter (1 == ON, 0 == OFF)

$global_user_filter = 1; /* filters out NT/ADS host names */

if (strlen(getenv("LANGUAGE")) == 2)
	$language_code = getenv("LANGUAGE");

if (isset($language_code)) {
 	putenv("LANG=" . $language_code);
	setlocale(LC_ALL, "");
	bindtextdomain("openfiler", $prefix . "/locale/");
	textdomain("openfiler");
	setcookie("language_code", $language_code, time() + (3600 * 24 * 365), "/");
}

function get_users() {
        global $global_user_filter;
        $user_array = array();
        $userp = popen("export LANG=C; /usr/bin/sudo getent passwd", "r");
        while (!feof($userp))
        {
                $user_line = explode(":", fgets($userp, 4096));
                if ($user_line[0]) {
                        if ($global_user_filter && ereg('[$]', $user_line[0]))
                                continue;
                        $user_array[] = array("pw_name" => $user_line[0], "pw_uid" => $user_line[2], "pw_gid" => $user_line[3]);
                }
        }
        pclose($userp);
        return $user_array;
}

function check_browser($params) {
	$GLOBALS["ver"] = 5; // for user-agents such as Google, etc.

	$GLOBALS["gecko"] = 0;
	$GLOBALS["opera"] = 0;
	$GLOBALS["msie"] = 0;

	if (strstr($GLOBALS["HTTP_USER_AGENT"], "MSIE")) {
		if (strstr($GLOBALS["HTTP_USER_AGENT"], "Opera"))
			$GLOBALS["opera"] = 1;
		else {
			$GLOBALS["ver"] = intval(substr(strstr($GLOBALS["HTTP_USER_AGENT"], "MSIE"), 5, 1));
			$GLOBALS["msie"] = 1;
		}
	}
	else if (strstr($GLOBALS["HTTP_USER_AGENT"], "Mozilla/")) {
		$GLOBALS["ver"] = intval(substr(strstr($GLOBALS["HTTP_USER_AGENT"], "Mozilla/"), 8, 1));

		if (strstr($GLOBALS["HTTP_USER_AGENT"], "Gecko"))
			$GLOBALS["gecko"] = 1;
	}
	
	if (($GLOBALS["ver"] < 5) && (!$params["css"])) {
		print("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n");
		print("<html>\n");
		print("<head>\n");
		print("<title>Old user-agent</title>\n");
		print("<meta http-equiv=\"Content-type\" content=\"text/html; charset=us-ascii\">\n");
		print("<meta name=\"MSSmartTagsPreventParsing\" content=\"TRUE\">\n");
		print("</head>\n");
		print("\n");
		print("<body bgcolor=\"white\">\n");
		print("\n");
		print("<h1>Old browser detected!</h1>\n");
		print("\n");
		print("<p>You seem to have a very old browser installed. You need atleast a version 5 browser to use this application. We are sorry, but this cannot be helped. Older user-agents are notoriously buggy, and do not conform to W3C recommendations. This application uses new W3C recommendations, and needs an updated user-agent.</p>\n");
		print("\n");
		print("<p>If you want to get a new browser, you might want to try <a href=\"http://www.mozilla.org/releases/\">Mozilla</a>, which is a free W3C conformant and open browser for multiple platforms including Linux and Microsoft Windows.</p>\n");
		print("\n");
		print("</body>\n");
		print("</html>\n");

		exit;
	}
}


function xhtml_javascript_begin() {
?>

<script type="text/javascript">
<!--/*--><![CDATA[<!--*/

<?php
}


function xhtml_javascript_end() {
?>

/*]]>*/ // -->
</script>

<?php
}



$isRootIndex = $_SERVER["PHP_SELF"];  



function generic_header($params = array()) {
    
    global $isRootIndex; 
        
	check_browser(array("css" => 0));
	
	ob_start();

	// work around problems with the PHP parser
	print("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	print("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
	print("<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">\n");
	print("<head>\n");
	print("<title>" . $params["title"] . "</title>\n");
	print("<link rel=\"stylesheet\" type=\"text/css\" media=\"screen, print\" href=\"/style.css\" />\n");
// css for AJAX effects	
	print("<link rel=\"stylesheet\" type=\"text/css\" media=\"screen, print\" href=\"/css/luna/luna.css\"/>\n");
	print("<link rel=\"stylesheet\" type=\"text/css\" media=\"screen, print\" href=\"/css/sliderstyles.css\"/>\n");
	
	// javascript to support multiple onload events
	
	print("<script type=\"text/javascript\" src=\"/js/multiOnLoad.js\" language=\"JavaScript\"></script> \n"); 
	print("<script type=\"text/javascript\" src=\"/js/runOnLoad.js\" language=\"JavaScript\"></script> \n"); 
// javascript for AJAX effects
	print("<script type=\"text/javascript\" src=\"/js/range.js\" language=\"JavaScript\"></script> \n");
	print("<script type=\"text/javascript\" src=\"/js/timer.js\" language=\"JavaScript\"></script> \n");
	print("<script type=\"text/javascript\" src=\"/js/slider.js\" language=\"JavaScript\"></script> \n");
	print("<script type=\"text/javascript\" src=\"/js/fat.js\" language=\"JavaScript\"></script> \n");
	print("<script type=\"text/javascript\" src=\"/js/slidingpanel.js\" language=\"JavaScript\"></script> \n");
	print("<script type=\"text/javascript\" src=\"/js/sa/prototype.js\" language=\"JavaScript\"></script> \n");
  	print("<script type=\"text/javascript\" src=\"/js/sa/scriptaculous.js\" language=\"JavaScript\"></script> \n");
// IE css hacks
    print("<!--[if IE]>");
    print("<style type=\"text/css\">"); 
    print(".dynamic-slider-control .line {box-sizing: content-box; behavior: url(\"/css/boxsizing.htc\");}\n");
    print(".Curtain {filter:alpha(opacity=35);}\n");
    print("</style>"); 
    print("<![endif]-->"); 
  	print("<!--[if lt IE 7.]>\n");
	print("<script defer=\"defer\" type=\"text/javascript\" src=\"/js/pngfix.js\"></script>\n");
	print("<![endif]-->\n");


	print("<link rel=\"icon\" type=\"image/png\" media=\"screen, print\" href=\"/images/of-icon.png\" />\n");
	print("<link rel=\"shortcut icon\" type=\"image/x-icon\" media=\"screen, print\" href=\"/images/of-icon.ico\" />\n");
 

// Volume graph plotting (only required in volumes.html for now)
    if (preg_match("/volumes/", $_SERVER['SCRIPT_FILENAME'])) {
        print("<script type=\"text/javascript\" src=\"/js/mochikit/MochiKit.js\"></script>\n");
        print("<script type=\"text/javascript\" src=\"/js/plotkit/Base.js\"></script>\n");
        print("<script type=\"text/javascript\" src=\"/js/plotkit/Layout.js\"></script>\n");
        print("<script type=\"text/javascript\" src=\"/js/plotkit/Canvas.js\"></script>\n");
        print("<script type=\"text/javascript\" src=\"/js/plotkit/SweetCanvas.js\"></script>\n");
        print("<script type=\"text/javascript\" src=\"/js/plotkit/excanvas.js\"></script>\n");
    }


 
// javascript for support popupPage
		
	print("
	
	<script language=\"JavaScript\" type=\"text/javascript\">
			function popupPage(link) {
					day = new Date();
					id = day.getTime();
			eval(\"page\" + id + \" = window.open(link, '\" + id + \"', 'toolbar=0,scrollbars=0,location=0,statusbar=0,menubar=0,resizable=0,width=1024,height=768,left = 328,top = 141');\");
	}
	
	</script>\n"); 
        
	reset($params);
	while (list($key, $val) = each($params))
		if (substr($key, 0, 4) == "link")
		{
			$ltitle = "";
			
			if (strlen(substr($key, 4, strlen($key) - 4)) > 0)
				if (strlen($params["title" . substr($key, 4, strlen($key) - 4)]) > 0)
				{
					$ltitle = "title=\"" . $params["title" . substr($key, 4, strlen($key) - 4)] . "\"";
				}
					
			print("<link rel=\"" . substr($key, 4, strlen($key) - 4) . "\" " . $ltitle . " media=\"screen, print\" href=\"" . $val . "\" />\n");
		}

	print("<?xml-stylesheet type=\"text/css\" media=\"screen, print\" href=\"/style.css\" ?>\n");
	print("<meta name=\"MSSmartTagsPreventParsing\" content=\"TRUE\" />\n");
	print("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");

                
	/* output javascript ajax callbacks */
  
	global $ajax; 
	xhtml_javascript_begin();
	$ajax->generateJavaScript();
	xhtml_javascript_end();
   
    
	
	print("</head>\n");

    print("<body " . ($isRootIndex == "/index.html" ? "style=\"background: #5b6469\""   : ""  ) . " onload=\"runOnLoad();\">\n");
    
    print("\n");
    
    print(($isRootIndex != "/index.html" ? "<div id=\"logodiv\" class=\"openfiler-logo\">
           <div id=\"globalButtons\">
            <ul>
                <li class=\"logout\"><a href=\"/account/logout.html\">Log Out</a></li>
                <li class=\"status\"><a href=\"/admin/status.html\">Status</a></li>
                <li class=\"update\"><a href=\"/admin/system_view_update.html\">Update</a></li>
                <li class=\"shutdown\"><a href=\"/admin/system_shutdown.html\">Shutdown</a></li>
            </ul>
           </div>
           <div style=\"clear: left; float: right; \">
            <iframe style=\"width: 450px; height: 44px;;\" src=\"/uptime.html\" align=\"middle\" frameborder=\"0\" scrolling=\"no\"></iframe>
           </div></div>\n" : ""));
	
    print("<div id=\"slidercontainer\"><div id=\"slideDownDiv\"><div class=\"show\" id=\"slidingpaneldiv\"></div></div></div> \n");

}

function generic_footer($params = array()) {
    
    global $isRootIndex; 
	if ((!isset($params["nofooter"])) || ($params["nofooter"] != 0)) {
		print("<p " .  ($isRootIndex == "/index.html" ? "class=\"footer\"" : "") . " align=\"center\">\n");
		print("&copy; 2001 - 2011 <a href=\"http://www.openfiler.com/\">" . _("Openfiler") . "</a>. " . _("All rights reserved.") . "<br />\n");
		print("<a href=\"/\">" . _("Home") . "</a> &middot; <a href=\"http://www.openfiler.com/buy/administrator-guide\">" . _("Documentation") . "</a> &middot; <a href=\"http://www.openfiler.com/support/\">" . _("Support") . "</a> &middot; <a href=\"http://www.openfiler.com/\">" . _("Website") . "</a> &middot; <a href=\"http://www.openfiler.com/about/license\">" . _("License") . "</a>");

		if ($GLOBALS["authenticated"] == 1)
			print(" &middot; <a href=\"/account/logout.html\">" . _("Log Out") . "</a>");

		print("\n</p>\n");
	}

	print("\n");
	print("<!-- " . $GLOBALS["HTTP_USER_AGENT"] . " (Accept-Encoding: " . $GLOBALS["HTTP_ACCEPT_ENCODING"] . ")" . " -->\n");

	$timeparts = explode(" ", microtime());
	$endtime = $timeparts[1] . substr($timeparts[0], 1);

	print("<!-- script execution time: " . ($endtime - $GLOBALS["starttime"]) . " seconds -->\n");
	print("\n");
	
	print("</body>\n");
	print("</html>\n");

	$output_content = ob_get_contents();
	$output_length = ob_get_length();
	ob_end_clean();
	header("Content-length: " . $output_length);
	print($output_content);
}

function single_begin($params = array()) {

	print("<table cellpadding=\"20\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n");
	print("<tr>\n");
	print("<td valign=\"top\">\n");
}

function single_end($params = array()) {
	print("</td>\n");
	print("</tr>\n");
	print("</table>\n");
}

function dual_begin($params = array()) {
    
   

	print("<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n");
	print("<tr>\n");
	print("<td valign=\"top\">\n");
	print("<table cellpadding=\"20\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n");
	print("<tr>\n");
	print("<td>\n");
}

function dual_middle($params = array()) {
	print("</td>\n");
	print("</tr>\n");
	print("</table>\n");
	print("</td>\n");
	
	print("<td bgcolor=\"#e7e7e7\" width=\"1\"><img src=\"/images/e7e7e7.png\" width=\"1\" height=\"1\" alt=\"\" border=\"0\" /></td>\n");
	print("<td bgcolor=\"#19421d\" width=\"1\"><img src=\"/images/19421d.png\" width=\"1\" height=\"1\" alt=\"\" border=\"0\" /></td>\n");
	
	print("<td width=\"240\" valign=\"top\">\n");
	print("<table cellpadding=\"20\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n");
	print("<tr>\n");
	print("<td>\n");
}

function dual_end($params = array()) {
	print("</td>\n");
	print("</tr>\n");
	print("</table>\n");
	print("</td>\n");
	print("</tr>\n");
	print("</table>\n");
}

function tab_begin($ncolumn, $ccolumn, $names, $urls) {
?>
<div align="center" class="container">
<table cellpadding="10" cellspacing="0" border="0" width="60%">
<tr>
<?php
	for ($i = 0; $i < $ncolumn; $i++)
		if ($i == $ccolumn)
			print("<td width=\"25%\" bgcolor=\"" . $GLOBALS["color_tab1_highlighted_background"] . "\" style=\"border: 1px solid white;\" align=\"center\" valign=\"middle\"><span style=\"color: " . $GLOBALS["color_tab1_highlighted"] . "; font-weight: bold;\">" . $names[$i] . "</span></td>");
		else
			print("<td width=\"25%\" bgcolor=\"" . $GLOBALS["color_tab1_regular_background"] . "\" style=\"border: 1px solid white;\" align=\"center\" valign=\"middle\"><a class=\"tablink1\" href=\"" . $urls[$i] . "\">" . $names[$i] . "</a></td>");
?>
</tr>
<tr bgcolor="<?php print($GLOBALS["color_inner_body"]); ?>">
	<td align="center" valign="middle" colspan="<?php print($ncolumn); ?>" style="border: 1px solid rgb(80, 80, 80);">
	<p>&nbsp;</p>
<?php
}

function tab_end() {
?>
	<p>&nbsp;</p>
	</td>
</tr>
</table>
</div>
<?php
}

function nested_tab_begin($tabs) {


        global $T_URLS;
        global $printTabs;
        global $sectionTitle;
        global $tabTabs;
        $tabTabs = $tabs; 
        $printTabs = getTabs($tabs);
        ?>
        <div align="center" class="container">
        <table cellpadding="0" cellspacing="0" border="0" width="100%">
        <tr>
       <?php  
		print("<td><div id=\"global_nav\" class=\"global_nav\"><ul>");
		
		foreach($T_URLS as $key => $value) {
			if (array_key_exists($key, $printTabs)) {
				print("<li class=\"active\"><span class=\"" . $value['li_class'] . "\">&nbsp;</span><span style=\"display: block; float: left;\"><a class=\"selected\" href=\"" . $value['url'] . "\">" . $value['title'] . "</a></span></li>");
                                $sectionTitle = $value['title']; 
                        }
			else
				print("<li><span class=\"" . $value['li_class'] . "\">&nbsp;</span><a href=\"" . $value['url'] . "\">" . $value['title'] . "</a></li>"); 
		}
		
		print ("</ul></div></td>"); 
                // code for tab parent tabs
        ?> 
        </tr>
        </table>
        <table cellpadding="5" cellspacing="1" border="0" width="100%">
		<tr>	
		<td valign="top">

		<div class="shadow">
			<div class="shadow_tr">
				<div class="shadow_bl">
					<div class="shadow_tl">
						<div style="padding-bottom: 20px; padding-left: 5px; padding-right: 5px;  min-height: 1%;"> 
							<div><p>&nbsp;</p></div>
						    <div align="center">
        <?php
}




function nested_tab_end() {
?>
        </div>
    <p>&nbsp;</p>
	</div>
	</div>
	</div>
	</div>
	</div>
    <div style="clear: both; height: 1px; overflow: hidden;"/>
	
	</td>
        <td valign="top" style="width: 200px;"> <!-- nav links -->
			<div class="shadow">
			<div class="shadow_tr">
				<div class="shadow_bl">
					<div class="shadow_tl">
						<div style="padding-bottom: 10px; padding-left: 5px; padding-right: 5px; padding-top: 5px; min-height: 1%;">
			<?php
                                
                                global $sectionTitle;
                                global $printTabs;
                                global $tabTabs; 
                                print("<div id=\"sub_nav\"><div class=\"sub_nav_title\">$sectionTitle section</div><ul>");
                        
				$children = array();
				foreach ($printTabs as $key => $value) {
					foreach ($value as $key2 => $value2) {
						$children = $value2;
					}
				}
				
				foreach ($children as $key => $value) {
					if ($tabTabs == $key)
						print("<li class=\"" . $value['li_class'] . "\"><a class=\"selected\" href=\"" . $value['url'] . "\">" . $value['title'] . "</a></li>");
					else
						print("<li class=\"" . $value['li_class'] . "\"><a href=\"" . $value['url'] . "\">" . $value['title'] . "</a></li>"); 
			
				}
		
				print ("</ul></div>"); 
				
			?>
			</div>
			</div>
			</div>
			</div>
			</div>


                        <!-- global links -->
                        
                        <div class="shadow">
			<div class="shadow_tr">
				<div class="shadow_bl">
					<div class="shadow_tl">
						<div style="padding-bottom: 10px;  padding-left: 5px; padding-right: 5px; padding-top: 5px; min-height: 1%;">
			<?php
                                
                                
                                print("<div id=\"support_nav\"><div class=\"sub_nav_title\">Support resources</div><ul>");
                        
                                print("<li class=\"support_bug\"><a href=\"javascript:popupPage('/admin/support.html?page=support_report');\">Report bug</a></li>");
                                print("<li class=\"support_get\"><a href=\"javascript:popupPage('/admin/support.html?page=support_get');\">Get support</a></li>");
                                print("<li class=\"support_forums\"><a href=\"javascript:popupPage('/admin/support.html?page=support_forums');\">Forums</a></li>"); 
                                print("<li class=\"support_admin_guide\"><a href=\"https://www.openfiler.com/buy/administrator-guide\">Admin Guide</a></li>"); 
			
				print ("</ul></div>"); 
				
			?>
			</div>
			</div>
			</div>
			</div>
			</div>
                        
                        <!-- end global links --> 


			</td>
</tr>
</table>
</div>
<?php
}

function showGplAcceptForm() {
        //open gpl file

        $filename = "/opt/openfiler/etc/gpl.txt";
        $fh = fopen($filename, "r");
        $gpltext = fread($fh, filesize($filename));
        fclose($fh);
?>

        <script language="JavaScript" type="text/javascript">
        <!--
        function enableAccept()
        {
            document.getElementById('license').style.background='rgb(202, 237, 178)';
            document.getElementById('accept').disabled=false;
        }
        //-->
        </script>
<?php
   
       

        print("<center><div style=\"  margin: auto ;\">");
        print("<form action=\"/index.html\" method=\"post\">");
        print("<input name=\"acceptedgpl\" type=\"hidden\" value=\"true\">");
        print("<textarea id=\"license\" style=\"background-color: rgb(255, 231, 231);\" cols=\"100\" rows=\"20\"  readonly=\"readonly\" onfocus=\"enableAccept()\">");
        print($gpltext);
        print("</textarea>");
        print("<br/><br/>");
        print("<input id=\"accept\" disabled=\"disabled\" type=\"submit\" name=\"accept\" value=\"I have read, and hereby accept, the license terms.\">");
        print("</form>");
        print("</div></center>");

       
}




// finally, after defining all functions in all
// those header files, the following statements
// are executed before returning to the owner.

if (isset($usercookie) && isset($passcookie))
	check_authenticated($usercookie, $passcookie);

?>
