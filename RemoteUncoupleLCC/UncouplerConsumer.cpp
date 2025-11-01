// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : 2025-11-01 15:20:57
//  Last Modified : <251101.1604>
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
/// @file UncouplerConsumer.cpp
/// @author Robert Heller
/// @date 2025-11-01 15:20:57
/// 
///
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";


#include <unistd.h>

#include "utils/logging.h"
#include "openlcb/EventHandlerTemplates.hxx"
#include "openlcb/EventService.hxx"

#include "UncouplerConsumer.h"

void UncouplerConsumer::handle_identify_global(const openlcb::EventRegistryEntry &registry_entry, 
                                               openlcb::EventReport *event, 
                                               BarrierNotifiable *done)
{
    if (event->dst_node && event->dst_node->node_id() != node_->node_id())
    {
        return done->notify();
    }
    openlcb::Defs::MTI mti = openlcb::Defs::MTI_CONSUMER_IDENTIFIED_UNKNOWN;
    event->event_write_helper<1>()->WriteAsync(node_, mti,
                                               openlcb::WriteHelper::global(),
                                               openlcb::eventid_to_buffer(frontOpen_),
                                               done->new_child());
    event->event_write_helper<2>()->WriteAsync(node_, mti,
                                               openlcb::WriteHelper::global(),
                                               openlcb::eventid_to_buffer(frontClose_),
                                               done->new_child());
    event->event_write_helper<3>()->WriteAsync(node_, mti,
                                               openlcb::WriteHelper::global(),
                                               openlcb::eventid_to_buffer(rearOpen_),
                                               done->new_child());
    event->event_write_helper<4>()->WriteAsync(node_, mti,
                                               openlcb::WriteHelper::global(),
                                               openlcb::eventid_to_buffer(rearClose_),
                                               done->new_child());
    done->maybe_done();                                                   
}

void UncouplerConsumer::handle_identify_consumer(const openlcb::EventRegistryEntry &entry,
                                                 openlcb::EventReport *event,
                                                 BarrierNotifiable *done)
{
    if (event->src_node.id == node_->node_id())
    {
        done->notify();
        return;
    }
    openlcb::Defs::MTI mti = openlcb::Defs::MTI_CONSUMER_IDENTIFIED_UNKNOWN;
    event->event_write_helper<1>()->WriteAsync(node_,mti,
                                               openlcb::WriteHelper::global(), 
                                               openlcb::eventid_to_buffer(event->event),
                                               done);
}

void UncouplerConsumer::handle_event_report(const openlcb::EventRegistryEntry &entry, 
                                            openlcb::EventReport *event,
                                            BarrierNotifiable *done)
{
    if (event->event == frontOpen_)
    {
        OpenFront();
    } 
    else if (event->event == frontClose_)
    {
        CloseFront();
    }
    else if (event->event == rearOpen_)
    {
        OpenRear();
    }
    else if (event->event == rearClose_)
    {
        CloseRear();
    }
}
void UncouplerConsumer::OpenFront()
{
    sleep_->clr();
    frontEn_->clr();
    dir_->write(OPENDIR);
    for(int x = 0; x < STEPS; x++)
    {
        step_->set();
        usleep(1000*2);
        step_->clr();
        usleep(1000*2);
    }
    frontEn_->set();
    sleep_->set();
}
void UncouplerConsumer::CloseFront()
{
    sleep_->clr();
    frontEn_->clr();
    dir_->write(CLOSEDIR);
    for(int x = 0; x < STEPS; x++)
    {
        step_->set();
        usleep(1000*2);
        step_->clr();
        usleep(1000*2);
    }
    frontEn_->set();
    sleep_->set();
}
void UncouplerConsumer::OpenRear()
{
    sleep_->clr();
    rearEn_->clr();
    dir_->write(OPENDIR);
    for(int x = 0; x < STEPS; x++)
    {
        step_->set();
        usleep(1000*2);
        step_->clr();
        usleep(1000*2);
    }
    rearEn_->set();
    sleep_->set();
}
void UncouplerConsumer::CloseRear()
{
    sleep_->clr();
    rearEn_->clr();
    dir_->write(CLOSEDIR);
    for(int x = 0; x < STEPS; x++)
    {
        step_->set();
        usleep(1000*2);
        step_->clr();
        usleep(1000*2);
    }
    rearEn_->set();
    sleep_->set();
}

    //void Open() const
    //{
    //    Wake();
    //    digitalWrite(enable_,LOW);
    //    digitalWrite(dir_,OPENDIR);
    //    for(int x = 0; x < STEPS; x++)
    //    {
    //        digitalWrite(step_, HIGH);
    //        delay(2);
    //        digitalWrite(step_, LOW);
    //        delay(2);
    //    }
    //    digitalWrite(enable_,HIGH);
    //    Sleep();
    //}
    //void Close() const
    //{
    //    Wake();
    //    digitalWrite(enable_,LOW);
    //    digitalWrite(dir_,CLOSEDIR);
    //    for(int x = 0; x < STEPS; x++)
    //    {
    //        digitalWrite(step_, HIGH);
    //        delay(2);
    //        digitalWrite(step_, LOW);
    //        delay(2);
    //    }
    //    digitalWrite(enable_,HIGH);
    //    Sleep();
    //}
    //void Wake() const
    //{
    //    digitalWrite(sleep_,HIGH);
    //}
    //void Sleep() const
    //{
    //    digitalWrite(sleep_,LOW);
    //}

void UncouplerConsumer::register_handlers()
{
    openlcb::EventRegistry::instance()->register_handler(
        openlcb::EventRegistryEntry(this, frontOpen_), 1);
    openlcb::EventRegistry::instance()->register_handler(
        openlcb::EventRegistryEntry(this, frontClose_), 1);
    openlcb::EventRegistry::instance()->register_handler(
        openlcb::EventRegistryEntry(this, rearOpen_), 1);
    openlcb::EventRegistry::instance()->register_handler(
        openlcb::EventRegistryEntry(this, rearClose_), 1);
}

void UncouplerConsumer::unregister_handler()
{
    openlcb::EventRegistry::instance()->unregister_handler(this);
}

