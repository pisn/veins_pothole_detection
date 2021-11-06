import xml.etree.ElementTree as ET
import random
import csv

edgesFilePath = '/home/pedro/Documents/USP/Mestrado/MAC5910/Article/Workspace/veins/examples/veins/osm.edg.xml'
nodesFilePath = '/home/pedro/Documents/USP/Mestrado/MAC5910/Article/Workspace/veins/examples/veins/osm.nod.xml'
routesFilePath = '/home/pedro/Documents/USP/Mestrado/MAC5910/Article/Workspace/veins/examples/veins/teste.rou.xml'
outputPath = '/home/pedro/Documents/USP/Mestrado/MAC5910/Article/Workspace/veins/examples/veins/potholes.csv'

potholesPerRoute = 30

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

            
            if startNode[0] > endNode[0]:
                pothole_x = endNode[0] + random.random() * (startNode[0] - endNode[0])
            else:
                pothole_x = startNode[0] + random.random() * (endNode[0] - startNode[0])

            pothole_y = (endNode[1] - startNode[1])/(endNode[0] - startNode[0])*(pothole_x - startNode[0]) + startNode[1]

        else:       
            shapeIndex = random.randint(0, len(selectedEdge['shape'])-2)

            shape_0 = selectedEdge['shape'][shapeIndex]
            shape_1 = selectedEdge['shape'][shapeIndex + 1]        

            if shape_0[0] > shape_1[0]:
                pothole_x = shape_1[0] + random.random() * (shape_0[0] - shape_1[0])
            else:
                pothole_x = shape_0[0] + random.random() * (shape_1[0] - shape_0[0])

            pothole_y = (shape_1[1] - shape_0[1])/(shape_1[0] - shape_0[0])*(pothole_x - shape_0[0]) + shape_0[1]

        laneId = random.randint(0, selectedEdge['numLanes'] -1)

        potholes.append({ 'roadId' : selectedEdge['id'],'x' : pothole_x, 'y' : pothole_y, 'laneId' : laneId})

with open(outputPath, 'w') as outFile:
    writer = csv.DictWriter(outFile, potholes[0].keys())
    writer.writeheader()
    writer.writerows(potholes)