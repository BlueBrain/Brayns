Core API Methods
----------------

The Brayns python client API is automatically generated when connecting to a running
backend service as shown in :ref:`usepythonclient-label`. This allows the client to
get only the available implementation that is exposed by the service we are connecting to.

All the exposed API by the backend, including the API entries added by the plugins, are
available throught this self-generated API. Nonetheless, some plugins offer specialized
python classes shipped with the ``brayns`` package, that eases the use of their functionality.

The core API refers to the available functions exposed by Brayns alone, without taking into
account any loaded plugin.

All the function parameters are keyword arguments, meaning the appearance order does not matter.

----

add_clip_plane
~~~~~~~~~~~~~~

.. code-block:: python

    add_clip_plane(array)

Add a clip plane; returns the clip plane descriptor.

Parameters:

* ``array``: 4 component ``float`` array. The 3 first components are a normalized vector perpendicular to the clipping plane. The last argument is the distance from the scene origin at which the plane starts clipping geometry.

Return value:

* ``dictionary`` with the plane descriptor. Members:

  * ``id``: ``Integer``, ID of the plane, used for removing the plane.
  * ``plane``: ``array of floats``: The plane description sent in the ``add_clip_plane`` request.

----

add_light_ambient
~~~~~~~~~~~~~~~~~

.. code-block:: python

    add_light_ambient(color, intensity, is_visible)

Adds an ambient light to the scene. Ambient lights cast lights everywhere, in every direction.

Parameters:

* ``color``: 3 component float array. The RGB normalized color of the light.
* ``intensity``: ``Float``, Intensity of the light
* ``is_visible``: ``Bool``, Wether this light is used in rendering or not

Return value:

* ``integer`` with the ID of the light.

----

add_light_directional
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    add_light_directional(color, intensity, is_visible, direction, angularDiameter)

AddS a directional light to the scene. Directional lights cast parallel light rays in a given direction (Sun-like lighting)
form an infinitesimal point on space (they have no area).

Parameters:

* ``color``: 3 component float array. The RGB normalized color of the light.
* ``intensity``: ``Float``, Intensity of the light
* ``is_visible``: ``Bool``, Wether this light is used in rendering or not
* ``direction``: 3 component float array. Normalized direction in world space that the light will cast light.
* ``angularDiameter``: ``Float``, Angular diameter to apply a cone shaped lighting from the direction. Usually, this parameter is left to 0.

Return value:

* ``integer`` with the ID of the light.

----

add_light_quad
~~~~~~~~~~~~~~

.. code-block:: python

    add_light_quad(color, position, edge1, edge2, intensity, is_visible)

Adds a quad light. Adds a square-shaped area light to the scene that allows to produce soft shadows and emulate real world
lights. Emits light on the direction the quad normal is facing.

Parameters:

* ``color``: 3 component ``float`` array. The RGB normalized color of the light.
* ``position``: 3 component ``float`` array. Position of the lower left corner of the square.
* ``edge1``: 3 component ``float`` array. Position of the lower right corner of the square.
* ``edge2``: 3 component ``float`` array. Position of the top left corner of the square.
* ``intensity``: ``Float``, Intensity of the light
* ``is_visible``: ``Bool``, Wether this light is used in rendering or not

Return value:

* ``integer`` with the ID of the light.

----

add_light_sphere
~~~~~~~~~~~~~~~~

.. code-block:: python

    add_light_sphere(color, position, radius, intensity, is_visible)

Adds a sphere light. Adds a sphere-shaped area light to the scene that allows to produce soft shadows and emulate real world
lights. Emits lights on every direction.

Parameters:

* ``color``: 3 component ``float`` array. The RGB normalized color of the light.
* ``position``: 3 component ``float`` array. Position of the sphere center.
* ``radius``: ``Float``, Radius of the sphere shape.
* ``intensity``: ``Float``, Intensity of the light
* ``is_visible``: ``Bool``, Wether this light is used in rendering or not

Return value:

* ``integer`` with the ID of the light.

----

add_light_spot
~~~~~~~~~~~~~~

.. code-block:: python

    add_light_spot(color, direction, intensity, is_visible, openingAngle, penumbraAngle, position, radius)

Add a spotlight to the scene. Adds a cone shaped light to the scene, such as a `spot light`.

Parameters:

* ``color``: 3 component ``float`` array. The RGB normalized color of the light.
* ``direction``: 3 component float array. Normalized direction in world space that the light will cast light.
* ``intensity``: ``Float``, Intensity of the light
* ``is_visible``: ``Bool``, Wether this light is used in rendering or not
* ``openingAngle``: ``Float``, Main light cone shaft angle to cast light.
* ``penumbraAngle``: ``Float``, Second light cone shaft in which the light transitions from max intensity to 0.
* ``position``: 3 component ``float`` array with the world space position of the light.
* ``radius``: ``Float``, radius of the light emitter.

Return value:

* ``integer`` with the ID of the light.

----

add_model
~~~~~~~~~

.. code-block:: python

    add_model(path, bounding_box=None, loader_name=None, loader_properties=None, name=None, transformation=None, visible=True)

Adds model from remote path; returns model descriptor on success.

Parameters:

