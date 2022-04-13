#include <iostream>
#include <string>
#include "sqlite3.h"

using namespace std;

void checkoutBook(sqlite3 * db);
void returnBook(sqlite3 * db);
void rollback(sqlite3* db);
void commit(sqlite3* db);

int main() 
{
    int rc;
    sqlite3 * db;
    string query;
    char * err;
    int choice;
    rc = sqlite3_open_v2("factdb.db", &db, SQLITE_OPEN_READWRITE, NULL);
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
    
    

    cout << "What would you like to do:" << endl;
    cout << "1. Checkout Book" << endl;
    cout << "2. Return Book" << endl;
    cin >> choice;
    while(!cin || choice < 1 || choice > 2)
    {
        if(!cin)
        {
            cin.clear();
            cin.ignore(1000, '\n');
        }
        cout << "Enter a valid number from the menu." << endl;
        cout << "What would you like to do:" << endl;
        cout << "1. Checkout Book" << endl;
        cout << "2. Return Book" << endl;
        cin >> choice;
    }
    if(choice == 1)
    {
        checkoutBook(db);
    } 
    else 
    {
        returnBook(db);
    }

    sqlite3_close(db);
    return 0;
}



void checkoutBook(sqlite3 * db)
{
    //get patron for checkout
    //get book for checkout
    //create checkout record for the patron and book
    //change book to match the patron
    string query;
    sqlite3_stmt *result;
    string strLastError;
    int pat_id, bookNum, pat, book, i = 1;
    query = "begin transaction;";
    char * err;
    int rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if(rc != SQLITE_OK)
    {
        cout << "Error starting transaction: " << err << endl;
        sqlite3_free(err);
        return;
    }
    query = "select pat_id, pat_fname || ' ' || pat_lname as name from patron;";
    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &result, NULL);
    if(rc != SQLITE_OK)
    {
        strLastError = sqlite3_errmsg(db);
        sqlite3_finalize(result);
        cout << "There was an error selecting patrons: " << strLastError << endl;
        rollback(db);
        return;
    }
    cout << "Please choose the patron doing the book checkout: " << endl;
    int columnCount = sqlite3_column_count(result);

    rc = sqlite3_step(result);
    while(rc == SQLITE_ROW)
    {
        cout << i << ". " << sqlite3_column_text(result, 0);
        cout << " - " << sqlite3_column_text(result, 1);
        cout << endl;
        i++;
        rc= sqlite3_step(result);
    }
    cin >> pat;
    while(!cin || pat < 1 || pat > i)
	{
		if(!cin)
		{
			cin.clear();
			cin.ignore(1000, '\n');
		}
		cout << "That is not a valid patron! Try again!" << endl;
		cin >> pat;
	}

	sqlite3_reset(result);

	for(int j = 0; j < pat; j++)
		sqlite3_step(result);
    
    pat_id =  sqlite3_column_int(result, 0);
    sqlite3_finalize(result);

    i = 1;
	query = "select book_num as 'Book Number', book_title as 'Title' from book where pat_id is null;";
    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &result, NULL);
	if(rc != SQLITE_OK)
	{
		strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(result);
		cout << "There was an error: " << strLastError << endl;
		rollback(db);
		return;
	}
    cout << "Please choose the book you want to checkout: " << endl;
	columnCount = sqlite3_column_count(result);
	rc = sqlite3_step(result);
	while(rc == SQLITE_ROW)
	{
		cout << i << ". " << sqlite3_column_text(result, 0);
		cout << " - " << sqlite3_column_text(result, 1);
		cout << endl;
		i++;
		rc = sqlite3_step(result);
	}
	cin >> book;
		
	while(!cin || book < 1 || book > i)
	{
		if(!cin)
		{
			cin.clear();
			cin.ignore(1000, '\n');
		}
		cout << "That is not a valid book! Try again!" << endl;
		cin >> book;
	}

	sqlite3_reset(result);

	for(int j = 0; j < book; j++)
		sqlite3_step(result);
        
	bookNum = sqlite3_column_int(result, 0);
	sqlite3_finalize(result);

    query = "Insert into checkout(book_num, pat_id, check_out_date, check_due_date) values (";
	query += to_string(bookNum) + ", ";
	query += to_string(pat_id) + ", ";
	query += "date('now'), date('now', '+7 day') )";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

	if(rc != SQLITE_OK)
	{
		cout << "Error inserting checkout: " << err << endl;
		sqlite3_free(err);
		rollback(db);
		return;
	}
	cout << "Checkout record inserted." << sqlite3_last_insert_rowid(db) << endl;

    query = "update book set pat_id = ";
	query += to_string(pat_id);
	query += " where book_num = ";
	query += to_string(bookNum);

	rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
	if(rc != SQLITE_OK)
	{
		cout << "Error updating book: " << err << endl;
		sqlite3_free(err);
		rollback(db);
		return;
	}
    commit(db);
    
}

void returnBook(sqlite3 * db)
{
	string query;
    sqlite3_stmt *result;
    string strLastError;
	
	
	
}

void rollback(sqlite3* db)
{
    string query = "rollback;";
	sqlite3_exec(db, query.c_str(), NULL, NULL, NULL);
}

void commit(sqlite3* db)
{
    string query = "commit;";
	sqlite3_exec(db, query.c_str(), NULL, NULL, NULL);
}