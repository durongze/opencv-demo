
#include <opencv2/opencv.hpp>                

#define GL_WIN_SIZE_X   640
#define GL_WIN_SIZE_Y   480

typedef unsigned char uchar;

int main(int argc, char** argv)
{
    cv::Mat ir_mat = cv::Mat(GL_WIN_SIZE_Y, GL_WIN_SIZE_X, CV_8UC3);
    uchar* pc = ir_mat.ptr<uchar>(0);

    cv::namedWindow("IR_video", CV_WINDOW_NORMAL);
    cvResizeWindow("IR_video", GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
    cv::moveWindow("IR_video", 0, 0);

    getchar();

    return 0;
}
/*ldd exec 
        linux-vdso.so.1 =>  (0x00007fff689fe000)
        libpthread.so.0 => /lib64/libpthread.so.0 (0x00007fef8d417000)
        libopencv_calib3d.so.3.3 => /opt/opencv330/lib/libopencv_calib3d.so.3.3 (0x00007fef8d082000)
        libopencv_core.so.3.3 => /opt/opencv330/lib/libopencv_core.so.3.3 (0x00007fef8cb1f000)
        libopencv_dnn.so.3.3 => /opt/opencv330/lib/libopencv_dnn.so.3.3 (0x00007fef8c5ef000)
        libopencv_features2d.so.3.3 => /opt/opencv330/lib/libopencv_features2d.so.3.3 (0x00007fef8c30f000)
        libopencv_flann.so.3.3 => /opt/opencv330/lib/libopencv_flann.so.3.3 (0x00007fef8c0b1000)
        libopencv_highgui.so.3.3 => /opt/opencv330/lib/libopencv_highgui.so.3.3 (0x00007fef8bea6000)
        libopencv_imgcodecs.so.3.3 => /opt/opencv330/lib/libopencv_imgcodecs.so.3.3 (0x00007fef8b9c0000)
        libopencv_imgproc.so.3.3 => /opt/opencv330/lib/libopencv_imgproc.so.3.3 (0x00007fef8b1b1000)
        libopencv_ml.so.3.3 => /opt/opencv330/lib/libopencv_ml.so.3.3 (0x00007fef8aee5000)
        libopencv_objdetect.so.3.3 => /opt/opencv330/lib/libopencv_objdetect.so.3.3 (0x00007fef8ac7f000)
        libopencv_photo.so.3.3 => /opt/opencv330/lib/libopencv_photo.so.3.3 (0x00007fef8a998000)
        libopencv_shape.so.3.3 => /opt/opencv330/lib/libopencv_shape.so.3.3 (0x00007fef8a75e000)
        libopencv_stitching.so.3.3 => /opt/opencv330/lib/libopencv_stitching.so.3.3 (0x00007fef8a4da000)
        libopencv_superres.so.3.3 => /opt/opencv330/lib/libopencv_superres.so.3.3 (0x00007fef8a2ad000)
        libopencv_videoio.so.3.3 => /opt/opencv330/lib/libopencv_videoio.so.3.3 (0x00007fef8a083000)
        libopencv_video.so.3.3 => /opt/opencv330/lib/libopencv_video.so.3.3 (0x00007fef89e16000)
        libopencv_videostab.so.3.3 => /opt/opencv330/lib/libopencv_videostab.so.3.3 (0x00007fef89bbf000)
        libstdc++.so.6 => /lib64/libstdc++.so.6 (0x00007fef898b8000)
        libm.so.6 => /lib64/libm.so.6 (0x00007fef895b6000)
        libgcc_s.so.1 => /lib64/libgcc_s.so.1 (0x00007fef8939f000)
        libc.so.6 => /lib64/libc.so.6 (0x00007fef88fde000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fef8d64d000)
        libdl.so.2 => /lib64/libdl.so.2 (0x00007fef88dda000)
        librt.so.1 => /lib64/librt.so.1 (0x00007fef88bd1000)
        libopenblas.so.0 => /opt/OpenBLAS/lib/libopenblas.so.0 (0x00007fef87dde000)
        libgfortran.so.3 => /lib64/libgfortran.so.3 (0x00007fef87abc000)
        libquadmath.so.0 => /lib64/libquadmath.so.0 (0x00007fef8787f000)
        */
