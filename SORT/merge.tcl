# actually built-in lsort command uses the mergesort algorithm

proc mergesort m {
    set len [llength $m]
    if {$len <= 1} {
        return $m
    }
    set middle [expr {$len / 2}]
    set left [lrange $m 0 [expr {$middle - 1}]]
    set right [lrange $m $middle end]
    return [merge [mergesort $left] [mergesort $right]]
}
 
proc merge {left right} {
    set result [list]
    while {[set lleft [llength $left]] > 0 && [set lright [llength $right]] > 0} {
        if {[lindex $left 0] <= [lindex $right 0]} {
            set left [lassign $left value]
        } else {
            set right [lassign $right value]
        }
        lappend result $value
    }
    if {$lleft > 0} {
        lappend result {*}$left
    }
    if {$lright > 0} {
        set result [concat $result $right] ;# another way append elements
    }
    return $result
}
 
puts [mergesort {8 6 0.1 4 2 3 5 -1 7 9}] ;# => -1 0.1 2 3 4 5 6 7 8 9
