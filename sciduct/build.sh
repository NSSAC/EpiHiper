#!/usr/bin/env bash

SINGULARITY=${SINGULARITY:-"$(which singularity)"}
DEF=singularity.def
IMAGE=epihiper.simg
sudo "${SINGULARITY}" build $IMAGE $DEF

