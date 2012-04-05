/* --------------------------------------------------------------------------
 * Copyright:		2012 by SSC Pacific
 *					53560 Hull St, San Diego, California, 92152-5001, U.S.A.
 *					All rights reserved.
 *					Property of the U.S. Government
 *
 * file:			GrabImgesWithinROI.cpp
 * 
 * Description:		Copies only images that are within a ROI of a given lat and long
 * 
 * @author	Sparta Cheung
 * @since	2012-03-08
 * --------------------------------------------------------------------------
 */

#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "KMLReader.h"
#include "N1Reader.h"

using namespace std;

/**
	Read the header of the image data for coordinate information

	@param input
	@param filedata
	@return void
 **/
void readImageFile(char *imgfilename, vector<float> &LatCoordsIMG, vector<float> &LonCoordsIMG)
{
	N1Reader n1file(imgfilename);

	if (n1file.readVertices(LatCoordsIMG, LonCoordsIMG))
		return;
	else
	{
		cerr << "Could not open N1 image\n";
		cerr << "Exiting...\n";
		exit(1);
	}
}

/**
	Read the coordinates from the KML file

	@param kmlfilename
	@param LonCoordsROI	longitude vertices coordinates
	@param LatCoordsROI	latitude vertices coordinates
	@return void
 **/
void readKMLFile(char *kmlfilename, vector<float> &LatCoordsROI, vector<float> &LonCoordsROI)
{
	string line;
	long lat, lon;
	int vertexCount = 0;
	Coords *vertex;
	KMLReader kmlfile(kmlfilename);
	bool nextValid;

	vector<Coords> vertex_list;

	//cout << "Vertices: " << endl;
	do
	{
		vertex = new Coords;

		nextValid = kmlfile.getNextCoords(vertex);
		if (!nextValid)
			break;

		//cout << "found vertex: " << vertex->strLat << ", " << vertex->strLon << endl;

		vertex_list.push_back(*vertex);

		vertexCount++;				//increment vertex count
	} while (nextValid);

	for (int i=0; i < vertexCount; i++)
	{
		//cout << "Setting coords: " << vertex_list.at(i).strLon << ", " << vertex_list.at(i).strLat << endl;
		LonCoordsROI.push_back(vertex_list.at(i).strLon);
		LatCoordsROI.push_back(vertex_list.at(i).strLat);
	}

	//NOTE: first vertex will be repeated as last vertex automatically by KML standards, so don't have to do it here
}

/**
	Tests if a point (testX, testY) falls within a polygon defined by vertex pairs (XVerts, YVerts)

	@param LatCoords		
	@param LonCoords		
	@param lat		
	@param lon		
	@return c			int value used as a boolean defining whether test point (testX, testY) is within polygon
 **/
bool testPointWithinPoly(vector<float> LatCoords, vector<float> LonCoords, float lat, float lon)
{
	int i;
	int j;
	int c = 0;

	for (i = 0, j = LatCoords.size()-1; i < LatCoords.size(); j = i++)
	{
		if ( ((LatCoords.at(i) > lat) != (LatCoords.at(j) > lat)) && 
			   (lon < (LonCoords.at(j)-LonCoords.at(i)) * (lat-LatCoords.at(i)) / (LatCoords.at(j)-LatCoords.at(i)) + LonCoords.at(i)) )
			c = !c;
	}

	if (c == 0) return false;
	else return true;
}

//=============================================== Main Function =============================================================
int main(int argc, char *argv[])
{
	vector<float> LatCoordsROI;	//Vertices of the ROI
	vector<float> LonCoordsROI;
	vector<float> LatCoordsIMG; //Vertices of the image
	vector<float> LonCoordsIMG;

	bool IMGinROI = false;
	bool ROIinIMG = false;
	
	if (argc < 3)
	{
		cerr << "Usage: GrabImagesWithinROI.exe <image_filename.N1> <ROI.kml>\n";
		cerr << "Exiting...\n";
		return 1;
	}

	//Set the bounding rect ---------------------------------------------------------
	char *roifilename = argv[2];

	//Get the ROI coordinates from a KML file ---------------------------------------
	readKMLFile(roifilename, LatCoordsROI, LonCoordsROI);

	//Grab image header data --------------------------------------------------------
	char *imgfilename = argv[1];
	readImageFile(imgfilename, LatCoordsIMG, LonCoordsIMG);

	//Check if the four coordinates of the image are within the defined ROI ---------
	for (int i=0; i < LatCoordsIMG.size(); i++)
	{
		IMGinROI = IMGinROI || testPointWithinPoly(LatCoordsROI, LonCoordsROI, LatCoordsIMG.at(i), LonCoordsIMG.at(i));

		//if (IMGinROI)
		//	cout << LatCoordsIMG.at(i) << ", " << LonCoordsIMG.at(i) << endl;
	}

	//Vice versa, check if the corners of the ROI are within the image --------------
	for (int i=0; i < LatCoordsROI.size(); i++)
	{
		ROIinIMG = ROIinIMG || testPointWithinPoly(LatCoordsIMG, LonCoordsIMG, LatCoordsROI.at(i), LonCoordsROI.at(i));
	}

	//cout << "Program is returning all the image filenames that are within the ROI...\n";

	//If any of the corners fall within the ROI, keep the image as within ROI -------
	//cout << "Printing image files that are within ROI:\n";
	if (IMGinROI || ROIinIMG)
	{
		//cout << imgfilename << ": WITHIN ROI\n";
		cout << imgfilename << endl;
		
		//string filename = argv[1];
		//string command = "copy /y " + filename + " c:\\users\\sparta\\desktop\\test\\"; // + inputfile.substr(12,inputfile.length());
		//system(command.c_str());
	}
	else
	{
		//cout << imgfilename << ": NOT WITHIN ROI\n";
	}


	//cout << "Copying done.\n";
	return 0;
}