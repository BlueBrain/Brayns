Core API methods
----------------

The Brayns python client API is automatically generated when connecting to a
running backend service as shown in :ref:`usepythonclient-label`.

The Core plugin is always loaded so the related entrypoints are always
available.

Other plugins might register additional entrypoints but must be loaded in the
renderer to be available.

The functions below are generated using the entrypoints of the Core plugin.

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

add_clip_plane
~~~~~~~~~~~~~~

Add a clip plane and returns the clip plane descriptor.

Parameters:

* ``plane``: ``list``. Plane normal vector XYZ and distance from origin.

  * ``items``: ``float``

Return value:

* ``dict``. The object has the following properties.

  * ``id``: ``int``. Plane ID.
  * ``plane``: ``list``. Plane normal vector XYZ and distance from origin.

    * ``items``: ``float``

----

add_light_ambient
~~~~~~~~~~~~~~~~~

Add an ambient light and return its ID.

Parameters:

* ``color``: ``list``. Light color RGB.

  * ``items``: ``float``

* ``intensity``: ``float``. Light intensity 0-1.
* ``is_visible``: ``bool``. Light is visible.

Return value:

* ``int``

----

add_light_directional
~~~~~~~~~~~~~~~~~~~~~

Add a directional light and return its ID.

Parameters:

* ``angular_diameter``: ``float``. Angular diameter in degrees.
* ``color``: ``list``. Light color RGB.

  * ``items``: ``float``

* ``direction``: ``list``. Light source direction.

  * ``items``: ``float``

* ``intensity``: ``float``. Light intensity 0-1.
* ``is_visible``: ``bool``. Light is visible.

Return value:

* ``int``

----

add_light_quad
~~~~~~~~~~~~~~

Add a quad light and return its ID.

Parameters:

* ``color``: ``list``. Light color RGB.

  * ``items``: ``float``

* ``edge1``: ``list``. First edge XYZ.

  * ``items``: ``float``

* ``edge2``: ``list``. Second edge XYZ.

  * ``items``: ``float``

* ``intensity``: ``float``. Light intensity 0-1.
* ``is_visible``: ``bool``. Light is visible.
* ``position``: ``list``. Light position XYZ.

  * ``items``: ``float``

Return value:

* ``int``

----

add_light_sphere
~~~~~~~~~~~~~~~~

Add a sphere light and return its ID.

Parameters:

* ``color``: ``list``. Light color RGB.

  * ``items``: ``float``

* ``intensity``: ``float``. Light intensity 0-1.
* ``is_visible``: ``bool``. Light is visible.
* ``position``: ``list``. Light position XYZ.

  * ``items``: ``float``

* ``radius``: ``float``. Sphere radius.

Return value:

* ``int``

----

add_light_spot
~~~~~~~~~~~~~~

Add a spot light and return its ID.

Parameters:

* ``color``: ``list``. Light color RGB.

  * ``items``: ``float``

* ``direction``: ``list``. Spot direction XYZ.

  * ``items``: ``float``

* ``intensity``: ``float``. Light intensity 0-1.
* ``is_visible``: ``bool``. Light is visible.
* ``opening_angle``: ``float``. Opening angle in degrees.
* ``penumbra_angle``: ``float``. Penumbra angle in degrees.
* ``position``: ``list``. Light position XYZ.

  * ``items``: ``float``

* ``radius``: ``float``. Spot radius.

Return value:

* ``int``

----

add_model
~~~~~~~~~

Add model from path and return model descriptor on success.

Parameters:

* ``bounding_box``: ``bool``. Display bounding box.
* ``loader_name``: ``str``. Name of the loader.
* ``loader_properties``: ``Any``. Loader properties.
* ``name``: ``str``. Model name.
* ``path``: ``str``. Model file path.
* ``transformation``: ``dict``. Model transformation. The object has the following properties.

  * ``rotation``: ``list``. Rotation XYZW.

    * ``items``: ``float``

  * ``rotation_center``: ``list``. Rotation center XYZ.

    * ``items``: ``float``

  * ``scale``: ``list``. Scale XYZ.

    * ``items``: ``float``

  * ``translation``: ``list``. Translation XYZ.

    * ``items``: ``float``

* ``visible``: ``bool``. Is visible.

Return value:

* ``list``

  * ``items``: ``dict``. The object has the following properties.

    * ``bounding_box``: ``bool``. Display bounding box.
    * ``bounds``: ``dict``. Model bounds. The object has the following properties.

      * ``max``: ``list``. Top-right XYZ.

        * ``items``: ``float``

      * ``min``: ``list``. Bottom-left XYZ.

        * ``items``: ``float``

    * ``id``: ``int``. Model ID.
    * ``loader_name``: ``str``. Name of the loader.
    * ``loader_properties``: ``Any``. Loader properties.
    * ``metadata``: ``dict``. Key-value data.

      * ``items``: ``str``

    * ``name``: ``str``. Model name.
    * ``path``: ``str``. Model file path.
    * ``transformation``: ``dict``. Model transformation. The object has the following properties.

      * ``rotation``: ``list``. Rotation XYZW.

        * ``items``: ``float``

      * ``rotation_center``: ``list``. Rotation center XYZ.

        * ``items``: ``float``

      * ``scale``: ``list``. Scale XYZ.

        * ``items``: ``float``

      * ``translation``: ``list``. Translation XYZ.

        * ``items``: ``float``

    * ``visible``: ``bool``. Is visible.

