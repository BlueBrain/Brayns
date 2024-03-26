Brayns API
==========

Introduction
------------

Brayns is a ray tracing based renderer written in C++ that can be configured to
run a websocket server. This binary executable is called ``braynsService`` and
can perform various renderings.

This instance can be monitored by a websocket client using a JSON-RPC API.

For convenience, a Python package is provided to wrap the websocket
communication with classes and functions and hide low level JSON-RPC messages.

This API (websocket or Python) has the following features:

- Load models into brayns (circuits, meshes, volumes, ...).
- Load and/or attach simulations to models (compartment and spike reports).
- Modify their visual appearance (color, simulation color ramp, ...).
- Tune the renderers to achieve different visual results.
- Render images and simulation videos and save it to disk.

.. toctree::
   :hidden:
   :titlesonly:
   :maxdepth: 2

   Home <self>
   install
   usage
   plugins
   examples
   jsonrpcapi/api
   pythonapi/api

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

Should you face any issue with using it, please submit a ticket to the
`issue tracker <https://bbpteam.epfl.ch/project/issues/browse/BRAYNS>`_; or drop
us an `email <mailto: bbp-ou-visualization@groupes.epfl.ch>`_.
