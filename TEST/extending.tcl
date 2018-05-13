/* This is your useful procedure. It can be a C++ or C linked procedure. */
int tclextfunction (
    ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]) {
    /*
        dosomething - clientdata is a pointer to something the programmer may
        or may not define
    */
    return TCL_OK;
}

#ifdef __cplusplus
extern "C" {
#endif

int Tclext_Init(Tcl_Interp *interp)
{
    /*
        Tcl stubs and tk stubs are needed for dynamic loading, you must have
        this set as a compiler option
    */
    #ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, TCL_VERSION, 1) == NULL) {
        Tcl_SetResult(interp, "Tcl_InitStubs failed",TCL_STATIC);
            return TCL_ERROR;
    }
    #endif

    #ifdef USE_TK_STUBS
    if (Tk_InitStubs(interp, TCL_VERSION, 1) == NULL) {
        Tcl_SetResult(interp, "Tk_InitStubs failed", TCL_STATIC);
        return TCL_ERROR;
    }
    #endif

    /* register your functions with Tcl */
    Tcl_CreateCommand( "tclextfunction", tclextfunction );
    return TCL_OK;
}
#ifdef __cplusplus
}
#endif
