#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

int main(int argc, char** argv) {
    VideoCapture capture(samples::findFile(argv[1]));
    //if (filename == "camera")
    //    capture(filename);

    if (!capture.isOpened()) {
        cerr << "Unable to open video file" << endl;
    }

    Mat _tmp, previousFrame;
    capture >> _tmp;
    cvtColor(_tmp, previousFrame, COLOR_BGR2GRAY);

    bool pausedFrame = false;
    while (true) {
        Mat tmp, nextFrame;
        capture >> tmp;
        cvtColor(tmp, nextFrame, COLOR_BGR2GRAY);

        Ptr<SURF> detector = SURF::create();
        detector->setHessianThreshold(2000);

        std::vector<KeyPoint> keypoint_1, keypoint_2;
        Mat descriptors_1, descriptors_2;

        detector->detectAndCompute(previousFrame, Mat(), keypoint_1, descriptors_1);
        detector->detectAndCompute(nextFrame, Mat(), keypoint_2, descriptors_2);

        BFMatcher matcher(NORM_L2);
        vector<DMatch> matches;
        matcher.match(descriptors_1, descriptors_2, matches);

        auto keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
        if (keyboard == 'p') {
            waitKey(-1);
        } 

        
        if (keyboard == '.')
            pausedFrame = true;

        Mat matchesFrame;
        drawMatches(previousFrame, keypoint_1, nextFrame, keypoint_2, matches, matchesFrame);

        imshow("SURF", matchesFrame);        

        previousFrame = nextFrame;
        if (pausedFrame) {
            keyboard = waitKey(-1);
            pausedFrame = (keyboard == '.');
        }
    }

    waitKey(0);
    capture.release();
    destroyAllWindows();
}