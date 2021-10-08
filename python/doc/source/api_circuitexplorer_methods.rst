Circuit Explorer API methods
----------------------------

The Brayns python client API is automatically generated when connecting to a
running backend service as shown in :ref:`usepythonclient-label`.

The Core plugin is always loaded so the related entrypoints are always
available.

Other plugins might register additional entrypoints but must be loaded in the
renderer to be available.

The functions below are generated using the entrypoints of the Circuit Explorer plugin.

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

add_box
~~~~~~~

Add a visual 3D box to the scene.

Parameters:

* ``color``: ``list``. Box color RGBA normalized.

  * ``items``: ``float``

* ``max_corner``: ``list``. Axis aligned maximum bound of the box.

  * ``items``: ``float``

* ``min_corner``: ``list``. Axis aligned minimum bound of the box.

  * ``items``: ``float``

* ``name``: ``str``. Name to give to the added model.

Return value:

* ``dict``. The object has the following properties.

  * ``id``: ``int``. ID of the model of the shape added.

----

add_column
~~~~~~~~~~

Add a visual 3D column as a cylinder to the scene.

Parameters:

* ``radius``: ``float``. Radius of the cylinder.

Return value:

This method has no return values.

----

add_cylinder
~~~~~~~~~~~~

Add a visual 3D cylinder to the scene.

Parameters:

* ``center``: ``list``. Center of the lower cylinder circumference.

  * ``items``: ``float``

* ``color``: ``list``. Cylinder color RGBA normalized.

  * ``items``: ``float``

* ``radius``: ``float``. Radius of the cylinder.
* ``up``: ``list``. Center of the upper cylinder circumference.

  * ``items``: ``float``

* ``name``: ``str``. Name to give to the added model.

Return value:

* ``dict``. The object has the following properties.

  * ``id``: ``int``. ID of the model of the shape added.

----

add_grid
~~~~~~~~

Add a visual 3D grid to the scene.

Parameters:

* ``max_value``: ``float``. Positive square grid length from world origin.
* ``min_value``: ``float``. Negative square grid length from world origin.
* ``plane_opacity``: ``float``. Opacity of the grid mesh material.
* ``radius``: ``float``. Radius of the cylinder that will be placed at each cell.
* ``show_axis``: ``bool``. Wether to show a world aligned axis.
* ``steps``: ``float``. Number of divisions.
* ``use_colors``: ``bool``. Use colors on the grid axes.

Return value:

This method has no return values.

----

add_pill
~~~~~~~~

Add a visual 3D pill to the scene.

Parameters:

* ``color``: ``list``. Pill color RGBA normalized.

  * ``items``: ``float``

* ``p1``: ``list``. Center of the lower pill circumference.

  * ``items``: ``float``

* ``p2``: ``list``. Center of the upper pill circumference.

  * ``items``: ``float``

* ``radius1``: ``float``. Radius of the lower pill circumference.
* ``radius2``: ``float``. Radius of the upper pill circumference.
* ``type``: ``str``. Type of pill.
* ``name``: ``str``. Name to give to the added model.

Return value:

* ``dict``. The object has the following properties.

  * ``id``: ``int``. ID of the model of the shape added.

----

add_sphere
~~~~~~~~~~

Add a visual 3D sphere to the scene.

Parameters:

* ``center``: ``list``. Sphere center XYZ.

  * ``items``: ``float``

* ``color``: ``list``. Sphere color RGBA normalized.

  * ``items``: ``float``

* ``radius``: ``float``. Radius of the sphere.
* ``name``: ``str``. Name of the sphere model.

Return value:

* ``dict``. The object has the following properties.

  * ``id``: ``int``. ID of the model of the shape added.

----

attach_cell_growth_handler
~~~~~~~~~~~~~~~~~~~~~~~~~~

Attach a dynamic cell growing rendering system for a given model.

Parameters:

* ``model_id``: ``int``. The model to which to attach the handler.
* ``nb_frames``: ``int``. The number of frames to perform the growth.

Return value:

This method has no return values.

----

attach_circuit_simulation_handler
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Dynamically loads and attach a simulation to a loaded model.

Parameters:

