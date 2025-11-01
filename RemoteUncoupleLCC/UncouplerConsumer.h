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
//  Last Modified : <251101.1538>
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

#ifndef __UNCOUPLERCONSUMER_H
#define __UNCOUPLERCONSUMER_H

#include "openlcb/EventHandlerTemplates.hxx"
#include "openlcb/ConfigRepresentation.hxx"
#include "utils/ConfigUpdateListener.hxx"
#include "utils/ConfigUpdateService.hxx"
#include "openlcb/RefreshLoop.hxx"

/// CDI Configuration for a @ref ConfiguredConsumer.
CDI_GROUP(UncouplerConfig)
/// Allows the user to assign a name for this output.
CDI_GROUP_ENTRY(description, openlcb::StringConfigEntry<8>, //
                Name("Description"), Description("User name of this Boxcar."));
/// Specifies the event ID to open the front coupler
CDI_GROUP_ENTRY(frontOpen,openlcb::EventConfigEntry, //
                Name("Open Front"),
                Description("(C) Open the front coupler."));
/// Specifies the event ID to close the front coupler
CDI_GROUP_ENTRY(frontClose,openlcb::EventConfigEntry, //
                Name("Close Front"),
                Description("(C) close the front coupler."));
/// Specifies the event ID to open the rear coupler
CDI_GROUP_ENTRY(rearOpen,openlcb::EventConfigEntry, //
                Name("Open Rear"),
                Description("(C) Open the rear coupler."));
/// Specifies the event ID to close the rear coupler
CDI_GROUP_ENTRY(rearClose,openlcb::EventConfigEntry, //
                Name("Close Rear"),
                Description("(C) Close the rear coupler."));
CDI_GROUP_END();



/// OpenLCB Consumer class integrating a simple CDI-based configuration for four
/// event IDs, and an output GPIO object that will be turned on or off
/// depending on the incoming event notifications. This is usually the most
/// important object for a simple IO node.
class UncouplerConsumer : public ConfigUpdateListener, 
                          public openlcb::SimpleEventHandler
{
public:
    UncouplerConsumer(openlcb::Node *node, const UncouplerConfig &cfg, 
                      const Gpio *dir, const Gpio *step, const Gpio *frontEn,
                      const Gpio *rearEn,const Gpio *sleep)
          : node_(node), cfg_(cfg), dir_(dir), step_(step), frontEn_(frontEn), 
    rearEn_(rearEn), sleep_(sleep)
    {
        ConfigUpdateService::instance()->register_update_listener(this);
        register_handlers();
    }
    ~UncouplerConsumer()
    {
        unregister_handler();
        ConfigUpdateService::instance()->unregister_update_listener(this);
    }
    UpdateAction apply_configuration(int fd, bool initial_load,
                                     BarrierNotifiable *done) OVERRIDE
    {
        AutoNotify n(done);
        openlcb::EventId cfg_frontOpen = cfg_.frontOpen().read(fd);
        openlcb::EventId cfg_frontClose = cfg_.frontClose().read(fd);
        openlcb::EventId cfg_rearOpen = cfg_.rearOpen().read(fd);
        openlcb::EventId cfg_rearClose = cfg_.rearClose().read(fd);
        if (cfg_frontOpen == frontOpen_ &&
            cfg_frontClose == frontClose_ &&
            cfg_rearOpen == rearOpen_ &&
            cfg_rearClose == rearClose_) return UPDATED; // nothing to do
        unregister_handler();
        frontOpen_ = cfg_frontOpen;
        frontClose_ = cfg_frontClose;
        rearOpen_ = cfg_rearOpen;
        rearClose_ = cfg_rearClose;
        register_handlers();
        return REINIT_NEEDED; // Causes events identify.
    }
    void factory_reset(int fd) OVERRIDE
    {
        cfg_.description().write(fd, "");
    }
    void handle_identify_global(const openlcb::EventRegistryEntry &registry_entry, 
                                openlcb::EventReport *event, 
                                BarrierNotifiable *done)
        OVERRIDE;
    void handle_identify_consumer(const openlcb::EventRegistryEntry &entry,
                                  openlcb::EventReport *event,
                                  BarrierNotifiable *done) override;
    void handle_event_report(const openlcb::EventRegistryEntry &entry, 
                             openlcb::EventReport *event,
                             BarrierNotifiable *done) override;
private:
    void register_handlers();
    void unregister_handler();
    void OpenFront();
    void CloseFront();
    void OpenRear();
    void CloseRear();
    static constexpr int STEPS = 1000; // Guess
    static constexpr int OPENDIR = Gpio::VHIGH;
    static constexpr int CLOSEDIR = Gpio::VLOW;
    openlcb::Node *node_;
    const UncouplerConfig cfg_;
    const Gpio *dir_;
    const Gpio *step_;
    const Gpio *frontEn_;
    const Gpio *rearEn_;
    const Gpio *sleep_;
    openlcb::EventId frontOpen_{0};
    openlcb::EventId frontClose_{0};
    openlcb::EventId rearOpen_{0};
    openlcb::EventId rearClose_{0};
};


#endif // __UNCOUPLERCONSUMER_H

