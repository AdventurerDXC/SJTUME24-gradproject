#!/usr/bin/env python

import time
import argparse

# Utility methods
from utility import quat2eulerZYX
from utility import parse_pt_states
from utility import list2str

import sys
import json
sys.path.insert(0, "/home/xcdai/Documents/flexiv_rdk/lib_py")
import flexivrdk
# fmt: on

def main():
    robot_ip = "192.168.2.100"            
    local_ip = "192.168.2.105"

    log = flexivrdk.Log()
    mode = flexivrdk.Mode

    robot = flexivrdk.Robot(robot_ip, local_ip)
    robot_states = flexivrdk.RobotStates()

    x_input = sys.argv[1]
    y_input = sys.argv[2]
    pos_x = json.loads(x_input)
    pos_y = json.loads(y_input)

    while pos_x:
        x_coord = pos_x.pop(0)
        y_coord = pos_y.pop(0)
    
        safe_origin = [0.471448, -0.369169, 0.536714, 177.687, 0.176, 157.846]
        pre_catch = [x_coord, y_coord, 0.536714, -0.896, -179.517, 47.301]
        catch = [x_coord, y_coord, 0.466700, -0.896, -179.517, 47.304]
        up = [x_coord, y_coord, 0.536714, -0.896, -179.517, 47.301]
        move_tube = [0.471448, -0.369169, 0.536714, 177.687, 0.176, 157.846]
        place_tube = [0.471448, -0.369169, 0.466266, 177.687, 0.176, 157.846]

        try:
            # Clear fault on robot server if any
            if robot.isFault():
                log.warn("Fault occurred on robot server, trying to clear ...")
                robot.clearFault()
                time.sleep(2)
                if robot.isFault():
                    log.error("Fault cannot be cleared, exiting ...")
                return
            log.info("Fault on robot server is cleared")

            # Enable the robot
            log.info("Enabling robot ...")
            robot.enable()

            while not robot.isOperational():
                time.sleep(1)

            log.info("Robot is now operational")

            # primitive execution mode
            robot.setMode(mode.NRT_PRIMITIVE_EXECUTION)
            gripper = flexivrdk.Gripper(robot)

            log.info("MoveL:safe origin")
            robot.executePrimitive(
                f"MoveL(target={list2str(safe_origin)} WORLD WORLD_ORIGIN, maxVel=0.9)"
            )
            while parse_pt_states(robot.getPrimitiveStates(), "reachedTarget") != "1":
                time.sleep(1)

            # gripper (width, speed, force)
            log.info("Opening gripper")
            gripper.move(0.012, 0.01, 30)
            time.sleep(2)

            log.info("MoveL:pre catch")
            robot.executePrimitive(
                f"MoveL(target={list2str(pre_catch)} WORLD WORLD_ORIGIN, maxVel=0.5)"
            )

            while parse_pt_states(robot.getPrimitiveStates(), "reachedTarget") != "1":
                time.sleep(1)
            robot.getRobotStates(robot_states)
            print(robot_states.tcpPose)
            print("pre",robot_states.tcpPoseDes)

            log.info("MoveL:catch")
            robot.executePrimitive(
                f"MoveL(target={list2str(catch)} WORLD WORLD_ORIGIN, maxVel=0.5)"
            )
            while parse_pt_states(robot.getPrimitiveStates(), "reachedTarget") != "1":
                time.sleep(1)

            log.info("Closing gripper")
            gripper.move(0.0082, 0.003, 50)
            time.sleep(2)

            log.info("MoveL:up")
            robot.executePrimitive(
                f"MoveL(target={list2str(up)} WORLD WORLD_ORIGIN, maxVel=0.5)"
            )
            while parse_pt_states(robot.getPrimitiveStates(), "reachedTarget") != "1":
                time.sleep(1)
            robot.getRobotStates(robot_states)
            print("up",robot_states.tcpPoseDes)

            log.info("MoveL:move tube")
            robot.executePrimitive(
                f"MoveL(target={list2str(move_tube)} WORLD WORLD_ORIGIN, maxVel=0.5)"
            )
            while parse_pt_states(robot.getPrimitiveStates(), "reachedTarget") != "1":
                time.sleep(1)
            robot.getRobotStates(robot_states)
            print("placehigh",robot_states.tcpPoseDes)

            log.info("MoveL:place tube")
            robot.executePrimitive(
                f"MoveL(target={list2str(place_tube)} WORLD WORLD_ORIGIN, maxVel=0.5)"
            )
            while parse_pt_states(robot.getPrimitiveStates(), "reachedTarget") != "1":
                time.sleep(1)

            log.info("Opening gripper")
            gripper.move(0.012, 0.1, 20)
            time.sleep(2)

            robot.stop()

        except Exception as e:
            # Print exception error message
            log.error(str(e))

if __name__ == "__main__":
    main()

