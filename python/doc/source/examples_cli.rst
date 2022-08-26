Command line interface
======================

Brayns Python package provides a command line interface (CLI) for the basic
usecases. It is built on top of the built-in module ``argparse`` and uses
brayns objects to perform the actual task.

All the CLI classes have the same name as the regular class with the suffix
``Cli`` (ex. ``Movie`` -> ``MovieCli``.

They are basically dataclasses with command line arguments as attributes (which
are also used as default values) and have a ``parse`` method to update these
attributes from a list of string.

.. attention::

    To parse ``sys.argv`` use ``sys.argv[1:]`` as the first argument will be the
    name of the Python script and is obviously not a valid parameter.

The main CLI tasks also have a ``run`` method to call once parsed to perform the
underlying task.

Example
-------

.. code-block:: python

    import sys

    # Default values can be overidden during construction or after.
    cli = brayns.MovieCli(...)

    # Fill movie CLI attributes using sys.argv.
    cli.parse(sys.argv[1:])

    # Run the CLI (here make a movie).
    cli.run()
