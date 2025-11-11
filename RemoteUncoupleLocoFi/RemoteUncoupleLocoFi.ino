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
//  Last Modified : <251110.1531>
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

#include "WiFiConfiguration.h"  
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;
#include "LocoFi.h"

//WiFiServer server(80);

NetworkUDP  udp;
MDNSResponder mDNS;

int LedState = LOW;

#include "BoxcarIdenity.h"

void setup() {
    front.Begin();
    rear.Begin();
    
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
    digitalWrite(LED_BUILTIN,LedState);
    delay(10);
    
    // We start by connecting to a WiFi network
    
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    int count = 100;    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        count--;
        if (count == 0) {
            Serial.println();
            Serial.println();
            Serial.print("Connecting to '");
            Serial.print(ssid);
            Serial.println("'");
            Serial.print(" [password '");
            Serial.print(password);
            Serial.println("']");
            count = 100;
        }
            
    }
    
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    mDNS.begin(BoxcarIdenity);
    udp.begin(LocoFi::LOCOFIPORT);
    MDNS.addService("loco", "udp", LocoFi::LOCOFIPORT);
    
}
                
void loop() {
    int plen = udp.parsePacket();
    unsigned char cmd;
    unsigned char flags;
    //Serial.print("*** loop(): plen = "); Serial.println(plen);
    if (plen > 0)
    {
        cmd = udp.read();
        //Serial.print("*** loop(): cmd = "); Serial.println(cmd,16);
        //unsigned char flags = udp.read();
        //Serial.print("*** loop(): flags = "); Serial.println(flags,16);
        //while (udp.available() > 0)
        //{
        //    unsigned char t = udp.read();
        //    Serial.print("*** loop(): t = "); Serial.println(t,16);
        //}
        switch (cmd) {
        case LocoFi::FWD_LIGHT: 
            flags = udp.read();
            Serial.print("*** FWD_LITE: flags are:");
            Serial.println(flags,16);
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
            flags = udp.read();
            Serial.print("*** REV_LITE: flags are:");
            Serial.println(flags,16);
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
        case LocoFi::QUERY_LOCO_STATE:
            Serial.println("*** QUERY_LOCO_STATE");
            while (udp.available() > 0) {
                uint8_t m = udp.read();
                Serial.println(m,16);
            }
            {
                //udp.beginPacket();
                //uint8_t version[] = {LocoFi::QUERY_LOCO_STATE,1};
                //udp.write(version,sizeof(version));
                //udp.endPacket();
            }
            break;
        case LocoFi::GET_CONSIST:
            Serial.println("*** GET_CONSIST");
            while (udp.available() > 0) {
                uint8_t m = udp.read();
                Serial.println(m,16);
            }
            {
                //udp.beginPacket();
                //uint8_t version[] = {LocoFi::GET_CONSIST,0};
                //udp.write(version,sizeof(version));
                //udp.endPacket();
            }
            break;
        case LocoFi::VERSION: 
            Serial.println("*** VERSION");
            while (udp.available() > 0) {
                uint8_t m = udp.read();
                Serial.println(m,16);
            }
            {
                //udp.beginPacket();
                //uint8_t version[] = {LocoFi::VERSION,0};
                //udp.write(version,sizeof(version));
                //udp.endPacket();
            }
            break;
        case LocoFi::HEARTBEAT:
            flags = udp.read();
            if (flags) {
                if (LedState == HIGH)
                    LedState = LOW;
                else
                    LedState = HIGH;
                digitalWrite(LED_BUILTIN,LedState);
            } else {
                LedState = LOW;
                digitalWrite(LED_BUILTIN,LedState);
            }
            break;
        default:
            Serial.print("*** loop(): cmd = "); Serial.println(cmd,16);
            while (udp.available() > 0) {
                uint8_t m = udp.read();
                Serial.println(m,16);
            }
            break;
        }
    } else {
        // Sleep()
        //esp_sleep_enable_wifi_wakeup();
        //esp_light_sleep_start();
        delay(100);
    }
}    
