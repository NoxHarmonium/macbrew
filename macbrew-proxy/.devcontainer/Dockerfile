# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.148.1/containers/rust/.devcontainer/base.Dockerfile

FROM mcr.microsoft.com/vscode/devcontainers/rust:0-1


RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    # Required native headers/libraries
    && apt-get -y install --no-install-recommends libudev-dev socat \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/* /tmp/library-scripts