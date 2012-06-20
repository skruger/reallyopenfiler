<?php

/*
 * $Id$
 * $URL$
 *
 * Ajax functions
 *
 * Copyright (C) 2005-2005 Mukund Sivaraman Iyer. All rights reserved.
 *
 * This code is based on the code of http://xajax.sourceforge.net
 * Copyright (C) 2005 by J. Max Wilson
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS AND/OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 */

// The tinkerAjaxResponse class is used to created responses to be sent back to your
// webpage.  A response contains one or more command messages for updating your page.
// Currently tinkerAjax supports five kinds of command messages:
// * Assign - sets the specified attribute of an element in your page
// * Append - appends data to the end of the specified attribute of an element in your page
// * Prepend - prepends data to teh beginning of the specified attribute of an element in your page
// * Replace - searches for and replaces data in the specified attribute of an element in your page
// * Script - runs JavaScript
// * Alert - shows an alert box with the suplied message text
// elements are identified by their HTML id

class tinkerAjaxResponse
{
	var $xml;

	function tinkerAjaxResponse()
	{
		$this->xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\n";
		$this->xml .= "<tinker-xml>\n";
	}
	
	// addAssign() adds an assign command message to your xml response
	// $sTarget is a string containing the id of an HTML element
	// $sAttribute is the part of the element you wish to modify ("innerHTML", "value", etc.)
	// $sData is the data you want to set the attribute to
	// usage: $objResponse->addAssign("contentDiv", "innerHTML", "Some Text");

	function addAssign($sTarget, $sAttribute, $sData)
	{
		$this->xml .= "<update action=\"assign\">\n";
		$this->xml .= "<target attribute=\"$sAttribute\">$sTarget</target>\n";
		$this->xml .= "<data><![CDATA[$sData]]></data>\n";
		$this->xml .= "</update>\n\n";
	}
	
	// addAppend() adds an append command message to your xml response
	// $sTarget is a string containing the id of an HTML element
	// $sAttribute is the part of the element you wish to modify ("innerHTML", "value", etc.)
	// $sData is the data you want to append to the end of the attribute
	// usage: $objResponse->addAppend("contentDiv", "innerHTML", "Some Text");

	function addAppend($sTarget, $sAttribute, $sData)
	{
		$this->xml .= "<update action=\"append\">\n";
		$this->xml .= "<target attribute=\"$sAttribute\">$sTarget</target>\n";
		$this->xml .= "<data><![CDATA[$sData]]></data>\n";
		$this->xml .= "</update>\n\n";
	}
	
	// addPrepend() adds an prepend command message to your xml response
	// $sTarget is a string containing the id of an HTML element
	// $sAttribute is the part of the element you wish to modify ("innerHTML", "value", etc.)
	// $sData is the data you want to prepend to the beginning of the attribute
	// usage: $objResponse->addPrepend("contentDiv", "innerHTML", "Some Text");

	function addPrepend($sTarget, $sAttribute, $sData)
	{
		$this->xml .= "<update action=\"prepend\">\n";
		$this->xml .= "<target attribute=\"$sAttribute\">$sTarget</target>\n";
		$this->xml .= "<data><![CDATA[$sData]]></data>\n";
		$this->xml .= "</update>\n\n";
	}
	
	// addReplace() adds an replace command message to your xml response
	// $sTarget is a string containing the id of an HTML element
	// $sAttribute is the part of the element you wish to modify ("innerHTML", "value", etc.)
	// $sSearch is a string to search for
	// $sData is a string to replace the search string when found in the attribute
	// usage: $objResponse->addReplace("contentDiv", "innerHTML", "text", "<b>text</b>");

	function addReplace($sTarget, $sAttribute, $sSearch, $sData)
	{
		$this->xml .= "<update action=\"replace\">\n";
		$this->xml .= "<target attribute=\"$sAttribute\">$sTarget</target>\n";
		$this->xml .= "<search><![CDATA[$sSearch]]></search>\n";
		$this->xml .= "<data><![CDATA[$sData]]></data>\n";
		$this->xml .= "</update>\n\n";
	}
	
	// addClear() adds an clear command message to your xml response
	// $sTarget is a string containing the id of an HTML element
	// $sAttribute is the part of the element you wish to clear ("innerHTML", "value", etc.)
	// usage: $objResponse->addClear("contentDiv", "innerHTML");

