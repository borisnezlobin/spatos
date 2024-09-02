# ramfs
A mountable filesystem, which contents is stored in RAM. Useful for early logging, before `redoxfs` has been started. The `initfs:` scheme seems to lack read-write support, and thus this ramfs driver allows logs to be written in initfs drivers.
