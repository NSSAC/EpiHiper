# Base Image
FROM rockylinux:9

# Update system
RUN dnf -y update

# required
RUN dnf -y groupinstall 'Development Tools'
RUN dnf -y install cmake libpq-devel


# OpenMPI (optional)
RUN dnf -y install openmpi openmpi-devel

# OpenMP (optional)
RUN dnf -y install libgomp

# Build EpiHiper
RUN --mount=type=secret,id=gh_token cd / && \
    git clone https://$(cat /run/secrets/gh_token)@github.com/NSSAC/EpiHiper.git && \
    mkdir build && \
    cd build && \
    export PATH="/usr/lib64/openmpi/bin/:$PATH" && \
    cmake -G "Unix Makefiles" -DENABLE_MPI=ON -DENABLE_OMP=ON -DENABLE_LOCATION_ID=ON ../EpiHiper && \
    make -j 4 && \
    make test

