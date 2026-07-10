FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    doxygen \
    graphviz \
    devscripts \
    debhelper \
    dos2unix \
    libboost-system-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

RUN doxygen Doxyfile || echo "Doxygen warnings ignored"

RUN find debian -type f -exec dos2unix {} \; || true

RUN chmod +x debian/rules

RUN dpkg-buildpackage -b -us -uc

RUN mkdir -p /output && cp /*.deb /output/

VOLUME /output