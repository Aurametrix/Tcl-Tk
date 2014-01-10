{#} {

   a simple sorting algorithm that builds the final sorted array one item at a time. 
   much less efficient on large lists than quicksort, heapsort, or merge sort. 
   advantages include efficiency for small or presorted datasets, stability & simple implementation

}

package require Tcl 8.5

proc insertionsort {m} {
    for {set i 1} {$i < [llength $m]} {incr i} {
        set val [lindex $m $i]
        set j [expr {$i - 1}]
        while {$j >= 0 && [lindex $m $j] > $val} {
            lset m [expr {$j + 1}] [lindex $m $j]
            incr j -1
        }
        lset m [expr {$j + 1}] $val
    }
    return $m
}

puts [insertionsort {8 6 10 4 2 1 3 5 7 -2 3.1 9 0}] ;# => -2 0 1 2 3 3.1 4 5 6 7 8 9
