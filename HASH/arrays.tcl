# Tcl doesn't have multidimensional arrays, but has associative arrays

# create lists simply by creating strings
set L {1 {2 3} 4}
puts $L

set a(1) 0
set a(foo) 1
set salary(Smith) 30000
set age(Smith) 45
set "x(white space)" foo
set s "rather long string"
puts "That was a list and here is a string: $s"
set length($s) [string length $s]

puts "with $length($s) characters"
expr $age(Smith) + $length($s)

set s "keith:cdridjyhTp2FQ:100:1100:Keith Waclena:/home/keith:/host/bin/zsh"
split $s
puts "after split: $s"

puts "Smith is $age(Smith) years old and his salary is $salary(Smith)"

foreach i [array names a] {
puts "a($i): $a($i)"
}
    

#set a("x y") bar
array names a
#puts foo {x y}
    
# set {white space(Smith)} foo
#puts ${white space}(Smith)
#puts ${white space(Smith)}
#puts [set {white space(Smith)}]