----

cancel
~~~~~~

Cancel the task started by the request with the given ID.

Parameters:

* ``id``: ``Union[None, int, str]``. ID of the request to cancel. Can be one of the following objects depending on the renderer configuration.

  * ``params``: ``None``
  * ``params``: ``int``
  * ``params``: ``str``

Return value:

* ``None``

----

clear_lights
~~~~~~~~~~~~

Clear all lights in the scene.

Parameters:

This method takes no parameters.

Return value:

* ``None``

----

exit_later
~~~~~~~~~~

Schedules Brayns to shutdown after a given amount of minutes.

Parameters:

* ``minutes``: ``int``. Number of minutes after which Brayns will shut down.

Return value:

* ``None``

----

export_frames
~~~~~~~~~~~~~

Export a list of keyframes as images to disk.

Parameters:

* ``format``: ``str``. Image format ('png' or 'jpg').
* ``image_size``: ``list``. Image dimenssions [width, height].

  * ``items``: ``int``

* ``key_frames``: ``list``. List of keyframes to export.

  * ``items``: ``dict``. The object has the following properties.

    * ``camera``: ``dict``. Camera definition. The object has the following properties.

      * ``current``: ``str``. Camera current type.
      * ``orientation``: ``list``. Camera orientation XYZW.

        * ``items``: ``float``

      * ``position``: ``list``. Camera position XYZ.

        * ``items``: ``float``

      * ``target``: ``list``. Camera target XYZ.

        * ``items``: ``float``

    * ``camera_params``: ``dict``. Camera-specific parameters.
    * ``frame_index``: ``int``. Integer index of the simulation frame.

* ``path``: ``str``. Path where the frames will be stored.
* ``quality``: ``int``. Image quality (100 = highest quality, 0 = lowest quality).
* ``spp``: ``int``. Samples per pixel.
* ``name_after_simulation_index``: ``bool``. Name the frame image file after the simulation frame index.
* ``renderer_name``: ``str``. Name of the renderer to use.
* ``renderer_parameters``: ``dict``. Renderer-specific parameters.
* ``volume_parameters``: ``dict``. Volume rendering parameters. The object has the following properties.

  * ``adaptive_max_sampling_rate``: ``float``. Max sampling rate.
  * ``adaptive_sampling``: ``bool``. Use adaptive sampling.
  * ``clip_box``: ``dict``. Clip box. The object has the following properties.

    * ``max``: ``list``. Top-right XYZ.

      * ``items``: ``float``

    * ``min``: ``list``. Bottom-left XYZ.

      * ``items``: ``float``

  * ``gradient_shading``: ``bool``. Use gradient shading.
  * ``pre_integration``: ``bool``. Use pre-integration.
  * ``sampling_rate``: ``float``. Fixed sampling rate.
  * ``single_shade``: ``bool``. Use a single shade for the whole volume.
  * ``specular``: ``list``. Reflectivity amount XYZ.

    * ``items``: ``float``

  * ``volume_dimensions``: ``list``. Dimensions XYZ.

    * ``items``: ``int``

  * ``volume_element_spacing``: ``list``. Element spacing XYZ.

    * ``items``: ``float``

  * ``volume_offset``: ``list``. Offset XYZ.

    * ``items``: ``float``

Return value:

* ``dict``. The object has the following properties.

  * ``error``: ``int``. Error code (0 = no error).
  * ``message``: ``str``. Message explaining the error.

----

get_animation_parameters
~~~~~~~~~~~~~~~~~~~~~~~~

Get the current state of the animation parameters.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``current``: ``int``. Current frame index.
  * ``dt``: ``float``. Frame time.
  * ``end_frame``: ``int``. Global final simulation frame index.
  * ``start_frame``: ``int``. Global initial simulation frame index.
  * ``unit``: ``str``. Time unit.

----

get_application_parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

Get the current state of the application parameters.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``image_stream_fps``: ``int``. Framerate of image stream.
  * ``jpeg_compression``: ``int``. JPEG compression rate.
  * ``plugins``: ``list``. Loaded plugins.

    * ``items``: ``str``

  * ``viewport``: ``list``. Window size.

    * ``items``: ``int``

----

get_camera
~~~~~~~~~~

Get the current state of the camera.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``current``: ``str``. Camera current type.
  * ``orientation``: ``list``. Camera orientation XYZW.

    * ``items``: ``float``

  * ``position``: ``list``. Camera position XYZ.

    * ``items``: ``float``

  * ``target``: ``list``. Camera target XYZ.

    * ``items``: ``float``

  * ``types``: ``list``. Available camera types.

    * ``items``: ``str``

----

get_camera_params
~~~~~~~~~~~~~~~~~

