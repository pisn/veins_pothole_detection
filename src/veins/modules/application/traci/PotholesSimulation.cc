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

#include <veins/modules/application/traci/PotholesSimulation.h>
#include "veins/modules/application/traci/PotholeDetectionMessage_m.h"
#include "veins/modules/application/traci/PotholeReportMessage_m.h"
#include "veins/modules/application/traci/PotholeCollection.h"
#include <map>

using namespace veins;

Define_Module(veins::PotholesSimulation);

std::map<std::string, std::vector<std::vector<std::string>>> potholesMap;
bool potholesMapLoaded = false;

std::map<std::string, std::vector<std::vector<std::string>>> read_potholes_map(std::string filename){
    // Reads a CSV file into a vector of <string, vector<int>> pairs where
    // each pair represents <column name, column values>
    std::string delimiter = ",";

    // Create a vector of <string, int vector> pairs to store the result
    std::map<std::string, std::vector<std::vector<std::string>>> result;

    // Create an input filestream
    std::ifstream myFile(filename);

    // Make sure the file is open
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");

    // Helper vars
    std::string line;
    std::string val;

    std::getline(myFile, line); //Just to read first line. Will descart

    // Read data, line by line
    while(std::getline(myFile, line))
    {
        std::vector<std::string> values;

        char* ptr;
        ptr=strtok(&line[0], ",");

        while(ptr != NULL){
            values.push_back(ptr);
            ptr = strtok(NULL,",");
        }

        std::vector<std::string> v {values[1],values[2],values[3].substr(0, values[3].size()-1)};

        if(result.count(values[0]) > 0){
            result[values[0]].push_back(v);
        }
        else {
            std::vector<std::vector<std::string>> listOfPotholesInRoad {v};
            result.insert(std::pair<std::string, std::vector<std::vector<std::string>>>(values[0], listOfPotholesInRoad));
        }
    }

    // Close file
    myFile.close();

    return result;
}


void PotholesSimulation::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        EV << "Initializing " << par("appName").stringValue() << std::endl;

        // Read three_cols.csv and ones.csv
        if(!potholesMapLoaded){
            potholesMap = read_potholes_map("potholes3.csv");
            potholesMapLoaded = true;
        }

        hitPotholes = 0;

        warnPotholes = par("warnPotholes").boolValue();
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void PotholesSimulation::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here

    recordScalar("nodeId", myId);
    recordScalar("hitPotholes",hitPotholes);
}

void PotholesSimulation::onBSM(DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
}

void PotholesSimulation::onWSM(BaseFrame1609_4* wsm)
{
    //Received pothole detection communication from another vehicle
    if(PotholeDetectionMessage* message = dynamic_cast<PotholeDetectionMessage*>(wsm))
    {
        if(message->getRetransmissionNumber() > 5)
        { //Limit of packet retransmission
            return;
        }

        if(sentMessages.count(message->getEventUniqueId()) == 0)
        {
            findHost()->getDisplayString().setTagArg("i", 1, "blue");

            message->setSenderAddress(myId);
            message->setSerial(3);
            message->setRetransmissionNumber(message->getRetransmissionNumber()+1);

            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), message->dup());

            sentMessages.insert(message->getEventUniqueId());
        }
    }
    else if (PotholeReportMessage* message = dynamic_cast<PotholeReportMessage*>(wsm)) //receiving potholes periodic report from RSUs
    {
        if(message->getRetransmissionNumber() > 5)
        { //Limit of packet retransmission
            return;
        }

        PotholeCollection potCollection = message->getPotholes();
        reportedPotholes = potCollection.getPotholesRoadMap();

        //Retransmit received pothole report for other nodes further from the RSU
        if(sentMessages.count(message->getEventUniqueId()) == 0)
        {
            message->setSenderAddress(myId);
            message->setSerial(3);
            message->setRetransmissionNumber(message->getRetransmissionNumber()+1);

            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), message->dup());

            sentMessages.insert(message->getEventUniqueId());
        }
    }
}

