//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/PotholesDetectionRSU.h"

#include "veins/modules/application/traci/PotholeDetectionMessage_m.h"

using namespace veins;

Define_Module(veins::PotholesDetectionRSU);

void PotholesDetectionRSU::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 57, it will tune to the chan
    std::cout<< "RSU received WSA" << std::endl;

    if (wsa->getPsid() == 57) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void PotholesDetectionRSU::onWSM(BaseFrame1609_4* frame)
{
    PotholeDetectionMessage* wsm = check_and_cast<PotholeDetectionMessage*>(frame);
    std::cout<< "RSU received WSM" << std::endl;
    // this rsu repeats the received traffic update in 2 seconds plus some random delay
    sendDelayedDown(wsm->dup(), 2 + uniform(0.01, 0.2));
}
