# singly-linked list

oo::define List {
    method for {varName script} {
        upvar 1 $varName var
        set elem [self]
        while {$elem ne ""} {
            set var [$elem value]
            uplevel 1 $script
            set elem [$elem next]
        }
    }
}

# demo

set list {}
foreach n {1 3 5 7 2 4 6 8} {
    set list [List new $n $list]
}
$list for x {
    puts "we have a $x in the list"
}
