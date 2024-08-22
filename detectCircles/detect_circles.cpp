#include "function.h"

void detectCircles(HObject ho_Image, HTuple *hv_CenterRow, HTuple *hv_CenterColumn)
{
    // Local iconic variables
    HObject ho_Region, ho_ConnectedRegions, ho_SelectedRegions;
    HObject ho_RegionBorder, ho_RegionDilation, ho_RegionUnion;
    HObject ho_ImageReduced, ho_Edges, ho_ContoursSplit, ho_SelectedContours;
    HObject ho_Circles, ho_ObjectSelected, ho_UnionContours;

    // Local control variables
    HTuple hv_NumberContours, hv_i, hv_Attrib, hv_NumberCircles;
    HTuple hv_Row, hv_Column, hv_Radius, hv_StartPhi, hv_EndPhi;
    HTuple hv_PointOrder;

    // Image processing operations
    FastThreshold(ho_Image, &ho_Region, 200, 255, 20);
    Connection(ho_Region, &ho_ConnectedRegions);
    SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 4000, 7500);

    Boundary(ho_SelectedRegions, &ho_RegionBorder, "inner_filled");
    DilationCircle(ho_RegionBorder, &ho_RegionDilation, 5);
    Union1(ho_RegionDilation, &ho_RegionUnion);
    ReduceDomain(ho_Image, ho_RegionUnion, &ho_ImageReduced);

    EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1.5, 30, 60);
    SegmentContoursXld(ho_Edges, &ho_ContoursSplit, "lines_circles", 5, 2, 2);
    SelectContoursXld(ho_ContoursSplit, &ho_SelectedContours, "contour_length", 100, 300, -0.5, 0.5);
    CountObj(ho_SelectedContours, &hv_NumberContours);
    GenEmptyObj(&ho_Circles);

    for (hv_i = 1; hv_i <= hv_NumberContours; hv_i += 1)
    {
        SelectObj(ho_SelectedContours, &ho_ObjectSelected, hv_i);
        GetContourGlobalAttribXld(ho_ObjectSelected, "cont_approx", &hv_Attrib);
        if (hv_Attrib == 1)
        {
            ConcatObj(ho_Circles, ho_ObjectSelected, &ho_Circles);
        }
    }

    UnionCocircularContoursXld(ho_Circles, &ho_UnionContours, HTuple(60).TupleRad(),
                               HTuple(10).TupleRad(), HTuple(30).TupleRad(), 100, 50, 10, "true", 1);
    CountObj(ho_UnionContours, &hv_NumberCircles);
    std::cout << "Detected " << hv_NumberCircles.ToString() << " circle centers:" << std::endl;

    *hv_CenterRow = HTuple();
    *hv_CenterColumn = HTuple();

    for (hv_i = 1; hv_i <= hv_NumberCircles; hv_i += 1)
    {
        SelectObj(ho_UnionContours, &ho_ObjectSelected, hv_i);
        FitCircleContourXld(ho_ObjectSelected, "algebraic", -1, 0, 0, 3, 2,
                            &hv_Row, &hv_Column, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);

        *hv_CenterRow = hv_CenterRow->TupleConcat(hv_Row);
        *hv_CenterColumn = hv_CenterColumn->TupleConcat(hv_Column);
    }
}
