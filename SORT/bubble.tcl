proc sort5 {list} {
   foreach i $list {
      if {[info exists l2]} {
         set cntr 0
         while {[lindex $i 0] >= [lindex [lindex $l2 $cntr] 0]
                && $cntr < [llength $l2]} {
            incr cntr
         }
         set l2 [linsert $l2 $cntr $i]
      } else {
         lappend l2 $i
      }
   }
   return $l2
 }
