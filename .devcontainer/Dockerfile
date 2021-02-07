# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.148.1/containers/rust/.devcontainer/base.Dockerfile

FROM mcr.microsoft.com/vscode/devcontainers/rust:1

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    # Required native headers/libraries
    && apt-get -y install --no-install-recommends socat tio libatk1.0-0 libgtk2.0-0 libtinyxml2-6a libsdl1.2debian \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/* /tmp/library-scripts

# TODO: I'm not sure why I can't just install this with apt-get
# https://packages.debian.org/buster/basilisk2
RUN curl -o basilisk2_0.9.20180101-1+b1_amd64.deb http://http.us.debian.org/debian/pool/contrib/b/basilisk2/basilisk2_0.9.20180101-1+b1_amd64.deb \
    && dpkg -i basilisk2_0.9.20180101-1+b1_amd64.deb \
    && rm basilisk2_0.9.20180101-1+b1_amd64.deb

RUN cargo install cargo-insta