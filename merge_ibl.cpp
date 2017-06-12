//
// Created by fatih on 6/12/17.
//

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>
#include <iostream>

using namespace cv;

void merge()
{
    auto full = imread("full.png");
    auto local = imread("local.png");
    auto distant = imread("distant.png");
    auto mask = imread("mask.png");

    Mat diff(mask.rows, mask.cols, CV_16SC3);

    for (int i = 0; i<mask.rows; ++i) {
        for (int j = 0; j<mask.cols; ++j) {
            auto& elem = mask.at<Vec3b>(i, j);
            //if (elem[2]>1 && elem[2]<=7)
            if (elem[2] == 129)
            {
                elem = Vec3b(1, 1, 1);
            }
            else {
                elem = Vec3b(0, 0, 0);
            }
        }
    }

    for (int i = 0; i<mask.rows; ++i) {
        for (int j = 0; j<mask.cols; ++j) {
            auto& f = full.at<Vec3b>(i, j);
            auto& elem = local.at<Vec3b>(i, j);

            auto r = (short)f[2] - (short)elem[2];
            auto g = (short)f[1] - (short)elem[1];
            auto b = (short)f[0] - (short)elem[0];

            diff.at<Vec3s>(i, j) = Vec3s(b,g,r);
        }
    }
//
//    imshow("mask", mask);
//    waitKey(0);

    Mat synth(mask.rows, mask.cols, CV_8UC3);

    for (int i = 0; i<mask.rows; ++i) {
        for (int j = 0; j<mask.cols; ++j) {
            auto& elem = mask.at<Vec3b>(i, j);
            auto& f = full.at<Vec3b>(i, j);
            synth.at<Vec3b>(i, j) = Vec3b(f[0] * elem[0], f[1] * elem[1], f[2] * elem[2]);
        }
    }

    Mat res(mask.rows, mask.cols, CV_8UC3);

    auto is_zero = [](auto& e) -> bool
    {
        return e[0] == 0 && e[1] == 0 && e[2] == 0;
    };
//
//    imshow("synth", synth);
//    waitKey(0);

    for (int i = 0; i<mask.rows; ++i) {
        for (int j = 0; j<mask.cols; ++j) {
            auto& res_p = res.at<Vec3b>(i, j);
            if (is_zero(mask.at<Vec3b>(i, j)))
            {
                auto& dist = distant.at<Vec3b>(i, j);
                auto dif = diff.at<Vec3s>(i, j) * 2;
                auto r = Vec3s(std::max(0,(int)dist[0] + (int)dif[0]), std::max(0, (int)dist[1] + int(dif[1])), std::max(0, int(dist[2]) + int(dif[2])));
                if (r[0] < 0 || r[0] > 255 || r[1] < 0 || r[1] > 255 || r[2] < 0 || r[2] > 255)
                {
                    std::cout << "fuck\n"  << (short)dist[0] << ", " << (short)dist[1] << ", " << (short)dist[2] << " + " << dif[0] << ", " << dif[1] << ", " << dif[2] << " = "<< r[0] << ", " << r[1] << ", " << r[2] << '\n';
                }
                res_p = r;
            }
            else
            {
                res_p = synth.at<Vec3b>(i,j);
            }
        }
    }

    imwrite("ibl2.png", res);
    imshow("res", res);
    waitKey(0);
}