* ``path``: ``string``, path to the file to load.
* ``bounding_box``: ``dictionary``. The bounding box of the model. Default is ``None``, and Brayns will compute it. Otherwise, it has 2 keys:

  * Key: ``min``, value: 3 components ``float`` array with the minimun position (lower left corner)
  * Key: ``max``, value: 3 components ``float`` array with the maximun position (top right corner)

* ``loader_name``: ``string`` name of the loader to use. Default is ``None``, and Brayns will gess it from the file extension. Otherwise, name of the loader.
* ``loader_properties``: ``dictionary`` with settings that will be passed to the choosen file loader.
* ``name``: ``string``, name to give to the object within the scene.
* ``transformation``: ``dictionary`` with the following entries:

  * ``rotation``: 4 component ``float`` array representing this model rotation as a quaternion.
  * ``rotation_center``: 3 component ``float`` array of the 3D space position around which this model rotations are performed.
  * ``scale``: 3 component ``float`` array with the scale on each axis.
  * ``translation``: 3 component ``float`` array with the 3D space position of this model.

* ``visible``: ``bool``, wether the model is visible for rendering or not. Default is ``true``.

Return value:

* ``dictionary`` with model metadata. The contents of the metadata will vary depending on the loader used. Constant parameters are:

  * ``id``: ``integer``, model ID.
  * ``bounds``: ``dictionary``` with 2 keys, ``min`` and ``max`` representing the axis aligned bounds of the model.
  * ``bounding_box``: ``bool``, wether this model was loaded with a defined bounding box or not.
  * ``metadata``: ``dictionary`` with loader specific data.
  * ``visible``: ``bool``, wether this model is visible for rendering or not.

.. hint::

    You can use ``get_loaders()`` method to get a list of available file loaders, the file extensions they support, and the list of available properties
    for each one of them.

----

animation_slider
~~~~~~~~~~~~~~~~

.. code-block:: python

    animation_slider()

Creates and displays an animation slider widget to control the simulation loaded, if any.

----

chunk
~~~~~

.. code-block:: python

    chunk(id)

Indicate sending of a binary chunk after this message.

Parameters:

* ``id``: ``string``, ID of the chunk that will be sent after this request.

----

clear_lights
~~~~~~~~~~~~

.. code-block:: python

    clear_lights()

Remove all lights in the scene.

----

client_state_get
~~~~~~~~~~~~~~~~

.. code-block:: python

    client_state_get(key)

Gets a key-value entry from the clientstate map given an entry key, if exists.

Parameters:

* ``key``: ``string``, key to look for on the clientstate map.

Return value:

* ``dictionary`` with the following entries:

  * ``key``: ``string``, the key that was requested.
  * ``value``: ``string``, the value corresponding to the requested key, or empty if it did not exists.

----

client_state_get_all
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    client_state_get_all()

Gets a key-value list containing all entries from the cientstate map.

Return value:

* ``dictionary`` with the following entries:

  * ``keys``: array of ``string``, all the keys currently stored.
  * ``values``: array of ``string``, all the corresponding values for all the returned keys.

----

client_state_set
~~~~~~~~~~~~~~~~

.. code-block:: python

    client_state_set(key, value)

Sets a key-value entry into a persistent across connections common map to all clients.

Parameters:

* ``key``: ``string``, the key that identifies the entry.
* ``value``: ``string``, the value associated with the given key.

----

exit_later
~~~~~~~~~~

.. code-block:: python

    exit_later(minutes)

Schedules Brayns to shutdown after a given amount of minutes.

Parameters:

* ``minutes``: ``integer``, number of minutes since the request is made after which the backend will automatically shutdown.

----

fs_exists
~~~~~~~~~

.. code-block:: python

    fs_exists(path)

Return the type of filer (file or folder) if a given path exists, or none if it does not exists

Parameters:

* ``path``: ``string``, path to a file within the backend's reachable filesystem.

Return value:

* ``dictionary`` with the following entries:

  * ``error``: ``integer``, an error code if something went wrong. 0 means the request was successful.
  * ``message``: ``string``, a descriptive message of the error if the error code was non-zero.
  * ``type``: ``string``, upon success, returns the type of file pointed by path:

    * ``none``: Path does not exists.
    * ``file``: Path points to a regular file.
    * ``directory``: Path points to a directory.

----

fs_get_content
~~~~~~~~~~~~~~

.. code-block:: python

    fs_get_content(path, base64)

Return the content of a file if possible, or an error otherwise.

Parameters:

* ``path``: ``string``, path to a file within the backend's reachable filesystem.
* ``base64``: ``bool``, Flag indicating wether the result must be returned as a base64 encoded string.

Return value:

* ``dictionary`` with the following entries:

  * ``error``: ``integer``, an error code if something went wrong. 0 means the request was successful.
  * ``message``: ``string``, a descriptive message of the error if the error code was non-zero.
  * ``content``: ``string``, upon success, returns the content of the given file.

----

fs_get_root
~~~~~~~~~~~

.. code-block:: python

    fs_get_root()

Return the root path of the current execution environment (sandbox).

Return value:

* ``dictionary`` with the following entries:

  * ``root``: ``string``, Root path of Brayns's reachable filesystem.


----

fs_list_dir
~~~~~~~~~~~

.. code-block:: python

    fs_list_dir(path)

Return the content of a file if possible, or an error otherwise.

Parameters:

* ``path``: ``string``, path to a file within the backend's reachable filesystem.

Return value:

* ``dictionary`` with the following entries:

  * ``error``: ``integer``, an error code if something went wrong. 0 means the request was successful.
  * ``message``: ``string``, a descriptive message of the error if the error code was non-zero.
  * ``dirs``: array of ``string``, upon success, returns the child directories of the given path (non-recurisve).
  * ``files``: dictionary with the following entries:

    * ``names``: array of ``string``, upon success, returns the child files of the given path (non-recursive).
    * ``sizes``: array of ``integers``, upon success, returns the size of each child file, in octets.


----

fs_set_content
~~~~~~~~~~~~~~

.. code-block:: python

    fs_set_content(path, content, base64)

Creates a file and sets its contents. If the file does not exists, it will be truncated.

Parameters:

* ``path``: ``string``, path to a file within the backend's reachable filesystem.
* ``content``: ``string``, content to append to the newly created/truncated file.
* ``base64``: ``bool``, Flag indicating wether the given content is a binary base64 encoded string.

Return value:

* ``dictionary`` with the following entries:

  * ``error``: ``integer``, an error code if something went wrong. 0 means the request was successful.
  * ``message``: ``string``, a descriptive message of the error if the error code was non-zero.

----

.. _get-animation-parameters-label:

get_animation_parameters
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_animation_parameters()

Get the current state of animation-parameters.

Return value:

* ``dictionary`` with the following entries:

  * ``current``: ``integer``, number of the current simulation frame being played, if any.
  * ``delta``: ``integer``, the (frame) delta to apply for animations to select the next frame. Default is 1.
  * ``dt``: ``float``, the simulation timestep.
  * ``unit``: ``string``, a string representation of the timestep time unit.
  * ``frame_count``: ``integer``, total number of frames loaded in the current simulation, if any.
  * ``playing``: ``bool``, flag indicating wether the current simulation, if any, is being played.

----

get_application_parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_application_parameters()

Get the current state of the application parameters.

Return value:

* ``dictionary`` with the following entries:

  * ``engine``: ``string``, name of the current render engine in use.
  * ``image_stream_fps``: ``integer``, Frames-per-second (FPS) rate at which to stream images, if possible.
  * ``jpeg_compression``: ``integer``, JPEG compression ratio.
  * ``viewport``:, array of ``integer``, image resolution currently being renderer.


----

.. _get-camera-label:

get_camera
~~~~~~~~~~

.. code-block:: python

    get_camera()

Get the current state of camera

Return value:

* ``dictionary`` with the following entries:

  * ``current``: ``string``, name of the current camera being used for rendering.
  * ``orientation``: 4 component ``float`` array, a quaternion representing the camera rotation.
  * ``position``: 3 component ``float`` array, 3D space position of the camera.
  * ``target``: 3 component ``float`` array, 3D space position at which the camera is pointing to.
  * ``types``: array of ``string``, list of all available camera types.

----

get_camera_params
~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_camera_params()

Get the params of the current camera

Return value:

* ``dictionary`` with the camera parameters. The content will vary depending on the ``current`` camera selected
                 for rendering.

----

get_clip_planes
~~~~~~~~~~~~~~~

.. code-block:: python

    get_clip_planes()

Get all clip planes.

Return value:

* ``None`` if no clip planes exists.
* array of ``dictionaries`` otherwise, each of which contains the following entries:

  * ``id``: ``integer``, ID of the clipping plane.
  * ``plane``: array of 4 ``floats``, with the plane representation (plane perpendicular normalized vector + distance from world origin)


----

get_environment_map
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_environment_map()

Get the environment map texture file path from the scene.

Return value:

* ``dictionary`` with the following entries:

  * ``filename``: ``string``, path to the environmental map texture file, empty if there is no environmental map.


----

get_instances
~~~~~~~~~~~~~

.. code-block:: python

    get_instances()

Return all the model instances for a given model Id.

Parameters:

* ``id``: ``integer``, ID of the model for which to check for instances.
* ``result_range``: 2 component ``integer`` array, specific range to return from the list of instances.º

Return value:

* array of ``dictionaries``, each of which have the following entries:

  * ``model_id``: ``integer``, the model ID to which this instance belongs to.
  * ``instance_id``: ``integer``, the ID of the instance.
  * ``bounding_box``: ``bool``, wether this instance has bounding box or not.
  * ``transformation``: ``dictionary`` with the following entries:

    * ``rotation``: 4 component ``float`` array representing this model instance rotation as a quaternion.
    * ``rotation_center``: 3 component ``float`` array of the 3D space position around which this model instance rotations are performed.
    * ``scale``: 3 component ``float`` array with the scale on each axis.
    * ``translation``: 3 component ``float`` array with the 3D space position of this model instance.

  * ``visible``: ``bool`` flag indicating wether this model instance is being rendered or not.

----

get_lights
~~~~~~~~~~

.. code-block:: python

    get_lights()

Return all scene lights.

Return value:

* array of ``dictionaries``, each of which will have the following entries:

  * ``id``: ``integer``, ID of the light.
  * ``type``: ``string``, type of light.
  * ``properties``: ``dictionary``, light-specific properties. Depends on the type of light.

----

get_loaders
~~~~~~~~~~~

.. code-block:: python

    get_loaders()

Get all available file loaders.

Return value:

* array of ``dictionaries``, each of which will have the following entries:

  * ``name``: ``string``, name of the loader that can be used to specify when calling ``add_model`` method.
  * ``extensions``: array of ``strings``, list of file extensions supported by this loader.
  * ``properties``: ``dictionary`` with the available property names and types for this loader.


----

get_mat
~~~~~~~

.. code-block:: python

    get_mat(materialId, modelId)

Return the properties of a material

Parameters:

* ``materialId``: ``integer``, ID of the material to query.
* ``modelId``: ``integer``, ID of the model that contains the requested material.

Return value:

* ``dictionary`` with the following entries:

  * ``error``: ``integer``, an error code if an exception occoured. 0 means the request was successful.
  * ``message``: ``string``, error description if ``error`` was non-zero.
  * ``modelId``: ``integer``, model ID to which this material belongs to.
  * ``materialId``: ``integer``, ID of this material.
  * ``materialProperties``: ``dictionary``, with the material properties. The contents will depend on the class of the given material.

----

get_model_properties
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_model_properties(id)

Get the properties (metadata) of the given model

Parameters:

* ``id``: ``integer``, ID of the model to query.

Return value:

* ``dictionary`` with model metadata. The contents of the metadata will vary depending on the type of model queried. Constant parameters are:

  * ``id``: ``integer``, model ID.
  * ``bounds``: ``dictionary``` with 2 keys, ``min`` and ``max`` representing the axis aligned bounds of the model.
  * ``bounding_box``: ``bool``, wether this model was loaded with a defined bounding box or not.
  * ``metadata``: ``dictionary`` with model specific data.
  * ``visible``: ``bool``, wether this model is visible for rendering or not.

