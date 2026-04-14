FROM ubuntu:22.04

# Ставим зависимости: компилятор, cmake, doxygen и инструменты сборки пакетов
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    doxygen \
	graphviz \
    devscripts \
    debhelper \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

# 1. Генерируем документацию
RUN doxygen Doxyfile

# 2. Собираем проект и упаковываем в .deb
# Команда ниже запустит процесс создания пакета из папки debian
#RUN dpkg-buildpackage -b -us -uc
#RUN mkdir -p /output
#RUN dpkg-buildpackage -b -us -uc && cp /editor*.deb /output/
#RUN dpkg-buildpackage -b -us -uc && cp ../editor*.deb /output/
# Пакет окажется на уровень выше, в / (корне) или в /build_parent

# Устанавливаем дополнительные утилиты для исправления строк
RUN apt-get update && apt-get install -y dos2unix

WORKDIR /build
COPY . .

# Исправляем концы строк, права доступа и создаем папку для вывода
RUN dos2unix debian/rules && \
    chmod +x debian/rules && \
    mkdir -p /output

# Запускаем сборку
RUN dpkg-buildpackage -b -us -uc && cp ../editor*.deb /output/

