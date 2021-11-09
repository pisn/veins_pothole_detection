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
#include "veins/modules/application/traci/PotholeReportMessage_m.h"

using namespace veins;

Define_Module(veins::PotholesDetectionRSU);

void PotholesDetectionRSU::handleSelfMsg(cMessage* msg){
    PotholeReportMessage* message = new PotholeReportMessage();
    populateWSM(message); //populating lower layers

    PotholeCollection collection;
    collection.potholes = detectedPotholes;

    message->setPotholes(collection);
    message->setRetransmissionNumber(0);
    message->setEventUniqueId(getEnvir()->getUniqueNumber());

    //if channel changing is enabled, send in service channel 2.
    if(dataOnSch){
        startService(Channel::sch2, 58, "Potholes information service");
        scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), message);
    }
    else { //send right away, no switching is available
        sendDown(message);
    }

    scheduleAt(simTime() + beaconInterval, sendBeaconEvt);

}

void PotholesDetectionRSU::onBSM(DemoSafetyMessage* bsm){

}

void PotholesDetectionRSU::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 57, it will tune to the chan

    if (wsa->getPsid() == 57) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void PotholesDetectionRSU::onWSM(BaseFrame1609_4* frame)
{
    if(PotholeDetectionMessage* wsm = dynamic_cast<PotholeDetectionMessage*>(frame)){
        if(receivedMessages.count(wsm->getEventUniqueId()) == 0){
            Pothole detectedPothole = wsm->getPothole();
            detectedPotholes.push_back(detectedPothole); //Assuming I have never seen it before (wrong, need to compare distance someway)

            receivedMessages.insert(wsm->getEventUniqueId());
        }
    }

}
