import re
import sys

nodeIdRegex = re.compile('(?<=node\[)[0-9]+(?=\].appl hitPotholes)')
potholesHitCount = re.compile('(?<=appl hitPotholes )[0-9]+')

#resultsFilePath = sys.argv[1]
resultsFilePath = '/home/pedro/Documents/USP/Redes/Article/resultsWithWarnings/General-#0.sca'

potholeTotalHit = 0
potholeMaxHit = 0
mostHitNode=-1

with open(resultsFilePath) as file:    
    for line in file:
        if potholesHitCount.search(line)!= None:
            hits = int(potholesHitCount.findall(line)[0])
            potholeTotalHit += hits
            
            if(hits > potholeMaxHit):
                nodeId = int(nodeIdRegex.findall(line)[0])
                potholeMaxHit = hits
                mostHitNode = nodeId

print('Final stats of round:')
print('Total potholes hit: ' + str(potholeTotalHit))
print('Max pothole hit: ' + str(potholeMaxHit))
print('Vehicle which hit the most potholes: ' + str(mostHitNode))