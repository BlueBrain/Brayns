The OpenDeck Module for Brayns
==================================

This module implements OpenDeck camera for Brayns

Usage
-----

- Point LD_LIBRARY_PATH to the folder which contains
  'libospray_module_opendeck.so'
- Run Brayns application either with command line '--module opendeck --camera-type opendeck' or do
  'ospLoadModule("opendeck")' programmatically
```
OSPCamera camera = ospNewCamera("opendeck");
```
