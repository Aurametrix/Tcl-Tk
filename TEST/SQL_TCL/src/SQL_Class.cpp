/*
* TCLSQL_Class.cpp, v1.0 2/22/2003, Scott J. Nichols
*
* This is the source code of an C++ implementation of
* the Microsoft ODBC 3.0 API
*
* This software is provided "AS IS", without a warranty of any kind.
* You are free to use/modify this code but leave this header intact.
*
*/

#include "SQL_Class.h"

// SQL Constructor
TCLSQL_CLASS::TCLSQL_CLASS()
{
	// Initialize handles to NULL
	henv = NULL;
	hdbc = NULL;
	hstmt = NULL;

}

// SQL Destructor
TCLSQL_CLASS::~TCLSQL_CLASS()
{

	SQLRETURN   retcode;

	if (hstmt != NULL)
		retcode = SQLFreeHandle(SQL_HANDLE_STMT,hstmt);

	retcode = SQLDisconnect(hdbc);
	retcode = SQLFreeHandle(SQL_HANDLE_DBC,hdbc);
	retcode = SQLFreeHandle(SQL_HANDLE_ENV,henv);

}

/* Sets ODBC Environment Handle */
TCLSQL_CLASS::SetEnvHandle(SQLHENV henv)
{
	this->henv = henv;

}

/* Sets ODBC Connection Handle */
TCLSQL_CLASS::SetCnnctHandle(SQLHDBC hdbc)
{
	this->hdbc = hdbc;
}

/* Sets ODBC Statement Handle */
TCLSQL_CLASS::SetStmtHandle(SQLHSTMT hstmt)
{
	this->hstmt = hstmt;

}

/* Returns ODBC Environment Handle */
SQLHENV TCLSQL_CLASS::GetEnvHandle()
{
	return henv;

}

/* Returns ODBC Connection Handle */
SQLHDBC TCLSQL_CLASS::GetCnnctHandle()
{
	return hdbc;
}

/* Returns ODBC Statement Handle */
SQLHSTMT TCLSQL_CLASS::GetStmtHandle()
{

	return hstmt;
}

/* Function used to Free Statement Handle */
SQLRETURN TCLSQL_CLASS::FreeStatementHandle()
{
	SQLRETURN   retcode;
	retcode = SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
	hstmt = NULL;

	return retcode;

}

/* Function used to close cursor. */
SQLRETURN TCLSQL_CLASS::CloseCursor()
{
	SQLRETURN   retcode;
	retcode = SQLCloseCursor(hstmt);

	return retcode;

}

/* Function used to cancel asynchronous request. */
SQLRETURN TCLSQL_CLASS::CancelStatement()
{
	SQLRETURN   retcode;
	retcode = SQLCancel(hstmt);

	return retcode;

}

/* Function: GetRowValues - Used to get row values */
SQLRETURN TCLSQL_CLASS::GetRowValues(Tcl_Obj *obj_row,Tcl_Interp *interp)
{
	errmsg[0] = '\0';
	Tcl_Obj *obj_col;
	SQLSMALLINT i=0;

	SQLRETURN retcode = SQLFetch(hstmt);

	for ( i = 0; i < nresultcols; i++ )
	{
		/* Check for NULL data */
		if ( outlen[i] == SQL_NULL_DATA )
		{
			obj_col = Tcl_NewStringObj((const char *)"NULL",-1);
			Tcl_ListObjAppendElement(interp,obj_row,obj_col);
		}
		else
		{ /* Build a truncation message for any columns truncated */
		   if ( outlen[i] >= collen[i] )
		   {
			  sprintf( ( char * ) errmsg + strlen( ( char * ) errmsg ),
					   "%d chars truncated, col %d\n",
					   ( int ) outlen[i] - collen[i] + 1,
					   i + 1
					 ) ;
		   }
		   if ( sData[i] != NULL )
				obj_col = Tcl_NewStringObj((const char *)sData[i],-1);
		   else
				obj_col = Tcl_NewStringObj((const char *)"",-1);
		   Tcl_ListObjAppendElement(interp,obj_row,obj_col);
		}
	} /* for all columns in this row  */

	return retcode;

}

/* Function: GetColumnHeaders - Used to get the SQL column headers (A.K.A meta data) */
SQLRETURN TCLSQL_CLASS::GetColumnHeaders(Tcl_Obj *obj_row,Tcl_Interp *interp)
{
	SQLRETURN retcode =0;
    Tcl_Obj	*obj_col;
	SQLSMALLINT i = 0;

	// Lets get the number of columns if any were returned */
	retcode = SQLNumResultCols(hstmt, &nresultcols );

	for ( i = 0; i < nresultcols; i++ )
	{
		retcode = SQLDescribeCol(hstmt,
						( SQLSMALLINT ) ( i + 1 ),
						colname,
						sizeof(colname),
						&colnamelen,
						&coltype,
						&collen[i],
						&scale,
						NULL
					  ) ;
		/* get display length for column */
		retcode = SQLColAttribute(hstmt,
						 ( SQLSMALLINT ) ( i + 1 ),
						 SQL_DESC_DISPLAY_SIZE,
						 NULL,
						 0,
						 NULL,
						 &displaysize
					   ) ;

		/*
		 Set column length to max of display length,
		 and column name length. Plus one byte for
		 null terminator.
		*/
		collen[i] = max( displaysize,
						 strlen( ( char * ) colname )
					   ) + 1 ;

		/* allocate memory to bind the column */
		sData[i] = ( SQLCHAR * ) malloc( ( int ) collen[i] ) ;

		/* bind columns to program vars, converting all types to CHAR */
		retcode = SQLBindCol(hstmt,
					( SQLSMALLINT ) ( i + 1 ),
					SQL_C_CHAR,
					sData[i],
					collen[i],
					&outlen[i]
				  );

		obj_col = Tcl_NewStringObj((const char *)colname,strlen((const char *)colname));
		Tcl_ListObjAppendElement(interp,obj_row,obj_col);
	}
	return retcode;
}