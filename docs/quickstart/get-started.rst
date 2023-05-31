Getting Started
===============


Installation
------------

EpiHiper code is available from `GitHub <https://github.com/NSSAC/EpiHiper/>`_

Obtain Code
^^^^^^^^^^^

.. code-block:: bash

  git clone https://github.com/NSSAC/EpiHiper.git

Dependencies
^^^^^^^^^^^^

Installation of dependencies may require platform and system dependent tools such as for example zypper, yum or aptitude on Linux. Since specifics of such tools and package version may change with versions, we refer the user to the documentation accompanying these tools. All other dependencies are provided through git submodules, which are automatically retrieved during configuration and compiled during make.

..
  `Microsoft Visual C++ <https://visualstudio.microsoft.com/vs/features/cplusplus/>`_, `XCode <https://developer.apple.com/xcode/>`_,

Requirements for building EpiHiper:
  - C++ 11 compatible compiler (`Clang <https://clang.llvm.org/>`_, or `gcc <https://gcc.gnu.org/>`_)
  - `Cmake <https://cmake.org/download/>`_ version 3.11 or above
  - `PostgreSQL <https://www.postgresql.org/download/>`_ library and header (provided by packages: libpq5, postgresql-devel)

**Centos** (8)

.. code-block:: bash

  # required
  sudo yum install cmake postgresql-libs postgresql-devel
  # OpenMPI
  sudo yum install openmpi openmpi-devel 
  # OpenMP
  sudo yum install libgomp1

**Debian** (bullseye)

.. code-block:: bash

  # required
  sudo apt-get install cmake libpq5 libpq-dev
  # OpenMPI
  sudo apt-get install libopenmpi libopenmpi-dev 
  # OpenMP
  sudo apt-get install libgomp1

**openSUSE** (Leap 15.3)

.. code-block:: bash

  # required
  sudo zypper install cmake libpq5 postgresql-devel
  # OpenMPI (optional)
  sudo zypper install openmpi3 openmpi3-devel 
  # OpenMP (optional)
  sudo zypper install libgomp1

**MacOS X** 

.. code-block:: bash
  # required
  brew install cmake libpq
  # OpenMPI (optional)
  brew install open-mpi
  # OpenMP (optional)
  brew install libomp

  mkdir build
  cd build
  cmake -G "Unix Makefiles" -D-DPostgreSQL_ROOT=/usr/local/opt/libpq -DENABLE_MPI=OFF -DENABLE_OMP=OFF -DENABLE_LOCATION_ID=OFF ..
  make -j4




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
  make -j4

Building an MPI (multi-process) executable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Additional dependencies:
  - `OpenMPI version 3.14 <https://www.open-mpi.org/software/ompi/v3.1/>`_ and above (provided by packages: openmpi3, openmpi3-devel)

From the root directory of the repository execute:

.. code-block:: bash

  mkdir build
  cd build
  cmake -G "Unix Makefiles" -DENABLE_MPI=ON -DENABLE_OMP=OFF -DENABLE_LOCATION_ID=OFF ..
  make -j4

Building an OpenMP (multi-threaded) executable
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Additional dependencies:
  - OpenMP is provided by the compiler and optionally additional run time libraries (provided by package: libgomp1)
  - Microsoft Visual C++ support of OpenMP is outdated please see `Getting started with OpenMP on Windows <https://stackoverflow.com/questions/11079586/getting-started-with-openmp-install-on-windows>`_ for help.

From the root directory of the repository execute:

.. code-block:: bash

  mkdir build
  cd build
  cmake -G "Unix Makefiles" -DENABLE_MPI=OFF -DENABLE_OMP=ON -DENABLE_LOCATION_ID=OFF ..
  make -j4

*Note*: It is possible to combine OpenMP and MPI

Configuration options:
^^^^^^^^^^^^^^^^^^^^^^

  - `-DCMAKE_BUILD_TYPE=[Debug|Release]` Enable debug build. (default: Release)
  - `-DENABLE_LOCATION_ID=[ON|OFF]` Enable location Id support for networks. (default: OFF)
  - `-DENABLE_MPI==[ON|OFF]` Enable MPI parallelization. (default: ON)
  - `-DENABLE_OMP==[ON|OFF]` Enable OpenMP parallelization. (default: OFF)
