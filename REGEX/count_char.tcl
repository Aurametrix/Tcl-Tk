#! /usr/tcl84/bin/tclsh

 set mystring "line1\nline2\nline3\nline4\n"

 proc splitline {string} {
        set rc [llength [split $string "\n"]] 
        incr rc -1
        return $rc
 }

 proc regline {string} {
        set rc [regexp -line -all "\n" $string]
        return $rc
 }

 proc splitchar {string} {
 foreach x [split $string ""] {if {[catch {incr counters($x)}]} {set counters($x) 1}}
        set rc $counters(\n)
        return $rc
 } 

 puts [time {splitline $mystring} 10000]
 puts [time {regline $mystring} 10000]
 puts [time {splitchar $mystring} 10000]
