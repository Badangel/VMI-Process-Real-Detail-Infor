import csv
def spiltvs_train_test():
    vslist = []
    listfile = 'C:\\Users\\1\\Desktop\\云计算安全\\学位论文\\数据\\新数据\\2778vs\\trainvsset.txt'
    vslistfile = open(listfile,'r')
    vslistname = vslistfile.readlines()
    for a in vslistname:
        a1 = a[:-1]
        vslist.append(a1)
    vslistfile.close()
    csv_reader = csv.reader(open('sampleonlyvs.csv', encoding='utf-8'))
    out = open('trainvsset.csv', 'w', newline='')
    csv_writer = csv.writer(out, dialect='excel')
    for row in csv_reader:
        if row[3] in vslist:
            print(row)
            csv_writer.writerow(row)
    print('vs sp over!')

def spiltbin_train_test():
    vslist = []
    listfile = 'C:\\Users\\1\\Desktop\\云计算安全\\学位论文\\数据\\新数据\\2778vs\\testbinset.txt'
    vslistfile = open(listfile,'r')
    vslistname = vslistfile.readlines()
    for a in vslistname:
        a1 = a[:-1]
        vslist.append(a1)
    vslistfile.close()
    csv_reader = csv.reader(open('all3.csv', encoding='utf-8'))
    out = open('testbinset.csv', 'w', newline='')
    csv_writer = csv.writer(out, dialect='excel')
    for row in csv_reader:
        if row[3] in vslist:
            print(row)
            csv_writer.writerow(row)
    print('bin sp over!')

def find_other_bin():
    testbinlist = []
    listfile = 'C:\\Users\\1\\Desktop\\云计算安全\\学位论文\\数据\\新数据\\2778vs\\testbinset.txt'
    vslistfile = open(listfile,'r')
    vslistname = vslistfile.readlines()
    for a in vslistname:
        a1 = a[:-1]
        testbinlist.append(a1)
    vslistfile.close()
    trainbinlist = []
    listtrainfile = 'C:\\Users\\1\\Desktop\\云计算安全\\学位论文\\数据\\新数据\\2778vs\\trainbinset.txt'
    trainbinlistfile = open(listtrainfile,'r')
    trainlistname = trainbinlistfile.readlines()
    for a in trainlistname:
        a1 = a[:-1]
        trainbinlist.append(a1)
    trainbinlistfile.close()
    csv_reader = csv.reader(open('C:\\Users\\1\\Desktop\\云计算安全\\学位论文\\数据\\新数据\\all3.csv', encoding='utf-8'))
    out = open('otherlist', 'w')
    csv_writer = csv.writer(out, dialect='excel')
    colbin=set()
    for row in csv_reader:
        colbin.add(row[3])
    colbinlist = list(colbin)
    
    for l in colbinlist:
        if l in testbinlist or l in trainbinlist:
            print(l+'find!')
        else:
            out.write(l+'\n')
    print('find other over!')

def get_bin_name():
    bimlist = []
    listfile = 'C:\\Users\\1\\Desktop\\云计算安全\\学位论文\\数据\\新数据\\vs提交检测\\samplebinlist'
    binlistfile = open(listfile,'r')
    binlistname = binlistfile.readlines()
    for a in binlistname:
        a1 = a[:-1]
        bimlist.append(a1)
    binlistfile.close()
    csv_reader = csv.reader(open('C:\\Users\\1\\Desktop\\云计算安全\\学位论文\\数据\\新数据\\all3.csv', encoding='utf-8'))
    out = open('binlist', 'w')
    csv_writer = csv.writer(out, dialect='excel')
    colbin=set()
    for row in csv_reader:
        colbin.add(row[3])
    colbinlist = list(colbin)
    for vsname in bimlist:
        if vsname in colbinlist:
            out.write(vsname+'\n')
    print('find bin over!')
if __name__=='__main__':
    find_other_bin()