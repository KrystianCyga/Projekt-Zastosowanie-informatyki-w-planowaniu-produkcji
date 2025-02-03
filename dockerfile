# Użycie konkretnej wersji Ubuntu jako obrazu bazowego
FROM ubuntu:20.04

# Ustawienie zmiennych środowiskowych dla nieinteraktywnej instalacji
ENV DEBIAN_FRONTEND=noninteractive

# Aktualizacja systemu i instalacja pakietów
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        cmake \
        git \
        libboost-all-dev \
        build-essential \
        libjsoncpp-dev \
        libtcmalloc-minimal4 \
        libasio-dev && \  
    # Utworzenie symbolicznego linku dla libtcmalloc_minimal.so
    ln -s /usr/lib/x86_64-linux-gnu/libtcmalloc_minimal.so.4 /usr/lib/libtcmalloc_minimal.so && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Kopiowanie lokalnego folderu Crow do obrazu
COPY Crow-1.2.0 /Crow

# Budowanie Crow z lokalnych plików
RUN set -x && \
    mkdir -p /Crow/build && \
    cd /Crow/build && \
    cmake .. && \
    make && \
    make install && \
    cd / && \
    rm -rf /Crow

# Instalacja nlohmann::json (jeśli jest potrzebna)
RUN apt-get update && \
    apt-get install -y nlohmann-json3-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Kopiowanie kodu źródłowego
COPY . /app
WORKDIR /app

# Kompilacja programu
RUN g++ -std=c++17 -O3 -o scheduler program.cpp -lpthread -I/usr/local/include



# Eksponowanie portu
EXPOSE 8080

# Komenda do uruchomienia serwera
CMD ["./scheduler"]
