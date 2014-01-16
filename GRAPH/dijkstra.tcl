#Dijkstra algorithm as Routing SPF(Shortest Path First) algorithm in CT.
#Reference Details: <Routing TCP/IP Volume I 2nd.Edition> Chapter 4, Dynamic Routing Protocols: Link State Routing Protocols
#This file was originally written by warmeng<warmeng@gmail.com>
#Distributed under the BSD software license
#USEAGE: 1. Modify parameter "root" to a node id in the topology as first member of Tree database.(please search "set root", donot include quote)
#        2. tclsh SPF.Algorithm.tcl

#topology:
#RA----4----RD----5----RG
# | \        |         /
# |  4       |        /
# |   \      |       /
# |    \     |      /
# 2     \    3     1
# |      \   |    /
# |       \  |   /
# |        5 |  /
# RB-10---2-\RE----8---RH
# |          |        /
# 1          |       6
# |          |      /
# |          2     /
# |          |    /
# 5          |   /
# |          |  4
# RC----2----RF/
#input: All router path & cost in the topology. following is reference :Table 4-2. <Routing TCP/IP Volume I 2nd.Edition>
set data {
Router ID Neighbor Cost
 
RA RB 2
RA RD 4
RA RE 4
RB RA 2
RB RC 1
RB RE 10
RC RB 5
RC RF 2
RD RA 4
RD RE 3
RD RG 5
RE RA 5
RE RB 2
RE RD 3
RE RF 2
RE RG 1
RE RH 8
RF RC 2
RF RE 2
RF RH 4
RG RD 5
RG RE 1
RH RE 8
RH RF 6
}
#output :
#SPF of All routers, following is a example
#RA,RA,0 RA,RB,2 RB,RC,1 RA,RD,4 RA,RE,4 RC,RF,2 RE,RG,1 RF,RH,4
set debug 1
#-----------------------begin procs----------------------------
#State machine reference:http://wiki.tcl.tk/8363
proc statemachine states {
    global root
        global TreeDatabase
        global lowestCostEntry
    array set S $states
    proc goto label {
        uplevel 1 set this $label
        return -code continue
    }
    set this [lindex $states 0]
    while 1 {eval $S($this)}
    rename goto {}
}
proc debugOutput {msgtype msg} {
    global debug
        switch $debug {
            0 {
                    #turn off debug output
                }
                1 {
                    #notification message
                        if {[string first NOTI $msgtype] == 0} {
                            puts "$msg"
                        }
                }
                2 {
                    #warning message
                        if {[string first NOTI $msgtype] == 0} {
                            puts "$msg"
                        }
                        if {[string first WARN $msgtype] == 0} {
                            puts "$msg"
                        }
                }
                3 {
                    #debug message
                        if {[string first DBG $msgtype] == 0} {
                            puts "$msg"
                        }
                        if {[string first WARN $msgtype] == 0} {
                            puts "$msg"
                        }
                        if {[string first NOTI $msgtype] == 0} {
                            puts "$msg"
                        }
                }
                default {
                    puts "Debug level should be 0,1,2,3, but got: $debug"
                }
        }
}
proc generateLinkStateDatabase {data} {
        return [regexp -all -inline {R[\w]+ R[\w]+ [0-9]+} $data]
}
proc checkNeighborsExistInTreeDB {entry Database} {
    set NeighborID [lindex $entry 1]
    set numbersOfEntries [llength $Database]
        for {set i 0} {$i < $numbersOfEntries} {incr i} {
            set neighborIDInDatabase [lindex [lindex $Database $i] 1]
                if {[string match $NeighborID $neighborIDInDatabase] == 1} {
                    #if neighbor exist, return 1
                    return 1
                }
        }
        #if not exist, return 0
        return 0
}
proc neighborIDOfEntry {entry} {
    return [lindex $entry 1]
}
proc calcChainsToRootCost {entry} {
        global TreeDatabase
        global root
        if {$entry == ""} {debugOutput DBG "calcChainsToRootCost entry is empty";return}
        set entry [split $entry " "]
        if {[string match [lindex $entry 0] $root] == 1} {
            return [lindex $entry 2]
        }
        if {[string match [lindex $entry 0] [lindex $entry 1]] == 1} {
            return 0
        }
        #Recursion generate full chain cost
        foreach line $TreeDatabase {
            debugOutput DBG "calcChainsToRootCost $entry $line [string match [lindex $line 1] [lindex $entry 0]]"
            if {[string match [lindex $line 1] [lindex $entry 0]] == 1} {
                    return [expr [lindex $entry 2] + [calcChainsToRootCost $line]]
                }
        }
}
proc purgeCandidateDatabase {} {
    #delete entry frome Candidate Database that match: have the same neighborID, but higher cost to root
        global CandidateDatabase
    set numbersOfEntries [llength $CandidateDatabase]
        for {set i 0} {$i < [expr $numbersOfEntries - 1]} {incr i} {
            for {set j [expr $i + 1]} {$j < [expr $numbersOfEntries]} {incr j} {
                    set left [lindex $CandidateDatabase $i]
                        set right [lindex $CandidateDatabase $j]
                        debugOutput DBG "purgeCandidateDatabase1 i$i,j$j,left$left,right$right,numbersOfEntries$numbersOfEntries"
                        #compare neighbor ID
                        if {[string match [lindex $left 1] [lindex $right 1]] == 1} {
                            debugOutput DBG "purgeCandidateDatabase found match: $left \- $right"
                            if {[expr [calcChainsToRootCost $left] - [calcChainsToRootCost $right]] >= 0} {
                                    lappend purgeList $i
                                } else {
                                    lappend purgeList $j
                                           }
                        }
                }
        }
        if {[info exist purgeList]} {
        set purgeList [lsort -decreasing $purgeList]
        for {set k 0} {$k < [llength $purgeList]} {incr k} {
            debugOutput DBG "purgeCandidateDatabase2 k$k purgeList$purgeList CandidateDatabase$CandidateDatabase [lindex $CandidateDatabase [lindex $purgeList $k] [lindex $purgeList $k]]"
            set CandidateDatabase [lreplace $CandidateDatabase [lindex $purgeList $k] [lindex $purgeList $k]]
        }
        }
}
proc addNeighborsOfRouterToCandidateDB {RouterID {except 0}} {
    global LinkStateDatabase
        global CandidateDatabase
        global TreeDatabase
        foreach entries $LinkStateDatabase {
            set ifmatch [regexp {^([\d\w]+) +([\d\w]+) +(\d+)} $entries match Neigh NextNeigh Cost]
                if {$ifmatch} {
                    if {[string match $RouterID $Neigh] ==1} {
                            if {[string match $except EXCEPT] == 1} {
                                    if {[checkNeighborsExistInTreeDB $entries $TreeDatabase] == 0} {
                                lappend CandidateDatabase [list $Neigh $NextNeigh $Cost]
                                        }
                                } else {
                                    lappend CandidateDatabase [list $Neigh $NextNeigh $Cost]
                                }
                        }
                } else {
                   debugOutput NOTI "addNeighborsOfRouterToCandidateDB DB error, cannot match correct entries, the Entry is: $entries"
                }
        }
        purgeCandidateDatabase
}

