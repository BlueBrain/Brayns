Changelog {#Changelog}
=========

# master {#master}

* [#18](https://github.com/BlueBrain/Brayns/pull/18):
  ZeroEQ/Buf refactoring and cleanup to fix circular dependency and allow MacOSX
  support.
* [#17](https://github.com/BlueBrain/Brayns/pull/17)
  Fixed ImageMagick dependency
* [#16](https://github.com/BlueBrain/Brayns/pull/16)
  Introduced engines abstraction layer
* [#10](https://github.com/BlueBrain/Brayns/pull/15)
  Ispc correctness. Most variables in ispc code were not properly defined with
  corresponding uniform and varying keywords.
* [#9](https://github.com/BlueBrain/Brayns/pull/9)
  Improved random generator used by OSPRay renderers to avoid obvious pattern
  artefacts, especially on large images.
* [#8](https://github.com/BlueBrain/Brayns/pull/8)
  Fixes morphology loading. The data provided by the Brion API provides
  diameters, and the current implementation was considering radiuses.
* [#7](https://github.com/BlueBrain/Brayns/pull/7)
  README update
* [#6](https://github.com/BlueBrain/Brayns/pull/6)
  Added bounding box as a scene environment option
* [#5](https://github.com/BlueBrain/Brayns/pull/5)
  Fixed alpha in recursive rays on transparent surfaces.
* [#4](https://github.com/BlueBrain/Brayns/pull/4)
  Fixed Unit Tests and Alpha in ExtendedOBJRenderer
* [#1](https://github.com/BlueBrain/Brayns/pull/1)
  Fixed alpha in OBJExtendedRenderer

# Release 0.1 (2016-06-16) {#Release010}

* Initial commit

