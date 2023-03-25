#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

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

void regionGrowing(const Mat& image, Mat& segmented, Mat& coloredSegmented, const vector<Vec3b>& colors, const vector<Point>& seeds, const int threshold) {
    // Initialize segmented image to all zeros
    segmented = Mat::zeros(image.size(), CV_8UC1);
    coloredSegmented = Mat::zeros(image.size(), CV_8UC3);

    // Create a queue to store the points to be processed
    vector<Point> pointsToProcess;

    // Add the seed points to the queue of points to be processed
    for (int i = 0; i < seeds.size(); i++) {
        pointsToProcess.push_back(seeds[i]);
    }

    // Loop through the queue of points to be processed
    while (!pointsToProcess.empty()) {
        // Get the next point to process
        Point currentPoint = pointsToProcess.back();
        pointsToProcess.pop_back();

        // Check if the current point is within the image boundaries
        if (currentPoint.x < 0 || currentPoint.y < 0 || currentPoint.x >= image.cols || currentPoint.y >= image.rows) {
            continue;
        }

        // Check if the current point has already been processed
        if (segmented.at<uchar>(currentPoint) > 0) {
            continue;
        }
        
        // Check if the intensity difference between the current point and the closest seed point is less than the threshold
        auto seedIndex = closestSeedIndex(seeds, currentPoint);
        int intensityDiff = abs(image.at<uchar>(currentPoint) - image.at<uchar>(seeds[seedIndex]));
        if (intensityDiff <= threshold) {
            // Mark the current point as part of the segmented region
            segmented.at<uchar>(currentPoint) = 255;
            coloredSegmented.at<Vec3b>(currentPoint) = colors[seedIndex % colors.size()];

            // Add the neighboring points to the queue of points to be processed
            pointsToProcess.push_back(Point(currentPoint.x+1, currentPoint.y));
            pointsToProcess.push_back(Point(currentPoint.x-1, currentPoint.y));
            pointsToProcess.push_back(Point(currentPoint.x, currentPoint.y+1));
            pointsToProcess.push_back(Point(currentPoint.x, currentPoint.y-1));
        }
    }
}

int main(int argc, char** argv) {
    // Read the input image
    Mat inputImage = imread(argv[1], IMREAD_GRAYSCALE);

    // Check if the image was loaded successfully
    if (inputImage.empty()) {
        printf("Unable to read input image\n");
        return -1;
    }

    // Apply Canny edge detection to the input image
    Mat edges;
    Canny(inputImage, edges, 100, 200);

    // Find contours in the edge image
    vector<vector<Point>> contours;
    findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Choose the center points of the bounding boxes around the contours as the initial seed points
    vector<Point> seeds;
    for (int i = 0; i < contours.size(); i++) {
        Rect bbox = boundingRect(contours[i]);
        seeds.push_back(Point(bbox.x + bbox.width/2, bbox.y + bbox.height/2));
    }

    // Set the intensity threshold for the region-growing algorithm
    int threshold = 47;

    // Perform region-growing on the input image using the seed points
    vector<Vec3b> colors = {
        Vec3b(0, 255, 255),
        Vec3b(255, 255, 0),
        Vec3b(255, 0, 255), 
        Vec3b(0, 0, 255),
        Vec3b(0, 255, 0),
        Vec3b(255, 0, 0)
    };
    Mat segmented;
    Mat coloredSegmented(segmented.size(), CV_8UC3);
    regionGrowing(inputImage, segmented, coloredSegmented, colors, seeds, threshold);

    // Display the segmented image
    namedWindow("Segmented Image", WINDOW_AUTOSIZE);
    imshow("Segmented Image", segmented);
    imshow("Segmented colored", coloredSegmented);
    waitKey(0);

    // Save the segmented image to disk
   //imwrite("segmented.png", segmented);

    return 0;
}

   
