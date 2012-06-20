<?php

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

	require("pre.inc");

	$width = $_GET["width"];		// width of final pie chart
	$height = $_GET["height"];		// height of final pie chart
	$scale = $_GET["scale"];		// antialiasing scale

	$pieheight = $_GET["pieheight"];	// height of pie chart
	$theta = $_GET["theta"];		// angles of rotation
	$phi = $_GET["phi"];

	$count = $_GET["count"];

	if ($width == 0)
		$width = 320;
	if ($height == 0)
		$height = 200;
	if ($scale == 0)
		$scale = 1;
	if ($pieheight == 0)
		$pieheight = 20;
	if ($count == 0)
		exit;
	if (($theta < 0) || ($theta > 90))
		exit;
	if (($phi < 0) || ($phi > 360))
		exit;

	$sum = 0;
	for ($i = 0; $i < $count; $i++)
		$sum += $_GET["value" . $i];

	if ($sum == 0)
		exit;

	$swidth = $width * $scale;
	$sheight = $height * $scale;
	$pieheight *= $scale;
	$pieradius = intval(min($swidth, $sheight) * 0.9);
	$cx = intval(min($swidth, $sheight)) / 2;
	$cy = intval(min($swidth, $sheight)) / 2;

	$im = imagecreatetruecolor($swidth, $sheight);

	$bgcolor = imagecolorallocate($im, $GLOBALS["color_inner_body_r"], $GLOBALS["color_inner_body_g"], $GLOBALS["color_inner_body_b"]);
	
	$outlinebottomcolor = imagecolorallocate($im, 0x80, 0x80, 0x80);
	$outlinetopcolor = imagecolorallocate($im, 0x00, 0x00, 0x00);
	imagefilledrectangle($im, 0, 0, $swidth - 1, $sheight - 1, $bgcolor);

	$piecolors[0] = imagecolorallocate($im, 0xa2, 0xe5, 0x4b);
	$piecolors[1] = imagecolorallocate($im, 0xff, 0x5d, 0x35);
	$piecolors[2] = imagecolorallocate($im, 0xdd, 0xdd, 0xee);
	$piecolors[3] = imagecolorallocate($im, 0x00, 0xff, 0xff);
	$piecolors[4] = imagecolorallocate($im, 0x00, 0xff, 0x00);
	$piecolors[5] = imagecolorallocate($im, 0xff, 0x00, 0xff);
	$piecolors[6] = imagecolorallocate($im, 0x7f, 0x00, 0x00);
	$piecolors[7] = imagecolorallocate($im, 0x7f, 0x00, 0x7f);
	$piecolors[8] = imagecolorallocate($im, 0x00, 0x00, 0x7f);
	$piecolors[9] = imagecolorallocate($im, 0x00, 0x7f, 0x7f);
	$piecolors[10] = imagecolorallocate($im, 0x00, 0x7f, 0x00);
	$piecolors[11] = imagecolorallocate($im, 0x82, 0x7f, 0x00);
	$piecolors[12] = imagecolorallocate($im, 0x00, 0x00, 0x00);
	$piecolors[13] = imagecolorallocate($im, 0x19, 0x19, 0x19);
	$piecolors[14] = imagecolorallocate($im, 0x33, 0x33, 0x33);
	$piecolors[15] = imagecolorallocate($im, 0x76, 0x76, 0x76);

	$angle = 0.0;

	for ($i = 0; $i < $count; $i++)
	{
		$pieangle = (($_GET["value" . $i]) / $sum) * 360;
		imagefilledarc($im, $cx, $cy + ($pieheight * (($phi * 4) / 360)), $pieradius, $pieradius * (1.0 - (($phi * 4) / 360)), $angle, $angle + $pieangle, $piecolors[($i % 16)], IMG_ARC_PIE);
		imagefilledarc($im, $cx, $cy + ($pieheight * (($phi * 4) / 360)), $pieradius, $pieradius * (1.0 - (($phi * 4) / 360)), $angle, $angle + $pieangle, $outlinebottomcolor, IMG_ARC_PIE | IMG_ARC_NOFILL | IMG_ARC_EDGED);
		$angle += $pieangle;
	}
	
	for ($j = 1; $j < ($pieheight * 2 * (($phi * 4) / 360)); $j++)
	{
		$angle = 0.0;

		for ($i = 0; $i < $count; $i++)
		{
			$pieangle = (($_GET["value" . $i]) / $sum) * 360;
			imagefilledarc($im, $cx, $cy + ($pieheight * (($phi * 4) / 360) - $j), $pieradius, $pieradius * (1.0 - (($phi * 4) / 360)), $angle, $angle + $pieangle, $piecolors[($i % 16)], IMG_ARC_PIE | IMG_ARC_NOFILL | IMG_ARC_EDGED);
			$angle += $pieangle;
		}
	}

	$angle = 0.0;

	for ($i = 0; $i < $count; $i++)
	{
		$pieangle = (($_GET["value" . $i]) / $sum) * 360;
		imagefilledarc($im, $cx, $cy - ($pieheight * (($phi * 4) / 360)), $pieradius, $pieradius * (1.0 - (($phi * 4) / 360)), $angle, $angle + $pieangle, $outlinetopcolor, IMG_ARC_PIE | IMG_ARC_NOFILL | IMG_ARC_EDGED);
		imagefilledarc($im, $cx + 1, $cy - ($pieheight * (($phi * 4) / 360)) + 1, $pieradius, $pieradius * (1.0 - (($phi * 4) / 360)), $angle, $angle + $pieangle, $outlinetopcolor, IMG_ARC_PIE | IMG_ARC_NOFILL | IMG_ARC_EDGED);
		$angle += $pieangle;
	}

/*
	for ($i = 0; $i < $count; $i++)
	{
		imagefilledrectangle($im, ($swidth * 0.05), (min($swidth, $sheight) * 1.2) + (15 * $scale * $i), ($swidth * 0.05) + (25 * $scale), (min($swidth, $sheight) * 1.2) + (15 * $scale * $i) + (10 * $scale), $piecolors[($i % 16)]);
		imagerectangle($im, ($swidth * 0.05), (min($swidth, $sheight) * 1.2) + (15 * $scale * $i), ($swidth * 0.05) + (25 * $scale), (min($swidth, $sheight) * 1.2) + (15 * $scale * $i) + (10 * $scale), $bordertopcolor);
	}
*/

	$targetim = imagecreatetruecolor($width, $height);
	imagecopyresampled($targetim, $im, 0, 0, 0, 0, $width, $height, $swidth, $sheight);

/*
	$units = $_GET["units"];

	for ($i = 0; $i < $count; $i++)
	{
		$str = $_GET["str" . $i];

		if (strlen($str) == 0)
			$str = "Foo";
		
		$str .= " (" . $_GET["value" . $i];
		if (strlen($units) > 0)
			$str .= " " . $units;
		$str .= ") [" . intval((100.0 * $_GET["value" . $i]) / $sum) . "%]";

		imagestring($targetim, 2, ($width * 0.05) + 32, (min($width, $height) * 1.2) + (15 * $i) - 2, $str, bordertopcolor);
	}
*/
	header("Content-type: image/png");
	imagepng($targetim);
?>