Get the current properties of the camera.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. Can be one of the following objects depending on the renderer configuration.

  * ``circuit_explorer_dof_perspective``: ``dict``. The object has the following properties.

    * ``apertureRadius``: ``float``
    * ``aspect``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``focusDistance``: ``float``
    * ``fovy``: ``float``

  * ``circuit_explorer_sphere_clipping``: ``dict``. The object has the following properties.

    * ``apertureRadius``: ``float``
    * ``aspect``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``focusDistance``: ``float``
    * ``fovy``: ``float``

  * ``fisheye``: ``dict``. The object has the following properties.

    * ``apertureRadius``: ``float``
    * ``aspect``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``focusDistance``: ``float``
    * ``fovy``: ``float``

  * ``orthographic``: ``dict``. The object has the following properties.

    * ``aspect``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``height``: ``float``

  * ``panoramic``: ``dict``. The object has the following properties.

    * ``enableClippingPlanes``: ``bool``
    * ``half``: ``bool``

  * ``perspective``: ``dict``. The object has the following properties.

    * ``apertureRadius``: ``float``
    * ``aspect``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``focusDistance``: ``float``
    * ``fovy``: ``float``

  * ``perspectiveParallax``: ``dict``. The object has the following properties.

    * ``aspect``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``fovy``: ``float``

----

get_clip_planes
~~~~~~~~~~~~~~~

Get all clip planes.

Parameters:

This method takes no parameters.

Return value:

* ``list``

  * ``items``: ``dict``. The object has the following properties.

    * ``id``: ``int``. Plane ID.
    * ``plane``: ``list``. Plane normal vector XYZ and distance from origin.

      * ``items``: ``float``

----

get_instances
~~~~~~~~~~~~~

Get instances of the given model.

Parameters:

* ``id``: ``int``. Model ID.
* ``result_range``: ``list``. Result list from/to indices.

  * ``items``: ``int``

Return value:

* ``list``

  * ``items``: ``dict``. The object has the following properties.

    * ``bounding_box``: ``bool``. Display bounding box.
    * ``instance_id``: ``int``. Instance ID.
    * ``model_id``: ``int``. Model ID.
    * ``transformation``: ``dict``. Model transformation. The object has the following properties.

      * ``rotation``: ``list``. Rotation XYZW.

        * ``items``: ``float``

      * ``rotation_center``: ``list``. Rotation center XYZ.

        * ``items``: ``float``

      * ``scale``: ``list``. Scale XYZ.

        * ``items``: ``float``

      * ``translation``: ``list``. Translation XYZ.

        * ``items``: ``float``

    * ``visible``: ``bool``. Check if rendered.

----

get_lights
~~~~~~~~~~

Get all lights.

Parameters:

This method takes no parameters.

Return value:

* ``list``

  * ``items``: ``dict``. The object has the following properties.

    * ``id``: ``int``. Light ID.
    * ``properties``: ``dict``. Light properties. Can be one of the following objects depending on the renderer configuration.

      * ``directional``: ``dict``. The object has the following properties.

        * ``angular_diameter``: ``float``. Angular diameter in degrees.
        * ``color``: ``list``. Light color RGB.

          * ``items``: ``float``

        * ``direction``: ``list``. Light source direction.

          * ``items``: ``float``

        * ``intensity``: ``float``. Light intensity 0-1.
        * ``is_visible``: ``bool``. Light is visible.

      * ``sphere``: ``dict``. The object has the following properties.

        * ``color``: ``list``. Light color RGB.

          * ``items``: ``float``

        * ``intensity``: ``float``. Light intensity 0-1.
        * ``is_visible``: ``bool``. Light is visible.
        * ``position``: ``list``. Light position XYZ.

          * ``items``: ``float``

        * ``radius``: ``float``. Sphere radius.

      * ``quad``: ``dict``. The object has the following properties.

        * ``color``: ``list``. Light color RGB.

          * ``items``: ``float``

        * ``edge1``: ``list``. First edge XYZ.

          * ``items``: ``float``

        * ``edge2``: ``list``. Second edge XYZ.

          * ``items``: ``float``

        * ``intensity``: ``float``. Light intensity 0-1.
        * ``is_visible``: ``bool``. Light is visible.
        * ``position``: ``list``. Light position XYZ.

          * ``items``: ``float``

      * ``spotlight``: ``dict``. The object has the following properties.

        * ``color``: ``list``. Light color RGB.

          * ``items``: ``float``

        * ``direction``: ``list``. Spot direction XYZ.

          * ``items``: ``float``

        * ``intensity``: ``float``. Light intensity 0-1.
        * ``is_visible``: ``bool``. Light is visible.
        * ``opening_angle``: ``float``. Opening angle in degrees.
        * ``penumbra_angle``: ``float``. Penumbra angle in degrees.
        * ``position``: ``list``. Light position XYZ.

          * ``items``: ``float``

        * ``radius``: ``float``. Spot radius.

      * ``ambient``: ``dict``. The object has the following properties.

        * ``color``: ``list``. Light color RGB.

          * ``items``: ``float``

        * ``intensity``: ``float``. Light intensity 0-1.
        * ``is_visible``: ``bool``. Light is visible.

    * ``type``: ``str``. Light type.

----

get_loaders
~~~~~~~~~~~

Get all loaders.

Parameters:

This method takes no parameters.

