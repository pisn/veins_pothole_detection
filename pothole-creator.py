import xml.etree.ElementTree as ET
import random
import csv
import getopt
import sys
import math

short_options = 'e:n:r:o:p:'
long_options = ['edges=','nodes=','routes=','output=','potholes=']

argumentsList = sys.argv[1:]

arguments, values = getopt.getopt(argumentsList, short_options,long_options)

edgesFilePath = None
nodesFilePath = None
routesFilePath = None
outputPath = None
potholesPerRoute = None

for arg, val in arguments:
    if arg in ('-e', '--edges'):
        edgesFilePath = val
    elif arg in ('-n','--nodes'):
        nodesFilePath = val
    elif arg in ('-r','--routes'):
        routesFilePath = val
    elif arg in ('-o','--output'):
        outputPath = val
    elif arg in ('-p','--potholes'):
        potholesPerRoute = int(val)

if edgesFilePath == None or nodesFilePath == None or routesFilePath == None or outputPath == None or potholesPerRoute == None:             
    exit('Invalid Arguments')

edgesTree = ET.parse(edgesFilePath)
nodesTree = ET.parse(nodesFilePath)
routesTree = ET.parse(routesFilePath)

nodes = dict()
edges = dict()
routes = list()

for node in nodesTree.findall('node'):
    nodes[node.attrib['id']] = {'id' : node.attrib['id'], 'x': float(node.attrib['x']), 'y': float(node.attrib['y'])}


for edge in edgesTree.findall('edge'):
    shape = None

    if 'shape' in edge.attrib:
        shape = list()

        for shapePair in str(edge.attrib['shape']).split(' '):
            x,y = shapePair.split(',')
            shape.append((float(x),float(y)))   

    edges[edge.attrib['id']] = {'id' : edge.attrib['id'], 'from' : edge.attrib['from'], 'to' : edge.attrib['to'], 'numLanes' : int(edge.attrib['numLanes']), 'shape': shape }

for route in routesTree.findall('route'):
    routes.append({'id' : route.attrib['id'], 'edges' : str(route.attrib['edges']).split(' ')})

potholes = list()
for route in routes:
    for i in range(potholesPerRoute):        
        id = random.choice(route['edges'])

        #Some Ids are from access ramps, and will not be found in the list
        if id not in edges:
            i -= 1
            continue 

        selectedEdge = edges[id]
        
        if selectedEdge['shape'] == None:
            startNode = nodes[selectedEdge['from']]
            endNode = nodes[selectedEdge['to']]

            startNode = (startNode['x'], startNode['y'])
            endNode = (endNode['x'], endNode['y'])

            edgeLength = math.sqrt((endNode[0] - startNode[0])**2 + (endNode[1] - startNode[1])**2)

            potholePosition = random.random()*edgeLength           

        else:       
            shapeIndex = random.randint(0, len(selectedEdge['shape'])-2)

            shape_0 = selectedEdge['shape'][shapeIndex]
            shape_1 = selectedEdge['shape'][shapeIndex + 1]        

            edgeLength = math.sqrt((shape_1[0] - shape_0[0])**2 + (shape_1[1] - shape_0[1])**2)

            potholePosition = random.random()*edgeLength

        laneId = random.randint(0, selectedEdge['numLanes'] -1)

        potholes.append({ 'roadId' : selectedEdge['id'],'position' : potholePosition, 'laneId' : laneId, 'routeId' : route['id']})

with open(outputPath, 'w') as outFile:
    writer = csv.DictWriter(outFile, potholes[0].keys())
    writer.writeheader()
    writer.writerows(potholes)
