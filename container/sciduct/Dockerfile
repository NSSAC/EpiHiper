# Base Image
FROM ghcr.io/nssac/epihiper-build as EHB
FROM rockylinux:9

ARG VERSION
ENV VERSION=${VERSION:-0.0.0}

# Update system
RUN dnf -y update
RUN dnf -y install openmpi openmpi-devel libpq-devel libgomp jq bzip2 git

RUN echo "StrictHostKeyChecking accept-new" >> /etc/ssh/ssh_config

ENV ENV_NAME="base"
ENV MAMBA_ROOT_PREFIX="/opt/conda"
ENV MAMBA_EXE="/bin/micromamba"
ENV PATH="${MAMBA_ROOT_PREFIX}/bin:${PATH}"

RUN curl -Ls https://micro.mamba.pm/api/micromamba/linux-64/latest | tar -xvj bin/micromamba

RUN micromamba install --name $ENV_NAME python=3.11 -y -c conda-forge && \
    micromamba run --name $ENV_NAME pip install jsonschema scif

COPY config.json /
COPY sciduct.scif /
RUN --mount=type=secret,id=gh_token \
    scif install /sciduct.scif && rm -f /sciduct.scif

COPY --from=EHB /build/src/Epi* /bin/
COPY --from=EHB /build/src/lib* /lib64/
COPY bin/*.sh /bin/
copy epihiper.scif /
RUN --mount=type=secret,id=gh_token \
    scif install /epihiper.scif && rm -f /epihiper.scif



RUN rm -rf /run/secrets || true
RUN rm -rf /usr/share/dotnet /opt/ghc /usr/local/share/boost $AGENT_TOOLSDIRECTORY /var/lib/apt/lists/* /tmp/* /var/tmp/* 


ENTRYPOINT ["scif","--quiet"]

CMD ["shell"]
