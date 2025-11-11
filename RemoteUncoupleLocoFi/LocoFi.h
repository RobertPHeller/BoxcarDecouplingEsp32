// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : 2025-11-04 19:16:46
//  Last Modified : <251109.0900>
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
/// @file LocoFi.h
/// @author Robert Heller
/// @date 2025-11-04 19:16:46
/// 
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __LOCOFI_H
#define __LOCOFI_H

namespace LocoFi {

const int LOCOFIPORT = 8266;


enum LocoFiByte1 {
    ENGINE = 0x00,
    DIRECTION = 0x01,
    FWD_LIGHT =  0x03,
    REV_LIGHT =  0x04,
    HORN_BELL =  0x05,
    RPM_REPORT =  0x06,
    SPEED_REPORT =  0x07,
    QUERY_LOCO_STATE =  0x08,
    CONNECT_SSID =  0x09,
    REBOOT =  0x0A,
    VERSION =  0x0B,
    HOSTNAME = 0x0C,
    SETTINGS = 0x0D,
    SET_SPEED = 0x0E, 
    HEARTBEAT = 0x0F,
    FACTORY_RESET = 0x10,
    QUERY_ABOUT_LOCO = 0x11,
    EMERGENCY_STOP = 0x12,
    GET_CONSIST = 0x13,
    CREATE_CONSIST = 0x14,
    DELETE_CONSIST = 0x15,
    SPEED_TABLE = 0x16,
    VOLUME = 0x17,
    HIGH_FREQ_OP = 0x18,
    HELPER = 0x7C,
    HELPER_ACK = 0x7D
};
          
          
          
    
}


#endif// __LOCOFI_H
