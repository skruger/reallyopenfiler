var onLoadEvents = new Array();

function multiOnLoad(onLoadFunction) {
    
    if  (window.onload) {
	if (window.onload != onLoadEvent) {
	    onLoadEvents[0] = window.onload;
	    window.onload = onLoadEvent;
	}		
		
        onLoadEvents[onLoadEvents.length] = onLoadFunction;
    }
    
    else
	window.onload = onLoadFunction;

}

function onLoadEvent()
{
	for (var i = 0; i < onLoadEvents.length; i++)
		onLoadEvents[i]();
}