Return value:

* ``list``

  * ``items``: ``dict``. The object has the following properties.

    * ``extensions``: ``list``. Supported file extensions.

      * ``items``: ``str``

    * ``inputParametersSchema``: ``Any``. Loader properties.
    * ``name``: ``str``. Loader name.

----

get_model
~~~~~~~~~

Get all the information of the given model.

Parameters:

* ``id``: ``int``. Model ID.

Return value:

* ``dict``. The object has the following properties.

  * ``bounding_box``: ``bool``. Display bounding box.
  * ``bounds``: ``dict``. Model bounds. The object has the following properties.

    * ``max``: ``list``. Top-right XYZ.

      * ``items``: ``float``

    * ``min``: ``list``. Bottom-left XYZ.

      * ``items``: ``float``

  * ``id``: ``int``. Model ID.
  * ``loader_name``: ``str``. Name of the loader.
  * ``loader_properties``: ``Any``. Loader properties.
  * ``metadata``: ``dict``. Key-value data.

    * ``items``: ``str``

  * ``name``: ``str``. Model name.
  * ``path``: ``str``. Model file path.
  * ``transformation``: ``dict``. Model transformation. The object has the following properties.

    * ``rotation``: ``list``. Rotation XYZW.

      * ``items``: ``float``

    * ``rotation_center``: ``list``. Rotation center XYZ.

      * ``items``: ``float``

    * ``scale``: ``list``. Scale XYZ.

      * ``items``: ``float``

    * ``translation``: ``list``. Translation XYZ.

      * ``items``: ``float``

  * ``visible``: ``bool``. Is visible.

----

get_model_properties
~~~~~~~~~~~~~~~~~~~~

Get the properties of the given model.

Parameters:

* ``id``: ``int``. Model ID.

Return value:

* ``dict``

----

get_model_transfer_function
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Get the transfer function of the given model.

Parameters:

* ``id``: ``int``. Model ID.

Return value:

* ``dict``. The object has the following properties.

  * ``colormap``: ``dict``. Colors to map. The object has the following properties.

    * ``colors``: ``list``. Colors to map.

      * ``items``: ``list``

        * ``items``: ``float``

    * ``name``: ``str``. Label of the color map.

  * ``opacity_curve``: ``list``. Control points.

    * ``items``: ``list``

      * ``items``: ``float``

  * ``range``: ``list``. Values range.

    * ``items``: ``float``

----

get_renderer
~~~~~~~~~~~~

Get the current state of the renderer.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``accumulation``: ``bool``. Multiple render passes.
  * ``background_color``: ``list``. Background color RGB.

    * ``items``: ``float``

  * ``current``: ``str``. Current renderer name.
  * ``head_light``: ``bool``. Light source follows camera origin.
  * ``max_accum_frames``: ``int``. Max render passes.
  * ``samples_per_pixel``: ``int``. Samples per pixel.
  * ``subsampling``: ``int``. Subsampling.
  * ``types``: ``list``. Available renderers.

    * ``items``: ``str``

  * ``variance_threshold``: ``float``. Stop accumulation threshold.

----

get_renderer_params
~~~~~~~~~~~~~~~~~~~

Get the current properties of the renderer.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. Can be one of the following objects depending on the renderer configuration.

  * ``basic``: ``dict``
  * ``circuit_explorer_advanced``: ``dict``. The object has the following properties.

    * ``epsilonFactor``: ``float``
    * ``exposure``: ``float``
    * ``fogStart``: ``float``
    * ``fogThickness``: ``float``
    * ``giDistance``: ``float``
    * ``giSamples``: ``int``
    * ``giWeight``: ``float``
    * ``maxBounces``: ``int``
    * ``maxDistanceToSecondaryModel``: ``float``
    * ``samplingThreshold``: ``float``
    * ``shadows``: ``float``
    * ``softShadows``: ``float``
    * ``softShadowsSamples``: ``int``
    * ``useHardwareRandomizer``: ``bool``
    * ``volumeAlphaCorrection``: ``float``
    * ``volumeSpecularExponent``: ``float``

  * ``circuit_explorer_basic``: ``dict``. The object has the following properties.

    * ``alphaCorrection``: ``float``
    * ``exposure``: ``float``
    * ``maxBounces``: ``int``
    * ``maxDistanceToSecondaryModel``: ``float``
    * ``simulationThreshold``: ``float``
    * ``useHardwareRandomizer``: ``bool``

  * ``pathtracer``: ``dict``. The object has the following properties.

    * ``maxContribution``: ``float``
    * ``rouletteDepth``: ``int``

  * ``raycast_Ng``: ``dict``
  * ``raycast_Ns``: ``dict``
  * ``scivis``: ``dict``. The object has the following properties.

    * ``aoDistance``: ``float``
    * ``aoSamples``: ``int``
    * ``aoTransparencyEnabled``: ``bool``
    * ``aoWeight``: ``float``
    * ``oneSidedLighting``: ``bool``
    * ``shadowsEnabled``: ``bool``

----

get_scene
~~~~~~~~~

