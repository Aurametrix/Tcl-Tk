set filename "words.txt"
set pattern {apple}
set count 0

set fid [open $filename r]
while {[gets $fid line] != -1} {
    incr count [regexp -all -- $pattern $line]
}
close $fid

puts "$count occurrances of $pattern in $filename"
