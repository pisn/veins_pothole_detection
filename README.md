This is a Veins simulator workspace, developed to simulate VANETs message dispersal in pothole detection applications for academic purposes. This simulator has been developed taking the veins erlangen example as a starting point. 

This simulator has been used to test the scenarios described in the paper [A Pothole Warning System using Vehicular Ad-hoc Networks (VANETs)](https://ieeexplore.ieee.org/document/10043092)

# How to run

1. First, make sure you have everything in place to run a Veins simulation. You can go through [Veins tutorial](https://veins.car2x.org/tutorial/) to install all dependencies, including OMNET++ and SUMO Simulator. 

2. Run the veins_launchd python script with the "-vv" argument in /bin folder to open SUMO TraCI (traffic control interface): `./veins_launchd -vv`

3. Import this project in a OMNET++ workspace. To run this simulation, run the omnetpp.ini inside simulations/veins folder. When the simulator opens, there are two execution options available: "General" and "With potholes warning".  General option runs the simulation without the potholes warning system, while the other ones simulate the potholes detection and further communication. Other vehicles change lanes when the communication is received, trying to avoid the reported potholes. 

The pre-configured simulation scenario is a simulation of vehicles flowing through SP-160 (Imigrantes highway) and SP-150 (Anchieta highway) roads in São Paulo metropolitan area, Brazil.  

# Editing to run our own simulation

Let's take a look on what is necessary to edit this simulation to fit your needs.

## Changing the scenario

You can use any SUMO scenario needed, including OpenStreetMap extracted ones. Vehicle flows can be generated using SUMO [netedit](https://sumo.dlr.de/docs/Netedit/index.html) or any external tool suitable for you. There might be a useful one suitable for your needs in SUMO [contributed software.](https://sumo.dlr.de/docs/Contributed/index.html)

You want to make sure the map and the routes are in correct format. Everything must be in "SUMO plain xml descriptions" (*.edg.xml, *.nod.xml, *.rou.xml, etc). It cannot be in OpenStreetMap *.osm.xml files, for instance. If needed, use the [netconvert](https://sumo.dlr.de/docs/netconvert.html) tool to make needed convertions. Also, there must be a .cfg configuration file. It has a very simple template, and you can manually assemble it if needed. Take the "osm.sumo.cfg" file as an example. 

## Positioning the potholes

The potholes.csv file must contain the list of potholes to be put in the roads for the simulation. The list must contain exactly these columns, seperated by commas:

* Sumo RoadId
* Position (in meters, starting from the edge
* Lane number on which the pothole is (zero based)
* Route ID - the name of the route in which this pothole is. If you have more than one active route passing over the same edges and want vehicles in all of them to pass through the potholes, you need to make sure there is an entry for each route Id. 

You might also generate potholes uniformly random using the pothole-creator.py script in the root directory. You need to pass the edges, nodes and routes xml files as argument, together with the output path and the number of potholes to be created per route. The example scenario, for instance, contains 30 potholes per route randomly placed. To do this, execute:  `python3 pothole-creator.py --edges=osm.edg.xml --nodes=osm.nod.xml --routes=teste.rou.xml --output=potholes.csv --potholes=30`

# Results

Every simulation run generates three files: a .vci, a .sca and a .sci file. The .sca file is specially useful to extract aggregate information over the simulations. You can output directly to the file using the **recordScalar/recordStatistic ** function. In the example scenario, the potholes hit cout for each node is written in this file. Therefore, we can use the pothole-statistics.pt script to count total pothole hit and compare "General" and "With potholes warning" execution.

