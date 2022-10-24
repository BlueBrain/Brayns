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

The ``Movie`` class in Brayns Python package is only a convenience tools to wrap
an FFMPEG command line. It requires a pattern to identify the frames and their
index and the movie FPS. An incoherent FPS from the export will generate an
incorrect movie duration.

The frame pattern must include the frame index using the C printf integer format
(see FFMPEG command line input documentation for more details).

.. code-block:: python

    # Pattern from previous section (export).
    pattern = 'frames/image-%d.png'

    # Movie settings.
    movie = brayns.Movie(
        frames_pattern=pattern,
        fps=25,
    )

    # Save movie, encoding is deduced from the path.
    movie.save('movie.mp4')

The duration of the output movie is simply fps x frame_count. A different
resolution can be specified but by default the frame size is used. The path to
FFMPEG executable is ``ffmpeg`` by default but can be modified.
