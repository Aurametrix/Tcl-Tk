package require Tcl 8.5

# simple implementation of comparison-based sorting algorithm, not stable 
# actually built-in heapsort command uses the heapsort algorithm 

proc heapsort {list {count ""}} {
    if {$count eq ""} {
	set count [llength $list]
    }
    for {set i [expr {$count/2 - 1}]} {$i >= 0} {incr i -1} {
	siftDown list $i [expr {$count - 1}]
    }
    for {set i [expr {$count - 1}]} {$i > 0} {} {
	swap list $i 0
	incr i -1
	siftDown list 0 $i
    }
    return $list
}
proc siftDown {varName i j} {
    upvar 1 $varName a
    while true {
	set child [expr {$i*2 + 1}]
	if {$child > $j} {
	    break
	}
	if {$child+1 <= $j && [lindex $a $child] < [lindex $a $child+1]} {
	    incr child
	}
	if {[lindex $a $i] >= [lindex $a $child]} {
	    break
	}
	swap a $i $child
	set i $child
    }
}
proc swap {varName x y} {
    upvar 1 $varName a
    set tmp [lindex $a $x]
    lset a $x [lindex $a $y]
    lset a $y $tmp
}

puts [heapsort {8 6 10 4 2 1 3 5 7 -2 3.1 9 0}] ;# => -2 0 1 2 3 3.1 4 5 6 7 8 9
