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
//  Last Modified : <251105.0646>
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

#include <WiFi.h>
#include <NetworkUdp.h>
#include <ESPmDNS.h>
#include <esp_sleep.h>

#include "WiFiConfiguration.h"  
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;
#include "LocoFi.h"

//WiFiServer server(80);

NetworkUDP  udp;
MDNSResponder mDNS;

void setup() {
    front.Begin();
    rear.Begin();
    
    Serial.begin(115200);
    pinMode(5, OUTPUT);      // set the LED pin mode
    
    delay(10);
    
    // We start by connecting to a WiFi network
    
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    mDNS.begin("loco");
    udp.begin(LOCOFIPORT);
}
                
void loop() {
    int plen = udp.parsePacket();
    if (plen > 0)
    {
        unsigned char cmd = udp.read();
        unsigned char flags = udp.read();
        while (udp.available() > 0)
        {
            unsigned char t = udp.read();
        }
        switch (cmd) {
            case LocoFi::FWD_LIGHT: 
                if (flags == 0x00) 
                {
                    front.Close();
                } else if (flags == 0x01)
                {
                    front.Open();
                } else
                {
                }
                break;
            case LocoFi::REV_LIGHT:
                if (flags == 0x00)
                {
                    rear.Close();
                } else if (flags == 0x01)
                {
                    rear.Open();
                } else
                {
                }
                break;
            default:
                break;
        }
    } else {
        // Sleep()
        esp_sleep_enable_wifi_wakeup();
        esp_light_sleep_start();
    }
}    
