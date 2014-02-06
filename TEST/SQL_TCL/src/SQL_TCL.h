/*
* SQL_TCL.h v1.0 2-24-2002 Scott Nichols
*
* This software is provided "AS IS", without a warranty of any kind.
* You are free to use/modify this code but leave this header intact.
*
*/

/* TCL Function prototype declarations */
#ifndef SQL_TCL_H
#define SQL_TCL_H

extern "C" {__declspec(dllexport) int Sql_tcl_Init(Tcl_Interp* interp);}

static int	SQLAllocHandle_ObjCmd _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));

static int	SQL_ObjCmd _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]));

/* MAX size of String Handle */
#define STRING_HANDLE_SIZE 32

/* Maximum Columns in result set */
#define MAXCOLS        255

/* Maximumn Column Header size */
/* NOTE: There in no size limit on column values */
#define MAX_COL_SIZE   128

/* Maximum Error Message Size */
#define MAX_ERROR_SIZE 1024

#endif