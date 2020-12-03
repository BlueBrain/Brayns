Circuit Explorer API Methods
============================

This is the API exposed by the Circuit Explorer plugin. To call this methods, you will need
a ``Client`` object (You can learn how to create one in :ref:`usepythonclinet-label`), as well
as a backend Brayns service that has been launched with the argument:

.. code-block:: console

    --plugin braynsCircuitExplorer

----

add_box
~~~~~~~


Parameters:

.. code-block:: python

    add_box(max_corner, name, color, min_corner)

Adds an axis algined box to the scene

Parameters:

* ``color``: ``array``, A 4 component normalized color (RGBA) to apply to the box surface
* ``max_corner``: ``array``, Axis aligned maximum bound of the box
* ``min_corner``: ``array``, Axis aligned minimum bound of the box
* ``name``: ``string``, Name to give to the added model

Return value:

* ``dictionary`` with the following entries:

  * ``id``: ``integer``, The id of the added model

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

add_column
~~~~~~~~~~


Parameters:

.. code-block:: python

    add_column(radius)

Adds a visual 3D column as a cylinder to the scene

Parameters:

* ``radius``: ``number``, Radium of the cylinder column to add

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

add_cylinder
~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_cylinder(name, up, center, color, radius)

Adds a cylinder to the scene

Parameters:

* ``center``: ``array``, Center of the lower cylinder circunference
* ``color``: ``array``, A 4 component normalized color (RGBA) to apply to the cylinder surface
* ``name``: ``string``, Name to give to the added model
* ``radius``: ``number``, Radius of the cylinder
* ``up``: ``array``, Center of the upper cylinder circunference

Return value:

* ``dictionary`` with the following entries:

  * ``id``: ``integer``, The id of the added model

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

add_grid
~~~~~~~~


Parameters:

.. code-block:: python

    add_grid(use_colors, plane_opacity, max_value, show_axis, steps, radius, min_value)

Adds a visual 3D grid to the scene

Parameters:

* ``max_value``: ``number``, Negative square grid length from world origin.
* ``min_value``: ``number``, Positive square grid length from world origin.
* ``plane_opacity``: ``number``, Opacity of the grid mesh material.
* ``radius``: ``number``, Radius of the cylinder that will be placed at each cell.
* ``show_axis``: ``boolean``, Wether to show a world aligned axis.
* ``steps``: ``number``, Number of divisions.
* ``use_colors``: ``boolean``, Use colors on the grid axes.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_pill
~~~~~~~~


Parameters:

.. code-block:: python

    add_pill(name, radius1, radius2, p1, p2, color, type)

Adds a pill shape to the scene

Parameters:

* ``color``: ``array``, A 4 component normalized color (RGBA) to apply to the pill surface
* ``name``: ``string``, Name to give to the added model
* ``p1``: ``array``, Center of the lower pill circunference
* ``p2``: ``array``, Center of the upper pill circunference
* ``radius1``: ``number``, Radius of the lower pill circunference
* ``radius2``: ``number``, Radius of the upper pill circunference
* ``type``: ``string``, Type of pill (pill, conepill or sigmoidpill)

Return value:

* ``dictionary`` with the following entries:

  * ``id``: ``integer``, The id of the added model

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_sphere
~~~~~~~~~~


Parameters:

.. code-block:: python

    add_sphere(name, color, radius, center)

Adds a visual 3D sphere to the scene

Parameters:

* ``center``: ``array``, The coordinates of the sphere center (X,Y,Z)
* ``color``: ``array``, A 4 component normalized color (RGBA) to apply to the sphere surface
* ``name``: ``string``, Name to give to the added model
* ``radius``: ``number``, Radius of the sphere

Return value:

* ``dictionary`` with the following entries:

  * ``id``: ``integer``, The id of the added model

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

attach_cell_growth_handler
~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    attach_cell_growth_handler(nb_frames, model_id)

Attach a dynamic cell growing rendering system for a given model

Parameters:

* ``model_id``: ``integer``, The model to which to attach the handler
* ``nb_frames``: ``integer``, The number of frames to perform the growth

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

attach_circuit_simulation_handler
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    attach_circuit_simulation_handler(synchronous_mode, model_id, report_name, circuit_configuration)

Dynamically loads and attach a simulation to a loaded model

Parameters:

* ``circuit_configuration``: ``string``, The path to the Blue Brain config file which contains the simulation
* ``model_id``: ``integer``, The model to which to attach the handler
* ``report_name``: ``string``, The name of the report to attach
* ``synchronous_mode``: ``boolean``, Wether to perform the load synchronously (blocking)

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

color_cells
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    color_cells(model_id, gids, colors)

Sets the color of the geometry of cells given by GID.

Parameters:

* ``model_id``: ``integer``, The model to which apply the color
* ``gid``: ``array``, List of GID ranges as string. Each string represents a batch of cells that will be mapped to the color in the same index. Examples of ranges: "80", "1-100,510".
* ``colors``: ``array``, Contiguous list of RGB values to apply. This list must be 3 times the size of the gid list. Example of color list (red, blue): [1,0,0,0,0,1]

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

export_frames_to_disk
~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    export_frames_to_disk(spp, camera_information, quality, animation_information, format, path, start_frame)

Export a set of frames from a simulation as images written to disk

Parameters:

* ``animation_information``: ``array``, A list of frame numbers to render
* ``camera_information``: ``array``, A list of camera definitions. Each camera definition contains origin, direction, up, apperture and radius. (1 entry per animation information entry)
* ``format``: ``string``, The image format (PNG or JPEG)
* ``path``: ``string``, Path to the directory where the frames will be saved
* ``quality``: ``integer``, The quality at which the images will be stored
* ``spp``: ``integer``, Samples per pixels (The more, the better visual result and the slower the rendering)
* ``start_frame``: ``integer``, The frame at which to start exporting frames
* ``name_after_step``: ``bool``, Name the file on disk after the simulation step index.

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

export_layer_to_disk
~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    export_layer_to_disk(name, frames_count, path, data, start_frame)

Export 1 or various layers to disk to be used in composition when generating a movie

Parameters:

