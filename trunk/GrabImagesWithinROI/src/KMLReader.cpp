/* --------------------------------------------------------------------------
 * Copyright:		2012 by SSC Pacific
 *					53560 Hull St, San Diego, California, 92152-5001, U.S.A.
 *					All rights reserved.
 *					Property of the U.S. Government
 *
 * file:			KMLReader.cpp
 * 
 * Description:		Class for reading KML files and obtaining the coordinates
 *					of a Region of Interest (ROI) polygon.
 * 
 * @author	Sparta Cheung
 * @since	2012-03-12
 * --------------------------------------------------------------------------
 */

#include "KMLReader.h"

#include <iostream>

/**
	Constructor
 **/
KMLReader::KMLReader(char *kmlfilename)
{
	kmlfile.open(kmlfilename);
	beginIndex = -1;
	currIndex = -1;
}

/**
	Destructor
 **/
KMLReader::~KMLReader()
{
	kmlfile.close();
}

/**
	Function to check file is opened

	@return boolean		whether the KML file is opened or not
 **/
bool KMLReader::isValid()
{
	if (kmlfile.is_open())
		return true;
	else
		return false;
}

/**
	Function to read the KML data of the image

	@return boolean		whether the KML file was readable or not
 **/
bool KMLReader::readKMLData()
{
	if (isValid())						//check if the file is openable
	{
		kmlfile.seekg (0, ios::end);	//seek to the end of the stream
		int length = (int) kmlfile.tellg();	// to get the length of the file
		kmlfile.seekg (0, ios::beg);	//seek to the beginning of the stream for reading
		char *buffer;
		buffer = new char [length];		//allocate memory for the data

		kmlfile.read(buffer,length);		//read the entire contents of the file
		filedata = buffer;				// and store it into filedata string

		//cout << filedata << endl;
	}
	else
		return false;
}

/**
	Function to set the indices of the header where the coordinates are located

	@return boolean		whether the KML file was opened or not
 **/
bool KMLReader::setIndexPolygonCoordinates()
{
	size_t found;

	if (readKMLData())	//read the KML contents to filedata string
	{
		if (debug)
			cout << "DEBUG: Successfully read KML file data\n";

		found = filedata.find("<Polygon>");
		if (found != string::npos)
		{
			if (debug)
				cout << "DEBUG: Successfully found <Polygon> string\n";

			found = filedata.find("<coordinates>",found+1);
			beginIndex = int(found) + 14;
			currIndex = beginIndex;

			found = filedata.find("</coordinates>",beginIndex);
			endIndex = int(found);
		}
	}
	else
		return false;
}

/**
	Function to get the next coordinates of the polygon in the KML file

	@param nextCoords
	@return boolean
 **/	
bool KMLReader::getNextCoords(Coords *nextCoords)
{
	if (beginIndex == -1)
	{
		if (setIndexPolygonCoordinates() == false)
		{
			cerr << "Could not set index in KML\n";
			return false;
		}
	}

	//Declare variables
	size_t found = currIndex;		//beginning index for coordinate's string
	size_t foundEnd;				//end index for coordinate's string
	string strCoords;				//string to hold next coordinate's string
	size_t foundLonBegin;
	size_t foundLonEnd;
	size_t foundLatEnd;
	string strTempLat, strTempLon;
	size_t decLoc;					//index of the decimal in a string
	
	//Find the ",0", or the ending of a coordinate point
	foundEnd = filedata.find(",0 ", found+1);

	if (debug)
		cout << "DEBUG: indices: " << found << ", " << foundEnd << endl;

	if (foundEnd > endIndex || foundEnd == string::npos)
	{
		if (debug)
			cout << "There are no more coordinates to parse\n";
		currIndex = foundEnd+1;
		return false;
	}

	//Extract next coordinate's string
	strCoords = filedata.substr(found,foundEnd-found+1);
	currIndex = foundEnd+3;			//set the current index to be the end of the current coordinate index for the next read, skipping the ",0<return>" characters

	if (debug)
		cout << "DEBUG: Substring: " << filedata.substr(found,foundEnd-found+1) << endl;


	//Found the next coordinate's string;  now, convert it to type long and store to *nextCoords
	//Parse the longitude
	foundLonBegin = found;
	foundLonEnd = filedata.find(",", foundLonBegin+1);
	if (debug)
	{
		cout << "String: " << filedata.substr(foundLonBegin, foundLonEnd-foundLonBegin) << endl;
		printf("Number captured: %.20f\n", atof(filedata.substr(foundLonBegin, foundLonEnd-foundLonBegin).c_str()));
	}
	// set the double value
	nextCoords->lon = atof(filedata.substr(foundLonBegin, foundLonEnd-foundLonBegin).c_str());
	// now, complicated code to set the long value, with exactly 6 decimal places and omit the actual decimal point
	strTempLon = filedata.substr(foundLonBegin, foundLonEnd-foundLonBegin);
	decLoc = strTempLon.find(".");
	strTempLon = strTempLon.substr(0, decLoc+7);
	strTempLon.erase(decLoc, 1);
	nextCoords->strLon = atol(strTempLon.c_str());


	//Parse the latitude
	foundLatEnd = foundEnd+1;
	if (debug)
	{
		cout << "String: " << filedata.substr(foundLonEnd+1, foundLatEnd-foundLonEnd-2) << endl;
		printf("Number captured: %.20f\n", atof(filedata.substr(foundLonEnd+1, foundLatEnd-foundLonEnd-2).c_str()));
	}
	// set the double value
	nextCoords->lat = atof(filedata.substr(foundLonEnd+1, foundLatEnd-foundLonEnd-2).c_str());
	// now, complicated code to set the long value, with exactly 6 decimal places and omit the actual decimal point
	strTempLat = filedata.substr(foundLonEnd+1, foundLatEnd-foundLonEnd-2);
	decLoc = strTempLat.find(".");
	strTempLat = strTempLat.substr(0, decLoc+7);
	strTempLat.erase(decLoc, 1);
	nextCoords->strLat = atol(strTempLat.c_str());

	if (debug)
	{
		cout << "coords: " << nextCoords->lat << ", " << nextCoords->lon << "  ; " << nextCoords->strLat << ", " << nextCoords->strLon << endl;
	}

	return true;
}