	function addClear($sTarget, $sAttribute)
	{
		$this->xml .= "<update action=\"clear\">\n";
		$this->xml .= "<target attribute=\"$sAttribute\">$sTarget</target>\n";
		$this->xml .= "</update>\n\n";
	}
	
	// addAlert() adds an alert command message to your xml response
	// $sMsg is a text to be displayed in the alert box
	// usage: $objResponse->addAlert("This is some text");

	function addAlert($sMsg)
	{
		$this->xml .= "<alert><![CDATA[$sMsg]]></alert>\n\n";
	}

	// addScript() adds a jscript command message to your xml response
	// $sJS is a string containing javascript code to be executed
	// usage: $objResponse->addAlert("var x = prompt('get some text');");

	function addScript($sJS)
	{
		$this->xml .= "<jscript><![CDATA[$sJS]]></jscript>\n\n";
	}
	
	// addRemove() adds a Remove Element command message to your xml response
	// $sTarget is a string containing the id of an HTML element to be removed
	// from your page
	// usage: $objResponse->addRemove("Div2");

	function addRemove($sTarget)
	{
		$this->xml .= "<update action=\"remove\">\n";
		$this->xml .= "<target>$sTarget</target>\n";
		$this->xml .= "</update>\n\n";
	}
	
	function addCreate($sParent, $sTag, $sId, $sType="")
	{
		$this->xml .= "<update action=\"create\">\n";
		$this->xml .= "<target attribute=\"$sTag\">$sParent</target>\n";
		$this->xml .= "<data><![CDATA[$sId]]></data>\n";

		if ($sType != "")
			$this->xml .= "<type><![CDATA[$sType]]></type>\n";

		$this->xml .= "</update>\n\n";
	}
	
	// getXML() returns the xml to be returned from your function to the tinkerAjax
	// processor on your page
	// usage: $objResponse->getXML();

	function getXML()
	{
		if (strstr($this->xml, "</tinker-xml>") == false)
			return ($this->xml . "</tinker-xml>\n");
		else
			return $this->xml;
	}
}


// the tinkerAjax class generates the tinkerAjax javascript for your page including the 
// javascript wrappers for the PHP functions that you want to call from your page.
// It also handles processing and executing the command messages in the xml responses
// sent back to your page from your PHP functions.

class tinkerAjax
{
	var $aFunctions;		// Array of PHP functions that will be callable through javascript wrappers
	var $aFunctionURLs;		// Array of respective URLs for functions that will be callable through javascript wrappers
	var $aFunctionRequestTypes;	// Array of RequestTypes to be used with each function (key=function name)
	var $bDebug;			// Show debug messages true/false
	var $bStatusMessages;		// Show status messages true/false
	
	// Contructor
	// usage: $tinkerAjax = new tinkerAjax();

	function tinkerAjax()
	{
		$this->aFunctions = array();
		$this->aFunctionURLs = array();

		$this->bDebug = false;
		$this->bStatusMessages = false;
	}
	

	// debugOn() enables debug messages for tinkerAjax
	// usage: $tinkerAjax->debugOn();

	function debugOn()
	{
		$this->bDebug = true;
	}
	

	// debugOff() disables debug messages for tinkerAjax
	// usage: $tinkerAjax->debugOff();

	function debugOff()
	{
		$this->bDebug = false;
	}
	

	// statusMessagesOn() enables messages in the statusbar for tinkerAjax
	// usage: $tinkerAjax->statusMessagesOn();

	function statusMessagesOn()
	{
		$this->bStatusMessages = true;
	}
	

	// statusMessagesOff() disables messages in the statusbar for tinkerAjax
	// usage: $tinkerAjax->statusMessagesOff();

	function statusMessagesOff()
	{
		$this->bStatusMessages = false;
	}
	

	// registerFunction() registers a PHP function to be callable through tinkerAjax
	// $sFunction is a string containing the function name
	// $sRequestType is the RequestType (GET/POST) that should be used 
	//		for this function.  Defaults to POST.
	// usage: $tinkerAjax->registerFunction("http://www.nessna.com/apis/foo", "myfunction", TINKER_HTTP_POST);

