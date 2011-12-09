#pragma once
//
//  TalkyException.h
//  Talky
//
//  Created by Elliot Woods on 09/12/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include <string>
using namespace std;

class TalkyException
{
public:
	TalkyException(string message) { this->message = message; }

	string getMessage() { return message; }
protected:
	string message;
};