Get the current state of the scene.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``bounds``: ``dict``. Scene boundary. The object has the following properties.

    * ``max``: ``list``. Top-right XYZ.

      * ``items``: ``float``

    * ``min``: ``list``. Bottom-left XYZ.

      * ``items``: ``float``

  * ``models``: ``list``. All models.

    * ``items``: ``dict``. The object has the following properties.

      * ``bounding_box``: ``bool``. Display bounding box.
      * ``bounds``: ``dict``. Model bounds. The object has the following properties.

        * ``max``: ``list``. Top-right XYZ.

          * ``items``: ``float``

        * ``min``: ``list``. Bottom-left XYZ.

          * ``items``: ``float``

      * ``id``: ``int``. Model ID.
      * ``loader_name``: ``str``. Name of the loader.
      * ``loader_properties``: ``Any``. Loader properties.
      * ``metadata``: ``dict``. Key-value data.

        * ``items``: ``str``

      * ``name``: ``str``. Model name.
      * ``path``: ``str``. Model file path.
      * ``transformation``: ``dict``. Model transformation. The object has the following properties.

        * ``rotation``: ``list``. Rotation XYZW.

          * ``items``: ``float``

        * ``rotation_center``: ``list``. Rotation center XYZ.

          * ``items``: ``float``

        * ``scale``: ``list``. Scale XYZ.

          * ``items``: ``float``

        * ``translation``: ``list``. Translation XYZ.

          * ``items``: ``float``

      * ``visible``: ``bool``. Is visible.

----

get_statistics
~~~~~~~~~~~~~~

Get the current state of the statistics.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``fps``: ``float``. Framerate.
  * ``scene_size_in_bytes``: ``int``. Scene size.

----

get_version
~~~~~~~~~~~

Get Brayns instance version.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``major``: ``int``. Major version.
  * ``minor``: ``int``. Minor version.
  * ``patch``: ``int``. Patch level.
  * ``revision``: ``str``. SCM revision.

----

get_volume_parameters
~~~~~~~~~~~~~~~~~~~~~

Get the current state of the volume parameters.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``adaptive_max_sampling_rate``: ``float``. Max sampling rate.
  * ``adaptive_sampling``: ``bool``. Use adaptive sampling.
  * ``clip_box``: ``dict``. Clip box. The object has the following properties.

    * ``max``: ``list``. Top-right XYZ.

      * ``items``: ``float``

    * ``min``: ``list``. Bottom-left XYZ.

      * ``items``: ``float``

  * ``gradient_shading``: ``bool``. Use gradient shading.
  * ``pre_integration``: ``bool``. Use pre-integration.
  * ``sampling_rate``: ``float``. Fixed sampling rate.
  * ``single_shade``: ``bool``. Use a single shade for the whole volume.
  * ``specular``: ``list``. Reflectivity amount XYZ.

    * ``items``: ``float``

  * ``volume_dimensions``: ``list``. Dimensions XYZ.

    * ``items``: ``int``

  * ``volume_element_spacing``: ``list``. Element spacing XYZ.

    * ``items``: ``float``

  * ``volume_offset``: ``list``. Offset XYZ.

    * ``items``: ``float``

----

image_jpeg
~~~~~~~~~~

Take a snapshot at JPEG format.

Parameters:

This method takes no parameters.

Return value:

* ``dict``. The object has the following properties.

  * ``data``: ``str``. Image data with base64 encoding.

----

image_streaming_mode
~~~~~~~~~~~~~~~~~~~~

Set the image streaming method between automatic or controlled.

Parameters:

* ``type``: ``str``. Stream mode.

Return value:

* ``None``

----

inspect
~~~~~~~

Inspect the scene at x-y position.

Parameters:

* ``position``: ``list``. Position XY (normalized).

  * ``items``: ``float``

Return value:

* ``dict``. The object has the following properties.

  * ``hit``: ``bool``. Check if the position is picked.
  * ``position``: ``list``. Picked position XYZ.

    * ``items``: ``float``

----

model_properties_schema
~~~~~~~~~~~~~~~~~~~~~~~

Get the property schema of the model.

Parameters:

* ``id``: ``int``. Model ID.

Return value:

This method has no return values.

----

quit
~~~~

Quit the application.

Parameters:

This method takes no parameters.

Return value:

* ``None``

----

registry
~~~~~~~~

Retreive the names of all registered entrypoints.

Parameters:

This method takes no parameters.

Return value:

* ``list``

  * ``items``: ``str``

----

remove_clip_planes
~~~~~~~~~~~~~~~~~~

Remove clip planes from the scene given their ids.

Parameters:

* ``ids``: ``list``. Clip planes ID list.

  * ``items``: ``int``

Return value:

* ``None``

----

remove_lights
~~~~~~~~~~~~~

Remove the model(s) from the ID list from the scene.

Parameters:

* ``ids``: ``list``. List of light ID to remove.

  * ``items``: ``int``

Return value:

* ``None``

----

remove_model
~~~~~~~~~~~~

Remove the model(s) from the ID list from the scene.

Parameters:

* ``ids``: ``list``. List of model ID to remove.

  * ``items``: ``int``

Return value:

* ``None``

----

request_model_upload
~~~~~~~~~~~~~~~~~~~~

