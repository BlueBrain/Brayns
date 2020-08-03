Installation
============

Brayns python client is distributed as a Python package available at BBP devpi server:

.. code-block:: console

    $ pip install -i https://bbpteam.epfl.ch/repository/devpi/simple/ brayns

Only Python 3.6+ is supported.


Prior to running *pip install*, we recommend updating *pip* in your virtual environment unless you have a compelling reason not to do it:

.. code:: console

    $ pip install --upgrade pip setuptools


On BB5
~~~~~~

On BB5, you can get a Brayns python client installation using virtual environments

You can create a dedicated virtual environment using this sequence of commands :

.. code-block:: console

    $ module load unstable
    $ module load python
    $ python -m venv my_virtualenv_folder

The name ``my_virtualenv_folder`` is just a suggestion and can be changed. Once the virtual environment is
created you need to activate it using :

.. code-block:: console

    $ . my_virtualenv_folder/bin/activate

This provides you with a virtual env with the latest supported version of python on BB5. Then you
need install Brayns python client in this virtual environment :

.. code-block:: console

    $ pip install pip setuptools --upgrade
    $ pip install -i https://bbpteam.epfl.ch/repository/devpi/simple/ brayns
