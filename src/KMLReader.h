/* --------------------------------------------------------------------------
 * Copyright:		2012 by SSC Pacific
 *					53560 Hull St, San Diego, California, 92152-5001, U.S.A.
 *					All rights reserved.
 *					Property of the U.S. Government
 *
 * file:			KMLReader.h
 * 
 * Description:		Class for reading KML files and obtaining the coordinates
 *					of a Region of Interest (ROI) polygon.
 * 
 * @author	Sparta Cheung
 * @since	2012-03-12
 * --------------------------------------------------------------------------
 */

#ifndef __KMLREADER__
#define __KMLREADER__

#include <string>
#include <fstream>

using namespace std;

typedef struct
{
	double lat;
	double lon;
	long strLat;
	long strLon;
} Coords;

class KMLReader
{
private:
	ifstream kmlfile;
	string filedata;
	unsigned int beginIndex;		//beginning index of Polygon's <coordinate>
	unsigned int currIndex;			//current index of coordinates
	unsigned int endIndex;			//end index of Polygon's <coordinate>
	bool readKMLData();

	static const bool debug = false;

public:
	//Constructor
	KMLReader(char *kmlfilename);
	//Destructor
	~KMLReader();

	bool isValid();
	bool setIndexPolygonCoordinates();
	bool getNextCoords(Coords *nextCoords);

};

#endif