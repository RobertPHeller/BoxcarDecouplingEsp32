// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : 2025-10-30 09:53:49
//  Last Modified : <251030.1112>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
//
//    Copyright (C) 2025  Robert Heller D/B/A Deepwoods Software
//			51 Locke Hill Road
//			Wendell, MA 01379-9728
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// 
//
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";

const int DIR = 5;
const int STEP = 6;
const int microMode = 16; // microstep mode, default is 1/16 so 16; ex: 1/4 would be 4
// full rotation * microstep divider
const int FRONTEN = 9;
const int REAREN = 10;
const int SLEEP  = 11;

#include "Uncoupler.h"

Uncoupler front(DIR,STEP,FRONTEN,SLEEP);
Uncoupler rear(DIR,STEP,REAREN,SLEEP);

void setup() {
    front.Begin();
    rear.Begin();
}
                
void loop() {
    // put your main code here, to run repeatedly:
}    
