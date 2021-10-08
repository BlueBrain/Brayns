Circuit Info API methods
------------------------

The Brayns python client API is automatically generated when connecting to a
running backend service as shown in :ref:`usepythonclient-label`.

The Core plugin is always loaded so the related entrypoints are always
available.

Other plugins might register additional entrypoints but must be loaded in the
renderer to be available.

The functions below are generated using the entrypoints of the Circuit Info plugin.

All arguments are keyword arguments extracted from the entrypoint params.

Renderer errors will be raised with an instance of brayns.ReplyError. This one
can be used to extract error code, description and some optional additional data
(mainly used to store JSON errors).

Example usage of some generated methods:

.. code-block: python
    import brayns

    with brayns.connect(uri='localhost:5000') as client:
        print(client.get_camera())
        client.set_camera(current='orthographic')
        print(client.get_camera())

ci_get_afferent_cell_ids
~~~~~~~~~~~~~~~~~~~~~~~~

Return a list of afferent synapses cell GIDs from a circuit and a set of source cells.

Parameters:

* ``path``: ``str``. Path to the circuit config file.
* ``sources``: ``list``. Source cells GIDs.

  * ``items``: ``int``

Return value:

* ``dict``. The object has the following properties.

  * ``ids``: ``list``. Afferent cells unique GIDs.

    * ``items``: ``int``

----

ci_get_cell_data
~~~~~~~~~~~~~~~~

Return data attached to one or many cells.

Parameters:

* ``ids``: ``list``. List of cell IDs.

  * ``items``: ``int``

* ``path``: ``str``. Path to circuit config file.
* ``properties``: ``list``. Desired properties.

  * ``items``: ``str``

Return value:

* ``dict``. The object has the following properties.

  * ``etypes``: ``list``. Requested cell e-types.

    * ``items``: ``str``

  * ``layers``: ``list``. Requested cell layers.

    * ``items``: ``str``

  * ``morphology_classes``: ``list``. Requested cell morphology classes.

    * ``items``: ``str``

  * ``mtypes``: ``list``. Requested cell m-types.

    * ``items``: ``str``

  * ``orientations``: ``list``. Requested cell orientations.

    * ``items``: ``float``

  * ``positions``: ``list``. Requested cell positions.

    * ``items``: ``float``

----

ci_get_cell_ids
~~~~~~~~~~~~~~~

Return the list of GIDs from a circuit.

Parameters:

* ``path``: ``str``. Path to the circuit config file.
* ``targets``: ``list``. Targets to query.

  * ``items``: ``str``

Return value:

* ``dict``. The object has the following properties.

  * ``ids``: ``list``. List of cell GIDs.

    * ``items``: ``int``

----

ci_get_cell_ids_from_model
~~~~~~~~~~~~~~~~~~~~~~~~~~

Return the list of GIDs from a loaded circuit.

Parameters:

* ``model_id``: ``int``. ID of the circuit model.

Return value:

* ``dict``. The object has the following properties.

  * ``ids``: ``list``. List of cell GIDs.

    * ``items``: ``int``

----

ci_get_efferent_cell_ids
~~~~~~~~~~~~~~~~~~~~~~~~

Return a list of efferent synapses cell GIDs from a circuit and a set of source cells.

Parameters:

* ``path``: ``str``. Path to the circuit config file.
* ``sources``: ``list``. Source cells GIDs.

  * ``items``: ``int``

Return value:

* ``dict``. The object has the following properties.

  * ``ids``: ``list``. Efferent cells unique GIDs.

    * ``items``: ``int``

----

ci_get_projection_efferent_cell_ids
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Return a list of efferent projected synapses cell GIDs from a circuit and a set of source cells.

Parameters:

* ``path``: ``str``. Path to the circuit config file.
* ``projection``: ``str``. Projection name.
* ``sources``: ``list``. Source cells GIDs.

  * ``items``: ``int``

Return value:

* ``dict``. The object has the following properties.

  * ``ids``: ``list``. Efferent cells GIDs.

    * ``items``: ``int``

----

ci_get_projections
~~~~~~~~~~~~~~~~~~

Return a list of projection names available on a circuit.

Parameters:

* ``path``: ``str``. Path to the circuit config file.

Return value:

* ``dict``. The object has the following properties.

  * ``projections``: ``list``. Projections names.

    * ``items``: ``str``

----

ci_get_report_info
~~~~~~~~~~~~~~~~~~

Return information about a specific report from a circuit.

Parameters:

* ``path``: ``str``. Path to the circuit BlueConfig from which to get the report.
* ``report``: ``str``. Name of the report from where to get the information.

Return value:

* ``dict``. The object has the following properties.

  * ``data_unit``: ``str``. Unit of the report values. Can be (mV, mA, ...).
  * ``end_time``: ``float``. Time at which the simulation ends.
  * ``frame_count``: ``int``. Number of simulation frames in the report.
  * ``frame_size``: ``int``. Number of values per frame in the report.
  * ``start_time``: ``float``. Time at which the simulation starts.
  * ``time_step``: ``float``. Time between two consecutive simulation frames.
  * ``time_unit``: ``str``. Unit of the report time values.

----

ci_get_reports
~~~~~~~~~~~~~~

Return a list of reports from a circuit.

Parameters:

* ``path``: ``str``. Path to the circuit config file.

Return value:

* ``dict``. The object has the following properties.

  * ``reports``: ``list``. Report names.

    * ``items``: ``str``

----

ci_get_spike_report_info
~~~~~~~~~~~~~~~~~~~~~~~~

Return wether the circuit has a spike report and its path.

Parameters:

* ``path``: ``str``. Path of the circuit to test.

Return value:

* ``dict``. The object has the following properties.

  * ``exists``: ``bool``. Check if the report exists.
  * ``path``: ``str``. Path to the spike report.

----

ci_get_targets
~~~~~~~~~~~~~~

Return a list of targets from a circuit.

Parameters:

* ``path``: ``str``. Path to the circuit config file.

Return value:

* ``dict``. The object has the following properties.

  * ``targets``: ``list``. Target names.

    * ``items``: ``str``

----

ci_info
~~~~~~~

Return general info about a circuit.

Parameters:

* ``path``: ``str``. Path of the circuit config file.

Return value:

* ``dict``. The object has the following properties.

  * ``cells_count``: ``int``. Number of cells in this circuit.
  * ``cells_properties``: ``list``. List of available cell properties.

    * ``items``: ``str``

  * ``e_types``: ``list``. List of electrical types available in this circuit.

    * ``items``: ``str``

  * ``m_types``: ``list``. List of morphology types available in this circuit.

    * ``items``: ``str``

  * ``reports``: ``list``. List of report names.

    * ``items``: ``str``

  * ``spike_report``: ``str``. Path to the spike report file.
  * ``targets``: ``list``. List of target names.

    * ``items``: ``str``

