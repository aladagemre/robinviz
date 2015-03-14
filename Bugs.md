# Known Bugs in Version 1.0.0 #

## Common ##
**Bug id W002: Flying nodes**

_Definition_: If user moves the protein nodes fast, nodes might fly to a distant position and the scene might get distorted by expanding.

_Solution_: Please move proteins slowly.

## Windows ##

**Bug id W001: Gene search indexing bug**

_Definition_ This bug may occur when a user runs Robinviz consequtively, where Robinviz may not delete _outputs/gene\_index.txt_ file results with an error at the GUI loading phase of a erroneous run.

_Solution_: Close Robinviz GUI, start with Robinviz.exe and use "Ctrl+D" shortcut. The last run should be loaded without any error.

## Linux ##