* ``circuit_configuration``: ``str``. Simulation configuration file path.
* ``model_id``: ``int``. The model to which to attach the handler.
* ``report_name``: ``str``. The name of the report to attach.

Return value:

This method has no return values.

----

color_cells
~~~~~~~~~~~

Color cells with given colors using their GID.

Parameters:

* ``colors``: ``list``. Cell colors.

  * ``items``: ``float``

* ``gids``: ``list``. Cells to color.

  * ``items``: ``str``

* ``model_id``: ``int``. Model to color.

Return value:

This method has no return values.

----

export_frames_to_disk
~~~~~~~~~~~~~~~~~~~~~

Export a set of frames from a simulation as image files.

Parameters:

* ``animation_information``: ``list``. A list of frame numbers to render.

  * ``items``: ``int``

* ``camera_information``: ``list``. A list of camera definitions. Each camera definition contains origin, direction, up, apperture and radius. (1 entry per animation information entry).

  * ``items``: ``float``

* ``format``: ``str``. The image format (PNG or JPEG).
* ``name_after_step``: ``bool``. Name the file on disk after the simulation step index.
* ``path``: ``str``. Directory to store the frames.
* ``quality``: ``int``. The quality at which the images will be stored.
* ``spp``: ``int``. Samples per pixels.
* ``start_frame``: ``int``. The frame at which to start exporting frames.

Return value:

This method has no return values.

----

get_export_frames_progress
~~~~~~~~~~~~~~~~~~~~~~~~~~

Get the progress of the last issued frame export.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``progress``: ``float``. Progress of the last export 0-1.

----

get_material
~~~~~~~~~~~~

Retreive the material with given ID in given model.

Parameters:

* ``material_id``: ``int``. Material ID.
* ``model_id``: ``int``. Model ID.

Return value:

* ``dict``. The object has the following properties.

  * ``clipping_mode``: ``str``. The choosen material clipping mode.
  * ``diffuse_color``: ``list``. Diffuse reflection color RGB normalized.

    * ``items``: ``float``

  * ``emission``: ``float``. The emissive property of a material.
  * ``glossiness``: ``float``. The glossy component of a material.
  * ``material_id``: ``int``. The ID that identifies this material.
  * ``model_id``: ``int``. The model which this material belongs to.
  * ``opacity``: ``float``. The transparency of the material (0 to 1).
  * ``reflection_index``: ``float``. The index of reflection of the material surface.
  * ``refraction_index``: ``float``. The index of refraction of a transparent material.
  * ``shading_mode``: ``str``. The chosen shading mode.
  * ``simulation_data_cast``: ``bool``. Wether to cast the user parameter for simulation.
  * ``specular_color``: ``list``. Specular reflection RGB normalized.

    * ``items``: ``float``

  * ``specular_exponent``: ``float``. The specular exponent to sharpen the specular reflection.
  * ``user_parameter``: ``float``. A custom parameter passed to the simulation.

----

get_material_ids
~~~~~~~~~~~~~~~~

Retreive the list of ID of the materials in given model.

Parameters:

* ``model_id``: ``int``. Model ID.

Return value:

* ``dict``. The object has the following properties.

  * ``ids``: ``list``. List of material ID.

    * ``items``: ``int``

----

get_odu_camera
~~~~~~~~~~~~~~

Get the properties of the current camera.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``aperture_radius``: ``float``. The camera aperture.
  * ``direction``: ``list``. Camera facing direction normalized.

    * ``items``: ``float``

  * ``focus_distance``: ``float``. Focus distance from the origin.
  * ``origin``: ``list``. Camera position.

    * ``items``: ``float``

  * ``up``: ``list``. Camera up direction normalized.

    * ``items``: ``float``

----

make_movie
~~~~~~~~~~

Builds a movie file from a set of frames stored on disk.

Parameters:

* ``dimensions``: ``list``. Video dimensions (width,height).

  * ``items``: ``int``

* ``erase_frames``: ``bool``. Wether to clean up the frame image files after generating the video file.
* ``fps_rate``: ``int``. The frames per second rate at which to create the video.
* ``frames_file_extension``: ``str``. The extension of the frame files to fetch (ex: png, jpg).
* ``frames_folder_path``: ``str``. Path to where to fetch the frames to create the video.
* ``output_movie_path``: ``str``. The path to where the movie will be created. Must include filename and extension.

