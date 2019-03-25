The Multiview Module for Brayns
==================================

This module implements the multiview(top/front/right/perspective) camera for Brayns

Usage
-----

- Point LD_LIBRARY_PATH to the folder which contains
  'libospray_module_multiview.so'
- Run Brayns application either with command line '--module multiview --camera-type multiview' or do
  'ospLoadModule("multiview")' programmatically
```
OSPCamera camera = ospNewCamera("multiview");
```
