# Patterns described using regular expression syntax 


set something candelabra

regexp a..a $something match
  puts "a..a match is  $match "
regexp a(.*)a $something match
  puts "a-dot-star-a match is  $match "
set match


set x {Dolores Sanchez
San Jose, CA}

regexp {^San Jose} $x match
puts $match
regexp -line {^San Jose} $x match
puts $match

puts -nonewline "What do you want to tell me? "
flush stdout
set something [gets stdin]


if [
  regexp -nocase {\<foo(?!bar\>)(\w*)} $something matchresult
] then {
  puts $matchresult
} else {
  puts "my regex could not match the subject string"
}