Return value:

This method has no return values.

----

mirror_model
~~~~~~~~~~~~

Mirrors a model along a given axis.

Parameters:

* ``mirror_axis``: ``list``. The axis used to mirror.

  * ``items``: ``float``

* ``model_id``: ``int``. Model to mirror.

Return value:

This method has no return values.

----

remap_circuit_color
~~~~~~~~~~~~~~~~~~~

Remap the circuit colors to the specified scheme.

Parameters:

* ``model_id``: ``int``. The model to remap.
* ``scheme``: ``str``. New color scheme.

Return value:

* ``dict``. The object has the following properties.

  * ``updated``: ``bool``. Check if the colors of the model changed.

----

save_model_to_cache
~~~~~~~~~~~~~~~~~~~

Saves given model in a cache file.

Parameters:

* ``model_id``: ``int``. The ID of the model to save.
* ``path``: ``str``. The path to save the cache file.

Return value:

This method has no return values.

----

set_circuit_thickness
~~~~~~~~~~~~~~~~~~~~~

Modify the geometry radiuses (spheres, cones, cylinders and SDF geometries).

Parameters:

* ``model_id``: ``int``. ID of the circuit model.
* ``radius_multiplier``: ``float``. Scaling factor.

Return value:

This method has no return values.

----

set_connections_per_value
~~~~~~~~~~~~~~~~~~~~~~~~~

Draw a point cloud representing the number of connections for a given frame and simulation value.

Parameters:

* ``epsilon``: ``float``. The value epsilon.
* ``frame``: ``int``. The frame of the simulation in which to apply.
* ``model_id``: ``int``. The ID of the model to save.
* ``value``: ``float``. The value.

Return value:

This method has no return values.

----

set_material
~~~~~~~~~~~~

Update the corresponding material with the given properties.

Parameters:

* ``clipping_mode``: ``str``. The choosen material clipping mode.
* ``diffuse_color``: ``list``. Diffuse reflection color RGB normalized.

  * ``items``: ``float``

* ``emission``: ``float``. The emissive property of a material.
* ``glossiness``: ``float``. The glossy component of a material.
* ``material_id``: ``int``. The ID that identifies this material.
* ``model_id``: ``int``. The model which this material belongs to.
* ``opacity``: ``float``. The transparency of the material (0 to 1).
* ``reflection_index``: ``float``. The index of reflection of the material surface.
* ``refraction_index``: ``float``. The index of refraction of a transparent material.
* ``shading_mode``: ``str``. The chosen shading mode.
* ``simulation_data_cast``: ``bool``. Wether to cast the user parameter for simulation.
* ``specular_color``: ``list``. Specular reflection RGB normalized.

  * ``items``: ``float``

* ``specular_exponent``: ``float``. The specular exponent to sharpen the specular reflection.
* ``user_parameter``: ``float``. A custom parameter passed to the simulation.

Return value:

This method has no return values.

----

set_material_extra_attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add extra material attributes necessary for the Circuit Explorer renderer.

Parameters:

* ``model_id``: ``int``. Model ID.

Return value:

This method has no return values.

----

set_material_range
~~~~~~~~~~~~~~~~~~

Update the corresponding materials with common properties.

Parameters:

* ``material_ids``: ``list``. The list of ID that identifies the materials.

  * ``items``: ``int``

* ``model_id``: ``int``. The model which this material belongs to.
* ``properties``: ``dict``. Material properties to apply on all given materials. The object has the following properties.

  * ``clipping_mode``: ``str``. The choosen material clipping mode.
  * ``diffuse_color``: ``list``. Diffuse reflection color RGB normalized.

    * ``items``: ``float``

  * ``emission``: ``float``. The emissive property of a material.
  * ``glossiness``: ``float``. The glossy component of a material.
  * ``opacity``: ``float``. The transparency of the material (0 to 1).
  * ``reflection_index``: ``float``. The index of reflection of the material surface.
  * ``refraction_index``: ``float``. The index of refraction of a transparent material.
  * ``shading_mode``: ``str``. The chosen shading mode.
  * ``simulation_data_cast``: ``bool``. Wether to cast the user parameter for simulation.
  * ``specular_color``: ``list``. Specular reflection RGB normalized.

    * ``items``: ``float``

  * ``specular_exponent``: ``float``. The specular exponent to sharpen the specular reflection.
  * ``user_parameter``: ``float``. A custom parameter passed to the simulation.

