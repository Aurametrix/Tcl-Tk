# It's easiest to implement stacks and queues with lists, push method = lappend 
# Priority has to be assigned at pushing time
# in a queue, first in first out, FIFO
# in a priority queue, the object with the highest priority comes first.

set L {"first-element" 66.25, 333, 333, 1, 1234.5}
puts "original list:  $L" 

interp alias {} push {} lappend

# now push will work by adding an element in the end

push L 111.11
push L "last-element"

puts "after pushing: $L"

# to pop a queue
 proc pop name {
    upvar 1 $name queue
    set res [lindex $queue 0]
    #set queue [lrange $queue 1 end]
    set queue [lreplace $queue [set queue 0] 0]
    set res
 }
 

pop L
puts "after popping:  $L" 

# Popping a priority queue requires sorting out which item has highest priority. 
 proc pqpop name {
    upvar 1 $name queue
    set queue [lsort -real -index 1 $queue]
    qpop queue ;# fall back to standard queue, now that it's sorted
 }