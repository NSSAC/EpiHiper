#!/usr/bin/env bash

# BEGIN: Copyright 
# MIT License 
#  
# Copyright (C) 2020 - 2024 Rector and Visitors of the University of Virginia 
#  
# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the "Software"), to deal 
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
# copies of the Software, and to permit persons to whom the Software is 
# furnished to do so, subject to the following conditions: 
#  
# The above copyright notice and this permission notice shall be included in all 
# copies or substantial portions of the Software. 
#  
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
# SOFTWARE 
# END: Copyright 

SINGULARITY=${SINGULARITY:-"$(which singularity)"}
JSONSCHEMA=${JSONSCHEMA:-"$(which jsonschema)"}
JOBSERVICE_SCHEMA_DIR=${JOBSERVICE_SCHEMA_DIR:-$(dirname $0)}

DEF=rivanna.def
IMAGE=epihiper.rivanna
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

[ -e generated_config.json ] && rm generated_config.json
BUILD_DATE=`node -e "console.log(new Date().toISOString())"`
cat config.json | jq --arg build_date $BUILD_DATE '. + {build_date: $build_date}' > generated_config.json
VERSION=`cat config.json | jq -r .version`

if [ -n "$JOBSERVICE_SCHEMA_DIR" ]; then
        if [ -n "$JSONSCHEMA" ]; then
                echo $JSONSCHEMA -i generated_config.json $JOBSERVICE_SCHEMA_DIR/image_configuration.json
                $JSONSCHEMA -i generated_config.json $JOBSERVICE_SCHEMA_DIR/image_configuration.json
                if [ $? -eq 0 ]; then
                        echo "Generated Configuration appears to be valid"
                else
                        echo "Generated Configuration failed validation"
                        exit 1
                fi
        else
                echo "Please install https://github.com/Julian/jsonschema to validate schemas during build"
                exit 1
        fi
else
        echo "Please install https://github.com/Julian/jsonschema to validate schemas during build and set the JOBSERVICE_SCHEMA_DIR to the location of image_configuratin.json schema to define the location of the schema"
fi

# Local cache.rivanna
[ -e cache.rivanna ] || mkdir -p cache.rivanna
    
cd cache.rivanna

# EpiHiper
if [ ! -e EpiHiper ]; then
    if [ -d /project/bii_nssac/EpiHiper/build/src ]; then
        cp /project/bii_nssac/EpiHiper/build/src/Epi* .
        cp /project/bii_nssac/EpiHiper/build/src/libEpiHiper* . 
    else
        scp rivanna:/project/bii_nssac/EpiHiper/build/src/Epi* .
        scp rivanna:/project/bii_nssac/EpiHiper/build/src/libEpiHiper* . 
    fi
fi

# Assure that the files are readable and executable by all
chmod 775 EpiHiper*
chmod 775 libEpiHiper*

# Intel Runtime Libraries
if [ ! -e l_comp_lib_2018.5.274_comp.cpp_redist.tgz ]; then
    if [ -d /project/bii_nssac/EpiHiper/intel/src ]; then
        cp /project/bii_nssac/EpiHiper/intel/src/l_comp_lib_2018.5.274_comp.cpp_redist.tgz .
    else
        scp rivanna:/project/bii_nssac/EpiHiper/intel/src/l_comp_lib_2018.5.274_comp.cpp_redist.tgz .
    fi
fi

# Intel MPI
if [ ! -e l_mpi_2018.5.288.tgz ]; then
    if [ -d /project/bii_nssac/EpiHiper/intel/src ]; then
        cp /project/bii_nssac/EpiHiper/intel/src/l_mpi_2018.5.288.tgz .
    else
        scp rivanna:/project/bii_nssac/EpiHiper/intel/src/l_mpi_2018.5.288.tgz .
    fi
fi

# IntelOPA
if [ ! -e IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz ]; then
    if [ -d /project/bii_nssac/EpiHiper/intel/src ]; then
        cp /project/bii_nssac/EpiHiper/intel/src/IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz .
    else
        scp rivanna:/project/bii_nssac/EpiHiper/intel/src/IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz .
    fi
fi

cd ..

[ -e "build.def" ] && rm build.def
cp $DEF build.def

sudo "${SINGULARITY}" build output_image.sif build.def && mv output_image.sif $IMAGE-$VERSION.sif

[ -e cache.rivanna ] && chown -R $ID cache.rivanna
[ -e $IMAGE ] && chown -R $ID $IMAGE
