.. _install-label:

Installation
============

This section describes how to install brayns Python package. Note that a running
renderer backend instance (braynsService) is required to be able to perform some
rendering, the Python package is basically just a websocket client.

To be able to interact with a running instance of braynsService, websockets can
be used directly but the easiest way is to use the Python API.

To use raw websocket and JSON-RPC to interact with the backend, the API
reference can be found under :ref:`jsonrpcapi-label` with endpoints
documentation. In this case, no installation is required and this section can be
skipped but a custom websocket client has to be written.

Otherwise the Brayns Python client is distributed as a Python package named
"brayns" and must be installed.

Currently only Python 3.9+ is supported so for platforms with older system
version, it can be installed using:

Python version
--------------

.. code-block:: console

    $ sudo apt install python3.9

As for all Python projects, it is recommended (but not required) to use a
virtual environment. It can be created with the following commands (update the
paths with the one your environment folder):

Python virtual environment (optional)
-------------------------------------

.. code-block:: console

    # Use python3.9 instead of python if system version is older
    $ python -m venv mypythonvenv

Now it can be activated with:

.. code-block:: console

    $ source mypythonvenv/bin/activate

And deactivated with:

.. code-block:: console

    $ deactivate

With the Python environment activated (optional) one of the following methods
can be used to install brayns Python package.

Package installation
--------------------

From Python Pacakge Index (PyPI)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The easiest way is to access the latest release is the following:

.. code-block:: console

    $ pip install brayns

And that's it, no other steps are required.

From source
~~~~~~~~~~~

For development or to get a more recent version than the one released on PyPI,
the package can also be installed from the github repository sources using:

.. code-block:: console

    # Clone Brayns repo
    $ git clone https://github.com/BlueBrain/Brayns.git

    # Go inside the Python package source folder
    $ cd Brayns/python

    # Build a source distribution (tar.gz)
    $ python setup.py sdist

    # Install the distribution with pip (output in dist folder)
    $ pip install dist/*

Now brayns installation is complete.

On BB5
~~~~~~

On BB5 the Python package can be installed like on a local machine with a
virtual environment but it is also available on spack without any installation.
It can be activated as follows:

.. code-block:: console

    # Allocate a node, can also be done with sbatch
    $ salloc --account=<projXXX> -p interactive -t 8:00:00 --exclusive --constraint=cpu -c 72 --mem 0

    # Load brayns module with Python
    $ module load unstable
    $ module load python
    $ module load brayns/latest

Now brayns package should be available in the Python system version.

The renderer backend (braynsService) should also be available in the current
path once brayns module is loaded. Otherwise on a local machine, it must be
compiled from source.
