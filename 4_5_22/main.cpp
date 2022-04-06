#include <iostream>
#include <string>
#include "sqlite3.h"

using namespace std;

int callback(void*, int, char**, char**);
void viewAssignmentsByProject(sqlite3*);

int main() 
{
    int rc;
    sqlite3 * db;
    string query;
    char * err;
    int choice;
    rc = sqlite3_open_v2("ConstructCo.db", &db, SQLITE_OPEN_READWRITE, NULL);
    if(rc != SQLITE_OK)
    {
        cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return -1;
    }
    else 
    {
        cout << "Database Opened Successfully" << endl;
    }
    
    query = "select * from employee";
    rc = sqlite3_exec(db, query.c_str(), callback, NULL, &err);
    if(rc != SQLITE_OK)
    {
        cout << "There was an error - select callback: " << err << endl;
        sqlite3_free(err);
    }

    string lname, fname, hiredate, mi;
    int job, years;
    lname = "Brown";
    fname = "Charlie";
    hiredate = "2022-04-05";
    job = 508;
    years = 0;

    query = "insert into employee (emp_lname, emp_fname, emp_hiredate, job_code, emp_years) ";
    query += "values ('";
    query += lname + "', '";
    query += fname + "', '";
    query += hiredate + "', ";
    query += to_string(job) + ", ";
    query += to_string(years) + ");";
    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if(rc != SQLITE_OK)
    {
        cout << "There was an error on insert: " << err << endl;
        cout << query << endl;
        sqlite3_free(err);
    }
    else 
    {
        int emp_num = sqlite3_last_insert_rowid(db);
        cout << fname << " " << lname << " inserted into the database as employee number " << emp_num << endl;
    }

    cout << "Would you like to see assignments by:" << endl;
    cout << "1. Project" << endl;
    cout << "2. Employee" << endl;
    cin >> choice;
    while(!cin || choice != 1)
    {
        if(!cin)
        {
            cin.clear();
            cin.ignore(1000, '\n');
        }
        cout << "Enter a valid number from the menu." << endl;
        cout << "Would you like to see assignments by:" << endl;
        cout << "1. Project" << endl;
        cout << "2. Employee" << endl;
        cin >> choice;
    }
    viewAssignmentsByProject(db);

    sqlite3_close(db);
    return 0;
}


int callback(void* data, int numCols, char** values, char** columnNames)
{
    for(int i = 0; i < numCols; i++)
    {
        cout << columnNames[i] << ": ";
        if(values[i] != NULL)
            cout << values[i];

        cout << endl;
    }
    cout << endl;
    return SQLITE_OK;
}

void viewAssignmentsByProject(sqlite3 * db)
{
    string query;
    sqlite3_stmt *result;
    string strLastError;

    query = "select proj_num, proj_name from project";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &result, NULL);
    if(rc != SQLITE_OK)
    {
        strLastError = sqlite3_errmsg(db);
        sqlite3_finalize(result);
        cout << "There was an error: " << strLastError << endl;
        return;
    }
    cout << "Please choose the project you want to see the assignments for:" << endl;
    int columnCount = sqlite3_column_count(result);
    int i = 1, choice;
    cout << left;
    rc = sqlite3_step(result);
    while(rc == SQLITE_ROW)
    {
        cout << i << ". " << sqlite3_column_text(result, 0);
        cout << " - " << sqlite3_column_text(result, 1);
        cout << endl;
        i++;
        rc = sqlite3_step(result); 
    }
    cin >> choice;
    while(!cin || choice < 1 || choice > i)
    {
        if(!cin)
        {
            cin.clear();
            cin.ignore(1000, '\n');
        }
        cout << "That is not a valid choice! Try again!" << endl;
        cin >> choice;
    }
    sqlite3_reset(result);
    for(int j = 0; j < choice; j++)
    {
        sqlite3_step(result);
    }
    //cout << "here before cast" << endl;
    string proj_num = reinterpret_cast<const char*>(sqlite3_column_text(result, 0));
    //cout << "here" << endl;
    sqlite3_finalize(result);
    //select sum(assign_hours) 'Total Hours', sum(assign_charge) 'Total Charges' from assignment where proj_num = group by proj_num
    query = "select sum(assign_hours) 'Total Hours', sum(assign_charge) 'Total Charges' ";
    query += "from assignment ";
    query += "where proj_num = " + proj_num;
    query += " group by proj_num";
    //cout << query << endl << endl;
    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &result, NULL);
    if(rc != SQLITE_OK)
    {
        strLastError = sqlite3_errmsg(db);
        sqlite3_finalize(result);
        cout << "There was an error: " << strLastError << endl;
        return;
    }
    columnCount = sqlite3_column_count(result);

    rc = sqlite3_step(result);
    while(rc == SQLITE_ROW)
    {
       for(int i = 0; i < columnCount; i++)
       {
           cout << sqlite3_column_name(result, i) << " - ";
           if(sqlite3_column_type(result, i) != SQLITE_NULL)
           {
               cout << sqlite3_column_text(result, i);
           }
           cout << endl;
       } 
       cout << endl;
       rc = sqlite3_step(result);
    }
    sqlite3_finalize(result);
}

void viewJob(sqlite3 * db)
{
	string query = "select job_code as 'Job Code', job_description as 'Description', job_chg_hour as 'Charge per Hour', job_last_update as 'Last Updated' ";
	query += "from job order by job_code";
	
	sqlite3_stmt *result;
	string strLastError;
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &result, NULL);
	if(rc != SQLITE_OK)
	{
		strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(result);
		cout << "There was an error: " << strLastError << endl;
		return;
	}
	else
	{
		cout << "Please choose the job you want to see: " << endl;
		int columnCount = sqlite3_column_count(result);
		int i = 1, choice;
		cout << left;
		rc = sqlite3_step(result);
		while(rc == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(result, 0);
			cout << " - " << sqlite3_column_text(result, 1);
			cout << endl;
			i++;
			rc = sqlite3_step(result);
		}
		cin >> choice;
		
		while(!cin || choice < 1 || choice > i)
		{
			if(!cin)
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cout << "That is not a valid choice! Try again!" << endl;
			cin >> choice;
		}
		sqlite3_reset(result);
		for(int j = 0; j < choice; j++)
			sqlite3_step(result);
		for(int j = 0; j < columnCount; j++)
		{
			cout << sqlite3_column_name(result, j) << ": ";
			if(sqlite3_column_text(result, j) != NULL)
				cout << sqlite3_column_text(result, j);
			cout << endl;
		}
		sqlite3_finalize(result);
	}
	
	
	
}