Brayns API
==========

Introduction
------------

Brayns provides a JSON-RPC API accessible through a websocket to interact with a running instance (renderer backend).

For convenience, it also provides a Python package to wrap the websocket communication with classes to hide low level JSON-RPC messages.

Through the API (websocket or Python) use can:

- Load models into brayns (circuits, DTIs, meshes, volumes, ...).
- Load and/or attach simulations (compartment and spike reports).
- Modify their visual appearance (color, simulation transfer function, ...).
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

Should you face any issue with using it, please submit a ticket to the `issue tracker <https://bbpteam.epfl.ch/project/issues/browse/BRAYNS>`_; or drop us an `email <mailto: bbp-ou-visualization@groupes.epfl.ch>`_.