Request model upload from next binary frame received and return model descriptors on success.

Parameters:

* ``chunks_id``: ``str``. Chunk ID.
* ``size``: ``int``. File size in bytes.
* ``type``: ``str``. File extension or type (MESH, POINTS, CIRCUIT).
* ``bounding_box``: ``bool``. Display bounds.
* ``loader_name``: ``str``. Loader name.
* ``loader_properties``: ``Any``. Loader properties.
* ``name``: ``str``. Model name.
* ``path``: ``str``. Model source path.
* ``transformation``: ``dict``. Model transformation. The object has the following properties.

  * ``rotation``: ``list``. Rotation XYZW.

    * ``items``: ``float``

  * ``rotation_center``: ``list``. Rotation center XYZ.

    * ``items``: ``float``

  * ``scale``: ``list``. Scale XYZ.

    * ``items``: ``float``

  * ``translation``: ``list``. Translation XYZ.

    * ``items``: ``float``

* ``visible``: ``bool``. Visible.

Return value:

* ``list``

  * ``items``: ``dict``. The object has the following properties.

    * ``bounding_box``: ``bool``. Display bounding box.
    * ``bounds``: ``dict``. Model bounds. The object has the following properties.

      * ``max``: ``list``. Top-right XYZ.

        * ``items``: ``float``

      * ``min``: ``list``. Bottom-left XYZ.

        * ``items``: ``float``

    * ``id``: ``int``. Model ID.
    * ``loader_name``: ``str``. Name of the loader.
    * ``loader_properties``: ``Any``. Loader properties.
    * ``metadata``: ``dict``. Key-value data.

      * ``items``: ``str``

    * ``name``: ``str``. Model name.
    * ``path``: ``str``. Model file path.
    * ``transformation``: ``dict``. Model transformation. The object has the following properties.

      * ``rotation``: ``list``. Rotation XYZW.

        * ``items``: ``float``

      * ``rotation_center``: ``list``. Rotation center XYZ.

        * ``items``: ``float``

      * ``scale``: ``list``. Scale XYZ.

        * ``items``: ``float``

      * ``translation``: ``list``. Translation XYZ.

        * ``items``: ``float``

    * ``visible``: ``bool``. Is visible.

----

reset_camera
~~~~~~~~~~~~

Reset the camera to its initial values.

Parameters:

This method takes no parameters.

Return value:

* ``None``

----

schema
~~~~~~

Get the JSON schema of the given entrypoint.

Parameters:

* ``endpoint``: ``str``. Name of the endpoint.

Return value:

* ``dict``. The object has the following properties.

  * ``async``: ``bool``. Check if the entrypoint is asynchronous.
  * ``description``: ``str``. Description of the entrypoint.
  * ``params``: ``list``. Input schema.
  * ``plugin``: ``str``. Name of the plugin that loads the entrypoint.
  * ``returns``: ``Any``. Output schema.
  * ``title``: ``str``. Name of the entrypoint.

----

set_animation_parameters
~~~~~~~~~~~~~~~~~~~~~~~~

Set the current state of the animation parameters.

Parameters:

* ``current``: ``int``. Current frame index.
* ``end_frame``: ``int``. Global final simulation frame index.
* ``start_frame``: ``int``. Global initial simulation frame index.

Return value:

* ``None``

----

set_application_parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

Set the current state of the application parameters.

Parameters:

* ``image_stream_fps``: ``int``. Framerate of image stream.
* ``jpeg_compression``: ``int``. JPEG compression rate.
* ``viewport``: ``list``. Window size.

  * ``items``: ``int``

Return value:

* ``None``

----

set_camera
~~~~~~~~~~

Set the current state of the camera.

Parameters:

* ``current``: ``str``. Camera current type.
* ``orientation``: ``list``. Camera orientation XYZW.

  * ``items``: ``float``

* ``position``: ``list``. Camera position XYZ.

  * ``items``: ``float``

* ``target``: ``list``. Camera target XYZ.

  * ``items``: ``float``

Return value:

* ``None``

----

set_camera_params
~~~~~~~~~~~~~~~~~

Set the current properties of the camera.

Parameters:

* ``params``: ``dict``. Can be one of the following objects depending on the renderer configuration.

  * ``circuit_explorer_dof_perspective``: ``dict``. The object has the following properties.

    * ``apertureRadius``: ``float``
    * ``aspect``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``focusDistance``: ``float``
    * ``fovy``: ``float``

  * ``circuit_explorer_sphere_clipping``: ``dict``. The object has the following properties.

    * ``apertureRadius``: ``float``
    * ``aspect``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``focusDistance``: ``float``
    * ``fovy``: ``float``

  * ``fisheye``: ``dict``. The object has the following properties.

    * ``apertureRadius``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``focusDistance``: ``float``
    * ``fovy``: ``float``

  * ``orthographic``: ``dict``. The object has the following properties.

    * ``enableClippingPlanes``: ``bool``
    * ``height``: ``float``

  * ``panoramic``: ``dict``. The object has the following properties.

    * ``enableClippingPlanes``: ``bool``
    * ``half``: ``bool``

  * ``perspective``: ``dict``. The object has the following properties.

    * ``apertureRadius``: ``float``
    * ``enableClippingPlanes``: ``bool``
    * ``focusDistance``: ``float``
    * ``fovy``: ``float``

  * ``perspectiveParallax``: ``dict``. The object has the following properties.

    * ``enableClippingPlanes``: ``bool``
    * ``fovy``: ``float``

