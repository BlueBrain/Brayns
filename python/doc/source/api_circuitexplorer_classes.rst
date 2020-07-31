Circuit Explorer API Classes
============================

The Circuit Explorer API classes are used as parameter for camera and renderer functions parameters (such as ``set_renderer_params``
or ``set_camera_params``).

For this classes to be available, the Brayns backend service must be started with the
following parameter:

.. code-block:: console

    --plugin braynsCircuitExplorer

CircuitExplorerDofPerspectiveCameraParams
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets the parameters of the perspective camera with depth of field. Used after calling ``set_camera`` with
``type="circuit_explorer_dof_perspective"``, by calling ``set_camera_params``.

Member variables:

* ``aperture_radius``: ``float``, camera lens aperture radius. Default value ``0.0``.
* ``aspect``: ``float``, image aspect ratio, computed as `width / height`. Default value ``1.0``.
* ``enable_clipping_planes``: ``bool``, wether to enable geometry clipping using clipping planes. Default value ``True``.
* ``focus_distance``: ``float``, distance at which to focus the camera clear view. Default value ``1.0``.
* ``fovy``: ``float``, vertical field of view, in degrees. Default value ``45.0``.

----

CircuitExplorerSphereClippingCameraParams
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets the parameters of the perspective camera with depth of field. Used after calling ``set_camera`` with
``type="circuit_explorer_sphere_clipping"``, by calling ``set_camera_params``.

Member variables:

* ``aperture_radius``: ``float``, camera lens aperture radius. Default value ``0.0``.
* ``aspect``: ``float``, image aspect ratio, computed as `width / height`. Default value ``1.0``.
* ``enable_clipping_planes``: ``bool``, wether to enable geometry clipping using clipping planes. Default value ``True``.
* ``focus_distance``: ``float``, distance at which to focus the camera clear view. Default value ``1.0``.
* ``fovy``: ``float``, vertical field of view, in degrees. Default value ``45.0``.

----

CircuitExplorerBasicRendererParams
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets the parameters of Circuit Explorer basic renderer. Used after calling ``set_renderer`` with
``type="circuit_explorer_basic"``, by calling ``set_renderer_params``.

Member variables:

* ``alpha_correction``: ``float``, alpha correction. Default ``0.5``.
* ``exposure``: ``float``, surface color exposure multiplier. Default ``1``.
* ``max_bounces``: ``integer``, ray max bounces. Default ``3``.
* ``max_distance_to_secondary_model``: ``float``, maximun distance to secondary model (simulation model). Default ``1``.
* ``simulation_threshold``: ``float``, simulation normalied value threshold. Default ``0``.
* ``use_hardware_randomizer``: ``bool``, wether to use hardware randomizer for rendering. Default ``False``.

----

CircuitExplorerAdvancedRendererParams
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets the parameters of Circuit Explorer basic renderer. Used after calling ``set_renderer`` with
``type="circuit_explorer_advanced"``, by calling ``set_renderer_params``.

Member variables:

* ``epsilon_factor``: ``float``, floating point epsilon error threshold. Default value ``1.0``.
* ``exposure``: ``float``, surface color intensity multiplier. Default value ``1.0``.
* ``fog_start``: ``float``, distance at which fog starts from the camera. Default value ``0.0``.
* ``fog_thickness``: ``float``, fog thickness, starting from ``fog_start``. Default value ``1000000.0``.
* ``gi_distance``: ``float``, global illumination shading maximun distance from camera. Default value ``10000.0``.
* ``gi_samples``: ``int``, global illumination samples. Default value ``0``.
* ``gi_weight``: ``float``, global illumination shading weight. Default value ``0.0``.
* ``max_bounces``: ``int``, max ray bounces. Default value ``3``.
* ``max_distance_to_secondary_model``: ``float``, maximun allowed distance between main model and simulation model. Default value ``30.0``.
* ``sampling_threshold``: ``float``, floating point epsilon for sampling. Default value ``0.001``.
* ``shadows``: ``float``, shadow intensity, maximun ``1.0``. Default value ``0.0``.
* ``soft_shadows``: ``float``, soft shadow intensity, maximun ``1.0``. Default value ``0.0``.
* ``soft_shadows_samples``: ``int``, samples for soft shadowing. Default value ``1``.
* ``use_hardware_randomizer``: ``bool``, use hardware-integrated random number generator. Default value ``False``.
* ``volume_alpha_correction``: ``float``, volume alpha correction. Default value ``0.5``.
* ``volume_specular_exponent``: ``float``, volume specular highlight exponent (sharpness). Default value ``20.0``.

