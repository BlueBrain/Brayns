#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2019, Blue Brain Project
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
# All rights reserved. Do not distribute without further notice.

"""Provides a class that wraps the API exposed by the braynsCircuitExplorer plug-in"""


class CircuitExplorer:
    """Circuit Explorer, a class that wraps the API exposed by the braynsCircuitExplorer plug-in"""

    # Network defaults
    DEFAULT_RESPONSE_TIMEOUT = 360

    # Circuit color schemes
    CIRCUIT_COLOR_SCHEME_NONE = 'None'
    CIRCUIT_COLOR_SCHEME_NEURON_BY_ID = 'By id'
    CIRCUIT_COLOR_SCHEME_NEURON_BY_LAYER = 'By layer'
    CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE = 'By mtype'
    CIRCUIT_COLOR_SCHEME_NEURON_BY_ETYPE = 'By etype'
    CIRCUIT_COLOR_SCHEME_NEURON_BY_TARGET = 'By target'
    CIRCUIT_COLOR_SCHEME_NEURON_SINGLE_COLOR = 'Single color'

    # Morphology color schemes
    MORPHOLOGY_COLOR_SCHEME_NONE = 'None'
    MORPHOLOGY_COLOR_SCHEME_BY_SECTION_TYPE = 'By segment type'

    # Morphology types
    MORPHOLOGY_SECTION_TYPE_ALL = 255
    MORPHOLOGY_SECTION_TYPE_SOMA = 1
    MORPHOLOGY_SECTION_TYPE_AXON = 2
    MORPHOLOGY_SECTION_TYPE_DENDRITE = 4
    MORPHOLOGY_SECTION_TYPE_APICAL_DENDRITE = 8

    # Geometry qualities
    GEOMETRY_QUALITY_LOW = 0
    GEOMETRY_QUALITY_MEDIUM = 1
    GEOMETRY_QUALITY_HIGH = 2

    # Shading modes
    SHADING_MODE_NONE = 'None'
    SHADING_MODE_DIFFUSE = 'Diffuse'
    SHADING_MODE_ELECTRON = 'Electron'
    SHADING_MODE_CARTOON = 'Cartoon'
    SHADING_MODE_ELECTRON_TRANSPARENCY = 'Electron transparency'
    SHADING_MODE_PERLIN = 'Perlin'
    SHADING_MODE_DIFFUSE_TRANSPARENCY = 'Diffuse transparency'
    SHADING_MODE_CHECKER = 'Checker'

    # Clipping modes
    CLIPPING_MODE_NONE = 'No clipping'
    CLIPPING_MODE_PLANE = 'Plane'
    CLIPPING_MODE_SPHERE = 'Sphere'

    # Simulation report types
    REPORT_TYPE_NONE = 'Undefined'
    REPORT_TYPE_VOLTAGES_FROM_FILE = 'Voltages from file'
    REPORT_TYPE_SPIKES = 'Spikes'

    # User data types
    USER_DATATYPE_NONE = 'Undefined'
    USER_DATATYPE_SIMULATION_OFFSET = 'Simulation offset'
    USER_DATATYPE_DISTANCE_TO_SOMA = 'Distance to soma'

    def __init__(self, client):
        """Create a new Circuit Explorer instance"""
        self._client = client

    # pylint: disable=W0102,R0913,R0914
    def load_circuit(self, path, name='Circuit', density=100.0, gids=list(),
                     random_seed=0, targets=list(), report='',
                     report_type=REPORT_TYPE_VOLTAGES_FROM_FILE,
                     user_data_type=USER_DATATYPE_SIMULATION_OFFSET,
                     synchronous_mode=True,
                     circuit_color_scheme=CIRCUIT_COLOR_SCHEME_NONE,
                     mesh_folder='', mesh_filename_pattern='',
                     mesh_transformation=False, radius_multiplier=1,
                     radius_correction=0, load_soma=True, load_axon=True,
                     load_dendrite=True, load_apical_dendrite=True,
                     use_sdf=False, dampen_branch_thickness_changerate=True,
                     use_metaballs_for_soma=False,
                     metaballs_section_samples=5, metaballs_grid_size=20,
                     metaballs_threshold=1,
                     morphology_color_scheme=MORPHOLOGY_COLOR_SCHEME_NONE,
                     morphology_quality=GEOMETRY_QUALITY_HIGH,
                     max_distance_to_soma=1e6, cell_clipping=False,
                     load_afferent_synapses=False,
                     load_efferent_synapses=False, synapse_radius=0.0,
                     load_layers=True, load_etypes=True, load_mtypes=True):
        """
        Load a circuit from a give Blue/Circuit configuration file.

        :param str path: Path to the CircuitConfig or BlueConfig configuration
               file.
        :param str name: Name of the model.
        :param float density: Circuit density (Value between 0 and 100).
        :param list gids: List of GIDs to load.
        :param int random_seed: Random seed used if circuit density is
             different from 100.
        :param list targets: List of targets to load.
        :param str report: Name of the simulation report, if applicable.
        :param int report_type: Report type (REPORT_TYPE_NONE,
            REPORT_TYPE_VOLTAGES_FROM_FILE, REPORT_TYPE_SPIKES).
        :param int user_data_type: Type of data mapped to the neuron surface
            (USER_DATATYPE_NONE, USER_DATATYPE_SIMULATION_OFFSET,
            USER_DATATYPE_DISTANCE_TO_SOMA).
        :param bool synchronous_mode: Defines if the simulation report should
            be loaded synchronously or not.
        :param int circuit_color_scheme: Color scheme to apply to the circuit (
            CIRCUIT_COLOR_SCHEME_NONE, CIRCUIT_COLOR_SCHEME_NEURON_BY_ID,
            CIRCUIT_COLOR_SCHEME_NEURON_BY_LAYER,
            CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE,
            CIRCUIT_COLOR_SCHEME_NEURON_BY_ETYPE,
            CIRCUIT_COLOR_SCHEME_NEURON_BY_TARGET).
        :param str mesh_folder: Folder containing meshes (if applicable).
        :param str mesh_filename_pattern: Filename pattern used to load the
            meshes ({guid} is replaced by the correponding GID during the
            loading of the circuit. e.g. mesh_{gid}.obj).
        :param bool mesh_transformation: Boolean defining is circuit
            transformation should be applied to the meshes.
        :param float radius_multiplier: Multiplies morphology radius by the
            specified value.
        :param float radius_correction: Forces morphology radii to the
            specified value.
        :param bool load_soma: Defines if the somas should be loaded.
        :param bool load_axon: Defines if the axons should be loaded.
        :param bool load_dendrite: Defines if the dendrites should be loaded.
        :param bool load_apical_dendrite: Defines if the apical dendrites
            should be loaded.
        :param bool use_sdf: Defines if signed distance field geometries
            should be used.
        :param bool dampen_branch_thickness_changerate: Defines if the dampen
            branch thickness change rate option should be used (Only
            application is use_sdf is True).
        :param bool use_metaballs_for_soma: Defines if metaballs should be
            used to build the soma.
        :param int metaballs_section_samples: Defines how many sections from
            the soma should be used to build the soma with metaballs (Only
            application if use_metaballs_for_soma is True).
        :param int metaballs_grid_size: Defines the size of grid to build the
            soma with metaballs (Only application if use_metaballs_for_soma is
            True).
        :param float metaballs_threshold: Defines the threshold to build the
            soma with metaballs (Only application if use_metaballs_for_soma is
            True).
        :param int morphology_color_scheme: Defines the color scheme to apply
            to the morphologies (MORPHOLOGY_COLOR_SCHEME_NONE,
            MORPHOLOGY_COLOR_SCHEME_BY_SECTION_TYPE).
        :param int morphology_quality: Defines the level of quality for each
            geometry (GEOMETRY_QUALITY_LOW, GEOMETRY_QUALITY_MEDIUM,
            GEOMETRY_QUALITY_HIGH).
        :param float max_distance_to_soma: Defines the maximum distance to the
            soma for section/segment loading (This is used by the growing
            neurons use-case).
        :param bool cell_clipping: Only load cells that are in the clipped
            region defined at the scene level.
        :param bool load_afferent_synapses: Load afferent synapses.
        :param bool load_efferent_synapses: Load efferent synapses.
        :param float synapse_radius: Synapse radius.
        :param bool load_layers: Load layer data for coloring the circuit.
            If False, speed up circuit loading. Ignored if circuit_color_scheme
            is CIRCUIT_COLOR_SCHEME_NEURON_BY_LAYER
        :param bool load_etypes: Load etypes for coloring the circuit.
            if False, speed up circuit loading. Ignored if circuit_color_scheme
            is CIRCUIT_COLOR_SCHEME_NEURON_BY_ETYPE
        :param bool load_mtypes: Load mtypes for coloring the circuit.
            if False, speed up circuit loading. Ignored if circuit_color_scheme
            is CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE

        :return: Model metadata if successful.
        :rtype: dict
        """

        props = dict()
        props['000DbConnectionString'] = ''  # Currently not used
        props['001Density'] = density / 100.0
        props['002RandomSeed'] = random_seed

        targets_as_string = ''
        for target in targets:
            if targets_as_string != '':
                targets_as_string += ','
            targets_as_string += target
        props['010Targets'] = targets_as_string

        gids_as_string = ''
        for gid in gids:
            if gids_as_string != '':
                gids_as_string += ','
            gids_as_string += str(gid)
        props['011Gids'] = gids_as_string
        props['012PreNeuron'] = ''
        props['013PostNeuron'] = ''

        props['020Report'] = report
        props['021ReportType'] = report_type
        props['022UserDataType'] = user_data_type
        props['023SynchronousMode'] = synchronous_mode
        props['024SpikeTransitionTime'] = 1.0

        props['030CircuitColorScheme'] = circuit_color_scheme

        props['040MeshFolder'] = mesh_folder
        props['041MeshFilenamePattern'] = mesh_filename_pattern
        props['042MeshTransformation'] = mesh_transformation

        props['050RadiusMultiplier'] = radius_multiplier
        props['051RadiusCorrection'] = radius_correction
        props['052SectionTypeSoma'] = load_soma
        props['053SectionTypeAxon'] = load_axon
        props['054SectionTypeDendrite'] = load_dendrite
        props['055SectionTypeApicalDendrite'] = load_apical_dendrite

        props['060UseSdfgeometry'] = use_sdf
        props['061DampenBranchThicknessChangerate'] = dampen_branch_thickness_changerate

        props['070RealisticSoma'] = use_metaballs_for_soma
        props['071MetaballsSamplesFromSoma'] = metaballs_section_samples
        props['072MetaballsGridSize'] = metaballs_grid_size
        props['073MetaballsThreshold'] = metaballs_threshold

        props['080MorphologyColorScheme'] = morphology_color_scheme

        props['090MorphologyQuality'] = morphology_quality
        props['091MaxDistanceToSoma'] = max_distance_to_soma
        props['100CellClipping'] = cell_clipping
        props['101AreasOfInterest'] = 0

        props['110SynapseRadius'] = synapse_radius
        props['111LoadAfferentSynapses'] = load_afferent_synapses
        props['112LoadEfferentSynapses'] = load_efferent_synapses

        props['120LoadLayers'] = load_layers
        props['121LoadEtypes'] = load_etypes
        props['122LoadMtypes'] = load_mtypes

        params = dict()
        params['name'] = name
        params['path'] = path
        params['loader_properties'] = props

        return self._get('add-model', params)

    # pylint: disable=R0913, R0914
    def set_material(self, model_id, material_id, diffuse_color=(1.0, 1.0, 1.0),
                     specular_color=(1.0, 1.0, 1.0), specular_exponent=20.0, opacity=1.0,
                     reflection_index=0.0, refraction_index=1.0, simulation_data_cast=True,
                     glossiness=1.0, shading_mode=SHADING_MODE_NONE, emission=0.0,
                     clipping_mode=CLIPPING_MODE_NONE, user_parameter=0.0):
        """
        Set a material on a specified model

        :param int model_id: ID of the model
        :param int material_id: ID of the material
        :param list diffuse_color: Diffuse color (3 values between 0 and 1)
        :param list specular_color: Specular color (3 values between 0 and 1)
        :param list specular_exponent: Diffuse exponent
        :param float opacity: Opacity
        :param float reflection_index: Reflection index (value between 0 and 1)
        :param float refraction_index: Refraction index
        :param bool simulation_data_cast: Casts simulation information
        :param float glossiness: Glossiness (value between 0 and 1)
        :param int shading_mode: Shading mode (SHADING_MODE_NONE, SHADING_MODE_DIFFUSE,
        SHADING_MODE_ELECTRON, SHADING_MODE_CARTOON, SHADING_MODE_ELECTRON_TRANSPARENCY,
        SHADING_MODE_PERLIN or SHADING_MODE_DIFFUSE_TRANSPARENCY)
        :param float emission: Light emission intensity
        :param bool clipping_mode: Clipped against clipping planes/spheres defined at the scene
        level
        :param float user_parameter: Convenience parameter used by some of the shaders
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['model_id'] = model_id
        params['material_id'] = material_id
        params['diffuse_color'] = diffuse_color
        params['specular_color'] = specular_color
        params['specular_exponent'] = specular_exponent
        params['reflection_index'] = reflection_index
        params['opacity'] = opacity
        params['refraction_index'] = refraction_index
        params['emission'] = emission
        params['glossiness'] = glossiness
        params['simulation_data_cast'] = simulation_data_cast
        params['shading_mode'] = shading_mode
        params['clipping_mode'] = clipping_mode
        params['user_parameter'] = user_parameter
        return self._get("set-material", params)

    # pylint: disable=W0102
    def set_materials(self, model_ids, material_ids, diffuse_colors, specular_colors,
                      specular_exponents=list(), opacities=list(), reflection_indices=list(),
                      refraction_indices=list(), simulation_data_casts=list(), glossinesses=list(),
                      shading_modes=list(), emissions=list(), clipping_modes=list(),
                      user_parameters=list()):
        """
        Set a list of material on a specified list of models

        :param int model_ids: IDs of the models
        :param int material_ids: IDs of the materials
        :param list diffuse_colors: List of diffuse colors (3 values between 0 and 1)
        :param list specular_colors: List of specular colors (3 values between 0 and 1)
        :param list specular_exponents: List of diffuse exponents
        :param list opacities: List of opacities
        :param list reflection_indices: List of reflection indices (value between 0 and 1)
        :param list refraction_indices: List of refraction indices
        :param list simulation_data_casts: List of cast simulation information
        :param list glossinesses: List of glossinesses (value between 0 and 1)
        :param list shading_modes: List of shading modes (SHADING_MODE_NONE, SHADING_MODE_DIFFUSE,
        SHADING_MODE_ELECTRON, SHADING_MODE_CARTOON, SHADING_MODE_ELECTRON_TRANSPARENCY,
        SHADING_MODE_PERLIN or SHADING_MODE_DIFFUSE_TRANSPARENCY)
        :param list emissions: List of light emission intensities
        :param list clipping_modes: List of clipping modes defining if materials should be clipped
        against clipping planes, spheres, etc, defined at the scene level
        :param list user_parameters: List of convenience parameter used by some of the shaders
        :return: Result of the request submission
        :rtype: str
        """
        material_count = len(model_ids)
        materials = material_count * [{}]
        for i in range(len(model_ids)):
            material = materials[i]
            material['model_id'] = model_ids[i]
            material['material_id'] = material_ids[i]
            material['diffuse_color'] = diffuse_colors[i]
            material['specular_color'] = specular_colors[i]
            material['specular_exponent'] = specular_exponents[i]
            material['reflection_index'] = reflection_indices[i]
            material['opacity'] = opacities[i]
            material['refraction_index'] = refraction_indices[i]
            material['emission'] = emissions[i]
            material['glossiness'] = glossinesses[i]
            material['simulation_data_cast'] = simulation_data_casts[i]
            material['shading_mode'] = shading_modes[i]
            material['clipping_mode'] = clipping_modes[i]
            material['user_parameter'] = user_parameters[i]
        return self._get("set-materials", {'materials': materials})

    # pylint: disable=R0913, R0914
    def set_material_range(self, model_id, material_ids, diffuse_color=(1.0, 1.0, 1.0),
                           specular_color=(1.0, 1.0, 1.0), specular_exponent=20.0, opacity=1.0,
                           reflection_index=0.0, refraction_index=1.0, simulation_data_cast=True,
                           glossiness=1.0, shading_mode=SHADING_MODE_NONE, emission=0.0,
                           clipping_mode=CLIPPING_MODE_NONE, user_parameter=0.0):
        """
        Set a range of materials on a specified model

        :param int model_id: ID of the model
        :param list material_ids: IDs of the material to change
        :param list diffuse_color: Diffuse color (3 values between 0 and 1)
        :param list specular_color: Specular color (3 values between 0 and 1)
        :param float specular_exponent: Diffuse exponent
        :param float opacity: Opacity
        :param float reflection_index: Reflection index (value between 0 and 1)
        :param float refraction_index: Refraction index
        :param bool simulation_data_cast: Casts simulation information
        :param float glossiness: Glossiness (value between 0 and 1)
        :param int shading_mode: Shading mode (SHADING_MODE_NONE, SHADING_MODE_DIFFUSE,
        SHADING_MODE_ELECTRON, SHADING_MODE_CARTOON, SHADING_MODE_ELECTRON_TRANSPARENCY,
        SHADING_MODE_PERLIN or SHADING_MODE_DIFFUSE_TRANSPARENCY)
        :param float emission: Light emission intensity
        :param int clipping_mode: Clipped against clipping planes/spheres defined at the scene
        level
        :param float user_parameter: Convenience parameter used by some of the shaders
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['model_id'] = model_id
        params['material_ids'] = material_ids
        properties = dict()
        properties['diffuse_color'] = diffuse_color
        properties['specular_color'] = specular_color
        properties['specular_exponent'] = specular_exponent
        properties['reflection_index'] = reflection_index
        properties['opacity'] = opacity
        properties['refraction_index'] = refraction_index
        properties['emission'] = emission
        properties['glossiness'] = glossiness
        properties['simulation_data_cast'] = simulation_data_cast
        properties['shading_mode'] = shading_mode
        properties['clipping_mode'] = clipping_mode
        properties['user_parameter'] = user_parameter
        params['properties'] = properties
        return self._get("set-material-range", params)

    def save_model_to_cache(self, model_id, path):
        """
        Save a model to the specified cache file

        :param int model_id: Id of the model to save
        :param str path: Path of the cache file
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['model_id'] = model_id
        params['path'] = path
        return self._get('save-model-to-cache', params)

    def set_material_extra_attributes(self, model_id):
        """
        Add extra attributes to all materials in the model (shading mode, clipped, etc)

        :param int model_id: Id of the model
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['model_id'] = model_id
        return self._get('set-material-extra-attributes', params)

    def set_camera(self, origin, direction, up):
        """
        Sets the camera using origin, direction and up vectors

        :param list origin: Origin of the camera
        :param list direction: Direction in which the camera is looking
        :param list up: Up vector
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['origin'] = origin
        params['direction'] = direction
        params['up'] = up
        return self._get('set-odu-camera', params)

    def get_camera(self):
        """
        Gets the origin, direction and up vector of the camera

        :return: A JSon representation of the origin, direction and up vectors
        :rtype: str
        """
        return self._get('get-odu-camera')

    def add_grid(self, min_value, max_value, interval, radius=1.0, opacity=0.5, show_axis=True,
                 colored=True):
        """
        Adds a reference grid to the scene

        :param float min_value: Minimum value for all axis
        :param float max_value: Maximum value for all axis
        :param float interval: Interval at which lines should appear on the grid
        :param float radius: Radius of grid lines
        :param float opacity: Opacity of the grid
        :param bool show_axis: Shows axis if True
        :param bool colored: Colors the grid it True. X in red, Y in green, Z in blue
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['min_value'] = min_value
        params['max_value'] = max_value
        params['steps'] = interval
        params['radius'] = radius
        params['plane_opacity'] = opacity
        params['show_axis'] = show_axis
        params['use_colors'] = colored
        return self._get('add-grid', params)

    def add_column(self, radius=0.01):
        """
        Adds a reference column to the scene

        :param float radius: Radius of column spheres, cylinders radii are half size.
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['radius'] = radius
        return self._get('add-column', params)

    def export_frames_to_disk(self, path, animation_frames, camera_definitions, image_format='png',
                              quality=100, samples_per_pixel=1, start_frame=0, name_after_step=False):
        """
        Exports frames to disk. Frames are named using a 6 digit representation of the frame number

        :param str path: Folder into which frames are exported
        :param list animation_frames: List of animation frames
        :param list camera_definitions: List of camera definitions (origin, direction and up)
        :param str image_format: Image format (the ones supported par Brayns: PNG, JPEG, etc)
        :param float quality: Quality of the exported image (Between 0 and 100)
        :param int samples_per_pixel: Number of samples per pixels
        :param int start_frame: Optional value if the rendering should start at a specific frame.
        :param bool name_after_step: Name the file on disk after the simulation step index.
        This is used to resume the rendering of a previously canceled sequence)
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['path'] = path
        params['format'] = image_format
        params['name_after_step'] = name_after_step
        params['quality'] = quality
        params['spp'] = samples_per_pixel
        params['start_frame'] = start_frame
        params['animation_information'] = animation_frames
        values = list()
        for camera_definition in camera_definitions:
            # Origin
            for i in range(3):
                values.append(camera_definition[0][i])
            # Direction
            for i in range(3):
                values.append(camera_definition[1][i])
            # Up
            for i in range(3):
                values.append(camera_definition[2][i])
            # Aperture radius
            values.append(camera_definition[3])
            # Focus distance
            values.append(camera_definition[4])
        params['camera_information'] = values
        return self._get('export-frames-to-disk', params)

    def get_export_frames_progress(self):
        """
        Queries the progress of the last export of frames to disk request

        :return: Dictionary with the result: "progress" with the normalized
            progress of the frame export. 1.0 or higher value means the export
            has finished.
        :rtype: dict
        """
        return self._get('get-export-frames-progress')

    def make_movie(self, output_movie_path, fps_rate, frames_folder_path,
                   frame_file_extension="png", dimensions=[1920, 1080], erase_frames=True):
        """
        Request to create a media video file from a set of frames

        :param str output_movie_path: Full path to the media video to store the movie
        (it will be created if it does not exists). It must include extension, as it will be used
        to determine the codec to be used (by_default it should be .mp4)
        :param int fps_rate: Desired frame rate in the video
        :param str frames_folder_path: Path to the folder containing the frames to be used to
        create the video
        :param str frame_file_extension: Image format to use (png, jpg)
        :param list dimensions: Desired width and height of the video to be created
        :param bool erase_frames: Wether to remove the frames after the video
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['dimensions'] = dimensions
        params['frames_folder_path'] = frames_folder_path
        params['frames_file_extension'] = frame_file_extension
        params['fps_rate'] = fps_rate
        params['output_movie_path'] = output_movie_path
        params['erase_frames'] = erase_frames

        return self._get('make-movie', params)

    def cancel_frames_export(self):
        """
        Cancel the exports of frames to disk

        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['path'] = '/tmp'
        params['format'] = 'png'
        params['quality'] = 100
        params['spp'] = 1
        params['start_frame'] = 0
        params['animation_information'] = []
        params['camera_information'] = []
        return self._get('export-frames-to-disk', params)

    def trace_anterograde(self, model_id, source_cells_gid, target_cells_gid,
                          source_cells_color=(5, 5, 0, 1), target_cells_color=(5, 0, 0, 1),
                          non_connected_color=(0.5, 0.5, 0.5, 1.0)):
        """
        Stain the cells based on their synapses

        :param int model_id: ID of the model to trace
        :param list source_cells_gid: list of cell GIDs as source of the connections
        :param list target_cells_gid: list of cell GIDs connected to the source(s)
        :param list source_cell_color: RGBA 4 floating point list as color for source cells
        :param list target_cell_color: RGBA 4 floating point list as color for target cells
        :param list non_connected_color: RGBA 4 floating point list as color for non connected cells
        :return: Result of the request submission as a dictionary {error:int, message:string}
        :rtype: dict
        """
        params = dict()
        params['model_id'] = model_id
        params['cell_gids'] = source_cells_gid
        params['target_cell_gids'] = target_cells_gid
        params['source_cell_color'] = source_cells_color
        params['connected_cells_color'] = target_cells_color
        params['non_connected_cells_color'] = non_connected_color
        return self._get('trace-anterograde', params)

    def add_sphere(self, center, radius, color, name=""):
        """
        Creates and adds a sphere to the scene

        :param list center: Position (in global coordinates) of the sphere center.
        :param float radius: Radius of the sphere
        :param list color: Color with transparency of the sphere (RGBA)
        :param str name: Name to give to the model to add
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['center'] = center
        params['radius'] = radius
        params['color'] = color
        params['name'] = name
        return self._get('add-sphere', params)

    def add_pill(self, p1, p2, radius, color, name=""):
        """
        Creates and adds a pill shape to the scene

        :param list p1: Position (in global coordinates) of the first pivot
        :param list p2: Position (in global coordinates) of the second pivot.
        :param float radius: Radius of the pill sides
        :param list color: Color with transparency of the pill (RGBA)
        :param str name: Name to give to the model to add
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['type'] = 'pill'
        params['p1'] = p1
        params['p2'] = p2
        params['radius1'] = radius
        params['radius2'] = radius
        params['color'] = color
        params['name'] = name
        return self._get('add-pill', params)

    def add_conepill(self, p1, p2, radius1, radius2, color, name=""):
        """
        Creates and adds a cone pill shape to the scene

        :param list p1: Position (in global coordinates) of the first pivot
        :param list p2: Position (in global coordinates) of the second pivot.
        :param float radius1: Radius to use around p1
        :param float radius2: Radius to use around p2
        :param list color: Color with transparency of the cone pill (RGBA)
        :param str name: Name to give to the model to add
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['type'] = 'conepill'
        params['p1'] = p1
        params['p2'] = p2
        params['radius1'] = radius1
        params['radius2'] = radius2
        params['color'] = color
        params['name'] = name
        return self._get('add-pill', params)

    def add_sigmoidpill(self, p1, p2, radius1, radius2, color, name=""):
        """
        Creates and adds a sigmoid pill (smoothed) shape to the scene

        :param list p1: Position (in global coordinates) of the first pivot
        :param list p2: Position (in global coordinates) of the second pivot.
        :param float radius1: Radius to use around p1
        :param float radius2: Radius to use around p2
        :param list color: Color with transparency of the sigmoid pill (RGBA)
        :param str name: Name to give to the model to add
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['type'] = 'sigmoidpill'
        params['p1'] = p1
        params['p2'] = p2
        params['radius1'] = radius1
        params['radius2'] = radius2
        params['color'] = color
        params['name'] = name
        return self._get('add-pill', params)

    def add_cylinder(self, center, up, radius, color, name=""):
        """
        Creates and adds a cylinder shape to the scene

        :param list center: Position of the center of the base of the cylinder
        :param list up: Position of the center of the top of the cylinder
        :param float radius: Radius of the cylinder
        :param list color: Color with transparency of the cylinder (RGBA)
        :param str name: Name to give to the model to add
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['center'] = center
        params['up'] = up
        params['radius'] = radius
        params['color'] = color
        params['name'] = name
        return self._get('add-cylinder', params)

    def add_box(self, minCorner, maxCorner, color, name=""):
        """
        Creates and adds a box shape to the scene

        :param list minCorner: Position of the minimun corner of the box
        :param list maxCorner: Position of the maximum corner of the box
        :param list color: Color with transparency of the box (RGBA)
        :param str name: Name to give to the model to add
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['min_corner'] = minCorner
        params['max_corner'] = maxCorner
        params['color'] = color
        params['name'] = name
        return self._get('add-box', params)

    def get_material_ids(self, model_id):
        """
        Return the list of material ids for the given model

        :param int model_id: Id of the model to query for material ids
        :return: list List of material ids
        :rtype: str
        """
        params = dict()
        params['model_id'] = model_id
        return self._get('get-material-ids', params)

    def _get(self, method, params=None):
        return self._client.get(
            method=method,
            params=params,
            timeout=CircuitExplorer.DEFAULT_RESPONSE_TIMEOUT
        )
