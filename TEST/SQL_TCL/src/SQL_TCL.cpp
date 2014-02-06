/*
* SQL_TCL.cpp, v1.2 7/09/2004, Scott J. Nichols
* snichols1@sbcglobal.net
*
* This is the source code for a TCL 8.3 library extension (DLL)
* This TCL extension is used for with the Microsoft ODBC 3.0 API.
*
* This software is provided "AS IS", without a warranty of any kind.
* You are free to use/modify this code but leave this header intact.
*
*/

#include "SQL_CLASS.h"
#include "SQL_TCL.h"

TCL_DECLARE_MUTEX(myMutex)

// using std::cout;

/*
 * Creates a new Tcl command and returns a database handle.
 * to the TCL Intepreter.
 */
static int	SQLAllocHandle_ObjCmd _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]))
{
	int index;
	static iHandles=0;
	SQLHENV     henv;
	SQLHDBC     hdbc;
	SQLHSTMT    hstmt;

	SQLRETURN   retcode;

	Tcl_MutexLock(&myMutex);

	/* Valid options */
    static char *options[] = {
		"SQLConnect",
		"SQLDriverConnect",
		NULL
	};

    enum options {
		SQLCONNECT,
		SQLDRIVERCONNECT
    };

	/* Parse the command (Note: This techique supports Tcl namespace improrts and Tcl command renaming) */
    if (Tcl_GetIndexFromObj(interp,
		                    Tcl_NewStringObj( (const char *)clientData,
		                    strlen((const char *)clientData)),
							options,
							"option",
							0,
							&index) != TCL_OK)
			return TCL_ERROR;


	/* Create a environment handle */
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

    /* Set the ODBC version environment attribute to ODBC 3*/
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

	/* Allocate connection handle */
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
   		retcode = SQLAllocHandle(SQL_HANDLE_DBC,henv, &hdbc);

    /* Switch on the sub-command */
	switch ((enum options) index)
    {

		case SQLCONNECT:
		{

			if (objc < 4)
			{
				Tcl_WrongNumArgs(interp,1,objv,
							   "ODBC_System_DSN UID PASSWORD [timeout]");
				return TCL_ERROR;
			}

			/* Set the connection timeout */
			if (objc > 4)
			{
				int timeout;
				int iError = Tcl_GetIntFromObj(interp, objv[4], &timeout);

				if (iError != TCL_OK)
					return iError;

				/* Set login and connection timeout */
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					retcode = SQLSetConnectAttr(hdbc,SQL_ATTR_CONNECTION_TIMEOUT,(void *)timeout, 0);
					retcode = SQLSetConnectAttr(hdbc,SQL_ATTR_LOGIN_TIMEOUT,(void *)timeout, 0);
				}

			}

			/* Connect to data source */
			retcode = SQLConnect(hdbc, (SQLCHAR*) Tcl_GetStringFromObj(objv[1], (int *) NULL), SQL_NTS,
						  (SQLCHAR*) Tcl_GetStringFromObj(objv[2], (int *) NULL), SQL_NTS,
						  (SQLCHAR*) Tcl_GetStringFromObj(objv[3], (int *) NULL), SQL_NTS);

			break;
		}
		case SQLDRIVERCONNECT:
		{

			if (objc < 2)
			{
				Tcl_WrongNumArgs(interp,1,objv,
							   "ConnectionString [timeout]");
				return TCL_ERROR;
			}

			/* Set the connection timeout */
			if (objc > 2)
			{
				int timeout;
				int iError = Tcl_GetIntFromObj(interp, objv[2], &timeout);

				if (iError != TCL_OK)
					return iError;

				/* Set login and connection timeout */
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					SQLSetConnectAttr(hdbc,SQL_ATTR_CONNECTION_TIMEOUT,(void *)timeout, 0);
					SQLSetConnectAttr(hdbc,SQL_ATTR_LOGIN_TIMEOUT,(void *)timeout, 0);
				}
			}

			SQLCHAR       *ConnStrOut;
			ConnStrOut = NULL;
			SQLSMALLINT   ConnStrOutLen;
			/* Connect to data source */
			retcode = SQLDriverConnect(hdbc, NULL, (unsigned char *)Tcl_GetStringFromObj(objv[1], (int *) NULL), SQL_NTS, ConnStrOut,
                                       sizeof(ConnStrOut), &ConnStrOutLen, SQL_DRIVER_NOPROMPT);
			break;
		}

	}

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
    {

		/* Create an ODBC statement handle */
		retcode = SQLAllocHandle(SQL_HANDLE_STMT,hdbc, &hstmt);

		// Create a new SQL_Class object and set handles within object.
		TCLSQL_CLASS *objSQL = new TCLSQL_CLASS;
		objSQL->SetEnvHandle(henv);
		objSQL->SetCnnctHandle(hdbc);
		objSQL->SetStmtHandle(hstmt);

		/* Increment the Handle Counter */
		iHandles++;

		Tcl_Obj* theHandle = Tcl_NewStringObj( (const char *)"::SQL::sql",strlen((const char *)"::SQL::sql"));
		Tcl_Obj* theNumber = Tcl_NewIntObj( iHandles );
		Tcl_IncrRefCount( theNumber );
		Tcl_AppendObjToObj( theHandle, theNumber );
		Tcl_DecrRefCount( theNumber );

		/* Create a new TCL command */
		Tcl_CreateObjCommand( interp, Tcl_GetString(theHandle), SQL_ObjCmd,
			(ClientData) objSQL, (Tcl_CmdDeleteProc *) NULL);

		Tcl_SetObjResult(interp,theHandle);
		Tcl_MutexUnlock(&myMutex);
		return TCL_OK;
	}
	else
	{
		SQLCHAR pError[6];
		SQLCHAR pMessage[256];
		SQLINTEGER Native;
		SQLSMALLINT iTxtLength;

		retcode = SQLGetDiagRec(
				  SQL_HANDLE_DBC,
				  hdbc,
				  1,
				  pError,
				  &Native,
                  pMessage,
				  sizeof(pMessage),
				  &iTxtLength);

		Tcl_Obj		*obj_result = Tcl_NewStringObj((char *)pMessage,strlen((const char *)pMessage));
		Tcl_SetObjResult(interp,obj_result);
		Tcl_MutexUnlock(&myMutex);
		return TCL_ERROR;
	}
}


