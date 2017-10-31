/**
* Copyright(c) 2017 dyf
* use my own sql interface, package the funtion
*
*/


#ifndef MYDBSQL_H_INCLUDED
#define MYDBSQL_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>



void init_db(MYSQL *mysql)
{
    mysql_init(mysql);
}
int conn_db(MYSQL *mysql,const char *hostname, const char *username, const char *password, const char *dbname)
{
    if (mysql != NULL)
        mysql_close(mysql);
    mysql = mysql_real_connect(mysql, hostname, username, password,
                                    dbname, 0, NULL, 0);///last three 0,0,0

    if (mysql == NULL)
    {
        printf("%s\n", mysql_error(mysql));
        return -1;
    }
    mysql_autocommit(mysql, 0);///close autocommit

    printf("success connect to mysql\n");
    return 0;

}
void disconn_db(MYSQL *mysql)
{
    if (mysql)
    {
        mysql_close(mysql);
        mysql = NULL;
    }
}
int open_db(MYSQL *mysql,const char *SQL)
{
    int state = mysql_query(mysql, SQL);
    if (state != 0)
    {
        printf("%s\n", mysql_error(mysql));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(mysql);
    if (result == (MYSQL_RES *) NULL)
    {
        printf("%s\n", mysql_error(mysql));
        return -1;
    }
    else
    {
        MYSQL_FIELD *sqlField;
        int iFieldCount = 0;
        while (1)
        {
            sqlField = mysql_fetch_field(result);
            if (sqlField == NULL)
                break;
            printf("%s\t", sqlField->name);
            iFieldCount++;
        }
        printf("\n");

        MYSQL_ROW sqlRow;
        while (1)
        {
            sqlRow = mysql_fetch_row(result);
            if (sqlRow == NULL)
                break;
            int i;
            for (i = 0; i < iFieldCount; i++)
            {
                if (sqlRow[i] == NULL)
                    printf("NULL\t");
                else
                    printf("%s\t", (const char *)sqlRow[i]);
            }
            printf("\n");
        }
        printf("query is ok, %u rows affected\n", (unsigned int)mysql_affected_rows(mysql));
        mysql_free_result(result);
    }
    return 0;
}
int exec_db(MYSQL *mysql,const char *SQL)
{
    int state = mysql_query(mysql, SQL);
    if (state != 0)
    {
        printf("%s\n", mysql_error(mysql));
        return -1;
    }
    //printf("query is ok, %u rows affected\n", (unsigned int)mysql_affected_rows(connection));
    return 0;
}

void comm_db(MYSQL *mysql)
{
    if(mysql_commit(mysql) != 0){
        mysql_rollback(mysql);
        printf("commit error, roolback!\n");
    }
    else{
        printf("commit right!\n");
    }

}

#endif // MYDBSQL_H_INCLUDED
