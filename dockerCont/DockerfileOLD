# Use an official Ubuntu base image
FROM ubuntu:latest

# Install necessary dependencies (Wine for running .exe files and stress-ng for stress testing)
RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y wine64 wine32 stress-ng && \
    apt-get clean

# Copy the zephyr.exe file from the host machine to the container
COPY zephyr.exe /FERRARA_BSC/environment_builder/test2/environment_builder/dockerCont/zephyrImageBundle/tesiMaterial/zephyr.exe
COPY training.ipynb /FERRARA_BSC/environment_builder/test2/environment_builder/dockerCont/zephyrImageBundle/tesiMaterial/training.ipynb

# Make sure the zephyr.exe file has the proper permissions
RUN chmod +x /FERRARA_BSC/environment_builder/test2/environment_builder/dockerCont/zephyrImageBundle/tesiMaterial/zephyr.exe

#language model req
RUN apt-get update && apt-get install -y python3 python3-pip 

# Set the default command to run the executable with Wine
CMD ["bin/bash"]

