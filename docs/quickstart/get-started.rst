Getting Started
===============


Installation
------------

EpiHiper code is available from `GitHub <https://github.com/NSSAC/EpiHiper/>`_

Obtain Code
^^^^^^^^^^^

.. code-block:: bash

  git clone https://github.com/NSSAC/EpiHiper.git

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

Dependencies
^^^^^^^^^^^^

Installation of dependencies may require platform and system dependent tools such as zypper, yum or aptitude on Linux. Since specifics of such tools and package version may change with versions, we refer the user to the documentation accompanying these tools. All other dependencies are provided through git submodules, which are automatically retrieved during configuration and compiled during make.

..
  `Microsoft Visual C++ <https://visualstudio.microsoft.com/vs/features/cplusplus/>`_, `XCode <https://developer.apple.com/xcode/>`_,

Requirements for building EpiHiper:
  - C++ 11 compatible compiler (`Clang <https://clang.llvm.org/>`_, or `gcc <https://gcc.gnu.org/>`_)
  - `Cmake <https://cmake.org/download/>`_ version 3.11 or above
  - `PostgreSQL <https://www.postgresql.org/download/>`_ library and header (provided by packages: libpq5, postgresql-devel)

**Ubuntu** (22.04) or **Debian** (bullseye)

.. code-block:: bash

  # required
  sudo apt-get update
  sudo apt-get install build-essential git
  sudo apt-get install cmake libpq5 libpq-dev
  # OpenMPI
  sudo apt-get install libopenmpi-dev
  # OpenMP
  sudo apt-get install libgomp1

**openSUSE** (Leap latest)

.. code-block:: bash

  # required
  sudo zypper refresh
  sudo zypper install patterns-devel-base-devel_basis
  sudo zypper install patterns-devel-C-C++-devel_C_C++
  sudo zypper install gcc-c++ git
  sudo zypper install cmake libpq5 postgresql-devel
  # OpenMPI (optional)
  sudo zypper install openmpi4-devel
  # OpenMP (optional)
  sudo zypper install libgomp1

**Arch Linux**  

.. code-block:: bash

  # required
  sudo pacman -S git cmake postgresql-libs
  # OpenMPI (optional)
  sudo pacman -S openmpi
  # OpenMP (optional)
  # no additional requirements

**Rocky Linux** (9)

.. code-block:: bash

  # required
  sudo dnf groupinstall 'Development Tools'
  sudo dnf -y install cmake libpq-devel
  # OpenMPI (optional)
  sudo dnf install openmpi openmpi-devel
  # Add MPI executables to your path, e.g.
  echo export PATH=\"/usr/lib64/openmpi/bin/:\$PATH\" >> ~/.profile
  # OpenMP (optional)
  sudo dnf install libgomp

**MacOS X** 

.. code-block:: bash

  # required
  brew install cmake git libpq
  # OpenMPI (optional)
  brew install open-mpi
  # OpenMP (optional)
  brew install libomp

All other dependencies are provided through git submodules, which are automatically retrieved during configuration and compiled during make. These dependencies are:
  - `jansson <https://github.com/akheron/jansson.git>`_
  - `libpqxx <https://github.com/jtv/libpqxx.git>`_
  - `spdlog <https://github.com/gabime/spdlog.git>`_

Configuration options:
^^^^^^^^^^^^^^^^^^^^^^

.. list-table:: Configuration options. 
  :name: get-started-options
  :header-rows: 1

  * - | Option
    - | Values
    - | Default
    - | Description
  * - | -DCMAKE_BUILD_TYPE
    - | Debug 
      | Release
    - | Release  
    - | Enable debug build.
  * - | -DENABLE_LOCATION_ID
    - | ON 
      | OFF
    - | ON 
    - | Enable location Id support for networks.
  * - | -DENABLE_MPI
    - | ON 
      | OFF
    - | ON 
    - | Enable MPI parallelization.
  * - | -DENABLE_OMP
    - | ON 
      | OFF
    - | ON 
    - | Enable OpenMP parallelization.
  * - | -DENABLE_LOGLEVEL_TRACE
    - | ON 
      | OFF
    - | OFF 
    - | Enable support for log level trace.
