.. _recordsimulationexample-label:

Record a simulation video
=========================

In this example we will load a BlueConfig file from a circuit, specifying a
compartment report from which we will record a video.

We will make use of the :ref:`cemodule-label` API, as it make it much easier to load
circuits into Brayns, as well as to export the simulation frames to disk and compile them
into a video media file. The Circuit Explorer plugin must be specified when launching
the Brayns backend.

Since we are also going to generate a media file, Brayns uses ``ffmpeg`` to do so. It
must be loaded before launching Brayns. If you started Brayns thought the WebBrayns web
interface, ``ffmpeg`` is loaded for you. If, on the other hand, you start the Brayns
backend service manually on BB5, you need to execute the following commands before launching
Brayns:

.. code-block:: console

    module load unstable
    module load ffmpeg/4.2

For further information on how to get started coding in Brayns, refer to:

* :ref:`launchbraynsbackend-label` to get a running Brayns service.
* :ref:`usepythonclient-label` to learn the first steps for any Brayns python client script.

Initialization
~~~~~~~~~~~~~~

For the initialization we add the needed imports, initialize both Brayns client and the
CircuitExplorer module, and define the path to our circuit, the name of the report
we want to load and a path to a folder where we will generate the frames and the video.

.. code-block:: python

    from brayns import Client, CircuitExplorer

    PATH_TO_CIRCUIT = "/home/nadir/Data/CircuitTestData/share/BBPTestData/circuitBuilding_1000neurons/CircuitConfig"
    REPORT_NAME = "somas"

    FRAME_VIDEO_PATH = "/home/nadir/Desktop/my_brayns_video"

    braynsClient = Client("r1i1n1.bbp.epfl.ch:5000")
    circuitExplorer = CircuitExplorer(braynsClient)

Circuit loading
~~~~~~~~~~~~~~~

We have now all the needed objects to actually request the circuit to be loaded by brayns.
This is done with a single API call from the CircuitExplorer module API: ``load_circuit``.

.. code-block:: python

    res = circuitExplorer.load_circuit(
                path=PATH_TO_CIRCUIT,                                       # The circuit itself
                density=100.0,                                              # We want the full density
                report_type=CircuitExplorer.REPORT_TYPE_VOLTAGES_FROM_FILE, # Type of report we want to load
                report=REPORT_NAME,                                         # Name of the report we want to load
                load_soma=True,                                             # We want to load the somas
                load_axon=False,                                            # We do NOT want to load the axons
                load_dendrite=False,                                        # We do NOT want to load the dendrites
                load_apical_dendrite=False,                                 # We do NOT want to load the apical dendrites
                radius_multiplier=8.0                                       # We want the soma to be "inflated" 8 times its normal size.
                )

After the function returns, the circuit will have been loaded, and we are ready
to request the frame export.

Export simulation frames as images to disk
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to generate a video from a simulation, first we have to write the
images that will compose such video to disk. This is done with a single call
from the CircuitExplorer module API: ``export_frames_to_disk``.

The ``export_frames_to_disk`` method works by specifying a list of the frame numbers
from the simulation that we want to export as images. We also need to specify, for
each one of these frames, an ODU camera definition. This allows to create a camera animation
that will be followed thought out the recording, adding smooth and beautiful navigation
thought the circuit while the simulation is executed. In order to know how to get
the camera parameters in this format, refer to :ref:`examplegetcamera-label`.

In this example, we will export the first 100 frames from the simulation, recording
from a static point of view, the same one we obtained in :ref:`examplegetcamera-label`.

.. code-block:: python

    # Camera parameters
    origin = [38.755, 998.582, 2370.736]
    direction = [0, 0, -1]
    up = [0, 1, 0]
    aperture = 0
    focus_distance = 0

    # Add all the camera lists and parameters into a list
    single_frame_camera = [origin, direction, up, aperture, focus_distance]

    # Gather the frames we want to render, and compose
    # the camera definitions array
    frames = list()
    camera_defs = list()
    for i in range(100):
        frames.append(i)
        camera_defs.append(single_frame_camera)

    circuitExplorer.export_frames_to_disk(path=FRAME_VIDEO_PATH,            # Where we will store the images
                                          animation_frames=frames,          # The list of simulation frames to export
                                          camera_definitions=camera_defs,   # The camera for each of the frames
                                          samples_per_pixel=64)             # 64 samples per pixel, good quality.


This request is asynchronous, meaning that the function will return immediately
after the call, although the export process is still onging.

Waiting for the frames to be exported
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The CircuitExplorer API provides a method that tell us the percentage of frames
that has been exported at the momment of the call: ``get_export_frames_progress``.

.. error::

    At the momment, the ``get_export_frames_progress`` does not work well when
    running the script remotely from the Brayns backend service.

    If you are running the script in the same machine where the backend is
    running, you should not have problems.

    Otherwise, the use of this function is discouraged for the time being.
    You should manually check that the requested frames image files are
    stored in the path you specify, before proceding further.

.. code-block:: python

    # We use time to sleep the python script while waiting for the
    # export to finish
    import time

    progress = 0.0

    while progress < 1.0:

        # Get the current export frame progress and print it.
        progress = circuitExplorer.get_export_frames_progress()["progress"]
        progressPercent = min(int(progress * 100.0), 100);
        print("Done " + str(progressPercent) + "%")

        time.sleep(3) # Wait three seconds before checking the progress again.


Generating the video media file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the frames have been finished being exported to disk, we can
request Brayns to generate a video out of them. This, again, is done with
a single CircuitExplorer api call: ``make_movie``.

.. code-block:: python

    # We will have to specify to make_movie the path where to store the video
    # In this path, we will include the name, as well as the EXTENSION.
    # In this case the name will be "my_movie" and the extension ".mp4"
    generated_video_path = FRAME_VIDEO_PATH + "/my_movie.mp4"

    make_movie(output_movie_path=generated_video_path,  # Where to store the movie
               fps_rate=30,                             # Frame rate. We sent 100 frames, at 30 fps, the movie will last for about 3 seconds
               frames_folder_path=FRAME_VIDEO_PATH,     # Where to find the frames to use to make the video
               dimensions=[1920, 1080],                 # The video resolution (full hd in this case)
               erase_frames=True)                       # Wether we want to delete the image files after the video is done.


After the command returns, we will find the movie in the given path, with
the given name. If the movie generation fails, but ``erase_frames`` is ``True``,
Brayns will NOT delete the frames. The frames are deleted only upon successful
movie file generation.
