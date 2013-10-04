# It's easiest to implement stacks and queues with lists, push method = lappend 
# Pop method different
# in a stack, last in first out, LIFO

set L {"first-element" 66.25, 333, 333, 1, 1234.5}
puts "original list:  $L" 

interp alias {} push {} lappend

# now push will work by adding an element in the end

push L 111.11
push L "last-element"

puts "after pushing: $L"

# to pop a stack
proc pop name {
    upvar 1 $name stack
    set res [lindex $stack end]
    #set stack [lrange $stack 0 end-1] -- may get slow
    set stack [lreplace $stack [set stack end] end] ; # TRICKY: Making sure list is not shared.
    set res
 }

 

pop L
puts "after popping:  $L" 

