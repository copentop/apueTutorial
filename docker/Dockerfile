FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install -yq --no-install-recommends wget curl zip unzip vim ca-certificates && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*


# Install dependencies
RUN apt-get update && \
apt-get install -y git build-essential 