* ``data``: ``string``, Base64 layer image data to store on every frame
* ``frames_count``: ``integer``, Number of frames to store, starting at startFrame
* ``name``: ``string``, Name to give to the layer frames
* ``path``: ``string``, Path where to store the frames
* ``start_frame``: ``integer``, The frame number of the first frame to store (For instance: name00025.png

Return value:

* ``dictionary`` with the following entries:

  * ``frames``: ``array``, List of frames that were successfully stored from the last export layer to disk request

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

get_export_frames_progress
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_export_frames_progress()

Returns the progress of the last issued export frames to disk request

Return value:

* ``dictionary`` with the following entries:

  * ``progress``: ``number``, The normalized progress (0.0 to 1.0) of the last export frames to disk request

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

get_material
~~~~~~~~~~~~


Parameters:

.. code-block:: python

    get_material(model_id, material_id)

Returns the properties from the given model and material

Parameters:

* ``material_id``: ``integer``, The id of the material
* ``model_id``: ``integer``, The id of the model

Return value:

* ``dictionary`` with the following entries:

  * ``clipping_mode``: ``integer``, The choosen material clipping mode (0 = no clipping, 1 = clip by plane, 2 = clip by sphere)
  * ``diffuse_color``: ``array``, A 3 component normalized color (RGB) describing the diffuse reflection
  * ``emission``: ``number``, The emissive property of a material
  * ``glossiness``: ``number``, The glossy component of a material
  * ``material_id``: ``integer``, The ID that identifies this material
  * ``model_id``: ``integer``, The model to which this material belongs to
  * ``opacity``: ``number``, The transparency of the material (0 to 1)
  * ``reflection_index``: ``number``, The index of reflection of the material surface
  * ``refraction_index``: ``number``, The index of refraction of a transparent material
  * ``shading_mode``: ``integer``, The choosen shading mode (0 = none, 1 = diffuse, 2 = electron, 3 = cartoon, 4 = electron transparency, 5 = perlin, 6 = diffuse transparency 7 = checker
  * ``simulation_data_cast``: ``boolean``, Wether to cast the user parameter for simulation
  * ``specular_color``: ``array``, A 3 component normalized color (RGB) describing the specular reflection
  * ``specular_exponent``: ``number``, The specular exponent to sharpen the specular reflection
  * ``user_parameter``: ``number``, A custom parameter passed to the simulation

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

get_material_ids
~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    get_material_ids(model_id)

Returns all the material IDs of a given model

Parameters:

* ``model_id``: ``integer``, The id of the model

Return value:

* ``dictionary`` with the following entries:

 * ``ids``: ``array``, The list of material ids

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

get_material_properties
~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    get_material_ids()

Returns all the editable material property names and their data types

Return value:

* ``dictionary`` with the following entries:

 * ``properties``: ``array``, The list of material property names
 * ``property_types``: ``array``, The list of material property data types names


----

get_odu_camera
~~~~~~~~~~~~~~

.. code-block:: python

    get_odu_camera()

Returns the properties of the current camera

Return value:

* ``dictionary`` with the following entries:

  * ``aperture_radius``: ``number``, The camera aperture
  * ``direction``: ``array``, A normalized vector in the direction the camera is facing
  * ``focus_distance``: ``number``, The distance from the origin, in the direction, at which the camera will focus
  * ``origin``: ``array``, The position of the camera
  * ``up``: ``array``, A normalized vector, perpendicular to the direction, that points to the camera upwards

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

make_movie
~~~~~~~~~~


Parameters:

.. code-block:: python

    make_movie(erase_frames, fps_rate, output_movie_path, frames_folder_path, dimensions, frames_file_extension, layers)

Builds a movie file from a set of frames stored on disk

Parameters:

* ``dimensions``: ``array``, Video dimensions (width,height)
* ``erase_frames``: ``boolean``, Wether to clean up the frame image files after generating the video file
* ``fps_rate``: ``integer``, The frames per second rate at which to create the video
* ``frames_file_extension``: ``string``, The extension of the frame files to fetch (png, jpg)
* ``frames_folder_path``: ``string``, Path to where to fetch the frames to create the video
* ``layers``: ``array``, List of layer names to compose in the video. Layer name ``movie`` must be always present.
* ``output_movie_path``: ``string``, The path to where the movie will be created. Must include filename and extension

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

remap_circuit_color
~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    remap_circuit_color(model_id, scheme)

Remap the circuit colors to the specified scheme

Parameters:

* ``model_id``: ``integer``, The model to remap
* ``scheme``: ``string``, Color scheme to remap a circuit to (Possible values: "By id", "By layer", "By mtype", "By etype", "By target")

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

save_model_to_cache
~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    save_model_to_cache(model_id, parsed, path, parse_error)

Builds and saves a Brayns cache model from a given loaded model

Parameters:

* ``model_id``: ``integer``, The model to save to a cache file
* ``parse_error``: ``string``, A descriptive string in case the parse failed
* ``parsed``: ``boolean``, A flag indicating wether the parsing was successful
* ``path``: ``string``, The path to save the cache file

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_connections_per_value
~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_connections_per_value(value, epsilon, model_id, frame)

Draws a point cloud representing the number of connections for a given frame and simulation value

Parameters:

* ``epsilon``: ``number``, The value epsilon
* ``frame``: ``integer``, The frame of the simulation in which to apply
* ``model_id``: ``integer``, The model to which apply the connections per value setting
* ``value``: ``number``, The value

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_material
~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_material(specular_color, specular_exponent, opacity, reflection_index, glossiness, refraction_index, shading_mode, clipping_mode, model_id, material_id, user_parameter, emission, simulation_data_cast, diffuse_color)

Modifies a specific material

Parameters:

* ``clipping_mode``: ``integer``, The choosen material clipping mode (0 = no clipping, 1 = clip by plane, 2 = clip by sphere)
* ``diffuse_color``: ``array``, A 3 component normalized color (RGB) describing the diffuse reflection
* ``emission``: ``number``, The emissive property of a material
* ``glossiness``: ``number``, The glossy component of a material
* ``material_id``: ``integer``, The ID that identifies this material
* ``model_id``: ``integer``, The model to which this material belongs to
* ``opacity``: ``number``, The transparency of the material (0 to 1)
* ``reflection_index``: ``number``, The index of reflection of the material surface
* ``refraction_index``: ``number``, The index of refraction of a transparent material
* ``shading_mode``: ``integer``, The choosen shading mode (0 = none, 1 = diffuse, 2 = electron, 3 = cartoon, 4 = electron transparency, 5 = perlin, 6 = diffuse transparency 7 = checker
* ``simulation_data_cast``: ``boolean``, Wether to cast the user parameter for simulation
* ``specular_color``: ``array``, A 3 component normalized color (RGB) describing the specular reflection
* ``specular_exponent``: ``number``, The specular exponent to sharpen the specular reflection
* ``user_parameter``: ``number``, A custom parameter passed to the simulation

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_material_extra_attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_material_extra_attributes(model_id)

Sets the extra material attributes necessary for the Circuit Explorer renderer

Parameters:

* ``model_id``: ``integer``, The model from which the materials will have the extra attributes setted

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_material_range
~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_material_range(specular_color, specular_exponent, material_ids, opacity, glossiness, reflection_index, refraction_index, shading_mode, clipping_mode, model_id, user_parameter, emission, simulation_data_cast, diffuse_color)

Sets a set of materials of a single model with common material data

Parameters:

* ``clipping_mode``: ``integer``, The choosen material clipping mode (0 = no clipping, 1 = clip by plane, 2 = clip by sphere)
* ``diffuse_color``: ``array``, A 3 component normalized color (RGB) describing the diffuse reflection (minimum 1)
* ``emission``: ``number``, The emissive property of a material
* ``glossiness``: ``number``, The glossy component of a material
* ``material_ids``: ``array``, The IDs that identifies the materials to modify of the given model (an empty list will modify all materials)
* ``model_id``: ``integer``, The model to which these materials belongs to
* ``opacity``: ``number``, The transparency of the material (0 to 1)
* ``reflection_index``: ``number``, The index of reflection of the material surface
* ``refraction_index``: ``number``, The index of refraction of a transparent material
* ``shading_mode``: ``integer``, The choosen shading mode (0 = none, 1 = diffuse, 2 = electron, 3 = cartoon, 4 = electron transparency, 5 = perlin, 6 = diffuse transparency 7 = checker
* ``simulation_data_cast``: ``boolean``, Wether to cast the user parameter for simulation
* ``specular_color``: ``array``, A 3 component normalized color (RGB) describing the specular reflection (minimum 1)
* ``specular_exponent``: ``number``, The specular exponent to sharpen the specular reflection
* ``user_parameter``: ``number``, A custom parameter passed to the simulation

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_materials
~~~~~~~~~~~~~

Parameters:

.. code-block:: python

    set_materials(opacities, material_ids, reflection_indices, specular_exponents, emissions, model_ids, shading_modes, simulation_data_casts, user_parameters, refraction_indices, glossinesses, clipping_modes, specular_colors, diffuse_colors)

Set a set of materials from one or more models

Parameters:

* ``clipping_modes``: ``array``, The choosen material clipping mode (0 = no clipping, 1 = clip by plane, 2 = clip by sphere) (1 per material)
* ``diffuse_colors``: ``array``, A 3 component normalized color (RGB) describing the diffuse reflection (1 per material)
* ``emissions``: ``array``, The emissive property of a material (1 per material)
* ``glossinesses``: ``array``, The glossy component of a material (1 per material)
* ``material_ids``: ``array``, The IDs that identifies these materials (1 per model id)
* ``model_ids``: ``array``, The list of models to which the list of materials belongs to
* ``opacities``: ``array``, The transparency of the material (0 to 1) (1 per material)
* ``reflection_indices``: ``array``, The index of reflection of the material surface (1 per material)
* ``refraction_indices``: ``array``, The index of refraction of a transparent material (1 per material)
* ``shading_modes``: ``array``, The choosen shading mode (0 = none, 1 = diffuse, 2 = electron, 3 = cartoon, 4 = electron transparency, 5 = perlin, 6 = diffuse transparency 7 = checker) (1 per material)
* ``simulation_data_casts``: ``array``, Wether to cast the user parameter for simulation (1 per material)
* ``specular_colors``: ``array``, A 3 component normalized color (RGB) describing the specular reflection (1 per material)
* ``specular_exponents``: ``array``, The specular exponent to sharpen the specular reflection (1 per material)
* ``user_parameters``: ``array``, A custom parameter passed to the simulation (1 per material)

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_metaballs_per_simulation_value
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_metaballs_per_simulation_value(threshold, model_id, grid_size, value, epsilon, frame)

Adds a metaballs model representing the number of connections for a given frame and simulation value

Parameters:

* ``epsilon``: ``number``, The value epsilon
* ``frame``: ``integer``, The frame in which the setting will be applied
* ``grid_size``: ``integer``, The size of a regular grid
* ``model_id``: ``integer``, The model to which apply the metaballs setting
* ``threshold``: ``number``, The threshold
* ``value``: ``number``, The value for the metaballs generation

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

set_odu_camera
~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_odu_camera(up, direction, focus_distance, origin, aperture_radius)

Set the camera in a position and with an specific orientation towards the scene

Parameters:

* ``aperture_radius``: ``number``, The camera aperture
* ``direction``: ``array``, A normalized vector in the direction the camera is facing
* ``focus_distance``: ``number``, The distance from the origin, in the direction, at which the camera will focus
* ``origin``: ``array``, The position of the camera
* ``up``: ``array``, A normalized vector, perpendicular to the direction, that points to the camera upwards

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_synapses_attributes
~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_synapses_attributes(gid, light_emission, circuit_configuration, html_colors, radius)

Sets sypnapse specific attributes for a given model

Parameters:

* ``circuit_configuration``: ``string``, Path to the circuit configuration file
* ``gid``: ``integer``, Target cell GID
* ``html_colors``: ``array``, List of rgb colors in hexadecimal
* ``light_emission``: ``number``, Emission parameter for the synapse material
* ``radius``: ``number``, Synapse geometry radius

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

trace_anterograde
~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    trace_anterograde(model_id, source_cell_color, cell_gids, non_connected_cells_color, connected_cells_color, target_cell_gids)

Performs neuronal tracing; Showing efferent and afferent synapse relationship  between cells (including projections)

Parameters:

* ``cell_gids``: ``array``, List of cell GIDs to use a source of the tracing
* ``connected_cells_color``: ``array``, A 4 component normalized color (RGBA) to apply to the target cells geometry
* ``model_id``: ``integer``, Model where to perform the neuronal tracing
* ``non_connected_cells_color``: ``array``, A 4 component normalized color (RGBA) to apply to the rest of cells
* ``source_cell_color``: ``array``, A 4 component normalized color (RGBA) to apply to the source cell geometry
* ``target_cell_gids``: ``array``, List of cells GIDs which are the result of the given tracing mode

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

update_material_properties
~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    update_material_properties(model_id, material_ids, property_names, property_values)

Update only the specified properties with the specified values for the given model and materials.

Parameters:

* ``model_id``: ``integer``, ID of the model to which apply the material modification
* ``material_ids``: ``array``, List of material ids belonging to the given model. If empty, all materials of the given model will be modified.
* ``property_names``: ``array``, List of property names to update. The names must match those returned by `get_material_properties()`
* ``property_values``: ``array``, List of property values as string corresponding to the specified properties. Must be convertible to the types returned by `get_material_properties()`

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

