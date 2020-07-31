.. _cemodule-label:

Circuit Explorer Module
=======================

The CircuitExplorer Module is a class shipped with the ``brayns`` package.
The main purpose is to encapsulate the API exposed by the Circuit Explorer plugin,
reducing the complexity of the calls and making development easier and faster.

To access the module functionality, we just need to import the module and
instantiate it:

.. code-block:: python

    from brayns import Client, CircuitExplorer

    # Stablish a connection with the remote server running on "..."
    braynsClient = Client("...")

    # Instantiate the module interface
    circuitExplorer = CircuitExplorer(braynsClient)

For the module calls to be successful, the Brayns backend service must be started
using the following parameter:

.. code-block:: console

    --plugin braynsCircuitExplorer

The module exoposes the following API.

----

add_box
~~~~~~~

.. code-block:: python

    add_box(minCorner, maxCorner, color, name='')

Creates and adds a box shape to the scene


Parameters:

* ``minCorner``: ``list``,  Position of the minimun corner of the box.
* ``maxCorner``: ``list``,  Position of the maximum corner of the box.
* ``color``: ``list``,  Color with transparency of the box (RGBA).
* ``name``: ``str``,  Name to give to the model to add.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

add_column
~~~~~~~~~~

.. code-block:: python

    add_column(radius=0.01)

Adds a reference column to the scene


Parameters:

* ``radius``: ``float``,  Radius of column spheres, cylinders radii are half size.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

add_conepill
~~~~~~~~~~~~

.. code-block:: python

    add_conepill(p1, p2, radius1, radius2, color, name='')

Creates and adds a cone pill shape to the scene


Parameters:

* ``p1``: ``list``,  Position (in global coordinates) of the first pivot.
* ``p2``: ``list``,  Position (in global coordinates) of the second pivot.
* ``radius1``: ``float``,  Radius to use around p1.
* ``radius2``: ``float``,  Radius to use around p2.
* ``color``: ``list``,  Color with transparency of the cone pill (RGBA).
* ``name``: ``str``,  Name to give to the model to add.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_cylinder
~~~~~~~~~~~~

.. code-block:: python

    add_cylinder(center, up, radius, color, name='')

Creates and adds a cylinder shape to the scene


Parameters:

* ``center``: ``list``,  Position of the center of the base of the cylinder.
* ``up``: ``list``,  Position of the center of the top of the cylinder.
* ``radius``: ``float``,  Radius of the cylinder.
* ``color``: ``list``,  Color with transparency of the cylinder (RGBA).
* ``name``: ``str``,  Name to give to the model to add.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_grid
~~~~~~~~

.. code-block:: python

    add_grid(min_value, max_value, interval, radius=1.0,
             opacity=0.5, show_axis=True, colored=True)

Adds a reference grid to the scene


Parameters:

* ``min_value``: ``float``,  Minimum value for all axis.
* ``max_value``: ``float``,  Maximum value for all axis.
* ``interval``: ``float``,  Interval at which lines should appear on the grid.
* ``radius``: ``float``,  Radius of grid lines.
* ``opacity``: ``float``,  Opacity of the grid.
* ``show_axis``: ``bool``,  Shows axis if True.
* ``colored``: ``bool``,  Colors the grid it True. X in red, Y in green, Z in blue.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_pill
~~~~~~~~

.. code-block:: python

    add_pill(p1, p2, radius, color, name='')

Creates and adds a pill shape to the scene


Parameters:

* ``p1``: ``list``,  Position (in global coordinates) of the first pivot.
* ``p2``: ``list``,  Position (in global coordinates) of the second pivot.
* ``radius``: ``float``,  Radius of the pill sides.
* ``color``: ``list``,  Color with transparency of the pill (RGBA).
* ``name``: ``str``,  Name to give to the model to add.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

add_sigmoidpill
~~~~~~~~~~~~~~~

.. code-block:: python

    add_sigmoidpill(p1, p2, radius1, radius2, color, name='')

Creates and adds a sigmoid pill (smoothed) shape to the scene


Parameters:

* ``p1``: ``list``,  Position (in global coordinates) of the first pivot.
* ``p2``: ``list``,  Position (in global coordinates) of the second pivot.
* ``radius1``: ``float``,  Radius to use around p1.
* ``radius2``: ``float``,  Radius to use around p2.
* ``color``: ``list``,  Color with transparency of the sigmoid pill (RGBA).
* ``name``: ``str``,  Name to give to the model to add.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_sphere
~~~~~~~~~~

