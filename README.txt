This OMNET++/Veins workspace contains a pothole detection simulator and potholes reporting using VANETs communication. 

The simulator considers cars passing through potholes emit a message to local RSUs (either directly or retransmitting from car to car until reaching out to RSUs). RSUs now are aware of potholes in the road, and start warning drivers passing through the road. Drivers are instructed to change lanes to avoid hitting a pothole. 

For instructions on how to run the simulation environment, please refer to VEINS tutorial (https://veins.car2x.org/tutorial/).

To create the potholes in a SUMO simulation environment, use pothole-creator.py script. 

This simulator was create with Veins native example simulator (erlangen) as a starting point.

All code in this repository is licensed under GPL2 license. 
