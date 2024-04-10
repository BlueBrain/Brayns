core
====

.. automodule:: brayns.core

geometry
--------

.. automodule:: brayns.core.geometry
    :members: add_clipping_geometries, add_geometries, BoundedPlane, Box, Capsule, clear_clipping_geometries, Geometry, Plane, Sphere
    :undoc-members:
    :show-inheritance:

light
-----

.. automodule:: brayns.core.light
    :members: add_light, AmbientLight, clear_lights, DirectionalLight, Light, QuadLight, SphereLight
    :undoc-members:
    :show-inheritance:

application
-----------

.. automodule:: brayns.core.application
    :members: Application, get_application, set_resolution, stop
    :undoc-members:
    :show-inheritance:

camera
------

.. automodule:: brayns.core.camera
    :members: Camera, get_camera, set_camera
    :undoc-members:
    :show-inheritance:

camera_controller
-----------------

.. automodule:: brayns.core.camera_controller
    :members: CameraController
    :undoc-members:
    :show-inheritance:

material
--------

.. automodule:: brayns.core.material
    :members: CarPaintMaterial, EmissiveMaterial, get_material_name, get_material, GlassMaterial, Material, MatteMaterial, MetalMaterial, PhongMaterial, PlasticMaterial, PrincipledMaterial, set_material
    :undoc-members:
    :show-inheritance:

version
-------

.. automodule:: brayns.core.version
    :members: check_version, get_version, Version, VersionError
    :undoc-members:
    :show-inheritance:

model
-----

.. automodule:: brayns.core.model
    :members: clear_models, clear_renderables, get_bounds, get_model, get_models, get_scene, instantiate_model, Model, remove_models, Scene, update_model
    :undoc-members:
    :show-inheritance:

coloring
--------

.. automodule:: brayns.core.coloring
    :members: color_model, ColorMethod, get_color_methods, get_color_values, set_model_color
    :undoc-members:
    :show-inheritance:

color_ramp
----------

.. automodule:: brayns.core.color_ramp
    :members: ColorRamp, get_color_ramp, set_color_ramp, ValueRange
    :undoc-members:
    :show-inheritance:

opacity_curve
-------------

.. automodule:: brayns.core.opacity_curve
    :members: ControlPoint, OpacityCurve
    :undoc-members:
    :show-inheritance:

simulation
----------

.. automodule:: brayns.core.simulation
    :members: enable_simulation, get_simulation, set_simulation_frame, Simulation, TimeUnit
    :undoc-members:
    :show-inheritance:

entrypoint
----------

.. automodule:: brayns.core.entrypoint
    :members: Entrypoint, get_entrypoint, get_entrypoints, get_methods
    :undoc-members:
    :show-inheritance:

projection
----------

.. automodule:: brayns.core.projection
    :members: Fovy, get_camera_name, get_camera_projection, OrthographicProjection, PerspectiveProjection, Projection, set_camera_projection
    :undoc-members:
    :show-inheritance:

framebuffer
-----------

.. automodule:: brayns.core.framebuffer
    :members: Framebuffer, ProgressiveFramebuffer, set_framebuffer, StaticFramebuffer
    :undoc-members:
    :show-inheritance:

gbuffer_exporter
----------------

.. automodule:: brayns.core.gbuffer_exporter
    :members: GBufferChannel, GBufferExporter
    :undoc-members:
    :show-inheritance:

near_clip
---------

.. automodule:: brayns.core.near_clip
    :members: get_camera_near_clip, set_camera_near_clip
    :undoc-members:
    :show-inheritance:

view
----

.. automodule:: brayns.core.view
    :members: get_camera_view, set_camera_view, View
    :undoc-members:
    :show-inheritance:

loader
------

.. automodule:: brayns.core.loader
    :members: get_loaders, Loader, LoaderInfo, MeshLoader, MhdVolumeLoader, RawVolumeLoader, VolumeDataType
    :undoc-members:
    :show-inheritance:

renderer
--------

.. automodule:: brayns.core.renderer
    :members: get_renderer_name, get_renderer, InteractiveRenderer, ProductionRenderer, Renderer, set_renderer
    :undoc-members:
    :show-inheritance:

image
-----

.. automodule:: brayns.core.image
    :members: Image, ImageInfo
    :undoc-members:
    :show-inheritance:

snapshot
--------

.. automodule:: brayns.core.snapshot
    :members: ImageMetadata, Snapshot
    :undoc-members:
    :show-inheritance:

pick
----

.. automodule:: brayns.core.pick
    :members: pick, PickResult
    :undoc-members:
    :show-inheritance: