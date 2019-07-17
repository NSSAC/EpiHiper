#!/usr/bin/env bash

SINGULARITY=${SINGULARITY:-"$(which singularity)"}
DEF=singularity.def
IMAGE=epihiper.simg
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

sudo "${SINGULARITY}" build $IMAGE $DEF

[ -e cache ] && sudo chown -R $ID cache
[ -e $IMAGE ] && sudo chown -R $ID $IMAGE