proc calcCandidateDB2rootCost {} {
    global CandidateDatabase
        foreach entries $CandidateDatabase {
            set ifmatch [regexp {^([\d\w]+) +([\d\w]+) +(\d+)} $entries match Neigh NextNeigh Cost]
                if {$ifmatch} {
            lappend tableOfChainsToRootCost [list $entries [calcChainsToRootCost $entries]]
                } else {
                   debugOutput NOTI "calcCandidateDB2rootCost DB error, cannot match correct entries, the Entry is: $entries"
                }
        }
        return $tableOfChainsToRootCost
}
proc lowestCostOfEntry {tableOfChainsToRootCost} {
    global lowestCostEntry
    set numbersOfEntry [llength $tableOfChainsToRootCost]
        set retVal [lindex $tableOfChainsToRootCost 0]
        for {set i 1} {$i < $numbersOfEntry} {incr i} {
            set tempVar1 [lindex $retVal 1]
            set leftside [lindex [lindex $tableOfChainsToRootCost $i] 1]
                if {[expr $tempVar1 - $leftside] > 0} {
                    set retVal [lindex $tableOfChainsToRootCost $i]
                }
        }
        set lowestCostEntry [lreplace $retVal 1 1]
        debugOutput DBG "lowestCostOfEntry Found lowest cost entry: $lowestCostEntry in\n$tableOfChainsToRootCost"
        return $lowestCostEntry
}
proc checkCandidateDatabaseEmpty {} {
    global CandidateDatabase
        return [llength $CandidateDatabase]
}
proc deleteEntryFromDatabase {entry Database} {
    set retVal [eval lsearch {$Database} $entry]
    if {$retVal != -1} {
            return [lreplace $Database $retVal $retVal]
        }
        return $Database
}
proc moveEntryFromCandidateDB2TreeDB {entry} {
    global lowestCostEntry
        global CandidateDatabase
        global TreeDatabase
        eval lappend TreeDatabase $entry
        set CandidateDatabase [deleteEntryFromDatabase $entry $CandidateDatabase]
        debugOutput NOTI "\nCandidate\tCostToRoot\tTree\n"
        foreach Candidate $CandidateDatabase Tree $TreeDatabase {
            debugOutput NOTI "[join $Candidate ","]\t[calcChainsToRootCost $Candidate]\t[join $Tree ","]"
                set Candidate ""
                set Tree ""
        }
}
#-------------------------end procs----------------------------

