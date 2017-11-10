import DBHelper
import math
import Queue

k = 4
db = DBHelper.DBHelper()
sql = "select 100-idl_cpu_in,usep_mem_in,usep_swap_in,pagein_in,pageout_in,interrupts1_in,interrupts2_in,interrupts3_in,loadavg1_in*100,loadavg5_in*100,loadavg15_in*100,read_total_in,writ_total_in,ps_root_in,ps_other_in,use_cpu_out,recv_net_out,send_net_out,lsmod_out,ps_out from state"
data = db.oncesql(sql)

#Calculate the K distance of each line
def getkdistance(data):
    datalen = len(data) 
    print ("len: ",datalen,len(data[0]))
    #print data
    matrix = [[0 for i in range(datalen)] for i in range(datalen)]
    kdis = [0 for i in range(datalen)]
    for i in range(datalen-1):
        for j in range(i+1,datalen):
            dis = 0
            for l in range(len(data[0])):
                #print("i:",i," j:",j," k:",k)
                dis = dis + (data[i][l]-data[j][l])*(data[i][l]-data[j][l])
            dis = math.sqrt(dis)
            matrix[i][j] = dis
            matrix[j][i] = dis
    #print matrix
    for i in range(datalen):
        pq = Queue.PriorityQueue(k)
        kdisfirst = 1
        for j in range(datalen):
            if i!=j:
                if pq.full() == False:
                    pq.put(matrix[i][j]*-1)
                else:
                    kdis[i] = pq.get()
                    if matrix[i][j]*-1>kdis[i]:
                        pq.put(matrix[i][j]*-1)
                    else:
                        pq.put(kdis[i])
        kdis[i] = pq.get()*-1

    '''
                    if kdisfirst == 1:
                        kdis[i] = pq.get()
                        pq.put(matrix[i][j])
                        kdisfirst = 0
                    else:
                        if matrix[i][j]<kdis[i]:
                            kdis[i] = pq.get()
                            pq.put(matrix[i][j])
                        else:
                            pass
    '''
    print(kdis)
    return kids

                        






	