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

color_circuit_by_id
~~~~~~~~~~~~~~~~~~~

Colors a circuit model by element ID. Specific IDs can be targetted as single IDs or ID Ranges (being-end),otherwise, random colors per ID will be applied.

Parameters:

* ``color_info``: ``list``. List of IDs with their corresponding color. If empty, all the model will be colored with random colors per ID.

  * ``items``: ``dict``. The object has the following properties.

    * ``color``: ``list``. Color applied to the given variable (Normalized RGBA).

      * ``items``: ``float``

    * ``variable``: ``str``. Variable of the coloring method being used.

* ``model_id``: ``int``. ID of the model to color.

Return value:

This method has no return values.

----

color_circuit_by_method
~~~~~~~~~~~~~~~~~~~~~~~

Colors a circuit model by grouping its elements using the given method. Specific variables can be targetted, otherwise, random colors per variable group will be applied.

Parameters:

* ``color_info``: ``list``. List of IDs with their corresponding color. If empty, all the model will be colored with random colors per ID.

  * ``items``: ``dict``. The object has the following properties.

    * ``color``: ``list``. Color applied to the given variable (Normalized RGBA).

      * ``items``: ``float``

    * ``variable``: ``str``. Variable of the coloring method being used.

* ``method``: ``str``. Method to use for coloring.
* ``model_id``: ``int``. ID of the model to color.

Return value:

This method has no return values.

----

color_circuit_by_single_color
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Colors a whole circuit model with a single color.

Parameters:

* ``color``: ``list``. Color to use for the whole circuit (Normalized RGBA).

  * ``items``: ``float``

* ``model_id``: ``int``. ID of the model to color.

Return value:

This method has no return values.

----

get_circuit_color_method_variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Return the available variables which can be specified when coloring a circuit model by the given method.

Parameters:

* ``method``: ``str``. Name of the method to query.
* ``model_id``: ``int``. ID of the model to query.

Return value:

* ``dict``. The object has the following properties.

  * ``variables``: ``list``. Available variables for the given circuit model and method.

    * ``items``: ``str``

----

get_circuit_color_methods
~~~~~~~~~~~~~~~~~~~~~~~~~

Return the available extra coloring methods for a circuit model.

Parameters:

* ``model_id``: ``int``. ID of the model to query.

Return value:

* ``dict``. The object has the following properties.

  * ``methods``: ``list``. Available coloring methods.

    * ``items``: ``str``

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

* ``id``: ``int``. Model ID.

Return value:

* ``dict``. The object has the following properties.

  * ``ids``: ``list``. List of material ID.

    * ``items``: ``int``

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

set_circuit_thickness
~~~~~~~~~~~~~~~~~~~~~

Modify the geometry radiuses (spheres, cones, cylinders and SDF geometries).

Parameters:

* ``model_id``: ``int``. ID of the circuit model.
* ``radius_multiplier``: ``float``. Scaling factor.

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

* ``id``: ``int``. Model ID.

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

set_simulation_color
~~~~~~~~~~~~~~~~~~~~

Enables or disables the color of a given Circuit Model by its simulation values.

Parameters:

* ``enabled``: ``bool``. If true, coloring by simulation value will be enabled for the given model.
* ``model_id``: ``int``. ID of the model to enable or disable simulation color.

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

