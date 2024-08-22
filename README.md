# SJTUME24-gradproject

2024机动本科毕业设计项目  
本项目的目标是搭建一个鲁棒性良好的试管抓取系统，以实现在深低温环境下生物样本的自动出入库。  
代码由三部分组成。第一部分（detectCircles）在海康工业相机上运行，需要事先安装[海康官方SDK](https://www.hikrobotics.com/cn/machinevision/service/download)、[非汐RDK](http://www.flexiv.cn/software/rdk)和[halcon机器视觉软件](https://www.mvtec.com/downloads). 安装好后打开MVS软件是否能连上相机，并跑跑halcon官方例程（着重跑通handeye_stationarycam_calibration.hdev，理解机器人手眼标定的步骤，文件夹里的所有坐标系转换dat文件均由该代码生成）。这部分代码能够让相机以仰视视角拍摄冻存盒底部图片，通过Hough圆变换算法识别白色试管底部，再通过已标定的机器人基坐标系到工具坐标系的转换关系，计算出夹爪抓取点的世界坐标。  
注意事项：
- 标定板描述文件.descr和图片文件.ps可由gen_caltab算子生成
- simpinput.py是驱动非汐机械臂完成单次抓取的程序，需要手动修改本地ip和非汐依赖库的路径
- 所有标定文件.dat由上文提及的例程程序生成，标定前务必确保标定板的实际大小和描述文件大小一致，标定精度会影响后续抓取操作，最好控制在1mm以内
- CMakeLists.txt文件中的HALCONROOT需改成自己的安装路径

第二部分（foundationpose）和第三部分（nanosam）均在深度相机zed上运行，需要事先安装[ZED SDK](https://www.stereolabs.com/en-sg/developers/release). 两部分代码分别基于[6D姿态估计算法FoundationPose](https://github.com/NVlabs/FoundationPose)和[统一图像分割模型NanoSAM](https://github.com/NVIDIA-AI-IOT/nanosam)开发，并在ZED相机上实现实时的特征提取和姿态估计。第二、三部分代码的环境配置与源码一致，请事先安装好foundationpose的docker环境，跑通nanosam的五个官方demo. 运行时只需执行foundationpose/pose_est.sh，打开深度相机并捕获rgbd图像，请用鼠标双击弹出的图像窗口中的待抓取物体，代码将自动识别物体的图像掩膜并标注物体三轴坐标系。  
注意事项：
- 本代码的foundationpose使用的是基于已知模型的设置，需要obj模型文件和mtl材质文件，这两个文件放置在demo_data/xxx/mesh文件夹下
- foundationpose二值化的掩码文件放置在demo_data/xxx/masks文件夹下，通过初始时刻rgb图像生成，标注了待抓取物体的大致位置
- realtime_pose_est.py内的所有路径需要修改
