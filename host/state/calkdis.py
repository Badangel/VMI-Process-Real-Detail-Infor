import DBHelper

db = DBHelper.DBHelper()
sql = "select * from state"
data = db.oncesql(sql)
datalen = len(data) 
print ("len: ",datalen)
print data
matrix = [[0 for i in range(datalen)] for i in range(datalen)]
	