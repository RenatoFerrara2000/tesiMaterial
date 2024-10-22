# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Install required packages including stress-ng
RUN apt-get update && \
    apt-get install -y build-essential stress-ng && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

COPY zephyr.exe /FERRARA_BSC/environment_builder/test2/environment_builder/dockerCont/zephyr.exe


# Define default command or entrypoint if needed (optional)
# CMD ["bash"]

