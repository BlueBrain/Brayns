.. _circuitinfoapi-label:

Circuit Info API
================

This is the API exposed by the Circuit Info plugin. This plugin allows to
query the backend for circuit information, such as GIDs, targets, reports, synapses, etc.
For this API to be available, the Brayns backend service must be started with the
following parameter:

.. code-block:: console

    --plugin braynsCircuitInfo

----

ci_get_afferent_cell_ids
~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_afferent_cell_ids(path, sources)

Return a list of afferent synapses cell GIDs from a circuit and a set of source cells

Parameters:

* ``path``: ``string``, Path to the circuit config to query for afferent synapses
* ``sources``: ``array``, List of synapse source cell GIDs

Return value:

* ``dictionary`` containing the following entries

  * ``ids``: ``array``, List of afferent synapses cell GIDs

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_cell_ids
~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_cell_ids(targets, path)

Return the list of GIDs from a circuit

Parameters:

* ``path``: ``string``, Path to the circuit configuration file
* ``targets``: ``array``, List of targets to query

Return value:

* ``dictionary`` containing the following entries

  * ``ids``: ``array``, List of cell GIDs

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_cell_ids_from_model
~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_cell_ids_from_model(model_id)

Return the list of GIDs from a loaded circuit

Parameters:

* ``model_id``: ``integer``, The ID of the model to query for cell GIDs

Return value:

* ``dictionary`` containing the following entries

  * ``ids``: ``array``, List of cell GIDs

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_efferent_cell_ids
~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_efferent_cell_ids(path, sources)

Return a list of efferent synapses cell GIDs from a circuit and set of source cells

Parameters:

* ``path``: ``string``, Path to the circuit config to query for efferent synapses
* ``sources``: ``array``, List of synapse target cell GIDs

Return value:

* ``dictionary`` containing the following entries

  * ``ids``: ``array``, List of efferent synapses cell GIDs

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_projection_efferent_cell_ids
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_projection_efferent_cell_ids(path, sources, projection)

Return a list of efferent projected synapses cell GIDs from a circuit and a set of source cells

Parameters:

* ``path``: ``string``, Path to the circuit config to query for projected efferent synapses
* ``projection``: ``string``, Projection name to query for
* ``sources``: ``array``, Projected target cell GIDs

Return value:

* ``dictionary`` containing the following entries

  * ``ids``: ``array``, List of projected efferent synapses cell GIDs

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_projections
~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_projections(path)

Return a list of projection names available on a circuit

Parameters:

* ``path``: ``string``, Path to the circuit config to query for projection names

Return value:

* ``dictionary`` containing the following entries

  * ``projections``: ``array``, List of projection names

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_report_info
~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_report_info(report, path)

Return information about a specific report from a given circuit

Parameters:

* ``path``: ``string``, Path to the circuit BlueConfig from which to get the report
* ``report``: ``string``, Name of the report from where to get the information

Return value:

* ``dictionary`` containing the following entries

  * ``data_unit``: ``string``, Unit of the report values. Can be "mV", "mA", ...
  * ``end_time``: ``number``, Time at which the simulation ends
  * ``frame_count``: ``integer``, Number of simulation frames in the report
  * ``frame_size``: ``integer``, Number of values per frame in the report
  * ``start_time``: ``number``, Time at which the simulation starts
  * ``time_step``: ``number``, Time between two consecutive simulation frames
  * ``time_unit``: ``string``, Unit of the report time values

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_reports
~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_reports(path)

Return a list of reports from a circuit

Parameters:

* ``path``: ``string``, Path to the circuit config to query for reports

Return value:

* ``dictionary`` containing the following entries

  * ``reports``: ``array``, List of report names

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_spike_report_info
~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_spike_report_info(path)

Return wether the circuit has a spike report, and the path to it if exists

Parameters:

* ``path``: ``string``, Path to the circuit to test

Return value:

* ``dictionary`` containing the following entries

  * ``exists``: ``integer``, Flag indicating wether there is a spike report
  * ``path``: ``string``, Path to the spike report

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

ci_get_targets
~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    ci_get_targets(path)

Return a list of targets from a cricuit

Parameters:

* ``path``: ``string``, Path to the circuit config to query for targets

Return value:

* ``dictionary`` containing the following entries:

  * ``targets``: ``array``, List of target names

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.