.. code-block:: python

    add_sphere(center, radius, color, name='')

Creates and adds a sphere to the scene


Parameters:

* ``center``: ``list``,  Position (in global coordinates) of the sphere center.
* ``radius``: ``float``,  Radius of the sphere.
* ``color``: ``list``,  Color with transparency of the sphere (RGBA).
* ``name``: ``str``,  Name to give to the model to add.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

cancel_frames_export
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    cancel_frames_export()

Cancel the exports of frames to disk

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

export_frames_to_disk
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    export_frames_to_disk(path, animation_frames, camera_definitions,
                          image_format='png', quality=100, samples_per_pixel=1,
                          start_frame=0)

Exports frames to disk. Frames are named using a 6 digit representation of the frame number
This is used to resume the rendering of a previously canceled sequence)


Parameters:

* ``path``: ``str``,  Folder into which frames are exported.
* ``animation_frames``: ``list``,  List of animation frames.
* ``camera_definitions``: ``list``,  List of camera definitions (origin, direction and up).
* ``image_format``: ``str``,  Image format (the ones supported par Brayns: PNG, JPEG, etc).
* ``quality``: ``float``,  Quality of the exported image (Between 0 and 100).
* ``samples_per_pixel``: ``int``,  Number of samples per pixels.
* ``start_frame``: ``int``,  Optional value if the rendering should start at a specific frame.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

get_camera
~~~~~~~~~~

.. code-block:: python

    get_camera()

Gets the origin, direction and up vector of the camera


Return value:

* ``dict``, with the following entries about the camera:

  * ``origin``: ``list`` of floats, 3D position of the camera.
  * ``direction``: ``list`` of floats, normalized direction of the camera in world space.
  * ``up``: ``list`` of floats, normalized up direction of the camera in world space.
  * ``aperture_radius``: ``float``, camera lens radius aperture.
  * ``focus_distance``: ``float``, distance from the camera in the direction of ``direction`` at which the camera is focused.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

get_export_frames_progress
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_export_frames_progress()

Queries the progress of the last export of frames to disk request.


Return value:

* ``dict``, Dictionary with the entries:

  * ``progress``: ``float``, normalized frame export progress. 1.0 or higher means the export has finished.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

get_material_ids
~~~~~~~~~~~~~~~~

.. code-block:: python

    get_material_ids(model_id)

Return the list of material ids for the given model


Parameters:

* ``model_id``: ``int``,  Id of the model to query for material ids.

Return value:

* ``dictionary`` conntaining the following entries:

  * ``ids``: ``list`` of integers, List of material ids.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

load_circuit
~~~~~~~~~~~~

.. code-block:: python

    load_circuit(path, name='Circuit', density=100.0, gids=[], random_seed=0,
                 targets=[], report='', report_type='Voltages from file',
                 user_data_type='Simulation offset', synchronous_mode=True,
                 circuit_color_scheme='None', mesh_folder='',
                 mesh_filename_pattern='', mesh_transformation=False,
                 radius_multiplier=1, radius_correction=0, load_soma=True,
                 load_axon=True, load_dendrite=True, load_apical_dendrite=True,
                 use_sdf=False, dampen_branch_thickness_changerate=True,
                 use_metaballs_for_soma=False, metaballs_section_samples=5,
                 metaballs_grid_size=20, metaballs_threshold=1,
                 morphology_color_scheme='None', morphology_quality=2,
                 max_distance_to_soma=1000000.0, cell_clipping=False,
                 load_afferent_synapses=False, load_efferent_synapses=False,
                 synapse_radius=0.0)

Load a circuit from a give Blue/Circuit configuration file.


Parameters:

* ``path``: ``str``,  Path to the CircuitConfig or BlueConfig configuration file.
* ``name``: ``str``,  Name of the model.
* ``density``: ``float``,  Circuit density (Value between 0 and 100).
* ``gids``: ``list``,  List of GIDs to load.
* ``random_seed``: ``int``,  Random seed used if circuit density is different from 100.
* ``targets``: ``list``,  List of targets to load.
* ``report``: ``str``,  Name of the simulation report, if applicable.
* ``report_type``: ``int``,  Report type (REPORT_TYPE_NONE, REPORT_TYPE_VOLTAGES_FROM_FILE, REPORT_TYPE_SPIKES).
* ``user_data_type``: ``int``,  Type of data mapped to the neuron surface (USER_DATATYPE_NONE, USER_DATATYPE_SIMULATION_OFFSET, USER_DATATYPE_DISTANCE_TO_SOMA).
* ``synchronous_mode``: ``bool``,  Defines if the simulation report should be loaded synchronously or not.
* ``circuit_color_scheme``: ``int``,  Color scheme to apply to the circuit ( CIRCUIT_COLOR_SCHEME_NONE, CIRCUIT_COLOR_SCHEME_NEURON_BY_ID, CIRCUIT_COLOR_SCHEME_NEURON_BY_LAYER, CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE, CIRCUIT_COLOR_SCHEME_NEURON_BY_ETYPE, CIRCUIT_COLOR_SCHEME_NEURON_BY_TARGET).
* ``mesh_folder``: ``str``,  Folder containing meshes (if applicable).
* ``mesh_filename_pattern``: ``str``,  Filename pattern used to load the meshes ({guid} is replaced by the correponding GID during the loading of the circuit. e.g. mesh_{gid}.obj).
* ``mesh_transformation``: ``bool``,  Boolean defining is circuit transformation should be applied to the meshes.
* ``radius_multiplier``: ``float``,  Multiplies morphology radius by the specified value.
* ``radius_correction``: ``float``,  Forces morphology radii to the specified value.
* ``load_soma``: ``bool``,  Defines if the somas should be loaded.
* ``load_axon``: ``bool``,  Defines if the axons should be loaded.
* ``load_dendrite``: ``bool``,  Defines if the dendrites should be loaded.
* ``load_apical_dendrite``: ``bool``,  Defines if the apical dendrites should be loaded.
* ``use_sdf``: ``bool``,  Defines if signed distance field geometries should be used.
* ``dampen_branch_thickness_changerate``: ``bool``,  Defines if the dampen branch thicknesschangerate option should be used (Only application is use_sdf is True).
* ``use_metaballs_for_soma``: ``bool``,  Defines if metaballs should be used to build the soma.
* ``metaballs_section_samples``: ``int``,  Defines how many sections from the soma should be used to build the soma with metaballs (Only application if use_metaballs_for_soma is True).
* ``metaballs_grid_size``: ``int``,  Defines the size of grid to build the soma with metaballs ( Only application if use_metaballs_for_soma is True).
* ``metaballs_threshold``: ``float``,  Defines the threshold to build the soma with metaballs ( Only application if use_metaballs_for_soma is True).
* ``morphology_color_scheme``: ``int``,  Defines the color scheme to apply to the morphologies ( MORPHOLOGY_COLOR_SCHEME_NONE, MORPHOLOGY_COLOR_SCHEME_BY_SECTION_TYPE).
* ``morphology_quality``: ``int``,  Defines the level of quality for each geometry ( GEOMETRY_QUALITY_LOW, GEOMETRY_QUALITY_MEDIUM, GEOMETRY_QUALITY_HIGH).
* ``max_distance_to_soma``: ``float``,  Defines the maximum distance to the soma for section/ segment loading (This is used by the growing neurons use-case).
* ``cell_clipping``: ``bool``,  Only load cells that are in the clipped region defined at the scene level.
* ``load_afferent_synapses``: ``bool``,  Load afferent synapses.
* ``load_efferent_synapses``: ``bool``,  Load efferent synapses.
* ``synapse_radius``: ``float``,  Synapse radius.

Return value:

* ``dict``, Circuit metadata, with the following entries:

  * ``bounding_box``: ``bool``, wether the circuit bounding box was provided by the user. False mean generated by Brayns.
  * ``bounds``: ``dictionary``, circuit bounding box, with entries:

    * ``min``: ``list`` of floats, 3D space position of the minimun bound
    * ``max``: ``list`` of floats, 3D space position of the maximun bound

  * ``visible``: ``bool``, flag indicating wether the model is visible during rendering.
  * ``metadata``: ``dictionary`` with circuit information:

    * ``CircuitPath``: ``str``, path to the CircuitConfig or BlueConfig from where the circuit was loaded.
    * ``Color scheme``: ``str``, string representation of the current circuit color schema.
    * ``Density``: ``float``, normalized loaded density.
    * ``GIDs``: ``list`` of integers. If the circuit was loaded specifying a list of GIDs, these will be present in this field.
    * ``Mesh filename pattern``: ``str``, pattern used to find cell mesh files.
    * ``Mesh folder``: ``str``, path to the folder where the cell meshes are stored.
    * ``Morphology quality``: ``str``, string representation of the morphology quality setting used to load the circuit.
    * ``Number of neurons``: ``str``, integer indicating the number of neuron cells loaded.
    * ``RandomSeed``: ``str``, float number with the random seed used to load the circuit.
    * ``Report``: ``str``, name of the comartment report loaded, if any.
    * ``Report type``: ``str``, string representation of the type of report loaded, if any.
    * ``Targets``: ``list`` of strings, the list of targets specified when loading the circuit, if any.
    * ``Use simulation model``: ``str``, Off if the are no meshes to load for simulation, On otherwise.

  * ``name``: ``str``, name given to the circuit on the scene.
  * ``path``: ``str``, path to the CircuitConfig or BlueConfig from where the circuit was loaded.
  * ``transformation``: ``dict`` with the circuit's affine transformation:

    * ``rotation``: ``list`` of floats, a quaternion representing the circuit rotation.
    * ``rotation_center``: ``list`` of floats, a 3D space position around which the rotations are performed.
    * ``translation``: ``list`` of floats, a 3D space translation.
    * ``scale``: ``list`` of floats, a 3D scalation.

