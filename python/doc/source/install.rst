Installation
============

To be able to interact with a running instance of Brayns renderer backend, user can use websockets directly or use the Python client.

If you choose to use raw websocket and JSON-RPC to interact with a Brayns renderer backend, see the API section :ref:`api-label`
for the documentation of the entrypoints.

In that case, no installation is required and you can skip this section.

Otherwise the Brayns Python client is distributed as a Python package named "brayns" and must be installed.

Be careful though, only Python 3.9+ is supported so if your system version is older, you can install it using:

.. code-block:: console

    $ sudo apt install python3.9

And don't forget to replace all python commands with python3.9.

As for all Python projects, it is recommended (but not required) to use a virtual environment created like this:

.. code-block:: console

    $ python -m venv mypythonvenv
    $ source mypythonvenv/bin/activate

Note: it can be deactivated with:

.. code-block:: console

    $ deactivate

With the Python environment activated (optional) one of the following methods can be used to install brayns Python package.

From Python Pacakge Index (PyPI)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The easiest way is to access the latest release is the following:

.. code-block:: console

    $ pip install brayns

From source
~~~~~~~~~~~

It can also be installed from the github repository sources using:

.. code-block:: console

    $ git clone https://github.com/BlueBrain/Brayns.git
    $ cd Brayns/python
    $ python setup.py sdist
    $ pip install dist/brayns-2.0.0.tar.gz

On BB5
~~~~~~

On BB5, the virual environment is mandatory.

It can be done as follows:

.. code-block:: console

    $ salloc --account=proj3 -p interactive -t 8:00:00 --exclusive --constraint=cpu -c 72 --mem 0
    $ module load unstable
    $ module load python
    $ python -m venv mypythonvenv
    $ source mypythonvenv/bin/activate

This provides you with a virtual env with the latest supported version of python on BB5.

Then you need to install the brayns Python package on that environment with one of the methods above.

.. code-block:: console

    $ pip install brayns
