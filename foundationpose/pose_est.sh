#!/bin/bash

DIR=$(pwd)
python ../nanosam/examples/click_segment_track_from_zed.py
docker exec -it foundationpose bash -c "cd $DIR && python realtime_pose_est.py"

