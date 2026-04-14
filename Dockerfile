FROM ubuntu:24.04

# Ставим всё одним махом
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    doxygen \
    devscripts \
    debhelper \
    dos2unix \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

# Исправляем файлы ДО генерации и сборки
RUN dos2unix debian/rules && chmod +x debian/rules

# 1. Генерируем документацию
RUN doxygen Doxyfile

# 2. Создаем папку и запускаем сборку
RUN mkdir -p /output && \
    dpkg-buildpackage -b -us -uc && \
    cp ../editor*.deb /output/
