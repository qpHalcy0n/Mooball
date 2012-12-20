/*
 *  cINI.h
 *  INI
 *
 *  Created by avansc on 2/18/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _quickINI_h
#define _quickINI_h

#include <string>
#include <map>

using namespace std;

class quickINI
{
public:
	quickINI(){}
	quickINI(char *fileName);
	string getValue(string data);
	int	getValueAsInt(string data);
	
	static quickINI* Instance()
	{
		static quickINI thisINI("Media/config.ini");
		return &thisINI;
	}
	
private:
	map<string, string> data;
};

#endif