	function registerFunction($sFunctionURL, $sFunction, $sRequestType = TINKER_HTTP_POST, $sSecurity = TINKER_INHERIT)
	{
		$url = "";

		if ((strncmp($sFunctionURL, "http://", 7) == 0) || (strncmp($sFunctionURL, "https://", 8) == 0))
		{
			if (strncmp($sFunctionURL, "http://", 7) == 0)
				$url_suffix = substr($sFunctionURL, 7, strlen($sFunctionURL) - 7);
			else if (strncmp($sFunctionURL, "https://", 8) == 0)
				$url_suffix = substr($sFunctionURL, 8, strlen($sFunctionURL) - 8);

			if ($sSecurity == TINKER_INHERIT)
				$url = $GLOBALS["http_url_scheme"] . "://" . $url_suffix;
			else if ($sSecurity == TINKER_HTTPS)
				$url = "https://" . $url_suffix;
			else
				$url = "http://" . $url_suffix;
		}
		else if (strncmp($sFunctionURL, "/", 1) == 0)
		{
			if ($sSecurity == TINKER_INHERIT)
				$url = $GLOBALS["http_url_scheme"] . "://" . $_SERVER["SERVER_NAME"] . ":" . $_SERVER["SERVER_PORT"] . $sFunctionURL;
			else if ($sSecurity == TINKER_HTTPS)
				$url = "https://" . $_SERVER["SERVER_NAME"] . ":" . $_SERVER["SERVER_PORT"] . $sFunctionURL;
			else
				$url = "http://" . $_SERVER["SERVER_NAME"] . ":" . $_SERVER["SERVER_PORT"] . $sFunctionURL;
		}

		$this->aFunctions[] = $sFunction;
		$this->aFunctionURLs[$sFunction] = $url;
		$this->aFunctionRequestTypes[$sFunction] = $sRequestType;
		$this->aFunctionPrefixes[$sFunction] = "tinker_";
	}
	

	// processRequests() is the main communications engine of tinkerAjax
	// The engine handles all incoming tinkerAjax requests, calls the apporiate PHP functions
	// and passes the xml responses back to the javascript response handler
	// if your RequestURI is the same as your web page then this function should
	// be called before any headers or html has been sent.
	// usage: $tinkerAjax->processRequests()

	function processRequests()
	{	
		$sFunctionName = "";
		$aArgs = array();
		$sResponse = "";
		
		$requestMode = -1;

		if (!empty($_GET["TinkerAjax"]))
			$requestMode = TINKER_HTTP_GET;
		else if (!empty($_POST["TinkerAjax"]))
			$requestMode = TINKER_HTTP_POST;
			
		if ($requestMode == -1) 
			return;
	
		header("Content-type: text/xml; charset=UTF-8");
			
		if ($requestMode == TINKER_HTTP_POST)
		{
			$sFunctionName = $_POST["TinkerAjax"];
			
			if (!empty($_POST["TinkerAjaxArgs"])) 
				$aArgs = $_POST["TinkerAjaxArgs"];
		}
		else
		{	
			$sFunctionName = $_GET["TinkerAjax"];
			
			if (!empty($_GET["TinkerAjaxArgs"]))
				$aArgs = $_GET["TinkerAjaxArgs"];
		}
		
		if (!in_array($sFunctionName, $this->aFunctions))
		{
			$objResponse = new tinkerAjaxResponse();
			$objResponse->addAlert("Unknown function " . $sFunctionName);
			$sResponse = $objResponse->getXML();
		}
		else if ($this->aFunctionRequestTypes[$sFunctionName] != $requestMode)
		{
			$objResponse = new tinkerAjaxResponse();
			$objResponse->addAlert("Incorrect request type for that function");
			$sResponse = $objResponse->getXML();
		}
		else
		{
			for ($i = 0; $i < sizeof($aArgs); $i++)
			{
				if ((strstr($aArgs[$i], "<tinker-query>") != false) || (strstr($aArgs[$i], "<tinker-object>") != false)){
					$dom = new DOMDocument;
					$dom->preserveWhiteSpace = FALSE;
					$dom->loadXML($aArgs[$i]);
					$aArgs[$i] = $this->xmlTreeToArray($dom);
					
				}
			}

			$sResponse = call_user_func_array($sFunctionName, $aArgs);
		}
		
		print($sResponse);
		exit();
	}


