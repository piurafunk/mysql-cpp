FROM    piurafunk/utility-git as watchman-src
RUN     git clone --depth 1 --branch v4.9.0 https://github.com/facebook/watchman.git /watchman

FROM    ubuntu:20.04 as watchman-build
ENV     DEBIAN_FRONTEND=noninteractive
COPY    --from=watchman-src /watchman /watchman
RUN     apt-get update && apt-get install -y \
                autoconf \
                automake \
                g++ \
                libssl-dev \
                libtool \
                make \
                openjdk-8-jre-headless \
                pkg-config \
                python2.7-dev \
                python-setuptools
RUN     cd /watchman && ./autogen.sh && ./configure --enable-lenient && make -j `nproc` && make install

FROM    piurafunk/utility-git as buck-src
RUN     git clone --depth 1 https://github.com/facebook/buck.git -b v2019.01.10.01 /buck

FROM    ubuntu:20.04 as buck-build
ENV     DEBIAN_FRONTEND=noninteractive
RUN     apt-get update && apt-get install -y \
                ant \
                git \
                openjdk-8-jdk-headless \
                python2.7 && \
        apt-get purge -y openjdk-11-jre-headless && \
        ln -s /usr/bin/python2.7 /usr/bin/python
COPY    --from=buck-src /buck /buck
RUN     cd /buck && ant && ./bin/buck build --show-output buck && cp buck-out/gen/programs/buck.pex /bin/buck

FROM    piurafunk/utility-git as buckaroo-src
RUN     git clone https://github.com/LoopPerfect/buckaroo.git /buckaroo && cd /buckaroo && git checkout f95792c47b3f4e6b4ed9ca121071422c88498908

FROM    mcr.microsoft.com/dotnet/sdk:5.0 as buckaroo-build
ENV     DEBIAN_FRONTEND=noninteractive
COPY    --from=buckaroo-src /buckaroo /buckaroo
RUN     cd /buckaroo && dotnet build ./buckaroo && dotnet build ./buckaroo-cli && ./warp-bundle-linux.sh


FROM    ubuntu:20.04 as build

ENV     DEBIAN_FRONTEND=noninteractive
RUN     apt-get update && apt-get install -y \
## Setup for general use
                gdbserver \
                jq \
## Setup for Watchman
                python2.7 \
## Setup for Buckaroo
                git \
## Setup for Buck
                openjdk-8-jre-headless \
## Install for code coverage tools
                lcov \
## Project specific things. TODO: Move this into separate Docker image and extend from it.
                g++ \
                make && \
	rm -rf /var/lib/apt/lists/*

## Install Watchman
COPY    --from=watchman-build /usr/local/bin/watchman* /usr/local/bin
COPY    --from=watchman-build /usr/local/lib/python2.7/site-packages /usr/local/lib/python2.7
COPY    --from=watchman-build /usr/local/share/doc/watchman-4.9.0 /usr/local/share/doc
COPY    --from=watchman-build /usr/local/var/run/watchman /usr/local/var/run/watchman

## Install Buck
COPY    --from=buck-build /bin/buck /bin/buck
RUN     ln -s /usr/bin/python2.7 /usr/bin/python

## Install Buckaroo
COPY    --from=buckaroo-build /buckaroo/warp/buckaroo-linux /bin/buckaroo

ARG     USER_ID=1000 GROUP_ID=1000 USERNAME=user
RUN     groupadd --gid $GROUP_ID $USERNAME
RUN     useradd --create-home --home-dir /home/$USERNAME --uid $USER_ID --gid $GROUP_ID $USERNAME
RUN     chown -R $USER_ID:$GROUP_ID /usr/local/var/run/watchman
USER    $USER_ID:$GROUP_ID