Return value:

This method has no return values.

----

set_materials
~~~~~~~~~~~~~

Update the corresponding materials with the given properties.

Parameters:

* ``materials``: ``list``. List of materials to update.

  * ``items``: ``dict``. The object has the following properties.

    * ``clipping_mode``: ``str``. The choosen material clipping mode.
    * ``diffuse_color``: ``list``. Diffuse reflection color RGB normalized.

      * ``items``: ``float``

    * ``emission``: ``float``. The emissive property of a material.
    * ``glossiness``: ``float``. The glossy component of a material.
    * ``material_id``: ``int``. The ID that identifies this material.
    * ``model_id``: ``int``. The model which this material belongs to.
    * ``opacity``: ``float``. The transparency of the material (0 to 1).
    * ``reflection_index``: ``float``. The index of reflection of the material surface.
    * ``refraction_index``: ``float``. The index of refraction of a transparent material.
    * ``shading_mode``: ``str``. The chosen shading mode.
    * ``simulation_data_cast``: ``bool``. Wether to cast the user parameter for simulation.
    * ``specular_color``: ``list``. Specular reflection RGB normalized.

      * ``items``: ``float``

    * ``specular_exponent``: ``float``. The specular exponent to sharpen the specular reflection.
    * ``user_parameter``: ``float``. A custom parameter passed to the simulation.

Return value:

This method has no return values.

----

set_metaballs_per_simulation_value
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add a metaballs model representing the number of connections for a given frame and simulation value.

Parameters:

* ``epsilon``: ``float``. The value epsilon.
* ``frame``: ``int``. The frame with metaballs.
* ``grid_size``: ``int``. The size of a regular grid.
* ``model_id``: ``int``. The model to set metaballs.
* ``threshold``: ``float``. The threshold.
* ``value``: ``float``. The value for the metaballs generation.

Return value:

This method has no return values.

----

set_odu_camera
~~~~~~~~~~~~~~

Set the properties of the current camera.

Parameters:

* ``aperture_radius``: ``float``. The camera aperture.
* ``direction``: ``list``. Camera facing direction normalized.

  * ``items``: ``float``

* ``focus_distance``: ``float``. Focus distance from the origin.
* ``origin``: ``list``. Camera position.

  * ``items``: ``float``

* ``up``: ``list``. Camera up direction normalized.

  * ``items``: ``float``

Return value:

This method has no return values.

----

set_synapses_attributes
~~~~~~~~~~~~~~~~~~~~~~~

Set synapses specific attributes for a given model.

Parameters:

* ``circuit_configuration``: ``str``. Path to the circuit configuration file.
* ``gid``: ``int``. Target cell GID.
* ``html_colors``: ``list``. List of rgb colors in hexadecimal.

  * ``items``: ``str``

* ``light_emission``: ``float``. Emission parameter for the synapse material.
* ``radius``: ``float``. Synapse geometry radius.

Return value:

This method has no return values.

----

trace_anterograde
~~~~~~~~~~~~~~~~~

Performs neuronal tracing showing efferent and afferent synapse relationship between cells (including projections).

Parameters:

* ``cell_gids``: ``list``. List of cell GIDs to use a source of the tracing.

  * ``items``: ``int``

* ``connected_cells_color``: ``list``. RGBA normalized color to apply to the target cells geometry.

  * ``items``: ``float``

* ``model_id``: ``int``. Model where to perform the neuronal tracing.
* ``non_connected_cells_color``: ``list``. RGBA normalized color to apply to the rest of cells.

  * ``items``: ``float``

* ``source_cell_color``: ``list``. RGBA normalized color to apply to the source cell geometry.

  * ``items``: ``float``

* ``target_cell_gids``: ``list``. List of cells GIDs which are the result of the given tracing mode.

  * ``items``: ``int``

Return value:

This method has no return values.