----

get_model_transfer_function
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_model_transfer_function(id)

Get the transfer function of the given model.

Parameters:

* ``id``:

Return value:

* ``dictionary`` with the following entries:

  * ``colormap``: ``dictionary`` with the transfer function color map. It contains the following entries:

    * ``colors``: array of 3 components ``float`` array with the colormap normalized RGB colors.
    * ``name``: ``string``, name of the color map.

  * ``opacity_curve``: array with two 2-components ``float`` array, containing the lower and upper bound of the transfer function.
  * ``range``: 2-components ``float`` array. Sampled value range. Brayns is, by default, initialized with range [-80, -10].

----

get_renderer
~~~~~~~~~~~~

.. code-block:: python

    get_renderer()

Get the current state of renderer.

Return value:

* ``dictionary`` containing the following entries:

  * ``accumulation``: ``bool``, flag indicating wether accumulation buffer is in use or not.
  * ``max_accum_frames``: ``integer``, maximun number of accumulation frames to render, if ``accumulation`` is set to ``True``.
  * ``background_color``: 3-component ``float`` array with the normalized RGB color of the background. Default is black (0,0,0).
  * ``curent``: ``string``, name of the current renderer in use.
  * ``head_light``: ``bool``, wether to have a persistent directional light pointing in the direction of the camera.
  * ``samples_per_pixel``: ``integer``, number of rays to launch from each pixel for each rendered frame.
  * ``subsampling``: ``integer``, number of rays to launch for sub-pixel level sampling.
  * ``types``: array of ``strings``, list of all available renderers.
  * ``variance_threshold``: ``float``, variance threshold of the current integration.