----

make_movie
~~~~~~~~~~

.. code-block:: python

    make_movie(output_movie_path, fps_rate, frames_folder_path,
               frame_file_extension='png', dimensions=[1920, 1080],
               erase_frames=True)

Request to create a media video file from a set of frames


Parameters:

* ``output_movie_path``: ``str``,  Full path to the media video to store the movie (it will be created if it does not exists). It must include extension, as it will be used to determine the codec to be used (By default it should be .mp4).
* ``fps_rate``: ``int``,  Desired frame rate in the video.
* ``frames_folder_path``: ``str``,  Path to the folder containing the frames to be used to create the video.
* ``frame_file_extension``: ``str``,  Image format to use (png, jpg).
* ``dimensions``: ``list``,  Desired width and height of the video to be created.
* ``erase_frames``: ``bool``,  Wether to remove the frames after the video.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

save_model_to_cache
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    save_model_to_cache(model_id, path)

Save a model to the specified cache file


Parameters:

* ``model_id``: ``int``,  Id of the model to save.
* ``path``: ``str``,  Path of the cache file.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

set_camera
~~~~~~~~~~

.. code-block:: python

    set_camera(origin, direction, up)

Sets the camera using origin, direction and up vectors


Parameters:

* ``origin``: ``list``,  Origin of the camera.
* ``direction``: ``list``,  Direction in which the camera is looking.
* ``up``: ``list``,  Up vector.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

set_material
~~~~~~~~~~~~

.. code-block:: python

    set_material(model_id, material_id, diffuse_color=(1.0, 1.0, 1.0),
                 specular_color=(1.0, 1.0, 1.0), specular_exponent=20.0,
                 opacity=1.0, reflection_index=0.0, refraction_index=1.0,
                 simulation_data_cast=True, glossiness=1.0, shading_mode=0,
                 emission=0.0, clipping_mode=0, user_parameter=0.0)

Set a material on a specified model


Parameters:

* ``model_id``: ``int``,  ID of the model.
* ``material_id``: ``int``,  ID of the material.
* ``diffuse_color``: ``list``,  Diffuse color (3 values between 0 and 1).
* ``specular_color``: ``list``,  Specular color (3 values between 0 and 1).
* ``specular_exponent``: ``list``,  Diffuse exponent.
* ``opacity``: ``float``,  Opacity.
* ``reflection_index``: ``float``,  Reflection index (value between 0 and 1).
* ``refraction_index``: ``float``,  Refraction index.
* ``simulation_data_cast``: ``bool``,  Casts simulation information.
* ``glossiness``: ``float``,  Glossiness (value between 0 and 1).
* ``shading_mode``: ``int``,  Shading mode (SHADING_MODE_NONE, SHADING_MODE_DIFFUSE, SHADING_MODE_ELECTRON, SHADING_MODE_CARTOON, SHADING_MODE_ELECTRON_TRANSPARENCY, SHADING_MODE_PERLIN or SHADING_MODE_DIFFUSE_TRANSPARENCY).
* ``emission``: ``float``,  Light emission intensity.
* ``clipping_mode``: ``bool``,  Clipped against clipping planes/spheres defined at the scene level.
* ``user_parameter``: ``float``,  Convenience parameter used by some of the shaders.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

set_material_extra_attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_material_extra_attributes(model_id)

Add extra attributes to all materials in the model (shading mode, clipped, etc)


Parameters:

* ``model_id``: ``int``,  Id of the model.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

