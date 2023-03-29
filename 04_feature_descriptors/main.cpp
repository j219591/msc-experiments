// #include <execution>
#include <iostream>
// #include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <omp.h>
#include "livewire.hpp"
//#include "detectors/SURF.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

int main(int argc, char** argv) {
    cuda::DeviceInfo info;
    cout << "Device name: " << info.name() << endl;
    cout << "Compute capability: " << info.majorVersion() << "." << info.minorVersion() << endl;

    VideoCapture capture;
    if (argv[1] != "camera")
        capture = VideoCapture(samples::findFile(argv[1]));
    else
        capture = VideoCapture(0);

    if (!capture.isOpened()) {
        cerr << "Unable to open video file" << endl;
    }

    Mat _tmp, previousFrame;
    capture >> _tmp;
    cvtColor(_tmp, previousFrame, COLOR_BGR2GRAY);

    auto [width, height] = _tmp.size();
    const int pathThreshold = (width > height) ? .3 * width : .35*height;
    cout << "Path threshold: " << pathThreshold << endl;

    bool pausedFrame = false;

    cv::VideoWriter writer("output_video.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 24, _tmp.size(), true);
    if (!writer.isOpened()) {
        std::cerr << "Error: Failed to open output video file\n";
        return -1;
    }

    auto detector = SIFT::create();
    //detector->setHessianThreshold(4000);
    while (true) {
        auto t0g = clock();
        Mat tmp, nextFrameColored, nextFrame;
        capture >> tmp;
        if (tmp.empty()) break;
        nextFrameColored = tmp.clone();
        cvtColor(tmp, nextFrame, COLOR_BGR2GRAY);

        std::vector<KeyPoint> keypoint_1, keypoint_2;
        Mat descriptors_1, descriptors_2;

        auto t0d = clock();
        //detector->detectAndCompute(previousFrame, cv::noArray(), keypoint_1, descriptors_1);
        detector->detectAndCompute(nextFrame, cv::noArray(), keypoint_2, descriptors_2);
        cout << "detect: " << (clock() - t0d)/(double)CLOCKS_PER_SEC << endl;

        auto keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
        if (keyboard == 'p') {
            waitKey(-1);
        } 

        if (keyboard == '.')
            pausedFrame = true;

        //BFMatcher matcher(NORM_L2);
        //auto matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
        if (!keypoint_2.empty()) {
            //vector<DMatch> matches;
           // matcher->match(descriptors_1, descriptors_2, matches);
            
            //Mat matchesFrame;
            //drawMatches(previousFrame, keypoint_1, nextFrame, keypoint_2, matches, matchesFrame);

            // imshow("SURF", matchesFrame);    

            vector<Pixel> seeds;
            // for (auto match : matches) {
            //     auto point = keypoint_2[match.trainIdx].pt;
            //     seeds.push_back(Pixel((uint32_t)point.y, (uint32_t)point.x));
            // }
            for (auto point : keypoint_2) {
                seeds.push_back(Pixel((uint32_t)point.pt.y, (uint32_t)point.pt.x));
            }

            if (!seeds.empty()) {
                auto t0 = clock();
                auto paths = livewire(nextFrame, seeds);
                cout << "livewire: " << (clock() - t0)/(double)CLOCKS_PER_SEC << endl;
                // for_each (
                //     execution::par,
                //     paths.begin(),
                //     paths.end(),
                //     [&](auto&& path) {
                //         cout << path.size() << endl;
                //         // for (auto &p : path) {
                //         //     cout << p.y << "," << p.x << endl;
                //         //     //circle(nextFrameColored, Point(p.y, p.x), 1, Scalar(255, 0, 255), 1);
                //         // }
                //     }
                // );

                auto t0c = clock();
                #pragma omp parallel for
                for (const auto &path : paths) {
                    if (path.size() > pathThreshold) {
                    #pragma omp parallel for
                    for (const auto p : path) {
                        circle(nextFrameColored, Point(p.y, p.x), 1, Scalar(255, 0, 255), 1);
                    }
                }
                }
                cout << "draw path: " << (clock() - t0c)/(double) CLOCKS_PER_SEC << endl;
            }
        }

        imshow("Image", nextFrameColored);
        writer.write(nextFrameColored);

        previousFrame = nextFrame.clone();
        if (pausedFrame) {
            keyboard = waitKey(-1);
            pausedFrame = (keyboard == '.');
        }
        cout << "iteration: " << (clock() - t0g)/(double) CLOCKS_PER_SEC << endl;
    }

    writer.release();
    capture.release();
    destroyAllWindows();
}