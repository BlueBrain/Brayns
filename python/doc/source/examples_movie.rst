Make a movie from exported frames
=================================

Now that we have exported some simulation frames, we can make a movie from it.

FFMPEG
------

Brayns Python package relies on FFMPEG binary executable to create a movie from
exported frames.

On Ubuntu personal machines, it can be installed with this command.

.. code-block:: console

    $ sudo apt install ffmpeg

On a BB5 node you can use these commands.

.. code-block:: console

    $ module load unstable
    $ module load ffmpeg

Movie
-----

The `Movie` class in Brayns Python package is only a convenience tools to wrap
an FFMPEG command line. It requires the frames folder, their format and the
movie FPS. An incoherent FPS from the export will generate an incorrect movie
duration.

.. code-block:: python

    movie = brayns.Movie(
        frames_folder='path/to/frames',
        frames_format=brayns.ImageFormat.PNG,
        fps=25,
    )

    movie.save('path/to/movie.mp4')

The duration of the output movie is simply fps x frame_count. A different
resolution can be specified but by default the frame size is used. The path to
FFMPEG executable is `ffmpeg` by default but can be modified.

.. attention::
    The frame filenames must have the exporter format with 5 digits specifying
    the frame index and the format extension (.png or .jpg). If the
    `FrameExporter` was used, they are already at the good format.

    Example: 00001.png, 00002.png, ...
