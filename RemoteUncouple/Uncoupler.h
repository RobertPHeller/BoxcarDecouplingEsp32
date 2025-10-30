// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : 2025-10-30 10:39:17
//  Last Modified : <251030.1111>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
/// @copyright
///    Copyright (C) 2025  Robert Heller D/B/A Deepwoods Software
///			51 Locke Hill Road
///			Wendell, MA 01379-9728
///
///    This program is free software; you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation; either version 2 of the License, or
///    (at your option) any later version.
///
///    This program is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License
///    along with this program; if not, write to the Free Software
///    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/// @file Uncoupler.h
/// @author Robert Heller
/// @date 2025-10-30 10:39:17
/// 
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __UNCOUPLER_H
#define __UNCOUPLER_H
#include <Arduino.h>

class Uncoupler {
public:
    Uncoupler(int dir, int step, int enable, int sleep)
          : dir_(dir), step_(step), enable_(enable), sleep_(sleep)
    {
    }
    void Begin() const
    {
        // setup step and dir pins as outputs
        pinMode(step_, OUTPUT);
        pinMode(dir_, OUTPUT);
        pinMode(enable_, OUTPUT);
        pinMode(sleep_, OUTPUT);
        digitalWrite(enable_,HIGH);
        digitalWrite(sleep_,LOW);
        
    }
    void Open() const
    {
        Wake();
        digitalWrite(enable_,LOW);
        digitalWrite(dir_,OPENDIR);
        for(int x = 0; x < STEPS; x++)
        {
            digitalWrite(step_, HIGH);
            delay(2);
            digitalWrite(step_, LOW);
            delay(2);
        }
        digitalWrite(enable_,HIGH);
        Sleep();
    }
    void Close() const
    {
        Wake();
        digitalWrite(enable_,LOW);
        digitalWrite(dir_,CLOSEDIR);
        for(int x = 0; x < STEPS; x++)
        {
            digitalWrite(step_, HIGH);
            delay(2);
            digitalWrite(step_, LOW);
            delay(2);
        }
        digitalWrite(enable_,HIGH);
        Sleep();
    }
    void Wake() const
    {
        digitalWrite(sleep_,HIGH);
    }
    void Sleep() const
    {
        digitalWrite(sleep_,LOW);
    }
private:
    static constexpr int STEPS = 1000; // Guess
    static constexpr int OPENDIR = HIGH;
    static constexpr int CLOSEDIR = LOW;
    int dir_;
    int step_;
    int enable_;
    int sleep_;
};


#endif // __UNCOUPLER_H