----

get_renderer_params
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_renderer_params()

Get the parameters of the current renderer.

Return value:

* ``dictionary`` with the renderer parameters. The contents will vary depending on which ``current`` renderer is selected.


----

get_scene
~~~~~~~~~

.. code-block:: python

    get_scene()

Get the current state and the contents of the scene.

Return value:

* ``dictionary`` with the following entries:

  * ``bounds``: ``dictionary`` with the following entries:

    * ``min``: 3-components ``float`` array with the scene's minimun bound.
    * ``max``: 3-components ``float`` array with the scene's maximun bound.

  * ``models``: array of ``dictionaries``, each of which contains a model metadata.


----

get_statistics
~~~~~~~~~~~~~~

.. code-block:: python

    get_statistics()

Get the current statistics of the rendering process.

Return value:

* ``dictionary`` with the following entries:

  * ``fps``: ``float``, current Frames per second (FPS) rate.
  * ``scene_size_in_bytes``: ``integer``, size of the scene in memory, in bytes.


----

get_videostream
~~~~~~~~~~~~~~~

.. code-block:: python

    get_videostream()

Get the videostream parameters.

.. attention::

    This method throws an exception if the it is called when the video streaming method is not enabled in the backend.

Return value:

* ``dictionary`` with the following entries:

  * ``enabled``: ``bool``, flag indicating wether the video streaming feature was enabled at startup.
  * ``kbps``: ``integer``, kb per second transfer rate.


----

get_volume_parameters
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    get_volume_parameters()

Get the current state of volume-parameters specification when rendering volumes.

Return value:

* ``dictionary`` with the following entries:

  * ``adaptative_sampling``: ``bool``, flag indicating wether adaptative sampling is in use.
  * ``adaptive_max_sampling_rate``: ``float``, maximun sampling rating increase when using adaptative sampling.
  * ``clip_box``: ``dictionary`` with the bounds of the volume rendering area within the scene. Entries are:

    * ``min``: 3-components ``float`` array with the minimun bound.
    * ``max``: 3-components ``float`` array with the maximun bound.

  * ``gradient_shading``: ``bool``, wether to use the volume gradient when shading the rendered volume.
  * ``pre_integration``: ``bool``, wether to perform an initial integration before starting sampling the volume for rendering.
  * ``sampling_rate``: ``float``, step at which the volume is sampled within a normalized bounds.
  * ``single_shade``: ``bool``, wether to use simpler shading when rendering the volume.
  * ``specular``: 3-components ``float`` array with the specular reflection normalized RGB color.
  * ``volume_dimensions``: 3-components ``float`` array representing the desired target volume dimensions in scene.
  * ``volume_offset``: 3-components ``float`` array representing an axis aligned volume offset in world space.
  * ``volume_element_spacing``: 3-components ``float`` array representing the spacing between elements on each axis.

----

image
~~~~~

.. code-block:: python

    image(size, format, animation_parameters, camera,
          quality, renderer, samples_per_pixel)


Request a snapshot from Brayns and returns a PIL image.


Parameters:

* ``size``: 2-components ``integer`` array specifying the desired width and height of the image to be renderer.
* ``format``: ``string``, image file format in which to store the rendered image. Must be either ``png`` or ``jpeg``.
* ``animation_parameters``: ``dictionary`` specifying the simulation state when rendering the image. By default is ``None`` and it will take the current simulation state. Must have the following entries:

  * ``current``: ``integer``, number of the current simulation frame being played, if any.
  * ``delta``: ``integer``, the (frame) delta to apply for animations to select the next frame. Default is 1.
  * ``dt``: ``float``, the simulation timestep.
  * ``unit``: ``string``, a string representation of the timestep time unit.
  * ``frame_count``: ``integer``, total number of frames loaded in the current simulation, if any.
  * ``playing``: ``bool``, flag indicating wether the current simulation, if any, is being played.

* ``camera``: ``dictionary`` specifying the camera parameters. By default is ``None``, and  it will take the current camera parameters. Must have the following entries:

  * ``current``: ``string``, name of the current camera being used for rendering.
  * ``orientation``: 4 component ``float`` array, a quaternion representing the camera rotation.
  * ``position``: 3 component ``float`` array, 3D space position of the camera.
  * ``target``: 3 component ``float`` array, 3D space position at which the camera is pointing to.
  * ``types``: array of ``string``, list of all available camera types.

* ``quality``: ``integer`` flag indicating the compression quality of the image to be rendered. Maximun quality is 100.
* ``renderer``: ``dictionary`` specifying the renderer parameters. By default is ``None``, and it will take the current renderer parameters. Must have the following entries:

  * ``accumulation``: ``bool``, flag indicating wether accumulation buffer is in use or not.
  * ``max_accum_frames``: ``integer``, maximun number of accumulation frames to render, if ``accumulation`` is set to ``True``.
  * ``background_color``: 3-component ``float`` array with the normalized RGB color of the background. Default is black (0,0,0).
  * ``curent``: ``string``, name of the current renderer in use.
  * ``head_light``: ``bool``, wether to have a persistent directional light pointing in the direction of the camera.
  * ``samples_per_pixel``: ``integer``, number of rays to launch from each pixel for each rendered frame.
  * ``subsampling``: ``integer``, number of rays to launch for sub-pixel level sampling.
  * ``types``: array of ``strings``, list of all available renderers.
  * ``variance_threshold``: ``float``, variance threshold of the current integration.

* ``samples_per_pixel``: ``integer``, number of accumulation frames to use.

----

image_jpeg
~~~~~~~~~~

.. code-block:: python

    image_jpeg()

Returns a base64-encode jpeg image of the current frame in Brayns

Return value:

* ``string`` with the base64-encoded jpeg image.


----

image_streaming_mode
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    image_streaming_mode(type)

Set the image streaming method between automatic or controlled

Parameters:

* ``type``: ``string`` type of image streaming mode to be employed in the backend. Possible values are:

  * ``quanta``: Controlled streaming from the client. The client must request a new frame throught ``tigger_jpeg_stream()``.
  * ``normal``: The backend will stream a new frame as soon as is ready.

----

inspect
~~~~~~~

.. code-block:: python

    inspect(array)

Inspect the scene at x-y screen position.

Parameters:

* ``array``: 2-components ``integer`` array with the screen pixel coordinates to inspect.

Return value:

* ``dictionary`` with the following entries:

  * ``hit``: ``bool`` flag indicating wether any model was intersected, false otherwise.
  * ``position``: 3-components ``float`` array with the hitted 3D space position if ``hit`` is ``True``.

----

loaders_schema
~~~~~~~~~~~~~~

.. code-block:: python

    loaders_schema()

Get the schema properties for all loaders.

Return value:

