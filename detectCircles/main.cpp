#include "function.h"
#include <sstream>
#include <cstdlib>
#include <string>

int main()
{
    HObject ho_Image;
    HTuple  hv_AcqHandle, hv_MeanGrayVal, hv_Deviation;
    HTuple  hv_CenterRow, hv_CenterColumn, hv_NumberCircles;

    try
    {
        OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
                         -1, "false", "default", "34bd203c0e0c_Hikrobot_MVCS06010GC", 0, -1, &hv_AcqHandle);
        GrabImageStart(hv_AcqHandle, -1);
        GrabImageAsync(&ho_Image, hv_AcqHandle, -1);
        hv_MeanGrayVal = 0;
        while (hv_MeanGrayVal == 0)
        {
            GrabImageAsync(&ho_Image, hv_AcqHandle, -1);
            Intensity(ho_Image, ho_Image, &hv_MeanGrayVal, &hv_Deviation);
        }
        // ReadImage(&ho_Image, "../detect_circles.bmp");
        detectCircles(ho_Image, &hv_CenterRow, &hv_CenterColumn);

        std::cout << "Rows: " << hv_CenterRow.ToString() << std::endl;
        std::cout << "Columns: " << hv_CenterColumn.ToString() << std::endl;
    }
    catch (HException& ex)
    {
        std::cerr << "Error: " << ex.ErrorMessage().Text() << std::endl;
    }

    HTuple hv_Row, hv_Column, hv_PoseRobotGrasp_ZYX;
    std::ostringstream ossx, ossy;
    ossx << "["; ossy << "[";
    for(int i=0; i < hv_CenterRow.Length(); i++) {
        hv_Row = hv_CenterRow[i];
        hv_Column = hv_CenterColumn[i];

        // Calculate the robot grasp pose
        calc_grasp_pose(hv_Row, hv_Column, hv_PoseRobotGrasp_ZYX);
        std::cout << "Grasp Pose: " << hv_PoseRobotGrasp_ZYX.ToString() << std::endl;
        ossx << hv_PoseRobotGrasp_ZYX[0].D();
        ossy << hv_PoseRobotGrasp_ZYX[1].D();
        if(i < hv_CenterRow.Length()-1) {
            ossx << ", "; ossy << ", ";
        }
    }
    ossx << "]"; ossy << "]";

    std::string command = "python3 ../simpinput.py '" + ossx.str() + "' '" + ossy.str() + "'";
    system(command.c_str());

    return 0;
}