void PotholesSimulation::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void PotholesSimulation::handleSelfMsg(cMessage* msg)
{
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    if (PotholeDetectionMessage* wsm = dynamic_cast<PotholeDetectionMessage*>(msg))
    {
        sendDown(wsm->dup());
    }
    else if (PotholeReportMessage *wsm = dynamic_cast<PotholeReportMessage*>(msg))
    {
        sendDown(wsm->dup());
    }
    else
    {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void PotholesSimulation::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

    std::string roadId = traciVehicle->getRoadId();

    int currentLane = traciVehicle->getLaneIndex();
    double currentPosition = traciVehicle->getLanePosition();

    //Check if car hit an actual pothole
    if(potholesMap.count(roadId) > 0){
        std::vector<std::vector<std::string>> potholes = potholesMap[roadId];

        for(std::vector<std::string> pothole : potholes){

            int potholeLane = std::stoi(pothole[1]);
            double potholePosition = std::stod(pothole[0]);
            double distanceToPothole = std::abs(potholePosition - currentPosition);

            if(currentLane == potholeLane){
                if(distanceToPothole < 0.8 ){
                    hitPotholes++;

                    EV_TRACE << "Pothole hit! NodeID:" + std::to_string(myId) <<std::endl;

                    //Updating color in simulation
                    findHost()->getDisplayString().setTagArg("i", 1, "red");
                    potholeHitRoundCount = 0;

                    if (warnPotholes) {
                        //Comunicating RSU of detected pothole
                        PotholeDetectionMessage* message = new PotholeDetectionMessage();
                        populateWSM(message); //populating lower layers

                        Pothole detectedPothole;
                        detectedPothole.roadId = roadId;
                        detectedPothole.potholePosition = currentPosition;
                        detectedPothole.potholeLane = currentLane;

                        message->setPothole(detectedPothole);

                        message->setRetransmissionNumber(0);
                        message->setEventUniqueId(getEnvir()->getUniqueNumber());

                        sentMessages.insert(message->getEventUniqueId());

                        //if channel changing is enabled, send in service channel 2.
                        if(dataOnSch){
                            startService(Channel::sch2, 57, "Potholes information service");
                            scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), message);
                        }
                        else { //send right away, no switching is available
                            sendDown(message);
                        }
                    }

                }
            }
        }
    }

    if (warnPotholes) {
        //Checking if car has been alerted by RSUs of pothole ahead. If it has, change lane
        if(reportedPotholes.count(roadId) > 0){
            for(Pothole p : reportedPotholes[roadId]){
                double distanceToPothole = p.potholePosition - currentPosition;

                if(p.potholeLane == currentLane && distanceToPothole < 200 && distanceToPothole > 0){
                    TraCICommandInterface::Road* road = new TraCICommandInterface::Road(mobility->getCommandInterface(), roadId);
                    int32_t numberOfLanes = road->getLanesNumber();

                    double neededDuration = distanceToPothole/traciVehicle->getSpeed();
                    if(neededDuration < 5){
                        neededDuration = 5; //minimum duration of lane change is 10 seconds.
                    }

                    //if is currently on the leftiest lane, change to right. If not, change to left
                    if(currentLane == (numberOfLanes - 1)){
                        traciVehicle->changeLane(currentLane-1, neededDuration*2);
                    }
                    else{
                        traciVehicle->changeLane(currentLane+1, neededDuration*2);
                    }

                    EV_TRACE << "Notified of pothole ahead. Changing lane to avoid." << std::endl;

                    findHost()->getDisplayString().setTagArg("i", 1, "yellow");
                    potholeHitRoundCount = 0;
                }
            }
        }
    }

    if(potholeHitRoundCount >=0){
        potholeHitRoundCount++;
    }

    if(potholeHitRoundCount >= 10){
        findHost()->getDisplayString().setTagArg("i", 1, "gray");
        potholeHitRoundCount = -1;
    }

}
