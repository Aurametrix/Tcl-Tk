Tcl - Tool Command Language used for test automation, GUIs, in embedded systems such as
Tivo set top boxes, AOL's web servers, NBC television network's control center
upported or required in network equipment products from companies such as Cisco, Ixia, 
Agilent, Fanfare, Spirent, Conformiq, Wind River. Companies such as f5 (iRules) and Nortel 
(Bay Command Console) use re-branded versions of Tcl in many of their devices.
Powered the Mars Rover and animations of armies in Lord of Rings movies.

The combination of Tcl and the Tk GUI toolkit is referred to as Tcl/Tk 
pronounced "tickle tock." 


To run: 

tclsh hello.tcl

to check the verson:

echo 'puts [info patchlevel];exit 0' | tclsh

or
    $ tclsh
    % puts $tcl_version
    % info patchlevel

Installing packages

tclsh
    % puts $auto_path
Then add the path to the directory which contains the pkgIndex.tcl

for VTK:
set auto_path "$auto_path /xxxxxx/lib/vtk-5.2"


+ [advantages of Tcl over Lisp](https://wiki.tcl.tk/13410)

+ [praising Tcl](http://yosefk.com/blog/i-cant-believe-im-praising-tcl.html)

+ [AOL server](https://github.com/aolserver)

+ [Everything is a string](https://wiki.tcl-lang.org/page/everything+is+a+string)
