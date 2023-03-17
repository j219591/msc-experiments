#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/optflow.hpp>

using namespace std;
using namespace cv;

int lucas_kanade(const string& filename, bool save)
{
    VideoCapture capture(filename);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }
    // Create some random colors
    vector<Scalar> colors;
    RNG rng;
    for(int i = 0; i < 100; i++)
    {
        int r = rng.uniform(0, 256);
        int g = rng.uniform(0, 256);
        int b = rng.uniform(0, 256);
        colors.push_back(Scalar(r,g,b));
    }
    Mat old_frame, old_gray;
    vector<Point2f> p0, p1;
    // Take first frame and find corners in it
    capture >> old_frame;
    cvtColor(old_frame, old_gray, COLOR_BGR2GRAY);
    goodFeaturesToTrack(old_gray, p0, 100, 0.3, 7, Mat(), 7, false, 0.07);
    
    // Create a mask image for drawing purposes
    Mat mask = Mat::zeros(old_frame.size(), old_frame.type());
    int counter = 0;
    while(true) {
        Mat frame, frame_gray;
        capture >> frame;
        if (frame.empty())
            break;
        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
        // calculate optical flow
        vector<uchar> status;
        vector<float> err;
        TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.02);
        calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, Size(20,20), 1, criteria);
        vector<Point2f> good_new;
        for(uint i = 0; i < p0.size(); i++)
        {
            // Select good points
            if(status[i] == 1) {
                good_new.push_back(p1[i]);
                // draw the tracks
                line(mask,p1[i], p0[i], colors[i], 2);
                circle(frame, p1[i], 5, colors[i], -1);
            }
        }
        Mat img;
        add(frame, mask, img);
      
        imshow("flow_lk", img);

        int keyboard = waitKey(25);
        if (keyboard == 'q' || keyboard == 27)
            break;
        // Now update the previous frame and previous points
        old_gray = frame_gray.clone();
        p0 = good_new;
        counter++;
    }

    waitKey(0);

    return 0;
}

template <typename Method, typename... Args>
void dense_optical_flow(string _filename, bool binaryFlow, Method method, bool to_gray, Args&&... args) {
    VideoCapture capture(samples::findFile(_filename));
    //if (filename == "camera")
    //    capture(filename);

    if (!capture.isOpened()) {
        cerr << "Unable to open video file" << endl;
    }

    Mat _tmp, previousFrame;
    capture >> _tmp;
    if (to_gray)
        cvtColor(_tmp, previousFrame, COLOR_BGR2GRAY);
    else
        previousFrame = _tmp;

    auto counter = 0;

    while (true) {
		Mat tmp, nextFrame;
        capture >> tmp;
        if (to_gray)
            cvtColor(tmp, nextFrame, COLOR_BGR2GRAY);
        else
            nextFrame = tmp;

        auto dim = previousFrame.size();
        namedWindow("frame",  WINDOW_GUI_NORMAL);
        namedWindow("binFrame", WINDOW_GUI_NORMAL);
        namedWindow("flow", WINDOW_GUI_NORMAL);

        moveWindow("frame", 50, 50);
        moveWindow("binFrame", 50 + dim.width + 25, 50);
        moveWindow("flow", 50, 50 + dim.height + 50);

        Mat previousFrameB;
        threshold(previousFrame, previousFrameB, 80, 255, THRESH_BINARY);

        Mat nextFrameB;
        threshold(nextFrame, nextFrameB, 80, 255, THRESH_BINARY);
        imshow("binFrame", nextFrameB);

        Mat flow(previousFrame.size(), CV_32FC2);
        if (!binaryFlow)
            method(previousFrame, nextFrame, flow, forward<Args>(args)...);
        else
            method(previousFrameB, nextFrameB, flow, forward<Args>(args)...);

        Mat flow_parts[2];
        split(flow, flow_parts);

        Mat magnitude, angle, magnitude_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magnitude_norm, 0.0f, 1.0f, NORM_MINMAX);

        angle *= ((1.f/360.f)*(180.f/255.f));

        // HSV image
        Mat _hsv[] {angle, Mat::ones(angle.size(), CV_32F), magnitude_norm};
        Mat hsv, hsv8, bgr;
        merge(_hsv, 3, hsv);

        hsv.convertTo(hsv8, CV_8U, 255.0);

        cvtColor(hsv8, bgr, COLOR_HSV2BGR);

        imshow("flow",  bgr);
        imshow("binFrame", previousFrameB);
        imshow("frame", tmp);

        auto keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;

        previousFrame = nextFrame;
        counter++;
    }

    waitKey(0);
    capture.release();
    destroyAllWindows();
}

int main(int argc, char** argv) {
    cout << "Method: " << argv[1] << endl;
    cout << "Will open file " << argv[2] << endl;

    if (string(argv[1]) == string("lk"))
        lucas_kanade(argv[2], false);
    else if (string(argv[1]) == string("dense"))
        dense_optical_flow(argv[2], false, optflow::calcOpticalFlowSparseToDense, true, 8, 128, 0.05f, true, 500.0f, 1.5f);
    else if (string(argv[1]) == string("dense-binary"))
        dense_optical_flow(argv[2], true, optflow::calcOpticalFlowSparseToDense, true, 8, 128, 0.05f, true, 500.0f, 1.5f);
}