	function xmlTreeToArray($xmlt)
	{
		$retarray = array();
		$i = 0;

		$objects = $xmlt->getElementsByTagName("object");
			
		foreach($objects as $object)
		{	
			$namesnodelist = $object->getElementsByTagName("name");
			$name = $namesnodelist->item(0)->nodeValue;				
			
			$valuesnodelist = $object->getElementsByTagName("value");
			$value = $valuesnodelist->item(0)->nodeValue;
			
			$retarray[$name] = $value;
		}	

		
		$objects = $xmlt->getElementsByTagName("tinker-object");


		foreach ($objects as $object)
		{
			$retarray["tinkerobject" . $i] = $this->xmlTreeToArray($object);
			$i++;
		}

		return $retarray;
	}


	// generateJavaScript() generates all of the TinkerAjax javascript code including the javascript
	// wrappers for the PHP functions specified by the registerFunction() method and the response
	// xml parser

	function generateJavaScript()
	{
		print("var TinkerAjaxDebug = " . ($this->bDebug ? "true" : "false") . ";\n");
		print("var TinkerAjaxStatusMessages = " . ($this->bStatusMessages ? "true" : "false") . ";\n");
?>

var TINKER_HTTP_GET = 0;
var TINKER_HTTP_POST = 1;

function TinkerAjax()
{

	this.DebugMessage = function(text)
	{
		if (TinkerAjaxDebug)
			alert("TinkerAjax debug:\n " + text);
	}
			
	this.StatusMessage = function(text)
	{
		if (TinkerAjaxStatusMessages)
			window.status = text;
	}
			
	this.workId = "TinkerAjaxWork" + new Date().getTime();
	this.depth = 0;
			

	// Get the XMLHttpRequest Object

	this.getRequestObject = function()
	{
		var req;

		this.DebugMessage("Initializing request object...");

		try
		{
			req = new ActiveXObject("Msxml2.XMLHTTP");
		}
		catch (e)
		{
			try
			{
				req = new ActiveXObject("Microsoft.XMLHTTP");
			}
			catch (e2)
			{
				req = null;
			}
		}

		if(!req && typeof XMLHttpRequest != "undefined")
			req = new XMLHttpRequest();
				
		if (!req)
			this.DebugMessage("Request object instantiation failed.");

		return req;
	}


	// TinkerAjax.$() is shorthand for document.getElementById()

	this.$ = function(sId)
	{
		return document.getElementById(sId);
	}
			

	// TinkerAjax.getFormValues() builds an XML query message from the elements of a form object

	this.getFormValues = function(frm)
	{
		var objForm;

		if (typeof(frm) == "string")
			objForm = this.$(frm);
		else
		{
			this.DebugMessage("Please identify forms by their id string");
			return false;
		}

		var sXml = "<tinker-query>\n";
		
		if (objForm && objForm.tagName == "FORM" )
		{
			var formElements = objForm.elements;

			
			for (var i = 0; i < formElements.length; i++)
			{
				if (formElements[i].name)
				{
					if (formElements[i].type == "radio" || formElements[i].type == "checkbox")
					{
						if (formElements[i].checked == false)
						//	sXml += ("\t\t<object><name>" + formElements[i].name + "</name>\n\t\t<value>" + formElements[i].value + "</value></object>\n");
						continue;
						else
							sXml += ("\t\t<object><name>" + formElements[i].name + "</name>\n\t\t<value>" + formElements[i].value + "</value></object>\n");
					}
					else
						sXml += ("\t<object><name>" + formElements[i].name + "</name>\n\t\t<value>" + formElements[i].value + "</value></object>\n");
				}
			}

		}
				
		sXml += "\n</tinker-query>\n";
				
		return sXml;
	}
			

	// Generates an XML message that TinkerAjax can understand from a javascript object

	this.objectToXML = function(obj)
	{
		var sXml = "<tinker-object>\n";

		for (i in obj)
		{
			try
			{
				if (i == "constructor")
					continue;

				if (obj[i] && typeof(obj[i]) == "function")
					continue;
							
				var key = i;
				var value = obj[i];
				
				if (value && typeof(value) == "object" && 
					(value.constructor == Array) && this.depth <= 50)
				{
					this.depth++;
					value = this.objectToXML(value);
					this.depth--;
				}
						
				sXml += "\t<object>\n\t\t<name>" + key + "</name>\n\t\t<value>" + value + "</value>\n\t</object>\n";
						
			}
			catch(e)
			{
				this.DebugMessage(e);
			}
		}
	
		sXml += "</tinker-object>\n";
			
		return sXml;
	}


	// Sends a XMLHttpRequest to call the specified PHP function on the server

	this.call = function(sURI, sFunction, aArgs, sRequestType)
	{
		var i, r, postData;

		this.StatusMessage("Sending request...");
		this.DebugMessage("Starting TinkerAjax...");

		var TinkerAjaxRequestType = sRequestType;
		var uri = sURI;
		var value;
		
		var uristr = "TinkerAjax=" + encodeURIComponent(sFunction);
		uristr += "&TinkerAjaxr=" + new Date().getTime();

		for (i = 0; i < aArgs.length; i++)
		{
			value = aArgs[i];

			if (typeof(value) == "object")
				value = this.objectToXML(value);
						
			uristr += "&TinkerAjaxArgs[]=" + encodeURIComponent(value);
		}

		switch(TinkerAjaxRequestType)
		{
			case TINKER_HTTP_GET:
				uri += (uri.indexOf("?") == -1 ? "?" : "&") + uristr;
				postData = null;
				break;

			case TINKER_HTTP_POST:
				postData = uristr;
				break;

			default:
				this.DebugMessage("Illegal request type: " + TinkerAjaxRequestType);
				return false;
				break;
		}

		r = this.getRequestObject();

		r.open(TinkerAjaxRequestType == TINKER_HTTP_GET ? "GET" : "POST", uri, true);

		if (TinkerAjaxRequestType == TINKER_HTTP_POST)
		{
			try
			{
				r.setRequestHeader("Method", "POST " + uri + " HTTP/1.1");
				r.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
			}
			catch(e)
			{
				alert("Your browser does not appear to support asynchronous requests using POST.");
				return false;
			}
		}

		r.onreadystatechange = function()
		{
			if (r.readyState != 4)
				return;
					
			if (r.status == 200)
			{
				tinkerAjax.DebugMessage("Received:\n" + r.responseText);

				var data = r.responseXML;

				if (data)
					tinkerAjax.processResponse(data);
			}
		}

		this.DebugMessage("Calling " + sFunction + " uri=" + uri + " (post:" + postData +")");

		r.send(postData);

		this.StatusMessage("Waiting for data...");

		this.DebugMessage(sFunction + " waiting...");

		delete r;
		return true;
	}
			

	// Tests if the new Data is the same as the extant data

	this.willChange = function(element, attribute, newData)
	{
		var oldData;

		if (attribute == "innerHTML")
		{
			tmpTinkerAjax = this.$(this.workId);

			if (tmpTinkerAjax == null)
			{
				tmpTinkerAjax = document.createElement("div");
				tmpTinkerAjax.setAttribute("id", this.workId);
				tmpTinkerAjax.style.display = "none";
				tmpTinkerAjax.style.visibility = "hidden";
				document.body.appendChild(tmpTinkerAjax);
			}

			tmpTinkerAjax.innerHTML = newData;
			newData = tmpTinkerAjax.innerHTML;
		}

		eval("oldData = document.getElementById('" + element + "')." + attribute);

		if (newData != oldData)
			return true;
					
		return false;
	}
			

	// Process XML TinkerAjaxResponses returned from the request

	this.processResponse = function(xml)
	{
		tinkerAjax.StatusMessage("Recieving data...");

		var tmpTinkerAjax = null;
		xml = xml.documentElement;

		for (i = 0; i < xml.childNodes.length; i++)
		{
			if (xml.childNodes[i].nodeName == "alert")
			{
				if (xml.childNodes[i].firstChild)
					alert(xml.childNodes[i].firstChild.nodeValue);
			}

			if (xml.childNodes[i].nodeName == "jscript")
			{
				if (xml.childNodes[i].firstChild)
					eval(xml.childNodes[i].firstChild.nodeValue);
			}

			if (xml.childNodes[i].nodeName == "update")
			{
				var action;
				var element;
				var attribute;
				var search;
				var data;
				var type;
				var objElement;
						
				for (j = 0; j < xml.childNodes[i].attributes.length; j++)
					if (xml.childNodes[i].attributes[j].name == "action")
						action = xml.childNodes[i].attributes[j].value;
						
				var node = xml.childNodes[i];

				for (j = 0; j < node.childNodes.length; j++)
				{
					if (node.childNodes[j].nodeName == "target")
					{
						for (k = 0; k < node.childNodes[j].attributes.length; k++)
							if (node.childNodes[j].attributes[k].name == "attribute")
								attribute = node.childNodes[j].attributes[k].value;

						element = node.childNodes[j].firstChild.nodeValue;
					}

					if (node.childNodes[j].nodeName == "search")
					{
						if (node.childNodes[j].firstChild)
							search = node.childNodes[j].firstChild.nodeValue;
						else
							search = "";
					}

					if (node.childNodes[j].nodeName == "data")
					{
						if (node.childNodes[j].firstChild)
							data = node.childNodes[j].firstChild.nodeValue;
						else
							data = "";
					}
							
					if (node.childNodes[j].nodeName == "type")
					{
						if (node.childNodes[j].firstChild)
							type = node.childNodes[j].firstChild.nodeValue;
						else
							type = "";
					}
				}

				if (action == "assign")
				{
					if (this.willChange(element,attribute,data))
						eval("document.getElementById('" + element + "')." + attribute + " = data;");
				}

				if (action == "append")
					eval("document.getElementById('" + element + "')." + attribute + " += data;");
				
				if (action == "prepend")
					eval("document.getElementById('" + element + "')." + attribute + " = data + document.getElementById('" + element + "')." + attribute);
				
				if (action == "replace")
				{
					eval("var v = document.getElementById('" + element + "')." + attribute);

					var v2 = v.indexOf(search) == -1 ? v : "";

					while (v.indexOf(search) > -1)
					{
						x = v.indexOf(search) + search.length + 1;
						v2 += v.substr(0, x).replace(search, data);
						v = v.substr(x, v.length - x);
					}
		
					if (this.willChange(element,attribute, v2))
						eval('document.getElementById("' + element + '").' + attribute + ' = v2;');
				}
				
				if (action == "clear")
					eval("document.getElementById('" + element + "')." + attribute + " = '';");

				if (action == "remove")
				{
					objElement = this.$(element);

					if (objElement.parentNode && objElement.parentNode.removeChild)
						objElement.parentNode.removeChild(objElement);
				}

				if (action == "create")
				{
					var objParent = this.$(element);
					objElement = document.createElement(attribute);
					objElement.setAttribute("id", data);
					
					if (type && type != "")
						objElement.setAttribute("type", type);
						
					objParent.appendChild(objElement);
				}
			}	
		}
		
		this.StatusMessage("Done");
	}
}
		
function dummy_timer_callback()
{
}

function Timer(timeMilli, callbackFunction)
{
	this.running = false;
	this.timer = null;

	if (typeof callbackFunction == "function")
		this.callbackFunc = callbackFunction;
	else
		this.callbackFunc = dummy_timer_callback;

	if (typeof timeMilli == "number")
		this.timeMil = timeMilli;
	else
		this.timeMil = 1000;

	this.start = function()
	{
		this.running = true;
		this.timer = window.setTimeout(this.callbackFunc, this.timeMil);
	}

	this.stop = function()
	{
		if (this.timer != null)
			window.clearTimeout(this.timer);

		this.running = false;
	}

	this.isRunning = function()
	{
		return this.running;
	}

	this.setTime = function(timeMilli)
	{
		if (typeof timeMilli == "number")
			this.timeMil = timeMilli;
		else
			this.timeMil = 1000;
	}
}

var tinkerAjax = new TinkerAjax();

<?php
		foreach($this->aFunctions as $sFunction)
			print("function " . $this->aFunctionPrefixes[$sFunction] . $sFunction . "()\n{\n\ttinkerAjax.call(\"" . $this->aFunctionURLs[$sFunction] . "\", \"" . $sFunction . "\", arguments, " . $this->aFunctionRequestTypes[$sFunction] . ");\n}\n");
	}
}

?>
