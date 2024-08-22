#ifndef DETECTCIRCLES_FUNCTION_H
#define DETECTCIRCLES_FUNCTION_H

#include "HalconCpp.h"
using namespace HalconCpp;

void detectCircles(HObject ho_Image, HTuple *hv_CenterRow, HTuple *hv_CenterColumn);
void define_reference_coord_system(HTuple hv_ImageName, HTuple hv_CamParam, HTuple hv_CalplateFile, HTuple *hv_PoseCamRef);
void calc_grasp_pose(HTuple hv_Row, HTuple hv_Column, HTuple &hv_PoseRobotGrasp_ZYX);

#endif //DETECTCIRCLES_FUNCTION_H
