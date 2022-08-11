import cv2 as cv
import numpy as np
import math
from dijkstar import Graph, find_path
import time as t

# Press and hold the mouse to select points
# Press down left click for first point
# Release mouse click for second point
def click(event, x, y, flags, param):
    global retPt
    # if the left mouse button was clicked, record the starting
	# (x, y) coordinates
    if event == cv.EVENT_LBUTTONDOWN:
        retPt = [(x, y)]
    elif event == cv.EVENT_LBUTTONUP:
        retPt.append((x, y))
	# record the ending (x, y) coordinates
    
    
start_time = t.time()
img = cv.imread("../data/plane.jpg", 1)
final = img.copy()
vertex = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
h,w = img.shape[1::-1]

graph = Graph(undirected=True)

neighbors = [((0,1), (1,0)),
             ((-1,0), (0,-1)),
             ((-1,-1), (-1,1)),
             ((1,-1), (1,1))]

theta = 0
neighbors_to_visit = 4

for i in range (1, w-1):
    for j in range(1, h-1):
        for n in neighbors[:neighbors_to_visit]:
            (di1, dj1), (di2, dj2) = n

            G_x = float(vertex[i,j]) - float(vertex[i + di1, j + dj1])
            G_y = float(vertex[i,j]) - float(vertex[i + di2, j + dj2]) 
            G = np.sqrt((G_x)**2 + (G_y)**2)
            if (G_x != 0):
                theta = math.atan(G_y/G_x)
                
            rotated_theta = theta #+ math.pi/2
            G_x_a = abs(G * math.cos(rotated_theta)) + 0.0000001
            G_y_a = abs(G * math.sin(rotated_theta)) + 0.0000001
            
            # Strongest Edge will have lowest weights
            W_x = 1/G_x_a
            W_y = 1/G_y_a
            
            # Assigning weights
            graph.add_edge((i,j), (i + di1, j + dj1), W_x) 
            graph.add_edge((i,j), (i + di2, j + dj2), W_y)
        

# for i in range (1, w-1):
#     for j in range(1, h-1):
#         # Gradients are calculated with a right-bottom orientation
#         G_x = float(vertex[i,j]) - float(vertex[i,j+1])    # Center - right
#         G_y = float(vertex[i,j]) - float(vertex[i+1, j])   # Center - bottom
#         G = np.sqrt((G_x)**2 + (G_y)**2)
#         if (G_x != 0):
#             theta = math.atan(G_y/G_x)
            
#         # Theeta is rotated in clockwise direction (90 degrees) to align with edge
#         rotated_theta = theta #+ math.pi/2
#         G_x_a = abs(G * math.cos(rotated_theta)) + 0.0000001
#         G_y_a = abs(G * math.sin(rotated_theta)) + 0.0000001
        
#         # Strongest Edge will have lowest weights
#         W_x = 1/G_x_a
#         W_y = 1/G_y_a
        
#         # Assigning weights
#         graph.add_edge((i,j), (i,j+1), W_x) # W_x is given to right of current vertex
#         graph.add_edge((i,j), (i+1,j), W_y) # W_y is given to bottom of current vertex
        
        
print (f"Nodes: {graph.node_count}, Edges: {graph.edge_count}")
print ("Time Taken to turn image to graph: {}".format(t.time()-start_time))
# Opens image select the points using mouse and press c to done
cv.namedWindow("image")

running = True
while running:
    while True:
        cv.setMouseCallback("image", click)
        cv.imshow("image", final)
        key = cv.waitKey(2) & 0xFF
        if key == ord("c"):
            #cv.destroyWindow("image")
            break
        if key == ord("q"):
            running = False
            break
    # Gets the starts and ending point in image formatss
    startPt = (retPt[0][1], retPt[0][0])
    endPt = (retPt[1][1], retPt[1][0])
    print(f"{startPt}, {endPt}")
    
    # Find_path[0] return nodes it travelled for shortest path
    path = find_path(graph,startPt,endPt)[0]
    if path is None:
        break
    # Turn those visited nodes to white
    for i in range(0,len(path)):
        #final[path[i][0]-1, path[i][1]-1] = (0, 0, 255)
        final[path[i][0], path[i][1]] = (255, 0, 255)
        
    cv.imshow('ImageWindow', final)
    cv.waitKey(0)
    cv.destroyWindow('ImageWindow')