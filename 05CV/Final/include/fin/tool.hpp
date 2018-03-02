#ifndef FIN_TOOL_HPP
#define FIN_TOOL_HPP

#include <string>
#include <vector>
#include <tuple>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace fin{
	enum class Dir { Obl, Left, Up, };

	void save3fImage(cv::Mat input, std::string path);
	std::tuple<cv::Mat, cv::Mat> LR3fImage(std::string imagepath);
	std::tuple<cv::Mat, cv::Mat> LR1UEdge(const std::string EdgePath, std::tuple<cv::Mat, cv::Mat> &Images);
	void Cutting(std::tuple<cv::Mat, cv::Mat>& Image, std::tuple<cv::Mat, cv::Mat> &Edge);
	cv::Mat norm3fImage(cv::Mat image);
	cv::Mat SSD_Norm(cv::Mat Obj, cv::Mat Tar, int halfWindow, bool DEBUG);
	cv::Mat SSD_LR(cv::Mat Left, cv::Mat Right, int halfWindow);
	void Correlation(cv::Mat Obj, cv::Mat Tar, cv::Mat EdgeL, cv::Mat EdgeR, int halfWindow);
	std::vector<std::tuple<cv::Vec3i, cv::Vec3f>> DynamicProgramming(const std::string& FilePath, const int row, const cv::Mat& Obj, const cv::Mat& Tar, int window);
	std::vector<std::vector<float>> tablefromfile(const std::string& FilePath);
	void GaussianSmooth(std::vector<std::vector<float>> &input);
}

#endif // !FIN_TOOL_HPP