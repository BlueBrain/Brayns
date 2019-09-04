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
    SHADING_MODE_NONE = 0
    SHADING_MODE_DIFFUSE = 1
    SHADING_MODE_ELECTRON = 2
    SHADING_MODE_CARTOON = 3
    SHADING_MODE_ELECTRON_TRANSPARENCY = 4
    SHADING_MODE_PERLIN = 5
    SHADING_MODE_DIFFUSE_TRANSPARENCY = 6

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
        self._client = client.rockets_client

    # pylint: disable=W0102,R0913,R0914
    def load_circuit(self, path, name='Circuit', density=100.0,
                     random_seed=0, targets=list(), report='', report_type=REPORT_TYPE_VOLTAGES_FROM_FILE,
                     user_data_type=USER_DATATYPE_SIMULATION_OFFSET, synchronous_mode=True,
                     circuit_color_scheme=CIRCUIT_COLOR_SCHEME_NONE, mesh_folder='',
                     mesh_filename_pattern='', mesh_transformation=False, radius_multiplier=1,
                     radius_correction=0, load_soma=True, load_axon=True, load_dendrite=True,
                     load_apical_dendrite=True, use_sdf=False,
                     dampen_branch_thickness_changerate=True, use_metaballs_for_soma=False,
                     metaballs_section_samples=5, metaballs_grid_size=20, metaballs_threshold=1,
                     morphology_color_scheme=MORPHOLOGY_COLOR_SCHEME_NONE,
                     morphology_quality=GEOMETRY_QUALITY_HIGH, max_distance_to_soma=1e6,
                     cell_clipping=False):
        """
        Load a circuit from a give Blue/Circuit configuration file

        :param str path: Path to the CircuitConfig or BlueConfig configuration file
        :param str name: Name of the model
        :param float density: Circuit density (Value between 0 and 100)
        :param int random_seed: Random seed used if circuit density is different from 100
        :param list targets: List of targets to load
        :param str report: Name of the simulation report, if applicable
        :param int report_type: Report type (REPORT_TYPE_NONE, REPORT_TYPE_VOLTAGES_FROM_FILE,
        REPORT_TYPE_SPIKES)
        :param int user_data_type: Type of data mapped to the neuron surface (USER_DATATYPE_NONE,
        USER_DATATYPE_SIMULATION_OFFSET, USER_DATATYPE_DISTANCE_TO_SOMA)
        :param bool synchronous_mode: Defines if the simulation report should be loaded
        synchronously or not
        :param int circuit_color_scheme: Color scheme to apply to the circuit (
        CIRCUIT_COLOR_SCHEME_NONE, CIRCUIT_COLOR_SCHEME_NEURON_BY_ID,
        CIRCUIT_COLOR_SCHEME_NEURON_BY_LAYER, CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE,
        CIRCUIT_COLOR_SCHEME_NEURON_BY_ETYPE, CIRCUIT_COLOR_SCHEME_NEURON_BY_TARGET)
        :param str mesh_folder: Folder containing meshes (if applicable)
        :param str mesh_filename_pattern: Filename pattern used to load the meshes ({guid} is
        replaced by the correponding GID during the loading of the circuit. e.g. mesh_{gid}.obj)
        :param bool mesh_transformation: Boolean defining is circuit transformation should be
        applied to the meshes
        :param float radius_multiplier: Multiplies morphology radius by the specified value
        :param float radius_correction: Forces morphology radii to the specified value
        :param bool load_soma: Defines if the somas should be loaded
        :param bool load_axon: Defines if the axons should be loaded
        :param bool load_dendrite: Defines if the dendrites should be loaded
        :param bool load_apical_dendrite: Defines if the apical dendrites should be loaded
        :param bool use_sdf: Defines if signed distance field geometries should be used
        :param bool dampen_branch_thickness_changerate: Defines if the dampen branch
        thicknesschangerate option should be used (Only application is use_sdf is True)
        :param bool use_metaballs_for_soma: Defines if metaballs should be used to build the soma
        :param int metaballs_section_samples: Defines how many sections from the soma should be used
        to build the soma with metaballs (Only application if use_metaballs_for_soma is True)
        :param int metaballs_grid_size: Defines the size of grid to build the soma with metaballs (
        Only application if use_metaballs_for_soma is True)
        :param float metaballs_threshold: Defines the threshold to build the soma with metaballs (
        Only application if use_metaballs_for_soma is True)
        :param int morphology_color_scheme: Defines the color scheme to apply to the morphologies (
        MORPHOLOGY_COLOR_SCHEME_NONE, MORPHOLOGY_COLOR_SCHEME_BY_SECTION_TYPE)
        :param int morphology_quality: Defines the level of quality for each geometry (
        GEOMETRY_QUALITY_LOW, GEOMETRY_QUALITY_MEDIUM, GEOMETRY_QUALITY_HIGH)
        :param float max_distance_to_soma: Defines the maximum distance to the soma for section/
        segment loading (This is used by the growing neurons use-case)
        :param bool cell_clipping: Only load cells that are in the clipped region defined at the
        scene level
        :return: Result of the request submission
        :rtype: str
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
        props['011Gids'] = ''
        props['012PreNeuron'] = ''
        props['013PostNeuron'] = ''

        props['020Report'] = report
        props['021ReportType'] = report_type
        props['022UserDataType'] = user_data_type
        props['023SynchronousMode'] = synchronous_mode

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

        props['110SynapseRadius'] = 1.0
        props['111LoadAfferentSynapses'] = False
        props['112LoadEfferentSynapses'] = False

        params = dict()
        params['name'] = name
        params['path'] = path
        params['loader_properties'] = props

        return self._client.request(method='add-model', params=params)

    # pylint: disable=R0913, R0914
    def set_material(self, model_id, material_id, diffuse_color=(1.0, 1.0, 1.0),
                     specular_color=(1.0, 1.0, 1.0), specular_exponent=20.0, opacity=1.0,
                     reflection_index=0.0, refraction_index=1.0, simulation_data_cast=True,
                     glossiness=1.0, shading_mode=SHADING_MODE_NONE, emission=0.0, clipped=False):
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
        :param bool clipped: Clipped against clipping planes defined at the scene level
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['modelId'] = model_id
        params['materialId'] = material_id
        params['diffuseColor'] = diffuse_color
        params['specularColor'] = specular_color
        params['specularExponent'] = specular_exponent
        params['reflectionIndex'] = reflection_index
        params['opacity'] = opacity
        params['refractionIndex'] = refraction_index
        params['emission'] = emission
        params['glossiness'] = glossiness
        params['simulationDataCast'] = simulation_data_cast
        params['shadingMode'] = shading_mode
        params['clipped'] = clipped

        return self._client.request("set-material", params=params,
                                    response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=W0102
    def set_materials(self, model_ids, material_ids, diffuse_colors, specular_colors,
                      specular_exponents=list(), opacities=list(), reflection_indices=list(),
                      refraction_indices=list(), simulation_data_casts=list(), glossinesses=list(),
                      shading_modes=list(), emissions=list(), clips=list()):
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
        :param list clips: List of boolean values defining if materials should be clipped against
        clipping planes defined at the scene level
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['modelIds'] = model_ids
        params['materialIds'] = material_ids

        dc = list()
        for diffuse in diffuse_colors:
            for k in range(3):
                dc.append(diffuse[k])
        params['diffuseColors'] = dc

        sc = list()
        for specular in specular_colors:
            for k in range(3):
                sc.append(specular[k])
        params['specularColors'] = sc

        params['specularExponents'] = specular_exponents
        params['reflectionIndices'] = reflection_indices
        params['opacities'] = opacities
        params['refractionIndices'] = refraction_indices
        params['emissions'] = emissions
        params['glossinesses'] = glossinesses
        params['simulationDataCasts'] = simulation_data_casts
        params['shadingModes'] = shading_modes
        params['clips'] = clips

        return self._client.request("set-materials", params=params,
                                    response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    def save_model_to_cache(self, model_id, path):
        """
        Save a model to the specified cache file

        :param int model_id: Id of the model to save
        :param str path: Path of the cache file
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['modelId'] = model_id
        params['path'] = path
        return self._client.request('save-model-to-cache', params=params,
                                    response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    def set_material_extra_attributes(self, model_id):
        """
        Add extra attributes to all materials in the model (shading mode, clipped, etc)

        :param int model_id: Id of the model
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['modelId'] = model_id
        return self._client.request('set-material-extra-attributes', params=params,
                                    response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

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
        return self._client.request('set-camera', params,
                                    response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    def get_camera(self):
        """
        Gets the origin, direction and up vector of the camera

        :return: A JSon representation of the origin, direction and up vectors
        :rtype: str
        """
        return self._client.request('get-camera', response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

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
        params['minValue'] = min_value
        params['maxValue'] = max_value
        params['steps'] = interval
        params['radius'] = radius
        params['planeOpacity'] = opacity
        params['showAxis'] = show_axis
        params['useColors'] = colored
        return self._client.request('add-grid', params,
                                    response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    def export_frames_to_disk(self, path, animation_frames, camera_definitions, image_format='png',
                              quality=100, samples_per_pixel=1, start_frame=0):
        """
        Exports frames to disk. Frames are named using a 6 digit representation of the frame number

        :param str path: Folder into which frames are exported
        :param list animation_frames: List of animation frames
        :param list camera_definitions: List of camera definitions (origin, direction and up)
        :param str image_format: Image format (the ones supported par Brayns: PNG, JPEG, etc)
        :param float quality: Quality of the exported image (Between 0 and 100)
        :param int samples_per_pixel: Number of samples per pixels
        :param int start_frame: Optional value if the rendering should start at a specific frame.
        This is used to resume the rendering of a previously canceled sequence)
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params['path'] = path
        params['format'] = image_format
        params['quality'] = quality
        params['spp'] = samples_per_pixel
        params['startFrame'] = start_frame
        params['animationInformation'] = animation_frames
        values = list()
        for camera_definition in camera_definitions:
            for i in range(3):
                values.append(camera_definition[0][i])
            for i in range(3):
                values.append(camera_definition[1][i])
            for i in range(3):
                values.append(camera_definition[2][i])
        params['cameraInformation'] = values
        return self._client.request('export-frames-to-disk', params,
                                    response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

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
        params['startFrame'] = 0
        params['animationInformation'] = []
        params['cameraInformation'] = []
        return self._client.request('export-frames-to-disk', params,
                                    response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)