Return value:

* ``None``

----

set_model_properties
~~~~~~~~~~~~~~~~~~~~

Set the properties of the given model.

Parameters:

* ``id``: ``int``. Model ID.
* ``properties``: ``dict``. Model properties.

Return value:

* ``None``

----

set_model_transfer_function
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Set the transfer function of the given model.

Parameters:

* ``id``: ``int``. Model ID.
* ``transfer_function``: ``dict``. Transfer function. The object has the following properties.

  * ``colormap``: ``dict``. Colors to map. The object has the following properties.

    * ``colors``: ``list``. Colors to map.

      * ``items``: ``list``

        * ``items``: ``float``

    * ``name``: ``str``. Label of the color map.

  * ``opacity_curve``: ``list``. Control points.

    * ``items``: ``list``

      * ``items``: ``float``

  * ``range``: ``list``. Values range.

    * ``items``: ``float``

Return value:

* ``None``

----

set_renderer
~~~~~~~~~~~~

Set the current state of the renderer.

Parameters:

* ``accumulation``: ``bool``. Multiple render passes.
* ``background_color``: ``list``. Background color RGB.

  * ``items``: ``float``

* ``current``: ``str``. Current renderer name.
* ``head_light``: ``bool``. Light source follows camera origin.
* ``max_accum_frames``: ``int``. Max render passes.
* ``samples_per_pixel``: ``int``. Samples per pixel.
* ``subsampling``: ``int``. Subsampling.
* ``variance_threshold``: ``float``. Stop accumulation threshold.

Return value:

* ``None``

----

set_renderer_params
~~~~~~~~~~~~~~~~~~~

Set the current properties of the renderer.

Parameters:

* ``params``: ``dict``. Can be one of the following objects depending on the renderer configuration.

  * ``basic``: ``dict``
  * ``circuit_explorer_advanced``: ``dict``. The object has the following properties.

    * ``epsilonFactor``: ``float``
    * ``exposure``: ``float``
    * ``fogStart``: ``float``
    * ``fogThickness``: ``float``
    * ``giDistance``: ``float``
    * ``giSamples``: ``int``
    * ``giWeight``: ``float``
    * ``maxBounces``: ``int``
    * ``maxDistanceToSecondaryModel``: ``float``
    * ``samplingThreshold``: ``float``
    * ``shadows``: ``float``
    * ``softShadows``: ``float``
    * ``softShadowsSamples``: ``int``
    * ``useHardwareRandomizer``: ``bool``
    * ``volumeAlphaCorrection``: ``float``
    * ``volumeSpecularExponent``: ``float``

  * ``circuit_explorer_basic``: ``dict``. The object has the following properties.

    * ``alphaCorrection``: ``float``
    * ``exposure``: ``float``
    * ``maxBounces``: ``int``
    * ``maxDistanceToSecondaryModel``: ``float``
    * ``simulationThreshold``: ``float``
    * ``useHardwareRandomizer``: ``bool``

  * ``pathtracer``: ``dict``. The object has the following properties.

    * ``maxContribution``: ``float``
    * ``rouletteDepth``: ``int``

  * ``raycast_Ng``: ``dict``
  * ``raycast_Ns``: ``dict``
  * ``scivis``: ``dict``. The object has the following properties.

    * ``aoDistance``: ``float``
    * ``aoSamples``: ``int``
    * ``aoTransparencyEnabled``: ``bool``
    * ``aoWeight``: ``float``
    * ``oneSidedLighting``: ``bool``
    * ``shadowsEnabled``: ``bool``

Return value:

* ``None``

----

set_scene
~~~~~~~~~

Set the current state of the scene.

Parameters:

This method takes no parameters.

Return value:

* ``None``

----

set_volume_parameters
~~~~~~~~~~~~~~~~~~~~~

Set the current state of the volume parameters.

Parameters:

* ``adaptive_max_sampling_rate``: ``float``. Max sampling rate.
* ``adaptive_sampling``: ``bool``. Use adaptive sampling.
* ``clip_box``: ``dict``. Clip box. The object has the following properties.

  * ``max``: ``list``. Top-right XYZ.

    * ``items``: ``float``

  * ``min``: ``list``. Bottom-left XYZ.

    * ``items``: ``float``

* ``gradient_shading``: ``bool``. Use gradient shading.
* ``pre_integration``: ``bool``. Use pre-integration.
* ``sampling_rate``: ``float``. Fixed sampling rate.
* ``single_shade``: ``bool``. Use a single shade for the whole volume.
* ``specular``: ``list``. Reflectivity amount XYZ.

  * ``items``: ``float``

* ``volume_dimensions``: ``list``. Dimensions XYZ.

  * ``items``: ``int``

* ``volume_element_spacing``: ``list``. Element spacing XYZ.

  * ``items``: ``float``

* ``volume_offset``: ``list``. Offset XYZ.

  * ``items``: ``float``

