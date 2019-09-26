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
	
	std::cout << points2d << std::endl;
	std::cout << points3d << std::endl;

	cv::Mat_<double> P(16, 12);
	
	int k = 0;
	for (int i = 0, k = 0; i < P.rows; i = 2 + i, k++)
	{
		for (int j = 0; j < P.cols; j++)
		{
			switch (j)
			{
			case 0: P(i, j) = points3d[k].x; break;
			case 1: P(i, j) = points3d[k].y; break;
			case 2: P(i, j) = points3d[k].z; break;
			case 3: P(i, j) = 1; break;
			case 4: P(i, j) = 0; break;
			case 5: P(i, j) = 0; break;
			case 6: P(i, j) = 0; break;
			case 7: P(i, j) = 0; break;
			case 8: P(i, j) = -points2d[k].x * points3d[k].x; break;
			case 9: P(i, j) = -points2d[k].x * points3d[k].y; break;
			case 10: P(i, j) = -points2d[k].x * points3d[k].z; break;
			case 11: P(i, j) = -points2d[k].x; break;
			}
		}
	}

	int f = 0;
	for (int i = 1, f = 0; i <= P.rows; i = 2 + i, f++)
	{
		for (int j = 0; j < P.cols; j++)
		{
			switch (j)
			{
			case 0: P(i, j) = 0; break;
			case 1: P(i, j) = 0; break;
			case 2: P(i, j) = 0; break;
			case 3: P(i, j) = 0; break;
			case 4: P(i, j) = points3d[f].x; break;
			case 5: P(i, j) = points3d[f].y; break;
			case 6: P(i, j) = points3d[f].z; break;
			case 7: P(i, j) = 1; break;
			case 8: P(i, j) = -points2d[f].y * points3d[f].x; break;
			case 9: P(i, j) = -points2d[f].y * points3d[f].y; break;
			case 10: P(i, j) = -points2d[f].y * points3d[f].z; break;
			case 11: P(i, j) = -points2d[f].y; break;
			}
		}
	}

	cv::Mat out;

	cv::SVD::solveZ(P, out);

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

	cv::Mat_<double> first_point(4,1);
	cv::Mat_<double> second_point(4, 1);
	
	first_point(0, 0) =  2; first_point(1, 0) = -2; first_point(2, 0) = 0; first_point(3, 0) = 1;
	second_point(0, 0) = 2; second_point(1, 0) = -2; second_point(2, 0) = 1.80; second_point(3, 0) = 1;

	cv::Mat coord1 = calibration * first_point;
	cv::Mat coord2 = calibration * second_point;

	double x1 = coord1.at<double>(0, 0) / coord1.at<double>(2, 0);
	double y1 = coord1.at<double>(1, 0) / coord1.at<double>(2, 0); 

	double x2 = coord2.at<double>(0, 0) / coord2.at<double>(2, 0);
	double y2 = coord2.at<double>(1, 0) / coord2.at<double>(2, 0);

	cv::Point p1 = cv::Point(x1 + origin_points[0].x, y1 + origin_points[0].y);
	cv::Point p2 = cv::Point(x2 + origin_points[0].x, y2 + origin_points[0].y);

	cv::line(img, p1, p2, cv::Scalar(255, 0, 255), 2, cv::LINE_8);

	cv::namedWindow(name, 1);
	cv::imshow(name, img);

	cv::waitKey(0);

	return 0;
}