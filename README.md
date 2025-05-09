# DG dp TSP solver
Java object allocation overhead is too much due to ... gc...
so I rewrote it in cpp with native binding.

Also cpp -> java jni call is soooo slow so I made it always do java->cpp call using coroutines.

Hopefully it does not crash people xd