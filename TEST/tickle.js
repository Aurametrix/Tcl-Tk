require(['tcl/interp'], function(Interp){
    var I = Interp();

    I.registerCommand('annoy_user', function(args){
        I.checkArgs(args, 1, 'string');
        alert(args[1]);
    });

    I.TclEval('annoy_user "hello, world"', function(result){
        switch (result.code) {
            case I.types.OK:
            case I.types.RETURN:
                break;
            case I.types.ERROR:
                console.warn('Uncaught error in Tcl script: '+result.result);
                break;
            default:
                console.warn('Unexpected return code from Tcl script: '+result.code);
        }
    });
});