/* Dynamic LDAP Handle Command*/
static int	SQL_ObjCmd _ANSI_ARGS_((ClientData data,
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]))
{

	Tcl_MutexLock(&myMutex);

	/* Get the object stored in memory */
	TCLSQL_CLASS *objSQL = (TCLSQL_CLASS *)data;

	SQLRETURN   retcode=0;
	SQLSMALLINT i = 0;

    char * sReturn = NULL;
	int index=0;

	// Declare TCL objects
	Tcl_Obj		*obj_result = Tcl_NewListObj(0,NULL);
	Tcl_Obj		*obj_row = Tcl_NewListObj(0,NULL);

	/* Valid Tcl options */
    static char *options[] = {
		"SQLExecDirect",
		"SQLFetch",
		"SQLMoreResults",
		"SQLDisconnect",
		"SQLSetStmtAttr",
		"SQLFreeStmt",
		"SQLFetchAll",
		"SQLGetDiagRec",
		"SQLCancel",
		"SQLCloseCursor",
		"SQLRowCount",
		"SQLPrimaryKeys",
		"SQLTables",
		"SQLForeignKeys",
		"SQLStatistics",
		"SQLTablePrivileges",
		"SQLProcedures",
		"SQLSpecialColumns",
		"SQLDataSources",
		NULL
	};

    enum options {
		SQLEXECDIRECT,
		SQLFETCH,
		SQLMORERESULTS,
		SQLDISCONNECT,
		SQLSETSTMTATTR,
		SQLFREESTMT,
		SQLFETCHALL,
		SQLGETDIAGREC,
		SQLCANCEL,
		SQLCLOSECURSOR,
		SQLROWCOUNT,
		SQLPRIMARYKEYS,
		SQLTABLES,
		SQLFOREIGNKEYS,
		SQLSTATISTICS,
		SQLTABLEPRIVILEGES,
		SQLPROCEDURES,
		SQLSPECIALCOLUMNS,
		SQLDATASOURCES
    };

	/* Parse the command */
    if (Tcl_GetIndexFromObj(interp, objv[1],options, "option", 0,
	    &index) != TCL_OK) {
		Tcl_MutexUnlock(&myMutex);
		return TCL_ERROR;
    }

    /* Switch on the sub-command */
	switch ((enum options) index)
    {

		case SQLEXECDIRECT:
		{

			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "SQL_QUERY");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			/* Create a fresh statement handle if needed */
			if (objSQL->hstmt == NULL)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc, &objSQL->hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				retcode = SQLExecDirect(objSQL->hstmt,
										(SQLCHAR *) Tcl_GetStringFromObj(objv[2], (int *) NULL),
			                            SQL_NTS);

			if ((retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) && objc > 3)
			{

				/* Get the column headers */
				retcode = objSQL->GetColumnHeaders(obj_row,interp);

				/* Set the TCL return variable */
				Tcl_ObjSetVar2(interp,objv[3],NULL,obj_row,TCL_LEAVE_ERR_MSG);

				/* free data buffers */
				for ( i = 0; i < objSQL->nresultcols; i++ )
					free( objSQL->sData[i] );
			}
			break;
		}
		case SQLFETCH:
		{

			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "TCL_VARIABLE_that_stores_row_values [TCL_VARIABLE_that_stores_column_headers]");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			obj_row = Tcl_NewListObj(0,NULL);

			/* Get the column headers */
			retcode = objSQL->GetColumnHeaders(obj_row,interp);

			// printf("GetColumnHeaders: %i",retcode);

			/* Set the TCL return variable if there is one */
			if ((retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) && objc > 3)
				Tcl_ObjSetVar2(interp,objv[3],NULL,obj_row,TCL_LEAVE_ERR_MSG);

			obj_row = Tcl_NewListObj(0,NULL);

			/* Get the one row of data */
			retcode = objSQL->GetRowValues(obj_row,interp);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				Tcl_ObjSetVar2(interp,objv[2],NULL,obj_row,TCL_LEAVE_ERR_MSG);

			/* free data buffers */
			for ( i = 0; i < objSQL->nresultcols; i++ )
				free( objSQL->sData[i] ) ;

			break;
		}
		case SQLMORERESULTS:
		{
			/* This sets the SQL cursor to the next result if there is one. */
			retcode = SQLMoreResults(objSQL->hstmt);

	 		break;
		}
		case SQLDISCONNECT:
		{

			// Delete the C++ object which stores ODBC handles to free up memory.
			// This also disconnects all three ODBC handles from the DBMS in
			// the object's destructor.
			delete objSQL;

			/* Delete the TCL Command associated with the ODBC handles */
			Tcl_DeleteCommand(interp,Tcl_GetStringFromObj(objv[0], (int *) NULL));

			break;
		}
		case SQLSETSTMTATTR:
		{

			if (objc < 4)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "attribute value");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			/* Valid options */
			static char *options[] = {
				"SQL_ATTR_MAX_ROWS",
				"SQL_ATTR_QUERY_TIMEOUT",
				"SQL_ATTR_ASYNC_ENABLE",
				NULL
			};

			enum options {
				SQLATTRMAXROWS,
				SQLATTRQUERYTIMEOUT,
				SQLATTRASYNCENABLE
			};

			/* Parse the command */
			if (Tcl_GetIndexFromObj(interp, objv[2],options, "option", 0,
				&index) != TCL_OK)
			{
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			int iValue;
			int iError = Tcl_GetIntFromObj(interp, objv[3], &iValue);

			if (iError != TCL_OK)
			{
				Tcl_MutexUnlock(&myMutex);
				return iError;
			}

			/* Switch on the sub-command */
			switch ((enum options) index)
			{
				case SQLATTRMAXROWS:
				{
					retcode = SQLSetStmtAttr(objSQL->hstmt,
					                         SQL_ATTR_MAX_ROWS,
					                         (SQLPOINTER *) iValue,
					                         NULL);
					break;
				}
				case SQLATTRQUERYTIMEOUT:
				{
					retcode = SQLSetStmtAttr(objSQL->hstmt,
					                         SQL_ATTR_QUERY_TIMEOUT,
					                         (SQLPOINTER *) iValue,
					                         NULL);
					break;
				}
				case SQLATTRASYNCENABLE:
				{
					if (iValue >=1)
						retcode = SQLSetStmtAttr(objSQL->hstmt,
												 SQL_ATTR_ASYNC_ENABLE,
												 (void *)SQL_ASYNC_ENABLE_ON,
												 NULL);
					else
						retcode = SQLSetStmtAttr(objSQL->hstmt,
												 SQL_ATTR_ASYNC_ENABLE,
												 (void *)SQL_ASYNC_ENABLE_OFF,
												 NULL);
					break;
				}
			}
	 		break;
		}
		case SQLFREESTMT:
		{

			retcode = objSQL->FreeStatementHandle();
			if (retcode == 0)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc,&objSQL->hstmt);

			break;

		}
		case SQLFETCHALL:
		{
			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "TCL_VARIABLE_that_stores_row_values [TCL_VARIABLE_that_stores_column_headers]");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			/* Get the column headers */
			retcode = objSQL->GetColumnHeaders(obj_row,interp);

			/* Set the TCL return variable if there is one */
			if ((retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) && objc > 3)
				Tcl_ObjSetVar2(interp,objv[3],NULL,obj_row,TCL_LEAVE_ERR_MSG);

			int j=0;

			while ( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				/* Counter used determine if a nomatch has occurred */
				j++;

				obj_row = Tcl_NewListObj(0,NULL);

				/* Get the one row of data */
				retcode = objSQL->GetRowValues(obj_row,interp);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
					Tcl_ListObjAppendElement(interp,obj_result,obj_row);

			}

			/* free data buffers */
			for ( i = 0; i < objSQL->nresultcols; i++ )
				free( objSQL->sData[i] );

			/* Was there at least one row of data ? */
			if (j>=1)
				retcode = 0;

			Tcl_ObjSetVar2(interp,objv[2],NULL,obj_result,TCL_LEAVE_ERR_MSG);

			break;
		}
		case SQLGETDIAGREC:
		{
			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "TCLVariable [HandleType]");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			SQLCHAR pError[6];
			SQLCHAR pMessage[256];
			SQLINTEGER Native;
			SQLSMALLINT iTxtLength;

			/* Parse the TCL subcommand if there is one */
			if (objc > 3)
			{
				/* Valid options */
				static char *options[] = {
					"SQL_HANDLE_ENV",
					"SQL_HANDLE_DBC",
					"SQL_HANDLE_STMT",
					NULL
				};

				enum options {
					SQLHANDLEENV,
					SQLHANDLEDBC,
					SQLHANDLESTMT
				};

				if (Tcl_GetIndexFromObj(interp, objv[3],options, "option", 0,
					&index) != TCL_OK)
				{
					Tcl_MutexUnlock(&myMutex);
					return TCL_ERROR;
				}

				/* Switch on the sub-command */
				switch ((enum options) index)
				{
					case SQLHANDLEENV:
					{
						retcode = SQLGetDiagRec(
								  SQL_HANDLE_ENV,
								  objSQL->henv,
								  1,
								  pError,
								  &Native,
								  pMessage,
								  sizeof(pMessage),
								  &iTxtLength);
						break;
					}
					case SQLHANDLEDBC:
					{
						retcode = SQLGetDiagRec(
								  SQL_HANDLE_DBC,
								  objSQL->hdbc,
								  1,
								  pError,
								  &Native,
								  pMessage,
								  sizeof(pMessage),
								  &iTxtLength);
						break;
					}
					case SQLHANDLESTMT:
					{
						retcode = SQLGetDiagRec(
								  SQL_HANDLE_STMT,
								  objSQL->hstmt,
								  1,
								  pError,
								  &Native,
								  pMessage,
								  sizeof(pMessage),
								  &iTxtLength);
						break;
					}
				}
			}
			else
				retcode = SQLGetDiagRec(
						  SQL_HANDLE_STMT,
						  objSQL->hstmt,
						  1,
						  pError,
						  &Native,
						  pMessage,
						  sizeof(pMessage),
						  &iTxtLength);

			if ( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				obj_result = Tcl_NewStringObj((const char *)pMessage,strlen((const char *)pMessage));

				/* Set the error description into the passed in TCL variable  */
				Tcl_ObjSetVar2(interp,objv[2],NULL,obj_result,TCL_LEAVE_ERR_MSG);

			}

			break;
		}
		case SQLCANCEL:
		{

			retcode = objSQL->CancelStatement();

			break;

		}
		case SQLCLOSECURSOR:
		{

			retcode = objSQL->CloseCursor();

			break;

		}
		case SQLROWCOUNT:
		{

			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "TCLVariable");
				return TCL_ERROR;
			}

			SQLINTEGER iRowCount;

			/* Get the number of rows affected by an insert,
			   delete or update SQL statement */
			retcode = SQLRowCount(objSQL->hstmt,
				                  &iRowCount);

			/* Only set the return TCL variable if successful */
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				Tcl_Obj *pRowCount = Tcl_NewIntObj(iRowCount);

				Tcl_ObjSetVar2(interp,objv[2],NULL,pRowCount,TCL_LEAVE_ERR_MSG);

			}

			break;

		}
		case SQLPRIMARYKEYS:
		{

			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "SQL_Table");
				return TCL_ERROR;
			}

			/* Create a fresh statement handle if needed */
			if (objSQL->hstmt == NULL)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc, &objSQL->hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				retcode = SQLPrimaryKeys(objSQL->hstmt,
										NULL, 0,             /* Catalog name */
										NULL, 0,             /* Schema name */
										(SQLCHAR *) Tcl_GetStringFromObj(objv[2], (int *) NULL), SQL_NTS);   /* Table name */


			if ((retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) && objc > 3)
			{

				/* Get the column headers */
				retcode = objSQL->GetColumnHeaders(obj_row,interp);

				/* Set the TCL return variable */
				Tcl_ObjSetVar2(interp,objv[3],NULL,obj_row,TCL_LEAVE_ERR_MSG);

				/* free data buffers */
				for ( i = 0; i < objSQL->nresultcols; i++ )
					free( objSQL->sData[i] );
			}
			break;
		}
		case SQLTABLES:
		{
			/* Create a fresh statement handle if needed */
			if (objSQL->hstmt == NULL)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc, &objSQL->hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				retcode = SQLTables(objSQL->hstmt,
									NULL, 0,                /* All catalogs */
									NULL, 0,                /* All schemas */
									NULL, 0,
									NULL, 0);               /* All columns */

			break;
		}
		case SQLFOREIGNKEYS:
		{
			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "SQL_Table");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			/* Create a fresh statement handle if needed */
			if (objSQL->hstmt == NULL)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc, &objSQL->hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				retcode = SQLForeignKeys(objSQL->hstmt,
						 NULL, 0,            /* Primary catalog */
						 NULL, 0,            /* Primary schema */
						 (SQLCHAR *) Tcl_GetStringFromObj(objv[2], (int *) NULL), SQL_NTS,   /* Primary table */
						 NULL, 0,            /* Foreign catalog */
						 NULL, 0,            /* Foreign schema */
						 NULL, 0);           /* Foreign table */

			break;
		}
		case SQLSTATISTICS:
		{
			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "SQL_Table");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			/* Create a fresh statement handle if needed */
			if (objSQL->hstmt == NULL)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc, &objSQL->hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				retcode = SQLStatistics(objSQL->hstmt,
									NULL, 0,                /* All catalogs */
									NULL, 0,                /* All schemas */
									(SQLCHAR *) Tcl_GetStringFromObj(objv[2], (int *) NULL), SQL_NTS,
									NULL, 0);               /* All columns */

			break;
		}
		case SQLTABLEPRIVILEGES:
		{
			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "SQL_Table");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			/* Create a fresh statement handle if needed */
			if (objSQL->hstmt == NULL)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc, &objSQL->hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				retcode = SQLTablePrivileges(
					 objSQL->hstmt,
					 NULL,
					 0,
					 NULL,
					 0,
					 (SQLCHAR *) Tcl_GetStringFromObj(objv[2], (int *) NULL),
					 SQL_NTS);

			break;
		}
		case SQLPROCEDURES:
		{

			/* Create a fresh statement handle if needed */
			if (objSQL->hstmt == NULL)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc, &objSQL->hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				retcode = SQLProcedures(
					 objSQL->hstmt,
					 NULL,
					 0,
					 NULL,
					 0,
					 NULL,
					 0);

			break;
		}
		case SQLSPECIALCOLUMNS:
		{
			if (objc < 4)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "type table");
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			}

			/* Valid options */
			static char *options[] = {
				"SQL_BEST_ROWID",
				"SQL_ROWVER",
				NULL
			};

			enum options {
				SQLBESTROWID,
				SQLROWVER
			};

			if (Tcl_GetIndexFromObj(interp, objv[2],options, "option", 0,
				&index) != TCL_OK)
			{
				Tcl_MutexUnlock(&myMutex);
				return TCL_ERROR;
			} else {
			index++;
			}


			/* Create a fresh statement handle if needed */
			if (objSQL->hstmt == NULL)
				retcode = SQLAllocHandle(SQL_HANDLE_STMT,objSQL->hdbc, &objSQL->hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				retcode = SQLSpecialColumns(
						objSQL->hstmt,
					 index,
					 NULL,
					 0,
					 NULL,
					 0,
					 (SQLCHAR *) Tcl_GetStringFromObj(objv[3], (int *) NULL),
					 SQL_NTS,
					 SQL_SCOPE_SESSION,
					 SQL_NULLABLE);


			break;
		}
		case SQLDATASOURCES:
		{
			if (objc < 3)
			{
				Tcl_WrongNumArgs(interp,2,objv,
							   "Tcl variable that stores DSN name.");
				return TCL_ERROR;
			}
			SQLCHAR serverName[256];
			SQLCHAR description[256];
			SQLSMALLINT nameLength1Ptr;
			SQLSMALLINT nameLength2Ptr;

			retcode = SQLDataSources(
				 objSQL->henv,
				 SQL_FETCH_NEXT,
				 serverName,
				 256,
				 &nameLength1Ptr,
				 description,
				 256,
				 &nameLength2Ptr);

			Tcl_Obj* r1 = Tcl_NewStringObj((char *)serverName,strlen((const char *)serverName));
			Tcl_Obj* r2 = Tcl_NewStringObj((char *)description,strlen((const char *)description));

			/* Set the first Tcl return variable */
			if ((retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) && objc > 2)
				Tcl_ObjSetVar2(interp,objv[2],NULL,r1,TCL_LEAVE_ERR_MSG);


			/* Set the second Tcl return variable if there is one */
			if ((retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) && objc > 3)
				Tcl_ObjSetVar2(interp,objv[3],NULL,r2,TCL_LEAVE_ERR_MSG);

			break;
		}

	}

	Tcl_Obj *pRetCode = Tcl_NewIntObj(retcode);
	Tcl_SetObjResult(interp,pRetCode);

	Tcl_MutexUnlock(&myMutex);

	return TCL_OK;

}

/* Main Routine in the TCL Extension DLL */
int Sql_tcl_Init(Tcl_Interp *interp)
{

	#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION >= 1
		// Does the TCL interpreter support version 8.3 of TCL?
		if (Tcl_InitStubs(interp,"8.3",0) == NULL)
			return TCL_ERROR;
	#endif

	if (Tcl_PkgProvide(interp,"SQL","1.1") != TCL_OK)
		return TCL_ERROR;

	Tcl_Eval(interp, "namespace eval ::SQL {namespace export *}");


	Tcl_CreateObjCommand(interp, "::SQL::SQLConnect", SQLAllocHandle_ObjCmd,
		      (ClientData) "SQLConnect", (Tcl_CmdDeleteProc *) NULL);

	Tcl_CreateObjCommand(interp, "::SQL::SQLDriverConnect", SQLAllocHandle_ObjCmd,
		      (ClientData) "SQLDriverConnect", (Tcl_CmdDeleteProc *) NULL);

	return TCL_OK;

}