* ``dictionary`` with the following entires:

  * ``oneOf``: array of ``dictionaries``, each of which has the following entries:

    * ``tittle``: ``string``, name of the loader.
    * ``type``: ``string``, type of properties parameters. Default is ``object`` (= ``dictionary``)
    * ``properties``: loader-specific properties. The type of this entry is defined by ``type``.


----

model_properties_schema
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    model_properties_schema(id)

Get the property schema of the model

Parameters:

* ``id``: ``integer``, ID of the model to query for its properties schema.

Return value:

* ``dictionary`` with the following entries:

  * ``title``: ``string`` name of the object to be returned in the properties entry.
  * ``type``: ``string`` type of the object returned in the properties entry. Default is ``object`` (= ``dictionary``).
  * ``properties``: Properties schema of the given model. The type of this object is given by the entry ``type``.

----

open_ui
~~~~~~~

.. code-block:: python

    open_ui()

Open the Brayns old UI in a new page of the default system browser.

----

quit
~~~~

.. code-block:: python

    quit()

Requests the backend to be shutsdown and closes the connection with it.

----

remove_clip_planes
~~~~~~~~~~~~~~~~~~

.. code-block:: python

    remove_clip_planes(array)

Remove clip planes from the scene given their gids

Parameters:

* ``array``: array of ``integers`` with the IDs of the clipping planes to be removed.

----

remove_lights
~~~~~~~~~~~~~

.. code-block:: python

    remove_lights(array)

Remove light given their IDs

Parameters:

* ``array``: array of ``integers`` with the IDs of the lights to be removed.

----

remove_model
~~~~~~~~~~~~

.. code-block:: python

    remove_model(array)

Remove the model(s) with the given ID(s) from the scene

Parameters:

* ``array``: array of ``integers`` with the IDs of the models to be removed.

----

request_model_upload
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    request_model_upload(chunks_id, path, size, type, bounding_box=None, loader_name=None,
                         loader_properties=None, name=None, transformation=None, visible=True)

Request upload of blob to trigger adding of model after blob has been received; returns model descriptor on success

Parameters:

* ``chunks_id``: ``string``, ID of the chunk being sent.
* ``path``: ``string``, path from where the binary data comes from.
* ``size``: ``integer``, size in bytes of the chunk.
* ``type``: ``type``, virtual extension for the backend to know which loader to use.
* ``bounding_box``: ``dictionary`` with ``min`` and ``max`` keys to create the bounding box, or ``None`` to let brayns compute it.
* ``loader_name``: ``string`` name of the loader to enforce to use.
* ``loader_properties``: ``dictionary`` with loader-specific properties.
* ``name``: ``string``, name to be given to the model in the scene.
* ``transformation``: ``dictionary`` with the entries ``rotation``, ``roation_center``, ``translation`` and ``scale`` as the model affine transform, or ``None`` to not apply any.
* ``visible``: ``bool``, flag indicating wether the model is visible for rendering.

Return value:

