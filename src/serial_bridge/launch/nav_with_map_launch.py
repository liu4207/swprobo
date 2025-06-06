#!/usr/bin/env python3
import os
from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    IncludeLaunchDescription,
    SetEnvironmentVariable,
)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, Command, PathJoinSubstitution
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node


def generate_launch_description():
    # ──────────────── Get Package Paths ────────────────
    pkg_serial_bridge = get_package_share_directory("serial_bridge")
    pkg_nav2_bringup = get_package_share_directory("nav2_bringup")

    # ──────────────── 通用参数 ────────────────
    use_sim_time = LaunchConfiguration("use_sim_time", default="false")

    map_yaml = LaunchConfiguration(
        "map", default=os.path.join(pkg_serial_bridge, "maps", "B406_half_map.yaml")
    )
    params_file = LaunchConfiguration(
        "params_file",
        default=os.path.join(pkg_serial_bridge, "config", "nav2_params.yaml"),
    )
    urdf_file = LaunchConfiguration(
        "urdf", default=os.path.join(pkg_serial_bridge, "urdf", "sr_robot.urdf.xacro")
    )

    # ──────────────── 生成 robot_description ────────────────
    robot_desc = Command(["xacro ", urdf_file, " use_sim_time:=", use_sim_time])

    # ──────────────── 引入官方 bringup ────────────────
    bringup_launch = PathJoinSubstitution(
        [pkg_nav2_bringup, "launch", "bringup_launch.py"]
    )

    bringup = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(bringup_launch),
        launch_arguments={
            "map": map_yaml,
            "use_sim_time": use_sim_time,
            "params_file": params_file,
            "autostart": "true",  # 禁掉其内部的 robot_state_publisher
        }.items(),
    )

    # ──────────────── ① robot_state_publisher  ────────────────
    rsp = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        parameters=[{"robot_description": robot_desc, "use_sim_time": use_sim_time}],
        output="screen",
    )

    # ──────────────── ② 静态 TF  ────────────────
    static_tf = [
        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            name="tf_basefootprint",
            arguments=["0", "0", "0", "0", "0", "0", "base_link", "base_footprint"],
            output="screen",
        ),
        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            name="tf_baselaser",
            arguments=["0", "0", "0.18", "0", "0", "0", "base_link", "base_laser"],
            output="screen",
        ),
        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            name="tf_imu",
            arguments=["0", "0", "0.10", "0", "0", "0", "base_link", "imu_link"],
            output="screen",
        ),
    ]

    # ──────────────── ④ RViz2 ────────────────
    rviz_config_file = os.path.join(pkg_serial_bridge, "config", "navigation_view.rviz")

    rviz = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        arguments=["-d", rviz_config_file],
        parameters=[{"use_sim_time": use_sim_time}],
        output="screen",
    )

    # ──────────────── ⑤ Trajectory Recorder ────────────────
    trajectory_recorder = Node(
        package="serial_bridge",
        executable="trajectory_recorder",
        name="trajectory_recorder",
        output="screen",
    )

    return LaunchDescription(
        [
            DeclareLaunchArgument(
                "use_sim_time",
                default_value="false",
                description="Use simulation clock if true",
            ),
            DeclareLaunchArgument(
                "map",
                default_value=os.path.join(
                    pkg_serial_bridge, "maps", "B406_half_map.yaml"
                ),
                description="Full path to map file to load",
            ),
            DeclareLaunchArgument(
                "params_file",
                default_value=os.path.join(
                    pkg_serial_bridge, "config", "nav2_params.yaml"
                ),
                description="Full path to the nav2 parameters file",
            ),
            DeclareLaunchArgument(
                "urdf",
                default_value=os.path.join(
                    pkg_serial_bridge, "urdf", "sr_robot.urdf.xacro"
                ),
                description="URDF or XACRO describing the robot model",
            ),
            SetEnvironmentVariable("RMW_IMPLEMENTATION", "rmw_fastrtps_cpp"),
            rsp,
            *static_tf,
            bringup,
            rviz,
            trajectory_recorder,
        ]
    )
