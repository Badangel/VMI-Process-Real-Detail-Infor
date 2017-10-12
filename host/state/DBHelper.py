import MySQLdb

class DBHelper:

    conn = None
    def __init__(self):
        if self.conn is None:
            self.conn = MySQLdb.connect("Localhost","root","309911","vmidata")

    def __del__(self):
        if not self.conn is None:
            self.conn.close()
            self.conn = None

    #excute an sql, return 0 for success, -1 for failed
    def dosql(self, sql):
        cursor = self.conn.cursor()
        try:
            cursor.execute(sql)
        except:
            self.conn.rollback()
        finally:
            self.conn.commit()

    #query ,return data list
    def select(self, sql):
        cursor = self.conn.cursor()
        cursor.execute(sql)
        data = cursor.fetchall()
        return data

    #do query or excute once
    def oncesql(self,sql):
        data = None
        if 'select' in sql or 'SELECT' in sql:
            data = self.select(sql)
        else:
            data = self.dosql(sql)
        return data
