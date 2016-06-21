set einstein [list "A. Einstein" "Patent Office Clerk" "Formulated Theory of Relativity."]

set mill [list "John Stuart Mill" "English Youth" "Was able to read Greek and Latin at age 3."]

# let's build the big list of lists
set average_folks [list $einstein $mill ...]

# let's pull out Einstein's title 
set einsteins_title [lindex $einstein 1]
