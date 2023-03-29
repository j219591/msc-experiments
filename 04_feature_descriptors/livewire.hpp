#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct Pixel {
	int x, y;
	Pixel(int x = 0, int y = 0) : x(x), y(y) {}
};

struct PixelNode {
	int x, y;
	double dist;
	PixelNode(int _x = 0, int _y = 0, double _dist = 0) : x(_x), y(_y), dist(_dist) {}
	bool operator<(const PixelNode &rhs) const {
		return dist > rhs.dist;
	}
};

vector<vector<Pixel>> livewire(Mat &img, const vector<Pixel> &seeds) {
	const int n = img.rows, m = img.cols;
	constexpr int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
	constexpr int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

	Mat gx, gy;
	Sobel(img, gx, CV_64F, 1, 0);
	Sobel(img, gy, CV_64F, 0, 1);

	vector<vector<Pixel>> paths;

	vector<vector<bool>> vis(n, vector<bool>(m, false));
	vector<vector<double>> dist(n, vector<double>(m, numeric_limits<double>::infinity()));
	for (auto &start : seeds) {
		dist[start.x][start.y] = 0;

		priority_queue<PixelNode> pq;
		pq.push(PixelNode(start.x, start.y, 0));
		while (!pq.empty()) {
			PixelNode u = pq.top(); pq.pop();
			if (vis[u.x][u.y]) continue;
			vis[u.x][u.y] = true;

			for (int i = 0; i < 8; ++i) {
				int nx = u.x + dx[i];
				int ny = u.y + dy[i];
				if (nx < 0 || nx >= n || ny < 0 || ny >= m) continue;
				double w = 1/sqrt(gx.at<double>(u.x, u.y) * gx.at<double>(nx, ny) + gy.at<double>(u.x, u.y) * gy.at<double>(nx, ny));
				//double w = 1/mag.at<double>(u.x, u.y);
				double alt = dist[u.x][u.y] + w;
				if (alt < dist[nx][ny]) {
					//cout << "adding " << nx << ", " << ny << endl;
					dist[nx][ny] = w;
					pq.push(PixelNode(nx, ny, alt));
				}
			}
		}

		vector<Pixel> path;
		path.push_back(start);

		vector<vector<bool>> vis_path(n, vector<bool>(m, false));
		while (true) {
			Pixel last = path.back();
			if (last.x < 0 || last.x >= n || last.y < 0 || last.y >= m) break;
			double min_dist = numeric_limits<double>::infinity();
			int nx = -1, ny = -1;
			for (int i = 0; i < 8; ++i) {
				int tx = last.x + dx[i];
				int ty = last.y + dy[i];
				if (tx < 0 || tx >= n || ty < 0 || ty >= m) continue;
				if (vis_path[tx][ty]) continue;

				vis_path[tx][ty] = true;
				//cout << "dist: " << dist[tx][ty] << ", " << min_dist;
				if (dist[tx][ty] < min_dist && dist[tx][ty] > 0) {
					min_dist = dist[tx][ty];
					nx = tx, ny = ty;
				}
				//cout << "(" << nx << "," << ny << ") ";
			}
			path.push_back(Pixel(nx, ny));
		}
		paths.push_back(path);
	}

	return paths;
}
