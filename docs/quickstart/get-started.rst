Getting Started
===============


Installation
---------------

EpiHiper code is available from `GitHub <https://github.com/NSSAC/EpiHiper/>`_

Obtain Code
^^^^^^^^^^^^^^^

.. code-block:: bash

  git clone https://github.com/NSSAC/EpiHiper.git

Dependencies
^^^^^^^^^^^^^^^
Requirements for building EpiHiper:
  - C++ 11 compatible compiler, e.g. gcc 7.1.0
  - Cmake version 3.11 or above
  - PostgreSQL library (provided by package: libpq5) 
  - PostgreSQL header  (provided by package: postgresql-devel)

All other dependencies are provided through git submodules, which are automatically retrieved during configuration and compiled during make. These dependencies are:
  - `jansson <https://github.com/akheron/jansson.git>`_
  - `libpqxx <https://github.com/jtv/libpqxx.git>`_
  - `spdlog <https://github.com/gabime/spdlog.git>`_

Building a single threaded executable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

From the root directory of the repository execute:

.. code-block:: bash

  mkdir build
  cd build
  cmake -G "Unix Makefiles" -DENABLE_MPI=OFF -DENABLE_OMP=OFF -DENABLE_LOCATION_ID=OFF ..
  make -j

Building an MPI (multi-process) executable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Additional dependencies:
  - OpenMPI version 3.14 and above (provided by package: openmpi3)
  - OpenMPI header files (provided by package: openmpi3-devel)

From the root directory of the repository execute:

.. code-block:: bash

  mkdir build
  cd build
  cmake -G "Unix Makefiles" -DENABLE_MPI=ON -DENABLE_OMP=OFF -DENABLE_LOCATION_ID=OFF ..
  make -j

Building an OpenMP (multi-threaded) executable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Additional dependencies:
  - OpenMP run time library (provided by package: libgomp1)

From the root directory of the repository execute:

.. code-block:: bash

  mkdir build
  cd build
  cmake -G "Unix Makefiles" -DENABLE_MPI=OFF -DENABLE_OMP=ON -DENABLE_LOCATION_ID=OFF ..
  make -l

*Note*: It is possible to combine OpenMP and MPI

Configuration options:
^^^^^^^^^^^^^^^^^^^^^^

  - `-DCMAKE_BUILD_TYPE=[Debug|Release]` Enable debug build. (default: Release)
  - `-DENABLE_LOCATION_ID=[ON|OFF]` Enable location Id support for networks. (default: OFF)
  - `-DENABLE_MPI==[ON|OFF]` Enable MPI parallelization. (default: ON)
  - `-DENABLE_OMP==[ON|OFF]` Enable OpenMP parallelization. (default: OFF)
