# shortest_path.tcl --
 #     Find the shortest path in a graph, using
 #     Floyd's algorithm
 #
 package require struct

 # mkMatrix --
 #     Make a square matrix with uniform entries
 # Arguments:
 #     size      Size (number of columns/rows) of the matrix
 #     value     Default value to use
 # Result:
 #     A list of lists that represents the matrix
 #
 proc mkMatrix {size value} {
     set row {}
     for { set i 0 } { $i < $size } { incr i } {
         lappend row $value
     }
     set matrix {}
     for { set i 0 } { $i < $size } { incr i } {
         lappend matrix $row
     }
     return $matrix
 }

 # mkPath --
 #     Use the resulting matrix to print the shortest path
 # Arguments:
 #     indices   Matrix of indices
 #     names     Names of the nodes
 #     from      The name of the node to start with
 #     to        The name of the node to go to
 # Result:
 #     A list of intermediate nodes along the path
 #
 proc mkPath {indices names from to} {
     set f [lsearch $names $from]
     set t [lsearch $names $to]

     set ipath [IntermediatePath $indices $f $t]
     set path  [list $from]

     foreach node $ipath {
         lappend path [lindex $names $node]
     }

     lappend path $to
     return $path
 }

 # IntermediatekPath --
 #     Construct the intermediate path
 # Arguments:
 #     indices   Matrix of indices
 #     from      The node to start with
 #     to        The node to go to
 # Result:
 #     A list of intermediate nodes along the path
 #
 proc IntermediatePath {indices from to} {

     set path {}
     set next [lindex $indices $from $to]
     if { $next >= 0 } {
        set path [concat $path [IntermediatePath $indices $from $next]]
        lappend path $next
        set path [concat $path [IntermediatePath $indices $next $to]]
     }
     return $path
 }

 # floydPaths --
 #     Construct the matrix that encodes the shortest paths,
 #     via Floyd's algorithm
 # Arguments:
 #     distances  Matrix of distances
 #     lmatrix    (Optional) the name of a variable to hold the
 #                shortest path lengths as a matrix
 # Result:
 #     A matrix encoding the shortest paths
 #
 proc floydPaths {distances {lmatrix {}}} {
     if { $lmatrix != {} } {
        upvar 1 $lmatrix lengths
     }

     set size [llength $distances]

     set indices [mkMatrix $size -1]
     set lengths $distances

     for { set k 0 } { $k < $size } { incr k } {
         for { set i 0 } { $i < $size } { incr i } {
             for { set j 0 } { $j < $size } { incr j } {
                 set dik [lindex $lengths $i $k]
                 set dij [lindex $lengths $i $j]
                 set dkj [lindex $lengths $k $j]

                 if { $dik == {} || $dkj == {} } {
                     continue ;# No connection - distance infinite
                 }

                 if { $dij == {} || $dik+$dkj < $dij } {
                     lset indices $i $j $k
                     lset lengths $i $j [expr {$dik+$dkj}]
                 }
             }
         }
     }

     return $indices
 }

 # determinePaths --
 #     Construct the matrix that encodes the shortest paths from
 #     the given graph
 # Arguments:
 #     graph      Graph to be examined
 #     key        Name of the (non-negative) attribute) holding the
 #                length of the arcs (defaults to "distance")
 #     lmatrix    (Optional) the name of a variable to hold the
 #                shortest path lengths as a matrix
 # Result:
 #     A matrix encoding the shortest paths
 #
 proc determinePaths {graph {key distance} {lmatrix {}} } {
     if { $lmatrix != {} } {
        upvar 1 $lmatrix lengths
     }

     set names     [$graph nodes]
     set distances [mkMatrix [llength $names] {}]
     for { set i 0 } { $i < [llength $names] } { incr i } {
         lset distances $i $i 0 ;# Distance of a node to itself is 0
     }

     foreach arc [$graph arcs $key] {
         set from [lsearch $names [$graph arc source $arc]]
         set to   [lsearch $names [$graph arc target $arc]]
         set d    [$graph arc get $arc $key]
         if { $from != $to } {
             lset distances $from $to $d
         }
     }
     puts $distances

     return [floydPaths $distances lengths]
 }

 # Small test --
 #    Construct a graph, make a matrix of distances out of it
 #    and query a few shortest paths. Note: the graph is undirected,
 #    so the arrows are doubled.
 #
 set names     {A B C D E F G}
 set distances {
   { 0  7  3 {} {} {} {}}
   { 7  0 {}  8 {} {} 40}
   { 3 {}  0 12  4 {} {}}
   {{}  8 12  0 {} {} {}}
   {{} {}  4 {}  0 10  7}
   {{} {} {} {} 10  0  8}
   {{} 40 {} {}  7  8  0}}

 # Construct the graph:
 #
 set graph [::struct::graph]

 set names     {A B C D E F G}
 set arcs  {
    A B 7
    A C 3
    B D 8
    B G 40
    C D 12
    C E 4
    E F 10
    E G 7
    F G 8
 }

 #
 #
 foreach n $names {
     $graph node insert $n
 }
 foreach {from to distance} $arcs {
     set arc [$graph arc insert $from $to]
     $graph arc append $arc distance $distance

     set arc [$graph arc insert $to $from]
     $graph arc append $arc distance $distance
 }

 #
 # Now that we have our graph, examine some shortest paths
 #
 # Note: the ordering of the nodes in the graph is not the
 # same as the order in which they were created! Hence the
 # call to [$graph nodes].

 set indices [determinePaths $graph "distance" lengths]
 puts $indices
 puts [mkPath $indices [$graph nodes] A B]
 puts [mkPath $indices [$graph nodes] B G]
