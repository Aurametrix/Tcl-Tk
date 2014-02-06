# test.tcl: Tcl procs for running Java tests
#
# Copyright (c) 1998 The Regents of the University of California.
#       All Rights Reserved.  See the COPYRIGHT file for details.
#

# This file is a modified version of the test suite file
# in the Tcl distribution:
#
# Copyright (c) 1990-1994 The Regents of the University of California.
# Copyright (c) 1994-1996 Sun Microsystems, Inc.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#

package require java

# These global variables keep track of the number of tests
set TEST_PASSED 0
set TEST_FAILED 0

# The main test procedure. A test suite is just a series of
# calls to this proc.
#
proc test {name description script answer} {
    global TEST_PASSED TEST_FAILED

    # Evaluate the script
    puts "Test $name"
    set code [catch {uplevel $script} result]

    # Print info if it failed
    if {$code != 0} {
	puts "Description:"
	puts "   $description"
	puts "Stack trace:"
	jdkStackTrace
	incr TEST_FAILED
    } elseif {[string compare $result $answer] != 0} then { 
	puts "Description:\n    $description"
	puts "Expected result:\n   $answer"
	puts "Received result:\n   $result"
	incr TEST_FAILED
    } else {
	incr TEST_PASSED
    }
    update
}

# A test procedure that expects to see an exception thrown.
#
proc testException {name description script exception {message {}}} {
    global TEST_PASSED TEST_FAILED
    global errorCode

    # Evaluate the script
    puts "Test $name"
    set code [catch {uplevel $script} result]

    # Print info if it failed
    if {$code != 0} {
	set failed 0
	if { [string match {JAVA *} $errorCode] } {
	    set e [lindex $errorCode 1]
	    if { [java::info class $e] != "$exception" } {
		set failed 1
	    } elseif {$message != ""} {
		if {[$e getMessage] != "$message"} {
		    set failed 1
		}
	    }
	}
	if { $failed } {
	    puts "Description:\n    $description"
	    puts "Expected:\n    $exception $message"
	    puts "Stack trace:"
	    jdkStackTrace
	    incr TEST_FAILED
	} else {
	    incr TEST_PASSED
	}
    } else {
	puts "Description:\n    $description"
	puts "Expected:\n    $exception $message"
	incr TEST_FAILED
    }
    update
}

# Print test results
#
proc testDone {} {
    global TEST_PASSED TEST_FAILED

    puts "Total: [expr $TEST_PASSED + $TEST_FAILED] \
	    (Passed: $TEST_PASSED, Failed: $TEST_FAILED)"
    flush stderr
    update

    set TEST_PASSED 0
    set TEST_FAILED 0
}

# If there is no update command, define a dummy proc.  Jacl needs this
if {[info command update] == ""} then { 
    proc update {} {}
}
