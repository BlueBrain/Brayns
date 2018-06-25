The Stereoscopy Module for Brayns
==================================

This module implements a more advanced stereo camera for Brayns

Usage
-----

- Point LD_LIBRARY_PATH to the folder which contains
  'libospray_module_stereoscopy.so'
- Run Brayns application either with command line '--module stereoscopy --camera-type stereoFull' or do
  'ospLoadModule("stereoscopy")' programmatically
```
OSPCamera camera = ospNewCamera("stereoFull");
```
