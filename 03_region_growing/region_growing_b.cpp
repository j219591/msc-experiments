#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

size_t closestSeedIndex(const vector<Point> &seeds, const Point &currentPoint) {
    // Find the seed point that is closest to the current point
    int minDistance = INT_MAX;
    size_t closestSeed;
    for (int i = 0; i < seeds.size(); i++) {
        int distance = norm(currentPoint - seeds[i]);
        if (distance < minDistance) {
            minDistance = distance;
            closestSeed = i;
        }
    }

    return closestSeed;
}

// Region growing algorithm
// void regionGrowing(const Mat& image, Mat& segmented, const vector<Point>& seeds, int threshold, int minSize) {
void regionGrowing(const Mat& image, Mat& segmented, Mat& coloredSegmented, const vector<Vec3b>& colors, const vector<Point>& seeds, int threshold, int minSize) {
    // Define 4-neighborhood connectivity
    const int dx[] = {-1, 0, 1, 0};
    const int dy[] = {0, -1, 0, 1};

    // Initialize queue with seeds
    queue<Point> queue(deque(seeds.begin(), seeds.end()));

    segmented = Mat::zeros(image.size(), CV_8UC1);
    coloredSegmented = Mat::zeros(image.size(), CV_8UC3);

    // Initialize visited matrix
    Mat visited = Mat::zeros(image.size(), CV_8UC1);

    // Region growing loop
    while (!queue.empty()) {
        // Get current pixel
        auto p = queue.front();
        queue.pop();

        // Check if pixel intensity is within threshold
        if (abs(image.at<uchar>(p.y, p.x) - image.at<uchar>(seeds[0].y, seeds[0].x)) > threshold) {
            continue;
        }

        // Check if pixel is visited
        if (visited.at<uchar>(p.y, p.x) == 1) {
            continue;
        }

        // Add pixel to segmented image
        segmented.at<uchar>(p.y, p.x) = 255;
        coloredSegmented.at<Vec3b>(p.y, p.x) = colors[closestSeedIndex(seeds, p) % colors.size()];

        // Mark pixel as visited
        visited.at<uchar>(p.y, p.x) = 1;

        // Add neighbors to queue
        for (int i = 0; i < 4; i++) {
            Point q(p.x + dx[i], p.y + dy[i]);

            // Check if neighbor is within image bounds
            if (q.x < 0 || q.x >= image.cols || q.y < 0 || q.y >= image.rows) {
                continue;
            }

            // Check if neighbor is visited
            if (visited.at<uchar>(q.y, q.x) == 1) {
                continue;
            }

            // Check if neighbor intensity is within threshold
            if (abs(image.at<uchar>(q.y, q.x) - image.at<uchar>(seeds[0].y, seeds[0].x)) > threshold) {
                continue;
            }

            // Add neighbor to queue
            queue.push(q);
        }
    }

    // //Post-processing: Remove small regions
    // vector<vector<Point>> contours;
    // findContours(segmented, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // for (const vector<Point>& contour : contours) {
    //     if (contour.size() < minSize) {
    //         for (const Point& p : contour) {
    //             segmented.at<uchar>(p.y, p.x) = 0;
    //         }
    //     }
    // }
}

int main(int argc, char** argv) {
    const int MIN_SIZE = 2;

    // Read the input image
    Mat coloredImage = imread(argv[1]);
    Mat planes[3];
    split(coloredImage, planes);  // planes[2] is the red channel
    Mat image = planes[2] - planes[0] + planes[1];

    // Detect edges in input image
    Mat edges;
    //Canny(image, edges, 50, 150);
    Canny(image, edges, 100, 300);

    // Find contours of edges
    vector<vector<Point>> contours;
    findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Compute bounding boxes of contours
    vector<Rect> boundingBoxes;
    vector<Point> seeds;
    for (const vector<Point>& contour : contours) {
        if (contour.size() > MIN_SIZE) {
            Rect bbox = boundingRect(contour);
            boundingBoxes.push_back(bbox);
            seeds.push_back(Point(bbox.x + bbox.width/2, bbox.y + bbox.height/2));
            //seeds.push_back(contour);
        }
    }

    // Draw bounding boxes on input image
    Mat edgesImage;
    cvtColor(edges, edgesImage, COLOR_GRAY2BGR);
    for (const Rect& bbox : boundingBoxes) {
        rectangle(edgesImage, bbox, Scalar(0, 0, 255), 1);
    }

    // Segment input image using region growing with seeds at edge pixels
    // vector<Point> seeds;
    // for (const vector<Point>& contour : contours) {
    //     for (const Point& p : contour) {
    //         seeds.push_back(p);
    //     }
    // }

    vector<Vec3b> colors = {
        Vec3b(0, 255, 255),
        Vec3b(255, 255, 0),
        Vec3b(255, 0, 255), 
        Vec3b(0, 0, 255),
        Vec3b(0, 255, 0),
        Vec3b(255, 0, 0)
    };
    Mat segmented;
    Mat coloredSegmented;
    
    regionGrowing(image, segmented, coloredSegmented, colors, seeds, 120, 50);

    // for (int r = 0; r < segmented.rows; r++) {
    //     for (int c = 0; c < segmented.cols; c++) {
    //         int seedIndex = segmented.at<int>(r, c);
    //         if (seedIndex == 0)
    //             coloredSegmented.at<Vec3b>(r, c) = {255, 255, 255};
    //         if (seedIndex > 0) {
    //             cout << seedIndex << endl;
    //             coloredSegmented.at<Vec3b>(r, c) = colors[seedIndex % colors.size()];
    //         }
    //     }
    // }

    // Display edges and bounding boxesq
    imshow("Edges", edges);
    //imshow("Contours", contours);
    imshow("Edges with bounding boxes", edgesImage);

    // Show input image and segmented image
    imshow("Image", planes[2]);
    imshow("Red", planes[2]);
    imshow("Green", planes[1]);
    imshow("Blue", planes[0]);
    imshow("Red - Blue", planes[2] + planes[0] - planes[1]);
    imshow("Segmented image", segmented);
    imshow("Segmented colored image", coloredSegmented);
    waitKey(0);

    return 0;
}