* ``dictionary`` with model metadata. The contents of the metadata will vary depending on the loader used. Constant parameters are:

  * ``id``: ``integer``, model ID.
  * ``bounds``: ``dictionary``` with 2 keys, ``min`` and ``max`` representing the axis aligned bounds of the model.
  * ``bounding_box``: ``bool``, wether this model was loaded with a defined bounding box or not.
  * ``metadata``: ``dictionary`` with loader specific data.
  * ``visible``: ``bool``, wether this model is visible for rendering or not.

.. hint::

    You can use ``get_loaders()`` method to get a list of available file loaders, the file extensions they support, and the list of available properties
    for each one of them.

----

reset_camera
~~~~~~~~~~~~

.. code-block:: python

    reset_camera()

Resets the camera to its initial values when the backend was launched.

----

schema
~~~~~~

.. code-block:: python

    schema(endpoint)

Get the schema of the given endpoint

Parameters:

* ``endpoint``: ``string`` endpoint name.

Return value:

* ``dictionary`` with the endpoint schema properties.

----

set_animation_parameters
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_animation_parameters(current, delta, dt, frame_count, playing, unit)

Set the new state of animation parameters.

Parameters:

* ``current``: ``integer``, number of the current frame to be played.
* ``delta``: ``integer``, the (frame) delta to apply for animations to select the next frame. Default is 1.
* ``dt``: ``float``, simulation timestep (in seconds).
* ``frame_count``: ``integer``, number of frames to use for the simulation.
* ``playing``: ``bool``, wether the animation should be played when rendering.
* ``unit``: ``string``, string representation of the time unit used (``s`` = secons, ``ms`` = milisecons)

----

set_application_parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_application_parameters(engine, image_stream_fps, jpeg_compression, viewport)

Set the new state of application parameters.

Parameters:

* ``engine``: ``string``, name of the renderer to use.
* ``image_stream_fps``: ``integer``, Frames per second (FPS) streaming target.
* ``jpeg_compression``: ``integer`` jpeg compression rate.
* ``viewport``: 2-components array of `integers`, with the width and height of the images to be rendered

----

set_camera
~~~~~~~~~~

.. code-block:: python

    set_camera(current, orientation, position, target, types)

Set the new state of camera.

Parameters:

* ``current``: ``string``, name of the camera type to use.
* ``orientation``: 4-components array of ``floats``, representing the camera rotation as a quaternion.
* ``position``: 3-components array of ``floats`` with the 3D space position where to place the camera.
* ``target``: 3-components array of ``floats`` with the 3D space position to which the camera will be looking at.
* ``types``: array of ``strings`` with the camera types that will be available.

.. hint::

    For the ``types`` parameter, the usual value is to first get the current state of the camera: ``camera = braynsClient.get_camera()``
    and then use the returned value to make the request: ``braynsClient.set_camera(...., types = camera["types"])``.

----

set_camera_params
~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_camera_params(params)

Set camera-type-specific parameters on the current camera.

Parameters:

* ``params``: ``dictionary`` with the camera parameters. The entries will depend on the ``current`` type of camera active.

----

set_colormap
~~~~~~~~~~~~

.. code-block:: python

    set_colormap(palette, intensity=1, opacity=1, data_range=(0, 255))


Set a colormap to Brayns, by modifying the current transfer function.

Parameters:

* ``palette``: array of 3-components ``floats`` array, each with a normalized RGB color.
* ``intensity``: ``float``, intensity to multiply the colors by.
* ``opacity``: ``float``, opacity to apply to all colors.
* ``data_range``: 2-components ``floats`` array, data range on which values the colormap should be applied.

----

set_environment_map
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_environment_map(filename)

Set a environment map in the scene

Parameters:

* ``filename``: ``string``, path to the environmental map file to be used.

----

set_mat
~~~~~~~

.. code-block:: python

    set_mat(error, message, modelId, materialId, materialProperties)

Set the material propertiese for the given model and material.

Parameters:

* ``error``: ``integer``, unused, must be 0.
* ``message``: ``string``, unused, must be an empty string.
* ``modelId``: ``integer``, ID of the model to which the material belongs to.
* ``materialId``: ``integer``, ID of the material to modify.
* ``materialProperties``: ``dictionary`` with the material properties. The entries will depend on the class of the material.

----

set_model_properties
~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_model_properties(id, properties)

Set the properties of the given model.

Parameters:

* ``id``: ``integer``, ID of the model to modify.
* ``properties``: ``dictionary``, with the new values of its properties. The entries will depend on the type of model.

----

set_model_transfer_function
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_model_transfer_function(id, transfer_function)

Set the transfer function of the given model.

Parameters:

* ``id``: ``integer``, ID of the model to which to modify the transfer function.
* ``transfer_function``: ``dictionary`` with the following entries:

  * ``colormap``: ``dictionary`` with the transfer function color map. It contains the following entries:

    * ``colors``: array of 3 components ``float`` array with the colormap normalized RGB colors.
    * ``name``: ``string``, name of the color map.

  * ``opacity_curve``: array with two 2-components ``float`` array, containing the lower and upper bound of the transfer function.
  * ``range``: 2-components ``float`` array. Sampled value range. Brayns is, by default, initialized with range [-80, -10].

----

set_renderer
~~~~~~~~~~~~

.. code-block:: python

    set_renderer(accumulation, max_accum_frames, background_color,
                 current, head_light, subsampling, types, variance_threshold)

Set the new state of renderer.

Parameters:

* ``accumulation``: ``bool``, flag indicating wether to use accumulation or not.
* ``max_accum_frames``: ``integer``, if ``accumulation`` is ``True``, number of accumulation frames to render per frame.
* ``background_color``: 3-components array of `floats` with the normalized RGB color of the background.
* ``current``: ``string``, name of the renderer to use.
* ``head_light``: ``bool``, flag to indicate there should be a light pointing in the same direction as the camera.
* ``samples_per_pixel``: ``integer``, number of rays to launch per pixel and per frame.
* ``subsampling``: ``integer``, number of sub-pixel level sampling to use per pixel and per frame.
* ``types``: array of ``strings`` with the available renderer types.
* ``variance_threshold``: ``float`` variance threshold.

.. hint::

    The easiest way to set up some of these parameters is to get the current state of the renderer: ``renderer = braynsClient.get_renderer()``
    and then re-use them when making the request, for example:

    .. code-block:: python

        braynsClient.set_renderer(..., types = renderer["types"], variance_threshold = renderer["variance_threshold"])

----

set_renderer_params
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_renderer_params(params)

Set the renderer-specific parameters on the current renderer.

Parameters:

* ``params``: ``dictionary`` with the properties of the renderer. The entries will depend on the type of renderer curently being used.

----

set_scene
~~~~~~~~~

.. code-block:: python

    set_scene(bounds, models)

Set the new state of scene.

Parameters:

* ``bounds``: ``dictionary`` with the entries ``min`` and ``max``, 3-components ``floats`` vector that specify the new scene bounds.
* ``models``: array of ``dictionaries``, each of which contains the metadata of the models. The entries of these will depend on each model.

----

set_videostream
~~~~~~~~~~~~~~~

.. code-block:: python

    set_videostream(enabled, kbps)

Enables the video streaming mode.

Parameters:

* ``enabled``: ``bool``, flag indicating wether to enable the videostreaming mode or not.
* ``kbps``: ``integer``, if ``enabled`` is ``True``, target kb per second to reach during streaming.

----

set_volume_parameters
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    set_volume_parameters(adaptative_sampling, adaptative_max_sampling_rate, clip_box, gradient_shading,
                          pre_integration, sampling_rate, single_shade, specular, volume_dimensions,
                          volume_element_spacing, volume_offset)

Set the new state of volume rendering parameters.

Parameters:

* ``adaptative_sampling``: ``bool``, flag indicating wether adaptative sampling is in use.
* ``adaptive_max_sampling_rate``: ``float``, maximun sampling rating increase when using adaptative sampling.
* ``clip_box``: ``dictionary`` with the bounds of the volume rendering area within the scene. Entries are:

  * ``min``: 3-components ``float`` array with the minimun bound.
  * ``max``: 3-components ``float`` array with the maximun bound.

* ``gradient_shading``: ``bool``, wether to use the volume gradient when shading the rendered volume.
* ``pre_integration``: ``bool``, wether to perform an initial integration before starting sampling the volume for rendering.
* ``sampling_rate``: ``float``, step at which the volume is sampled within a normalized bounds.
* ``single_shade``: ``bool``, wether to use simpler shading when rendering the volume.
* ``specular``: 3-components ``float`` array with the specular reflection normalized RGB color.
* ``volume_dimensions``: 3-components ``float`` array representing the desired target volume dimensions in scene.
* ``volume_offset``: 3-components ``float`` array representing an axis aligned volume offset in world space.
* ``volume_element_spacing``: 3-components ``float`` array representing the spacing between elements on each axis.


----

show
~~~~

.. code-block:: python

    show()

Show the live rendering of Brayns.


----

snapshot
~~~~~~~~

.. code-block:: python

    snapshot(format, size, animation_parameters=None, camera=None,
             filePath, name, quality, rendered=None, samples_per_pixel)

Make a snapshot of the current view. Either return it as base64 encoded image, or save it to backend's local filesystem
depending on the ``filePath`` parameter.

Parameters:

* ``format``: ``string``, format in which the image will be sent. Must be either ``jpeg`` or ``png``.
* ``size``: 2-components array of `integers` with the width and height of the image to be renderer.
* ``animation_parameters``: ``dictionary`` with the animation parameters to use when rendering the image. `get_animation_parameters`_. Default is ``None`` and will use the current parameters.
* ``camera``: ``dictionary`` with the camera properties to use when rendering the image. `get_camera`_. Default is ``None`` and will use the current parameters.
* ``filePath``: ``string`` path on the backend's local filesystem where to store the snapshot. If empty, the image wont be stored, but sent as an base64 encoded image.
* ``name``: ``string`` snapshot name to be used to identify the process in the backend logs.
* ``quality``: ``integer`` snapshot compression quality, 0 is maximun quality.
* ``renderer``: ``dictionary`` with the renderer parameters to use then rendering the image. `get_renderer`_. Default is ``None`` and will use the current parameters.
* ``samples_per_pixel``: ``integer``, number of accumulation frames to use, if the property is enabled in the renderer.

Return value:

* ``dictionary`` with the following entries:

  * ``data``: ``string``, base64 encoded image, or empty if the ``filePath`` property was no empty.

----

trigger_jpeg_stream
~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    trigger_jpeg_stream()

Triggers the engine to stream a frame to the clients.


----

update_clip_plane
~~~~~~~~~~~~~~~~~

.. code-block:: python

    update_clip_plane(id, plane)

Update a clip plane with the given coefficients.

Parameters:

* ``id``: ``integer``, ID of the plane to update.
* ``plane``: 4-components array of ``floats`` with the perpendicular normalized vector of the plane, and the distance from the scene origin.

----

update_instance
~~~~~~~~~~~~~~~

.. code-block:: python

    update_instance(model_id, instance_id, bounding_box, transformation, visible)

Update the instance with the given values.

Parameters:

* ``model_id``: ``integer``, ID of the model to which the instance belongs to.
* ``instance_id``: ``integer``, ID of the instance to modify.
* ``bounding_box``: ``bool`` flag indicating wether this instance has a bounding box.
* ``transformation``: ``dictionary`` with the entries ``rotation``, ``rotation_center``, ``translation`` and ``scale`` with the new transformation parameters.
* ``visible``: ``bool``, flag indicating wether the instance is visible for rendering or not.

----

update_model
~~~~~~~~~~~~

.. code-block:: python

    update_model(id, bounding_box=False, bounds=None, metadata=None, name=None,
                 path=None, transformation=None, visible=True)

Update the model with the given values, and triggers its reconstruction.

Parameters:

* ``id``: ``integer``, ID of the model to update
* ``bounding_box``: ``bool``, flag indicating wether this model should have bounding box.
* ``bounds``: ``dictionary`` with the entries ``min`` and ``max``, 3-components array of ``floats`` indicating the model new bounds.
* ``metadata``: ``dictionary`` with model specific metadata properties. The entries will depend on the type of model being manipulated.
* ``name``: ``string``, new name to give to the model in the scene.
* ``path``: ``string``, new path to which the model belongs to on disk.
* ``transformation``: ``dictionary`` with the entries ``rotation``, ``rotation_center``, ``translation`` and ``scale`` with the new transformation parameters.
* ``visible``: ``bool``, flag indicating wether the instance is visible for rendering or not.
