FROM    piurafunk/utility-git as watchman-src
RUN     git clone --depth 1 --branch v4.9.0 https://github.com/facebook/watchman.git /watchman

FROM    ubuntu:20.04 as build

RUN     apt-get update && apt-get install -y \
            software-properties-common && \
        rm -rf /var/lib/apt/lists/*

## Setup for Watchman
RUN     apt update -y && apt install -y software-properties-common && rm -rf /var/lib/apt/lists/*
RUN     apt-get update && apt-get install -y \
            autoconf \
            automake \
            g++ \
            gdbserver \
            libssl-dev \
            libtool \
            make \
            pkg-config \
            python2.7-dev \
            python-setuptools \
            sudo && \
	    rm -rf /var/lib/apt/lists/*

## Install Watchman
COPY    --from=watchman-src /watchman /watchman
RUN     cd /watchman && ./autogen.sh && ./configure --enable-lenient && make -j `nproc` && make install && rm -r /watchman

## Setup for Buck
RUN     apt-get update && apt-get install -y \
	        wget && \
	    rm -rf /var/lib/apt/lists/*

## Install Buck
RUN     wget https://github.com/njlr/buck-warp/releases/download/v0.3.0/buck-2019.01.10.01-linux -O /bin/buck && \
        chmod +x /bin/buck

## Setup for Buckaroo
RUN     apt-get update && apt-get install -y \
            git \
            libssl1.1 && \
        rm -rf /var/lib/apt/lists/*

## Install Buckaroo
RUN	    wget https://github.com/LoopPerfect/buckaroo/releases/download/v2.2.0/buckaroo-linux -O /bin/buckaroo && \
	    chmod +x /bin/buckaroo

## Install code coverage tools
RUN     apt-get update && apt-get install -y \
                lcov && \
        rm -rf /var/lib/apt/lists/*
