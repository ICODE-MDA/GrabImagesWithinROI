/* --------------------------------------------------------------------------
 * Copyright:		2012 by SSC Pacific
 *					53560 Hull St, San Diego, California, 92152-5001, U.S.A.
 *					All rights reserved.
 *					Property of the U.S. Government
 *
 * file:			N1Reader.h
 * 
 * Description:		Class for reading N1 image files and obtaining the 
 *					coordinates of the image.
 * 
 * @author	Sparta Cheung
 * @since	2012-03-14
 * --------------------------------------------------------------------------
 */

#ifndef __N1READER__
#define __N1READER__

#include <string>
#include <fstream>
#include <vector>

using namespace std;

class N1Reader
{
private:
	ifstream n1file;
	string filedata;
	static const bool debug = false;

public:
	//Constructor
	N1Reader(char *imgfilename);
	//Destructor
	~N1Reader();

	bool isValid();
	bool readImgHeader();
	string SearchForString(string &searchTerm, string notFoundString, char endChar, int beginIndex, int offset);
	signed long GetNumber(string searchterm);
	bool readVertices(vector<float> &LatCoordsIMG, vector<float> &LonCoordsIMG);
};

#endif