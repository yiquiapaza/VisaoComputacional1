// TestOpenCV.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/calib3d.hpp>

int main()
{
	cv::setBreakOnError(true);

	std::vector <cv::Point3d> model_points;
	std::vector <cv::Point2d> imagen_points;
	std::vector <cv::Point2d> origin_points;

	std::vector <cv::Point2d> points2d;
	std::vector <cv::Point3d> points3d;
	cv::Mat dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);
	std::vector<cv::Mat> rvecs;
	std::vector<cv::Mat> tvecs;

	std::string name;
	std::cin >> name;
	std::string dir_img = "D:/OpenCV/visao_trabalho1/" + name + ".jpg";
	std::string dir_yml = "D:/" + name + ".yml";

	cv::Mat img = cv::imread(dir_img);

	if (img.empty())
	{
		std::cout << "Could not open or find imagen" << std::endl;
		system("pause");
	}

	cv::FileStorage fs(dir_yml, cv::FileStorage::READ);

	cv::FileNode center = fs["origin"];
	cv::FileNodeIterator it_o = center.begin(), it_o_end = center.end();

	cv::FileNode points = fs["points2d"];
	cv::FileNodeIterator it = points.begin(), it_end = points.end();

	cv::FileNode model = fs["points3d"];
	cv::FileNodeIterator it_model = model.begin(), it_end_model = model.end();

	std::cout << (std::string)fs["calibrationDate"];

	for (it_o; it_o != it_o_end; it_o++)
	{
		imagen_points.push_back(cv::Point2d((int)(*it_o)["x"], (int)(*it_o)["y"]));
		origin_points.push_back(cv::Point2d((int)(*it_o)["x"], (int)(*it_o)["y"]));
	}

	for (it; it != it_end; it++)
	{
		//imagen_points.push_back(cv::Point2d((int)(*it)["x"], (int)(*it)["y"]));
		std::cout << (double)(*it)["x"] << "-" << (double)(*it)["y"] << std::endl;
		points2d.push_back(cv::Point2d((*it)["x"], (*it)["y"]));
	}

	for (std::vector<cv::Point2d>::iterator it = imagen_points.begin(); it != imagen_points.end(); it++)
	{
		if (it != imagen_points.begin())
		{
			cv::circle(img, *it, 5, cv::Scalar(0, 0, 255), 2, cv::LINE_8);
		}
		else
		{
			cv::circle(img, *it, 5, cv::Scalar(0, 255, 0), 2, cv::LINE_8);
		}

	}
	for (it_model; it_model != it_end_model; it_model++)
	{
		model_points.push_back(cv::Point3d((*it_model)["x"], (*it_model)["y"], (*it_model)["z"]));
		std::cout << (double)(*it_model)["x"] << "-" << (double)(*it_model)["y"] << (double)(*it_model) << std::endl;
		points3d.push_back(cv::Point3d((*it_model)["x"], (*it_model)["y"], (*it_model)["z"]));
	}
	fs.release();
	cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);

	std::cout << points2d << std::endl;
	std::cout << points3d << std::endl;

	cv::Mat_<double> P(16, 12);

	//Part 0
	P(0, 0) = points3d[0].x; P(0, 1) = points3d[0].y; P(0, 2) = points3d[0].z; P(0, 3) = 1;
	P(0, 4) = 0; P(0, 5) = 0; P(0, 6) = 0; P(0, 7) = 0;

	P(0, 8) = -points2d[0].x * points3d[0].x;
	P(0, 9) = -points2d[0].x * points3d[0].y;
	P(0, 10) = -points2d[0].x * points3d[0].z;
	P(0, 11) = -points2d[0].x;

	//Part 1
	P(1, 0) = 0; P(1, 1) = 0; P(1, 2) = 0; P(1, 3) = 0;
	P(1, 4) = points3d[0].x; P(1, 5) = points3d[0].y; P(1, 6) = points3d[0].z; P(1, 7) = 1;

	P(1, 8) = -points2d[0].y * points3d[0].x;
	P(1, 9) = -points2d[0].y * points3d[0].y;
	P(1, 10) = -points2d[0].y * points3d[0].z;
	P(1, 11) = -points2d[0].y;

	//Part 2
	P(2, 0) = points3d[1].x; P(2, 1) = points3d[1].y; P(2, 2) = points3d[1].z; P(2, 3) = 1;
	P(2, 4) = 0; P(2, 5) = 0; P(2, 6) = 0; P(2, 7) = 0;

	P(2, 8) = -points2d[1].x * points3d[1].x;
	P(2, 9) = -points2d[1].x * points3d[1].y;
	P(2, 10) = -points2d[1].x * points3d[1].z;
	P(2, 11) = -points2d[1].x;

	//Part 3
	P(3, 0) = 0; P(3, 1) = 0; P(3, 2) = 0; P(3, 3) = 0;
	P(3, 4) = points3d[1].x; P(3, 5) = points3d[1].y; P(3, 6) = points3d[1].z; P(3, 7) = 1;

	P(3, 8) = -points2d[1].y * points3d[1].x;
	P(3, 9) = -points2d[1].y * points3d[1].y;
	P(3, 10) = -points2d[1].y * points3d[1].z;
	P(3, 11) = -points2d[1].y;


	//Part 4
	P(4, 0) = points3d[2].x; P(4, 1) = points3d[2].y; P(4, 2) = points3d[2].z; P(4, 3) = 1;
	P(4, 4) = 0; P(4, 5) = 0; P(4, 6) = 0; P(4, 7) = 0;

	P(4, 8) = -points2d[2].x * points3d[2].x;
	P(4, 9) = -points2d[2].x * points3d[2].y;
	P(4, 10) = -points2d[2].x * points3d[2].z;
	P(4, 11) = -points2d[2].x;

	//Part 5
	P(5, 0) = 0; P(5, 1) = 0; P(5, 2) = 0; P(5, 3) = 0;
	P(5, 4) = points3d[2].x; P(5, 5) = points3d[2].y; P(5, 6) = points3d[2].z; P(5, 7) = 1;

	P(5, 8) = -points2d[2].y * points3d[2].x;
	P(5, 9) = -points2d[2].y * points3d[2].y;
	P(5, 10) = -points2d[2].y * points3d[2].z;
	P(5, 11) = -points2d[2].y;

	//Part 6
	P(6, 0) = points3d[3].x; P(6, 1) = points3d[3].y; P(6, 2) = points3d[3].z; P(6, 3) = 1;
	P(6, 4) = 0; P(6, 5) = 0; P(6, 6) = 0; P(6, 7) = 0;

	P(6, 8) = -points2d[3].x * points3d[3].x;
	P(6, 9) = -points2d[3].x * points3d[3].y;
	P(6, 10) = -points2d[3].x * points3d[3].z;
	P(6, 11) = -points2d[3].x;

	//Part 7
	P(7, 0) = 0; P(7, 1) = 0; P(7, 2) = 0; P(7, 3) = 0;
	P(7, 4) = points3d[3].x; P(7, 5) = points3d[3].y; P(7, 6) = points3d[3].z; P(7, 7) = 1;

	P(7, 8) = -points2d[3].y * points3d[3].x;
	P(7, 9) = -points2d[3].y * points3d[3].y;
	P(7, 10) = -points2d[3].y * points3d[3].z;
	P(7, 11) = -points2d[3].y;

	//Part 8
	P(8, 0) = points3d[4].x; P(8, 1) = points3d[4].y; P(8, 2) = points3d[4].z; P(8, 3) = 1;
	P(8, 4) = 0; P(8, 5) = 0; P(8, 6) = 0; P(8, 7) = 0;

	P(8, 8) = -points2d[4].x * points3d[4].x;
	P(8, 9) = -points2d[4].x * points3d[4].y;
	P(8, 10) = -points2d[4].x * points3d[4].z;
	P(8, 11) = -points2d[4].x;

	//Part 9
	P(9, 0) = 0; P(9, 1) = 0; P(9, 2) = 0; P(9, 3) = 0;
	P(9, 4) = points3d[4].x; P(9, 5) = points3d[4].y; P(9, 6) = points3d[4].z; P(9, 7) = 1;

	P(9, 8) = -points2d[4].y * points3d[4].x;
	P(9, 9) = -points2d[4].y * points3d[4].y;
	P(9, 10) = -points2d[4].y * points3d[4].z;
	P(9, 11) = -points2d[4].y;

	//Part 10
	P(10, 0) = points3d[5].x; P(10, 1) = points3d[5].y; P(10, 2) = points3d[5].z; P(10, 3) = 1;
	P(10, 4) = 0; P(10, 5) = 0; P(10, 6) = 0; P(10, 7) = 0;

	P(10, 8) = -points2d[5].x * points3d[5].x;
	P(10, 9) = -points2d[5].x * points3d[5].y;
	P(10, 10) = -points2d[5].x * points3d[5].z;
	P(10, 11) = -points2d[5].x;

	//Part 11
	P(11, 0) = 0; P(11, 1) = 0; P(11, 2) = 0; P(11, 3) = 0;
	P(11, 4) = points3d[5].x; P(11, 5) = points3d[5].y; P(11, 6) = points3d[5].z; P(11, 7) = 1;

	P(11, 8) = -points2d[5].y * points3d[5].x;
	P(11, 9) = -points2d[5].y * points3d[5].y;
	P(11, 10) = -points2d[5].y * points3d[5].z;
	P(11, 11) = -points2d[5].y;

	//Part 12
	P(12, 0) = points3d[6].x; P(12, 1) = points3d[6].y; P(12, 2) = points3d[6].z; P(12, 3) = 1;
	P(12, 4) = 0; P(12, 5) = 0; P(12, 6) = 0; P(12, 7) = 0;

	P(12, 8) = -points2d[6].x * points3d[6].x;
	P(12, 9) = -points2d[6].x * points3d[6].y;
	P(12, 10) = -points2d[6].x * points3d[6].z;
	P(12, 11) = -points2d[6].x;

	//Part 13
	P(13, 0) = 0; P(13, 1) = 0; P(13, 2) = 0; P(13, 3) = 0;
	P(13, 4) = points3d[6].x; P(13, 5) = points3d[6].y; P(13, 6) = points3d[6].z; P(13, 7) = 1;

	P(13, 8) = -points2d[6].y * points3d[6].x;
	P(13, 9) = -points2d[6].y * points3d[6].y;
	P(13, 10) = -points2d[6].y * points3d[6].z;
	P(13, 11) = -points2d[6].y;

	//Part 14
	P(14, 0) = points3d[7].x; P(14, 1) = points3d[7].y; P(14, 2) = points3d[7].z; P(14, 3) = 1;
	P(14, 4) = 0; P(14, 5) = 0; P(14, 6) = 0; P(14, 7) = 0;

	P(14, 8) = -points2d[7].x * points3d[7].x;
	P(14, 9) = -points2d[7].x * points3d[7].y;
	P(14, 10) = -points2d[7].x * points3d[7].z;
	P(14, 11) = -points2d[7].x;

	//Part 15
	P(15, 0) = 0; P(15, 1) = 0; P(15, 2) = 0; P(15, 3) = 0;
	P(15, 4) = points3d[7].x; P(15, 5) = points3d[7].y; P(15, 6) = points3d[7].z; P(15, 7) = 1;

	P(15, 8) = -points2d[7].y * points3d[7].x;
	P(15, 9) = -points2d[7].y * points3d[7].y;
	P(15, 10) = -points2d[7].y * points3d[7].z;
	P(15, 11) = -points2d[7].y;


	std::cout << P << std::endl;
	//cv::Mat O_ = P;

	cv::Mat O = cv::Mat::zeros(14, 1, CV_64F);
	cv::Mat out;

	if (cv::solve(P, O, out, cv::DECOMP_QR)) {
		std::cout << out << std::endl;
		std::cout << out.at<double>(0, 0) << std::endl;
	}
	else
	{
		std::cout << "bad" << std::endl;
		std::cout << out.at<double>(0, 0) << std::endl;
	}

	cv::Mat_<double> calibration(3, 4);
	calibration(0, 0) = out.at<double>(0, 0);
	calibration(0, 1) = out.at<double>(1, 0);
	calibration(0, 2) = out.at<double>(2, 0);
	calibration(0, 3) = out.at<double>(3, 0);

	calibration(1, 0) = out.at<double>(4, 0);
	calibration(1, 1) = out.at<double>(5, 0);
	calibration(1, 2) = out.at<double>(6, 0);
	calibration(1, 3) = out.at<double>(7, 0);

	calibration(2, 0) = out.at<double>(8, 0);
	calibration(2, 1) = out.at<double>(9, 0);
	calibration(2, 2) = out.at<double>(10, 0);
	calibration(2, 3) = out.at<double>(11, 0);

	std::cout << calibration << std::endl;

	cv::Mat_<double> first_point(4, 1);
	cv::Mat_<double> second_point(4, 1);

	first_point(0, 0) = 3.660; first_point(1, 0) = 2; first_point(2, 0) = 0; first_point(3, 0) = 1;
	second_point(0, 0) = 3.66; second_point(1, 0) = 2; second_point(2, 0) = 1,80; second_point(3, 0) = 1;

	cv::Mat coord1 = calibration * first_point;
	cv::Mat coord2 = calibration * second_point;

	std::cout << origin_points << std::endl;
	/*
	std::cout << coord1 << std::endl;
	std::cout << coord2 << std::endl;
	std::cout << origin_points[0].x << std::endl;
	std::cout << origin_points[0].y << std::endl;
	*/
	double x1 = coord1.at<double>(0, 0) / coord1.at<double>(2, 0);
	double y1 = coord1.at<double>(1, 0) / coord1.at<double>(2, 0);

	double x2 = coord2.at<double>(0, 0) / coord2.at<double>(2, 0);
	double y2 = coord2.at<double>(1, 0) / coord2.at<double>(2, 0);

	cv::Point2d p1 = cv::Point(x1 + origin_points[0].x, y1 + origin_points[0].y);
	cv::Point2d p2 = cv::Point(x2 + origin_points[0].x, y2 + origin_points[0].y);
	std::cout << p1 << std::endl;
	std::cout << p2 << std::endl;

	cv::line(img, p1, p2, cv::Scalar(255, 0, 0), 2, cv::LINE_8);

	cv::namedWindow(name, 1);
	cv::imshow(name, img);

	cv::waitKey(0);

	return 0;
}