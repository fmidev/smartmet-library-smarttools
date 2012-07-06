// NFmiColorSpaces.h
//
// NFmiColorSpaces-namespace hanskaa RGB ja HSL v�ri avaruudet ja niiden
// v�liset konversiot. Lis�ksi voidaan vaalentaa ja tummentaa NFmiColor-v�rej�.
// Koodit on p��litty CodeProject:ista:
// http://www.codeproject.com/KB/recipes/colorspace1.aspx

#ifndef NFMICOLORSPACES_H
#define NFMICOLORSPACES_H

#include <NFmiColor.h>

namespace NFmiColorSpaces
{
	struct RGB_color
	{
		RGB_color(void);
		RGB_color(int r_, int g_, int b_);
		RGB_color(const NFmiColor &theColor);

		int r; // 0-255
		int g; // 0-255
		int b; // 0-255
	};

	struct HSL_color
	{
		HSL_color(void);
		HSL_color(int h_, int s_, int l_);

		int h; // 0-360
		int s; // 0-100
		int l; // 0-100
	};

	// Color conversio funktioita:
	// <summary>
	// Converts RGB to HSL.
	// </summary>
	// <param name="red">Red value, must be in [0,255].</param>
	// <param name="green">Green value, must be in [0,255].</param>
	// <param name="blue">Blue value, must be in [0,255].</param>
	HSL_color RGBtoHSL(int red, int green, int blue);

	// <summary>
	// Converts HSL to RGB.
	// </summary>
	// <param name="h">Hue, must be in [0, 360].</param>
	// <param name="s">Saturation, must be in [0, 1].</param>
	// <param name="l">Luminance, must be in [0, 1].</param>
	RGB_color HSLtoRGB(double h, double s, double l);

	// theBrightningFactor on prosentti luku, jolla annettua v�ri� saadaan vaalennettua tai tummennettua.
	// jos prosentti luku on > 0, vaalenee v�ri, jos se on < 0, tummenee v�ri.
	NFmiColor GetBrighterColor(const NFmiColor &theColor, double theBrightningFactor);

}

#endif
