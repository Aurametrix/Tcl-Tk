# tcltest package provides utility tools for writing and running tests in the Tcl test suite

package require test::tap
namespace import ::test::tap::*

# let's run four tests.
plan 4

ok {expr {1 == 1}} -description "1 is equal to 1"
# use [concat] to pass a variable or constant.
is {concat 1} 1

ok {expr {0 == 1}}
# more diagnostics:
is {concat 0} 1
