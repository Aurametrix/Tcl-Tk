/*
* TCLSQL_Class.h, v1.0 2/22/2003, Scott J. Nichols
*
* This is the source code header file of an C++ implementation of
* the Microsoft ODBC 3.0 API
*
* This software is provided "AS IS", without a warranty of any kind.
* You are free to use/modify this code but leave this header intact.
*/

#define USE_NON_CONST
#define TCL_USE_STUBS
#define TCL_THREADS
#include <string.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <odbcss.h>
#include <tcl.h>
#include "SQL_TCL.h"
#include <iostream>

#ifndef TCLSQL_CLASS_H
#define TCLSQL_CLASS_H

class TCLSQL_CLASS {

public:

	// Constructor
	TCLSQL_CLASS();

	// Destructor
	~TCLSQL_CLASS();

	// Give Function access to private and protected variables.
	friend static int	SQL_ObjCmd _ANSI_ARGS_((ClientData data,
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));

	// Functions used to set ODBC handles
	SetEnvHandle(SQLHENV);
	SetCnnctHandle(SQLHDBC);
	SetStmtHandle(SQLHSTMT);

	// Functions used to get ODBC handles
	SQLHENV  GetEnvHandle();
	SQLHDBC  GetCnnctHandle();
	SQLHSTMT GetStmtHandle();

	// Function used to free statement handle.
	SQLRETURN FreeStatementHandle();

	// Function used to close cursor.
	SQLRETURN CloseCursor();

	// Function used to cancel asyncronous request.
	SQLRETURN CancelStatement();

	// Function used to get retrieve one row of the result set.
	SQLRETURN GetRowValues(Tcl_Obj *obj_row,Tcl_Interp *interp);

	// Function used to get the column headers of a result set.
	SQLRETURN GetColumnHeaders(Tcl_Obj *obj_row,Tcl_Interp *interp);

private:

	// Declare ODBC handles and variables.
	SQLHENV     henv;
	SQLHDBC     hdbc;
	SQLHSTMT    hstmt;
	SQLCHAR     colname[MAX_COL_SIZE];
    SQLSMALLINT coltype;
    SQLSMALLINT colnamelen;
    SQLUINTEGER collen[MAXCOLS];
    SQLSMALLINT scale;
    SQLINTEGER  outlen[MAXCOLS];
	SQLCHAR     *sData[MAXCOLS];
    SQLCHAR     errmsg[MAX_ERROR_SIZE];
    SQLSMALLINT nresultcols;
    SQLINTEGER  displaysize;

};

#endif