----

CircuitExplorerCellGrowthRendererParams
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets the parameters of Circuit Explorer basic renderer. Used after calling ``set_renderer`` with
``type="circuit_explorer_cell_growth"``, by calling ``set_renderer_params``.

Member variables:

* ``alpha_correction``: ``float``, alpha correction. Default value ``0.5``.
* ``exposure``: ``float``, surface color intensity multiplier. Default value ``1.0``.
* ``fog_start``: ``float``, distance at which fog starts from the camera. Default value ``0.0``.
* ``fog_thickness``: ``float``, fog thickness, starting from ``fog_start``. Default value ``1000000.0``.
* ``shadow_distance``: ``float``, max distance from the camera at which to calculate shadows. Default value ``10000.0``.
* ``shadows``: ``float``, shadow intensity, maximun ``1.0``. Default value ``0.0``.
* ``simulation_threshold``: ``float``, simulation threshold. Default value ``0.0``.
* ``soft_shadows``: ``float``, soft shadow intensity, maximun ``1.0``. Default value ``0.0``.
* ``tf_color``: ``bool``, wether to use the transfer function color map or not. Default value ``False``.
* ``use_hardware_randomizer``: ``bool``, use hardware-integrated random number generator. Default value ``False``.

----

CircuitExplorerProximityDetectionRendererParams
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets the parameters of Circuit Explorer basic renderer. Used after calling ``set_renderer`` with
``type="circuit_explorer_proximity_detection"``, by calling ``set_renderer_params``.

Member variables:

* ``alpha_correction``: ``float``, alpha correction. Default value ``0.5``.
* ``detection_distance``: ``float``, detection distance. Default value ``1.0``.
* ``detection_far_color``: ``list``, normalized RGB color to apply for far areas. Default value ``[1.0, 0.0, 0.0]``.
* ``detection_near_color``: ``list``, normalized RGB color to apply for near areas. Default value ``[0.0, 1.0, 0.0]``.
* ``detection_on_different_material``: ``bool``, wether to perform proximity on different material. Default value ``False``.
* ``exposure``: ``float``, surface color intensity multiplier. Default value ``1.0``.
* ``max_bounces``: ``int``, max ray bounces. Default value ``3``.
* ``surface_shading_enabled``: ``bool``, wether to use surface shading or flat coloring. Default value ``True``.
* ``use_hardware_randomizer``: ``bool``, use hardware-integrated random number generator. Default value ``False``.

----

CircuitExplorerVoxelizedSimulationRendererParams
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sets the parameters of Circuit Explorer basic renderer. Used after calling ``set_renderer`` with
``type="circuit_explorer_voxelized_simulation"``, by calling ``set_renderer_params``.

Member variables:

* ``alpha_correction``: ``float``, alpha correction. Default value ``0.5``.
* ``exposure``: ``float``, surface color intensity multiplier. Default value ``1.0``.
* ``fog_start``: ``float``, distance at which fog starts from the camera. Default value ``0.0``.
* ``fog_thickness``: ``float``, fog thickness, starting from ``fog_start``. Default value ``1000000.0``.
* ``max_bounces``: ``int``, max ray bounces. Default value ``3``.
* ``simulation_threshold``: ``float``, simulation threshold. Default value ``0.0``.
* ``use_hardware_randomizer``: ``bool``, use hardware-integrated random number generator. Default value ``False``.
