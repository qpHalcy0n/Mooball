/*
 *  cINI.cpp
 *  INI
 *
 *  Created by avansc on 2/18/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "quickINI.h"

using namespace std;

quickINI::quickINI(char *fileName)
{
	cout << "ini loading..." << endl;

	string line(100,' ');
	size_t found;
	ifstream file(fileName);

	if(!file.is_open())
	{
		cout << "Failed to open INI " << fileName << endl;
	}

	while(!file.eof())
	{
		file >> line;
		found = line.find_first_of("=");
		this->data[line.substr(0, found)] = line.substr(found+1);
	}
}

string quickINI::getValue(string data)
{
	return this->data[data];
}

int	quickINI::getValueAsInt(string data)
{
	return atoi(this->data[data].c_str());
}
