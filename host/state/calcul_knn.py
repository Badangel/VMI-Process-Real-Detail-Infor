import Queue
import math

#Calculate the distance of each point
def getdisance(data,datalen):
    #print data
    dismatrix = [[0 for i in range(datalen)] for i in range(datalen)]
    
    for i in range(datalen-1):
        for j in range(i+1,datalen):
            dis = 0
            for l in range(len(data[0])):
                #print("i:",i," j:",j," k:",k)
                dis = dis + (data[i][l]-data[j][l])*(data[i][l]-data[j][l])
            dis = math.sqrt(dis)
            dismatrix[i][j] = dis
            dismatrix[j][i] = dis
    for d in range(datalen):
        print dismatrix[d]
    return dismatrix

#Calculate the Knn distance of each point
def getknn(matrix,datalen,k):
    kdis = [0 for i in range(datalen)]
    for i in range(datalen):
        pq = Queue.PriorityQueue(k)
        for j in range(datalen):
            if i!=j:
                if pq.full() == False:
                    pq.put([matrix[i][j]*-1,j])
                else:
                    temp = pq.get()
                    if matrix[i][j] < temp[0]*-1:
                        pq.put([matrix[i][j]*-1,j])
                    else:
                        pq.put(temp)
        while not pq.empty():
            temp = pq.get()
            kdis[i] = kdis[i] + temp[0]*-1
            print temp,
        kdis[i] = kdis[i]/k
        print " "
    #print("kids:",kdis)
    return kdis

#Calculate the Knn proportion with k point of each point
def getknnpro(matrix,datalen,k,kdis):
    knnpro = [0 for i in range(datalen)]
    for i in range(datalen):
        pq = Queue.PriorityQueue(k)
        for j in range(datalen):
            if i!=j:
                if pq.full() == False:
                    pq.put([matrix[i][j]*-1,j])
                else:
                    temp = pq.get()
                    if matrix[i][j] < temp[0]*-1:
                        pq.put([matrix[i][j]*-1,j])
                    else:
                        pq.put(temp)
        while not pq.empty():
            temp = pq.get()
            knnpro[i] = knnpro[i] + kdis[temp[1]]
            print temp,
        knnpro[i] = k*kdis[i]/knnpro[i]
        print " "
    #print("kids:",kdis)
    return knnpro

if __name__ == '__main__':
    k = 3
    data = [[55,88,40,62,75],
            [40,85,45,65,75],
            [55,80,40,58,75],
            [45,88,42,62,70],
            [55,82,38,66,78],
            [55,50,99,99,20],
            [58,70,40,60,80],
            [0,0,0,0,0]] 
    datalen = len(data)
    knndismatrix = getdisance(data,datalen)
    kdis = getknn(knndismatrix,datalen,k)
    print getknnpro(knndismatrix,datalen,k,kdis)