Return value:

* ``None``

----

snapshot
~~~~~~~~

Take a snapshot with given parameters.

Parameters:

* ``format``: ``str``. Image format (extension without the dot).
* ``size``: ``list``. Image dimensions.

  * ``items``: ``int``

* ``animation_parameters``: ``dict``. Animation parameters. The object has the following properties.

  * ``current``: ``int``. Current frame index.
  * ``end_frame``: ``int``. Global final simulation frame index.
  * ``start_frame``: ``int``. Global initial simulation frame index.

* ``camera``: ``dict``. Camera parameters. The object has the following properties.

  * ``current``: ``str``. Camera current type.
  * ``orientation``: ``list``. Camera orientation XYZW.

    * ``items``: ``float``

  * ``position``: ``list``. Camera position XYZ.

    * ``items``: ``float``

  * ``target``: ``list``. Camera target XYZ.

    * ``items``: ``float``

* ``file_path``: ``str``. Path if saved on disk. If empty, image will be sentto the client as a base64 encoded image.
* ``name``: ``str``. Name of the snapshot.
* ``quality``: ``int``. Image quality from 0 to 100.
* ``renderer``: ``dict``. Renderer parameters. The object has the following properties.

  * ``accumulation``: ``bool``. Multiple render passes.
  * ``background_color``: ``list``. Background color RGB.

    * ``items``: ``float``

  * ``current``: ``str``. Current renderer name.
  * ``head_light``: ``bool``. Light source follows camera origin.
  * ``max_accum_frames``: ``int``. Max render passes.
  * ``samples_per_pixel``: ``int``. Samples per pixel.
  * ``subsampling``: ``int``. Subsampling.
  * ``variance_threshold``: ``float``. Stop accumulation threshold.

* ``samples_per_pixel``: ``int``. Samples per pixel.
* ``volume_parameters``: ``dict``. Volume parameters. The object has the following properties.

  * ``adaptive_max_sampling_rate``: ``float``. Max sampling rate.
  * ``adaptive_sampling``: ``bool``. Use adaptive sampling.
  * ``clip_box``: ``dict``. Clip box. The object has the following properties.

    * ``max``: ``list``. Top-right XYZ.

      * ``items``: ``float``

    * ``min``: ``list``. Bottom-left XYZ.

      * ``items``: ``float``

  * ``gradient_shading``: ``bool``. Use gradient shading.
  * ``pre_integration``: ``bool``. Use pre-integration.
  * ``sampling_rate``: ``float``. Fixed sampling rate.
  * ``single_shade``: ``bool``. Use a single shade for the whole volume.
  * ``specular``: ``list``. Reflectivity amount XYZ.

    * ``items``: ``float``

  * ``volume_dimensions``: ``list``. Dimensions XYZ.

    * ``items``: ``int``

  * ``volume_element_spacing``: ``list``. Element spacing XYZ.

    * ``items``: ``float``

  * ``volume_offset``: ``list``. Offset XYZ.

    * ``items``: ``float``

Return value:

* ``dict``. The object has the following properties.

  * ``data``: ``str``. Image data with base64 encoding.

----

trigger_jpeg_stream
~~~~~~~~~~~~~~~~~~~

Triggers the engine to stream a frame to the clients.

Parameters:

This method takes no parameters.

Return value:

* ``None``

----

update_clip_plane
~~~~~~~~~~~~~~~~~

Update a clip plane with the given coefficients.

Parameters:

* ``id``: ``int``. Plane ID.
* ``plane``: ``list``. Plane normal vector XYZ and distance from origin.

  * ``items``: ``float``

Return value:

* ``None``

----

update_instance
~~~~~~~~~~~~~~~

Update the model instance with the given values.

Parameters:

* ``bounding_box``: ``bool``. Display bounding box.
* ``instance_id``: ``int``. Instance ID.
* ``model_id``: ``int``. Model ID.
* ``transformation``: ``dict``. Model transformation. The object has the following properties.

  * ``rotation``: ``list``. Rotation XYZW.

    * ``items``: ``float``

  * ``rotation_center``: ``list``. Rotation center XYZ.

    * ``items``: ``float``

  * ``scale``: ``list``. Scale XYZ.

    * ``items``: ``float``

  * ``translation``: ``list``. Translation XYZ.

    * ``items``: ``float``

* ``visible``: ``bool``. Check if rendered.

Return value:

* ``None``

----

update_model
~~~~~~~~~~~~

Update the model with the given values.

Parameters:

* ``id``: ``int``. Model ID.
* ``bounding_box``: ``bool``. Display model bounds.
* ``name``: ``str``. Model name.
* ``transformation``: ``dict``. Model transformation. The object has the following properties.

  * ``rotation``: ``list``. Rotation XYZW.

    * ``items``: ``float``

  * ``rotation_center``: ``list``. Rotation center XYZ.

    * ``items``: ``float``

  * ``scale``: ``list``. Scale XYZ.

    * ``items``: ``float``

  * ``translation``: ``list``. Translation XYZ.

    * ``items``: ``float``

* ``visible``: ``bool``. Model visibility.

Return value:

* ``None``

