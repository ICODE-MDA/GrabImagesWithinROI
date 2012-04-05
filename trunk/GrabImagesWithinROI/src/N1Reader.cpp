/* --------------------------------------------------------------------------
 * Copyright:		2012 by SSC Pacific
 *					53560 Hull St, San Diego, California, 92152-5001, U.S.A.
 *					All rights reserved.
 *					Property of the U.S. Government
 *
 * file:			N1Reader.cpp
 * 
 * Description:		Class for reading N1 image files and obtaining the 
 *					coordinates of the image.
 * 
 * @author	Sparta Cheung
 * @since	2012-03-14
 * --------------------------------------------------------------------------
 */

#include "N1Reader.h"

#include <iostream>


/**
	Constructor
 **/
N1Reader::N1Reader(char *imgfilename)
{
	n1file.open(imgfilename);
}

/**
	Destructor
 **/
N1Reader::~N1Reader()
{
	n1file.close();
}

/**
	Function to check if the N1 image file is valid

	@return boolean		whether the file is opened or not
 **/
bool N1Reader::isValid()
{
	if (n1file.is_open())
		return true;
	else
		return false;
}

/**
	Function to read the header of the N1 image file for coordinates

	@return boolean		whether the file reading was successful or not
 **/
bool N1Reader::readImgHeader()
{
	if (isValid())
	{
		char* TEMP;

		n1file.seekg (0, ios::end);	//seek to the end of the stream
		int length = n1file.tellg();//get the length of the stream
		n1file.seekg (0, ios::beg);	//seek to the beginning of the stream

		//Can't read entire stream, too large.  Just read first 8,000
		length = 8000;
		TEMP = new char[length];

		// read data as a block:
		n1file.read (TEMP,length);
		filedata = TEMP;			//the XMLfile data

		return true;
	}
	else
		return false;
}

/**
	Searches for a specific string

	@param searchTerm
	@param notFoundString
	@param endChar
	@param beginIndex
	@param offset
	@return outputString
 **/
string N1Reader::SearchForString(string &searchTerm, string notFoundString, char endChar, int beginIndex, int offset)
{
	unsigned int loc = 0;			//location of start of searchTerm in inputString
	unsigned int index	= 0;		//current index of inputString
	string outputString;			//the string output
	//check to see if the inputString is empty
	if (!filedata.empty())
	{   //not empty, so access the sensor information
		loc = filedata.find(searchTerm, beginIndex);		//find location of sensorString
		//check to see if searchTerm exists
		if (loc != string::npos)
		{
			//initial location of desired string output
			index = loc + searchTerm.length() + offset;
			while(filedata[index] != endChar)
			{
				outputString.append(1,filedata[index]);
				index++;
			}
		}
		else
		{
			return notFoundString;	//returned if searchTerm not found
		}
	}
	else
	{
		return notFoundString;		//returned if inputString is empty
	}

	return outputString;			//returned if searchTerm was found
}

/**
	Grabs the number from a string and returns its value as a signed long

	@param searchterm
	@return Base		
 **/
signed long N1Reader::GetNumber(string searchterm)
{
	string foundterm;
	string notFoundString = "NULL";
	char endChar = '<';
	bool neg = false;

	string RangeResolutionSearch = searchterm;

	foundterm = SearchForString(RangeResolutionSearch, notFoundString, endChar, 0, 0);

	size_t foundE;
	foundE = foundterm.find("E+");
	// Grab all after the E+
	
	string ExponentString = foundterm.substr(foundE+2);

	size_t foundPlus;
	foundPlus = foundterm.find("+");

	if (foundPlus > 1000)
	{
		foundPlus = foundterm.find("-");
		neg = true;
	}

	int BaseLength = foundE-foundPlus;
	string BaseString = foundterm.substr(foundPlus+1, BaseLength-1);

	//atof requires conversion to C-Style array (char *)
	signed long Base = atof(BaseString.c_str());
	//long Exp = atof(ExponentString.c_str());
	//long Long = Base*pow(10,Exp);

	if (neg)
		Base = -Base;

	return Base;
}

/**
	Function to read the vertices of the image header

	@param LatCoordsIMG
	@param LonCoordsIMG
	@return boolean
 **/
bool N1Reader::readVertices(vector<float> &LatCoordsIMG, vector<float> &LonCoordsIMG)
{
	if (readImgHeader())
	{
		signed long first_near_lat, first_near_long, first_far_lat, first_far_long, last_near_lat, last_near_long, last_far_lat, last_far_long;

		//Get the image coordinates
		first_near_lat = GetNumber("FIRST_NEAR_LAT");
		first_near_long = GetNumber("FIRST_NEAR_LONG");
		//cout << "lat " << first_near_lat << " long " << first_near_long << endl;
		first_far_lat = GetNumber("FIRST_FAR_LAT");
		first_far_long = GetNumber("FIRST_FAR_LONG");
		//cout << "lat " << first_far_lat << " long " << first_far_long << endl;
		last_near_lat = GetNumber("LAST_NEAR_LAT");
		last_near_long = GetNumber("LAST_NEAR_LONG");
		//cout << "lat " << last_near_lat << " long " << last_near_long << endl;
		last_far_lat = GetNumber("LAST_FAR_LAT");
		last_far_long = GetNumber("LAST_FAR_LONG");
		//cout << "lat " << last_far_lat << " long " << last_far_long << endl;
		LatCoordsIMG.push_back(first_near_lat);
		LatCoordsIMG.push_back(first_far_lat);
		LatCoordsIMG.push_back(last_near_lat);
		LatCoordsIMG.push_back(last_far_lat);
		LonCoordsIMG.push_back(first_near_long);
		LonCoordsIMG.push_back(first_far_long);
		LonCoordsIMG.push_back(last_near_long);
		LonCoordsIMG.push_back(last_far_long);

		return true;
	}
	else
		return false;
}