set_material_range
~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_material_range(model_id, material_ids, diffuse_color=(1.0, 1.0, 1.0),
                       specular_color=(1.0, 1.0, 1.0), specular_exponent=20.0,
                       opacity=1.0, reflection_index=0.0, refraction_index=1.0,
                       simulation_data_cast=True, glossiness=1.0, shading_mode=0,
                       emission=0.0, clipping_mode=0, user_parameter=0.0)

Set a range of materials on a specified model


Parameters:

* ``model_id``: ``int``,  ID of the model.
* ``material_ids``: ``list``,  IDs of the material to change.
* ``diffuse_color``: ``list``,  Diffuse color (3 values between 0 and 1).
* ``specular_color``: ``list``,  Specular color (3 values between 0 and 1).
* ``specular_exponent``: ``list``,  Diffuse exponent.
* ``opacity``: ``float``,  Opacity.
* ``reflection_index``: ``float``,  Reflection index (value between 0 and 1).
* ``refraction_index``: ``float``,  Refraction index.
* ``simulation_data_cast``: ``bool``,  Casts simulation information.
* ``glossiness``: ``float``,  Glossiness (value between 0 and 1).
* ``shading_mode``: ``int``,  Shading mode (SHADING_MODE_NONE, SHADING_MODE_DIFFUSE, SHADING_MODE_ELECTRON, SHADING_MODE_CARTOON, SHADING_MODE_ELECTRON_TRANSPARENCY, SHADING_MODE_PERLIN or SHADING_MODE_DIFFUSE_TRANSPARENCY).
* ``emission``: ``float``,  Light emission intensity.
* ``clipping_mode``: ``bool``,  Clipped against clipping planes/spheres defined at the scene level.
* ``user_parameter``: ``float``,  Convenience parameter used by some of the shaders.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

set_materials
~~~~~~~~~~~~~

.. code-block:: python

    set_materials(model_ids, material_ids, diffuse_colors, specular_colors,
                  specular_exponents=[], opacities=[], reflection_indices=[],
                  refraction_indices=[], simulation_data_casts=[], glossinesses=[],
                  shading_modes=[], emissions=[], clipping_modes=[],
                  user_parameters=[])

Set a list of material on a specified list of models


Parameters:

* ``model_ids``: ``int``,  IDs of the models.
* ``material_ids``: ``int``,  IDs of the materials.
* ``diffuse_colors``: ``list``,  List of diffuse colors (3 values between 0 and 1).
* ``specular_colors``: ``list``,  List of specular colors (3 values between 0 and 1).
* ``specular_exponents``: ``list``,  List of diffuse exponents.
* ``opacities``: ``list``,  List of opacities.
* ``reflection_indices``: ``list``,  List of reflection indices (value between 0 and 1).
* ``refraction_indices``: ``list``,  List of refraction indices.
* ``simulation_data_casts``: ``list``,  List of cast simulation information.
* ``glossinesses``: ``list``,  List of glossinesses (value between 0 and 1).
* ``shading_modes``: ``list``,  List of shading modes (SHADING_MODE_NONE, SHADING_MODE_DIFFUSE, SHADING_MODE_ELECTRON, SHADING_MODE_CARTOON, SHADING_MODE_ELECTRON_TRANSPARENCY, SHADING_MODE_PERLIN or SHADING_MODE_DIFFUSE_TRANSPARENCY).
* ``emissions``: ``list``,  List of light emission intensities.
* ``clipping_modes``: ``list``,  List of clipping modes defining if materials should be clipped against clipping planes, spheres, etc, defined at the scene level.
* ``user_parameters``: ``list``,  List of convenience parameter used by some of the shaders.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

trace_anterograde
~~~~~~~~~~~~~~~~~

.. code-block:: python

    trace_anterograde(model_id, source_cells_gid, target_cells_gid,
                      source_cells_color=(5, 5, 0, 1), target_cells_color=(5, 0, 0, 1),
                      non_connected_color=(0.5, 0.5, 0.5, 1.0))

Stain the cells based on their synapses


Parameters:

* ``model_id``: ``int``,  ID of the model to trace.
* ``source_cells_gid``: ``list``,  list of cell GIDs as source of the connections.
* ``target_cells_gid``: ``list``,  list of cell GIDs connected to the source(s).
* ``source_cell_color``: ``list``,  RGBA 4 floating point list as color for source cells.
* ``target_cell_color``: ``list``,  RGBA 4 floating point list as color for target cells.
* ``non_connected_color``: ``list``,  RGBA 4 floating point list as color for non connected cells.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.
