docker rm -f foundationpose
DIR=$(pwd)/../
xhost +  && docker run --gpus all --env NVIDIA_DISABLE_REQUIRE=1 -it --network=host --name foundationpose --privileged --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -v $DIR:$DIR -v /home:/home -v /mnt:/mnt -v /tmp/.X11-unix:/tmp/.X11-unix -v /tmp:/tmp -v /usr/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu --device=/dev/video2 --ipc=host -e DISPLAY=${DISPLAY} -e GIT_INDEX_FILE foundationpose:latest bash -c "apt update && apt install -y zstd && cd $DIR/FoundationPose && $DIR/ZED_SDK_Ubuntu20_cuda11.8_v4.1.2.zstd.run -- silent && bash"
