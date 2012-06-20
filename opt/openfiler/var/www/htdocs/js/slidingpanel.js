// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// by Scott Andrew - http://scottandrew.com
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
function addEvent(obj, evType, fn)
	{
	if (obj.addEventListener)
		{
		obj.addEventListener(evType, fn, false); 
		return true;
		}
	else if (obj.attachEvent)
		{
		var r = obj.attachEvent('on'+evType, fn);
		return r;
		}
	else
		{
		return false;
		}
	}


// event listeners eventually to replace the one above
function addEventToObject(obj, evt, func)
	{
	var oldhandler = obj[evt];
	if (typeof obj[evt] != 'function')
		{
		obj[evt] = func;
		}
	else
		{
		obj[evt] = function()
			{
			oldhandler();
			func();
			}
		}
	}
	

function primeSlide()
	{
	// set the class name of the containing div to hide appropriate html elements
	document.getElementById('slidingpaneldiv').className = 'hide';
	

	
	// create html elements using js
	createHTML();
	
	// set panel vars (just for neatness' sake)
	var infoPanel = document.getElementById('panelinfo');
	//var infoTrigger = document.getElementById('panelinfo-trigger');
	var infoHide = document.getElementById('panelinfo-hideme');
	
	
	// add in the panel state variables
	infoPanel.state = 'up';
	
	// set event listeners
	//addEvent(infoTrigger, 'click', function(e){clearScreen(infoPanel)});
	addEvent(infoHide, 'click', function(e){clearScreen(infoPanel)});
	
	}

function clearScreen(thisPanel)
	{
	// set panel vars
	var infoPanel = document.getElementById('panelinfo');

	// loop through and close any open windows that aren't this panel
	var panels = new Array(infoPanel);
	for (var i=0; i<panels.length; i++)
		{
		if (((panels[i].state == 'down') || (panels[i].state == 'downing')) && (panels[i] != thisPanel))
			{
			setSlide(panels[i]);
			}
		}
	// run for this panel
	setSlide(thisPanel);
	}

function setSlide(thisPanel)
	{
	// grab the height (this is a number such as -500)
	thisPanel.height = thisPanel.offsetHeight;
	var numframes;
	
	// if this panel is fully up
	if (thisPanel.state == 'up')
		{	
		// re-style the panel so it's only just out of site
		thisPanel.style.top = "-"+thisPanel.height+"px";
		
		// set number of frames the animation will take
		numFrames = 12;
		}
	else
		{
		// calculate the number of frames the animation will take
		// (bearing in mind it could be halfway through an up or down movement)
		var finPos = thisPanel.height;
		var nowPos = thisPanel.style.top;
		nowPos = nowPos.substring(0, (nowPos.length - 2));
		var toGo = (finPos - nowPos);
		numFrames = Math.round((12 / thisPanel.height) * toGo);
		numFrames = (numFrames == 0) ? 1 : numFrames;
		}
	
	// set y start position
	var yStart = thisPanel.offsetTop;
	var yEnd;
	// set y end position
	switch (thisPanel.state)
		{
		case 'up':
			thisPanel.state = 'downing';
			yEnd = 0;
			break;
		case 'downing':
			thisPanel.state = 'uping';
			yEnd = 0;
			break;
		case 'down':
			thisPanel.state = 'uping';
			yEnd = "-"+thisPanel.height;
			break;
		case 'uping':
			thisPanel.state = 'downing';
			yEnd = "-"+thisPanel.height;
			break;
		}
	
	// set current frame number
	frameNum = 0;
	// calculate y distance to be moved each frame
	var yDist = (yEnd - yStart)/(numFrames - 1);
	
	// run the sliding function
	runSlide(thisPanel, yStart, yDist, frameNum, numFrames);
	}

function runSlide(thisPanel, yStart, yDist, frameNum, numFrames)
	{
	// if the animation still has frames left to run
	if (frameNum < numFrames)
		{
		thisPanel.style.top = (yStart + yDist*frameNum) + "px";
		frameNum++;
		return setTimeout(function(){runSlide(thisPanel, yStart, yDist, frameNum, numFrames)}, 8);
		}
	// if the animation is complete
	else
		{
		// if it was moving down
		if (thisPanel.state == 'downing')
			{
			// set to all the way down
			thisPanel.state = 'down';
			}
		else
			{
			thisPanel.state = 'up';
			thisPanel.style.top = "-5000px";
			}
		}
	return true;
	}
	
function createHTML()
	{

	
	// create sliding panel elements
	var infoPanel = document.createElement('div');
	infoPanel.id = 'panelinfo';
	infoPanel.innerHTML = '<div id="paneltitle" class="border"><h3 id="slidingpaneltitle"></h3><div id="paneldata"></div><div id="panelinfo-hideme"></div></div> ';
	document.getElementById('slideDownDiv').appendChild(infoPanel);
	
	
	return;
	}
	
function runScripts()
	{
	
	    primeSlide();
	
	}
