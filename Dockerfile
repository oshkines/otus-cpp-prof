FROM ubuntu:22.04

# Устанавливаем зависимости
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    doxygen \
    graphviz \
    devscripts \
    debhelper \
    libboost-all-dev \
    dos2unix \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

# Генерируем документацию
RUN doxygen Doxyfile

# Подготавливаем debian-файлы
RUN dos2unix debian/rules && \
    chmod +x debian/rules && \
    mkdir -p /output

# Сборка DEB-пакета
RUN dpkg-buildpackage -b -us -uc && cp ../*.deb /output/