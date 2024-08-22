#include "function.h"

// defines reference coordinate system by the position of the calibration plate in an image
void define_reference_coord_system (HTuple hv_ImageName, HTuple hv_CamParam, HTuple hv_CalplateFile, HTuple *hv_PoseCamRef)
{
  // Local iconic variables
  HObject  ho_RefImage, ho_Caltab;

  // Local control variables
  HTuple  hv_X, hv_Y, hv_Z, hv_SizeGauss, hv_MarkThresh;
  HTuple  hv_MinDiamMarks, hv_StartThresh, hv_DeltaThresh;
  HTuple  hv_MinThresh, hv_Alpha, hv_MinContLength, hv_MaxDiamMarks;
  HTuple  hv_RCoord, hv_CCoord;

  ReadImage(&ho_RefImage, hv_ImageName);
  if (HDevWindowStack::IsOpen())
    DispObj(ho_RefImage, HDevWindowStack::GetActive());
  CaltabPoints(hv_CalplateFile, &hv_X, &hv_Y, &hv_Z);
  //parameter settings for find_caltab and find_marks_and_pose
  hv_SizeGauss = 3;
  hv_MarkThresh = 50;
  hv_MinDiamMarks = 5;
  hv_StartThresh = 64;
  hv_DeltaThresh = 5;
  hv_MinThresh = 18;
  hv_Alpha = 0.5;
  hv_MinContLength = 15;
  hv_MaxDiamMarks = 200;
  FindCaltab(ho_RefImage, &ho_Caltab, hv_CalplateFile, hv_SizeGauss, hv_MarkThresh,
      hv_MinDiamMarks);
  FindMarksAndPose(ho_RefImage, ho_Caltab, hv_CalplateFile, hv_CamParam, hv_StartThresh,
      hv_DeltaThresh, hv_MinThresh, hv_Alpha, hv_MinContLength, hv_MaxDiamMarks,
      &hv_RCoord, &hv_CCoord, &(*hv_PoseCamRef));
  return;
}

void calc_grasp_pose(HTuple hv_Row, HTuple hv_Column, HTuple &hv_PoseRobotGrasp_ZYX) {
    // Local control variables
    HTuple hv_CamParam, hv_BaseInCamPose, hv_cam_H_base, hv_CalplateInToolPose;
    HTuple hv_tool_H_calplate, hv_GripperInToolPose, hv_tool_H_gripper;
    HTuple hv_CalplateFile, hv_PoseRef, hv_cam_H_ref, hv_CenterPointX_ref;
    HTuple hv_CenterPointY_ref, hv_HomMat3DIdentity, hv_GraspPhiZ_ref;
    HTuple hv_HomMat3D_RZ_Phi, hv_ref_H_grasp, hv_cam_H_grasp;
    HTuple hv_GripperInCamPose, hv_base_H_cam, hv_base_H_grasp;
    HTuple hv_gripper_H_tool, hv_base_H_tool, hv_PoseRobotGrasp;

    // Initialize paths and parameters (Assuming hv_NameStart has been set globally or passed as parameter)
    HTuple hv_NameStart = "../stationarycam_";
    hv_CalplateFile = "../caltab0430.descr";

    // Read internal camera parameters and calibrated poses
    ReadCamPar(hv_NameStart + "final_campar.dat", &hv_CamParam);
    ReadPose(hv_NameStart + "final_pose_cam_base.dat", &hv_BaseInCamPose);
    PoseToHomMat3d(hv_BaseInCamPose, &hv_cam_H_base);
    ReadPose(hv_NameStart + "final_pose_tool_calplate.dat", &hv_CalplateInToolPose);
    PoseToHomMat3d(hv_CalplateInToolPose, &hv_tool_H_calplate);

    // Read pose of gripper in tool coordinates
    ReadPose(hv_NameStart + "pose_tool_gripper.dat", &hv_GripperInToolPose);
    PoseToHomMat3d(hv_GripperInToolPose, &hv_tool_H_gripper);

    // Define reference coordinate system
    define_reference_coord_system(hv_NameStart + "calib_01", hv_CamParam, hv_CalplateFile, &hv_PoseRef);
    PoseToHomMat3d(hv_PoseRef, &hv_cam_H_ref);

    // Convert image points to world plane
    ImagePointsToWorldPlane(hv_CamParam, hv_PoseRef, hv_Row, hv_Column, "m", &hv_CenterPointX_ref, &hv_CenterPointY_ref);

    // Calculate grasp pose
    HomMat3dIdentity(&hv_HomMat3DIdentity);
    hv_GraspPhiZ_ref = 3.14159 / 4; // Example rotation; this should be calculated based on application specifics
    HomMat3dRotate(hv_HomMat3DIdentity, hv_GraspPhiZ_ref, "z", 0, 0, 0, &hv_HomMat3D_RZ_Phi);
    HomMat3dTranslate(hv_HomMat3D_RZ_Phi, hv_CenterPointX_ref, hv_CenterPointY_ref, 0, &hv_ref_H_grasp);

    // Compose to find gripper pose in camera coordinates
    HomMat3dCompose(hv_cam_H_ref, hv_ref_H_grasp, &hv_cam_H_grasp);
    HomMat3dToPose(hv_cam_H_grasp, &hv_GripperInCamPose);

    // base_H_tool = base_H_cam * cam_H_ref * ref_H_grasp * gripper_H_tool
    // calc where to position the tool to grasp the nut
    HomMat3dInvert(hv_cam_H_base, &hv_base_H_cam);
    HomMat3dCompose(hv_base_H_cam, hv_cam_H_grasp, &hv_base_H_grasp);
    HomMat3dInvert(hv_tool_H_gripper, &hv_gripper_H_tool);
    HomMat3dCompose(hv_base_H_grasp, hv_gripper_H_tool, &hv_base_H_tool);
    HomMat3dToPose(hv_base_H_tool, &hv_PoseRobotGrasp);

    // Convert pose type to the one used by the robot controller (ZYX)
    ConvertPoseType(hv_PoseRobotGrasp, "Rp+T", "abg", "point", &hv_PoseRobotGrasp_ZYX);
}