#-------------------------main start---------------------------
#Define Table Of Construction Tree(set I)
set TreeDatabase ""

#Define Table Of Candidate Database(set II)
set CandidateDatabase ""

#Initial set III
set LinkStateDatabase [generateLinkStateDatabase $data]

#Define "Rx" as root of TreeDatabase, the final SPF result is as same as "Rx" routing table should be.
set root RA

#Following comment comes from:  <Routing TCP/IP Volume I 2nd.Edition> Chapter 4, Dynamic Routing Protocols: Link State Routing Protocols
#It is unfortunate that Dijkstra's algorithm is so commonly referred to in the routing world as the shortest path first algorithm. After all, 
#the objective of every routing protocol is to calculate shortest paths. It is also unfortunate that Dijkstra's algorithm is often made to
# appear more esoteric than it really is; many writers just can't resist putting it in set theory notation. The clearest description of the 
#algorithm comes from E. W. Dijkstra's original paper. Here it is in his own words, followed by a "translation" for the link state routing protocol:
#Construct [a] tree of minimum total length between the n nodes. (The tree is a graph with one and only one path between every two nodes.)
#In the course of the construction that we present here, the branches are divided into three sets:
#the branches definitely assigned to the tree under construction (they will be in a subtree);
#the branches from which the next branch to be added to set I, will be selected;
#the remaining branches (rejected or not considered).
#The nodes are divided into two sets:
#the nodes connected by the branches of set I,
#the remaining nodes (one and only one branch of set II will lead to each of these nodes).
#We start the construction by choosing an arbitrary node as the only member of set A, and by placing all branches that end in this node in set II. 
#To start with, set I is empty. From then onwards we perform the following two steps repeatedly.
#Step 1.  The shortest branch of set II is removed from this set and added to set I. As a result, one node is transferred from set B to set A.
#Step 2.  Consider the branches leading from the node, which has just been transferred to set A, to the nodes that are still in set B. If the branch 
#under construction is longer than the corresponding branch in set II, it is rejected; if it is shorter, it replaces the corresponding branch in set II, 
#and the latter is rejected.

statemachine {
  1 {
    #Step 1:A router initializes the Tree database by adding itself as the root. This entry
    #shows the router as its own neighbor, with a cost of 0.
    lappend TreeDatabase [list $root $root 0]
    goto 2
  }
  2 {
    #Step 2:All triples in the link state database describing links to the root router's neighbors
    #are added to the Candidate database.
    addNeighborsOfRouterToCandidateDB $root
    goto 3
  }
  3 {
    #Step 3:The cost from the root to each link in the Candidate database is calculated. The
    #link in the Candidate database with the lowest cost is moved to the Tree database. If
    #two or more links are an equally low cost from the root, choose one.
    moveEntryFromCandidateDB2TreeDB [lowestCostOfEntry [calcCandidateDB2rootCost]]
    goto 4
  }
  4 {
    #Step 4:The Neighbor ID of the link just added to the Tree database is examined. With the
    #exception of any triple whose Neighbor ID is already in the Tree database, triples in the
    #link state database describing that router's neighbors are added to the Candidate database.
    addNeighborsOfRouterToCandidateDB [eval neighborIDOfEntry $lowestCostEntry] EXCEPT
    goto 5
  }
  5 {
    #Step 5:If entries remain in the Candidate database, return to step 3. If the Candidate database
    #is empty, terminate the algorithm. At termination, a single Neighbor ID entry in the Tree
    #database should represent every router, and the shortest path tree is complete.
    if {[checkCandidateDatabaseEmpty] > 0} {
      goto 3
    }
    puts "SPF Calculate finished. \n When root is $root, Tree Database is: \n $TreeDatabase"
    break
  }
}

