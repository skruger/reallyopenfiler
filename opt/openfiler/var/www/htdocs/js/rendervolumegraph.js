/*
 *
 *
 * --------------------------------------------------------------------
 * Copyright (c) 2001 - 2008 Openfiler Project.
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



function drawGraph(jsondata_size, jsondata_label) {

    var plotItems = new Array();
    var plotLabels = new Array();
    var xTicksData = new Array();

    for (i = 0; i < jsondata_size.length; i++) {
        plotItems.push(new Array(i,jsondata_size[i]));
        var obj = {v:i, label:jsondata_label[i]};
        xTicksData.push(obj);

    }

    var colorNum = 1;

    var documentURL = document.URL;

    if (documentURL.match("volumes_editpartitions.html"))
        colorNum = 5;

    if (document.URL.match("volumes.html"))
         colorNum = 2;

    var options = {
        "IECanvasHTC": "/js/plotkit/iecanvas.htc",
        "colorScheme": PlotKit.Base.palette(PlotKit.Base.baseColors()[colorNum]),
        "padding": {left: 0, right: 0, top: 10, bottom: 30},
        "drawBackground": false,
        "xTicks" : xTicksData,
        "axisLabelFontSize": 9



    };


    var layout = new PlotKit.Layout("pie", options);
    layout.addDataset("volumes", plotItems);
    layout.evaluate();
    var canvas = MochiKit.DOM.getElement("graph");
    var plotter = new PlotKit.SweetCanvasRenderer(canvas, layout, options);
    plotter.render();
}
//MochiKit.DOM.addLoadEvent(drawGraph);