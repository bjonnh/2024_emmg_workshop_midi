FROM ubuntu:latest AS downloader
RUN apt-get update && apt-get install -y python3 wget bzip2 curl && rm -rf /var/lib/apt/lists/*

RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/usr/local/bin sh

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8
WORKDIR /root
ENV USER=root
ENV ARDUINO_DATA_DIR=/opt/arduino
COPY cli-config.yml /opt/arduino/arduino-cli.yaml
RUN arduino-cli core update-index --verbose
RUN arduino-cli core install rp2040:rp2040
