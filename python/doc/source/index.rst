.. Brayns documentation master file, created by
   sphinx-quickstart on Mon Jul 23 16:11:34 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Brayns python client
====================

Introduction
------------

The Brayns python client library provides access to remote control a running Brayns instance using
websockets and HTTP requests messaging.

It provides a generated API from the current running Brayns instance. There are also some helper classes
that encapsulate commonly used fuctions for an easy usage.

The Brayns python client allows to:

- Load models into brayns (circuits, DTIs, meshes, volumes, ...)
- Load and/or attach simulations (compartment and spike reports)
- Modify their visual appearance (color, simulation transfer function, ...)
- Tune the renderers to achieve different visual results
- Render images and simulation videos to disk
- Show specific visuals (syanpse tracing, synapse visualization, ...)

.. toctree::
   :hidden:
   :titlesonly:
   :maxdepth: 2

   Home <self>
   install
   usage
   plugins
   api


Acknowledgments
---------------

Authors and Contributors
~~~~~~~~~~~~~~~~~~~~~~~~

* Daniel Nachbaur
* Cyrille Favreau
* Jonas Karlsson
* Roland Groza
* Nadir Roman
* Grigori Chevtchenko
* Pawel Podhajski
* Adrien Devresse
* Sebastien Speierer
* Fabien Petitjean
* Adrien Fleury


Reporting issues
----------------

Brayns is currently maintained by the BlueBrain Visualization team.

Should you face any issue with using it, please submit a ticket to the `issue tracker <https://bbpteam.epfl.ch/project/issues/browse/BRAYNS>`_; or drop us an `email <mailto: bbp-ou-visualization@groupes.epfl.ch>`_.
