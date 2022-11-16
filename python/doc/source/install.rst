.. _install-label:

Installation
============

This section describes how to install brayns Python package. Note that a running
renderer backend instance (braynsService) is required to be able to perform some
rendering, the Python package is basically just a websocket client to monitor it.

A Brayns service instance can be monitored using websocket JSON-RPC messages
either using the Python wrapper (API) or any other websocket client.

To use raw websocket and JSON-RPC to interact with the backend, the API
reference can be found under :ref:`jsonrpcapi-label` with a documentation
of all existing entrypoints. In this case, no installation is required and
this section can be skipped but a custom websocket client has to be written.

Otherwise the Brayns Python client is distributed as a Python package named
``brayns`` and must be installed.

Python version
--------------

``brayns`` package only supports Python 3.9+ so for platforms with an older
system version, it can be installed on Ubuntu using:

.. code-block:: console

    $ sudo apt install python3.9

Python virtual environment (optional)
-------------------------------------

As for all Python projects, it is recommended (but not required) to use a
virtual environment. It can be created with the following commands (update the
paths with the one your environment folder):

.. code-block:: console

    # Use python3.9 instead of python if system version is older.
    $ python -m venv mypythonvenv

Now the environment can be activated with:

.. code-block:: console

    $ source mypythonvenv/bin/activate

And deactivated with:

.. code-block:: console

    $ deactivate

With the Python environment activated (optional) one of the following methods
can be used to install brayns Python package.

Python package installation
---------------------------

From Python Pacakge Index (PyPI)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The easiest way to access the latest release is the following:

.. code-block:: console

    $ pip install brayns

From source (github repository)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For development or to get a more recent version than the one released on PyPI,
the package can also be installed from the sources on the github repository using:

.. code-block:: console

    # Clone Brayns repo.
    $ git clone https://github.com/BlueBrain/Brayns.git

    # Go inside the Python package source folder.
    $ cd Brayns/python

    # Build a source distribution (tar.gz).
    $ python setup.py sdist

    # Install the distribution with pip (output in dist folder).
    $ pip install dist/*

Now brayns installation is complete.

On BB5
~~~~~~

First allocate a BB5 node:

.. code-block:: console

    # Allocate a node, can also be done with sbatch.
    $ salloc --account=<projXXX> -p interactive -t 8:00:00 --exclusive --constraint=cpu -c 72 --mem 0

Both the renderer backend (braynsService) and the Python package are available
through spack modules ``brayns`` and ``py-brayns``.

.. code-block:: console

    # Load latest spack version.
    $ module load unstable

    # Make braynsService available in the system PATH.
    $ module load brayns

    # Check braynsService is available.
    $ braynsService -h

    # Make the python package available in the system Python.
    $ module load py-brayns

The Python package can aslo be installed like on a local machine with a
virtual environment (mandatory this time) but the python spack module is
required.

.. code-block:: console

    # Load latest available python module.
    $ module load unstable
    $ module load python

    # Setup Python venv (like before).
    $ python -m venv mypythonvenv
    $ source mypythonvenv/bin/activate
    $ pip install brayns

Now brayns package should be available with the Python venv activated.

The renderer backend (braynsService) should also be available in the current
path once brayns module is loaded. Otherwise on a local machine, it must be
compiled from source by cloning the [repository](https://github.com/BlueBrain/Brayns).
