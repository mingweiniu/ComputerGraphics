#ifndef HW2_TOOL_HPP
#define HW2_TOOL_HPP

#include <string>
#include <vector>
#include <tuple>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace hw2{
	std::vector<std::tuple<std::vector<cv::KeyPoint>, cv::Mat>> sift_files(
		std::vector<std::string> inputs);

	std::vector<std::vector<std::tuple<float, int, int>>> findKNN(
		cv::Mat object,
		cv::Mat target,
		int k,
		int distance
	);
	std::vector<int> random_id(int max, int sample);
	std::vector<std::vector<int>> permutation(std::vector<std::vector<int>> input);
	cv::Mat ransac(
		std::vector<std::vector<std::tuple<float, int, int>>> matches,
		std::vector<cv::KeyPoint> obj_kp,
		std::vector<cv::KeyPoint> tar_kp,
		int sample_num,
		int distance
	);
	cv::Mat get_H_mat(std::vector<std::tuple<float, float, float, float>> obj_tar);
	void forward_warping(cv::Mat H, std::string obj_path, std::string tar_path, std::string save_path);
	void backward_warping(cv::Mat H, std::string obj_path, std::string tar_path, std::string save_path);
}

#endif // !HW2_TOOL_HPP