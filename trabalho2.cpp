 // Maracana2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/persistence.hpp>

void callbackEvent(int event, int x, int y, int flaangs, void* userdata);

int main()
{
	std::vector<cv::Point2d> points_pixel;
	std::vector<cv::Point2d> points2D;
	std::vector<cv::Point3d> points3D;

	std::string name;
	std::cin >> name;
	std::string dir_img = "D:/OpenCV/visao_trabalho1/" + name + ".jpg";
	std::string dir_yml = "D:/" + name + ".yml";

	cv::Mat img = cv::imread(dir_img);
	if (img.empty())
	{
		std::cout << "Could not open ir find the image" << std::endl;
		return -1;
	}

	cv::FileStorage fs(dir_yml, cv::FileStorage::READ);

	cv::FileNode center = fs["origin"];
	cv::FileNodeIterator it_o = center.begin(), it_o_end = center.end();

	for (; it_o != it_o_end; it_o++)
	{
		points_pixel.push_back(cv::Point2d((*it_o)["x"], (*it_o)["y"]));
	}

	cv::FileNode points_2d = fs["points2d"];
	cv::FileNodeIterator it_p2d = points_2d.begin(), it_p2d_end = points_2d.end();

	for (; it_p2d != it_p2d_end; it_p2d++ )
	{
		points2D.push_back(cv::Point2d((*it_p2d)["x"], (*it_p2d)["y"]));
	}

	cv::FileNode points_3d = fs["points3d"];
	cv::FileNodeIterator it_p3d = points_3d.begin(), it_p3d_end = points_3d.end();

	for (; it_p3d != it_p3d_end ; it_p3d++ )
	{
		points3D.push_back(cv::Point3d((*it_p3d)["x"], (*it_p3d)["y"], (*it_p3d)["z"]));
	}

	fs.release();

	for (std::vector<cv::Point2d>::iterator it = points_pixel.begin(); it != points_pixel.end(); it++)
	{
		if (it != points_pixel.begin())
		{
			cv::circle(img, *it, 5, cv::Scalar(0, 0, 255), 2, cv::LINE_8);
		}
		else
		{
			cv::circle(img, *it, 5, cv::Scalar(0, 255, 0), 2, cv::LINE_8);
		}
	}

	cv::Mat_<double> P(16, 12);
	int k = 0;
	for (int i = 0, k = 0; i < P.rows ; i = 2 + i, k++ )
	{
		for (int j = 0; j < P.cols; j++)
		{
			switch (j)
			{
			case 0: P(i ,j) = points3D[k].x; break;
			case 1: P(i, j) = points3D[k].y; break;
			case 2: P(i, j) = points3D[k].z; break;
			case 3: P(i, j) = 1; break;
			case 4: P(i, j) = 0; break;
			case 5: P(i, j) = 0; break;
			case 6: P(i, j) = 0; break;
			case 7: P(i, j) = 0; break;
			case 8: P(i, j) = -points2D[k].x * points3D[k].x ; break;
			case 9: P(i, j) = -points2D[k].x * points3D[k].y; break;
			case 10: P(i, j) = -points2D[k].x * points3D[k].z; break;
			case 11: P(i, j) = -points2D[k].x; break;
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
			case 4: P(i, j) = points3D[f].x; break;
			case 5: P(i, j) = points3D[f].y; break;
			case 6: P(i, j) = points3D[f].z; break;
			case 7: P(i, j) = 1; break;
			case 8: P(i, j) = -points2D[f].y * points3D[f].x; break;
			case 9: P(i, j) = -points2D[f].y * points3D[f].y; break;
			case 10: P(i, j) = -points2D[f].y * points3D[f].z; break;
			case 11: P(i, j) = -points2D[f].y; break;
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
	
	//cv::Point x;
	
	//cv::setMouseCallback(name, callbackEvent, &x);
		
	cv::Mat_<double> point_cal(3, 1);
	point_cal.at<double>(0, 0) =  -100;
	point_cal.at<double>(1, 0) =  -100;
	point_cal.at<double>(2, 0) = 1;	

	cv::Mat_<double> o1 = calibration.t() * calibration;

	cv::Mat_<double> o5 = o1.inv(cv::DECOMP_SVD);
	
	std::cout << "1" << std::endl;
	std::cout << o1 << std::endl;
	
	cv::Mat_<double> o2 = calibration.t() * point_cal;

	std::cout << "2" << std::endl;
	std::cout << o2 << std::endl;

	cv::Mat_<double> o3 = o5 * o2;

	std::cout << "5" << std::endl;
	std::cout << o3 << std::endl;

	cv::Mat o = (calibration.t() * calibration).inv(cv::DECOMP_SVD) * calibration.t() * point_cal;

	double x0 = o.at<double>(0.0) / o.at<double>(3.0);
	double y0 = o.at<double>(1.0) / o.at<double>(3.0);
	double z0 = o.at<double>(2.0) / o.at<double>(3.0);

	std::cout << "x: " << x0 << " - " <<"y: " << y0 << "z: " << std::endl;

	
	cv::Mat_<double> first_point(4, 1);
	cv::Mat_<double> second_point(4, 1);
	
	first_point(0, 0) = 0; first_point(1, 0) = 0; first_point(2, 0) = 0; first_point(3, 0) = 1;
	second_point(0, 0) = 20; second_point(1, 0) = 0; second_point(2, 0) = 0; second_point(3, 0) = 1;
	
	cv::Mat_<double> out1 = calibration * first_point;
	cv::Mat_<double> out2 = calibration * second_point;
	
	double x1 = out1.at<double>(0, 0) / out1.at<double>(2, 0);
	double y1 = out1.at<double>(1, 0) / out1.at<double>(2, 0);

	double x2 = out2.at<double>(0, 0) / out2.at<double>(2, 0);
	double y2 = out2.at<double>(1, 0) / out2.at<double>(2, 0);

	cv::Point p1 = cv::Point(x1 , y1 );
	cv::Point p2 = cv::Point(x2 , y2 );

	std::cout << x0 << " " << y0 << " " << z0 << " " << std::endl;



	cv::line(img, p1, p2, cv::Scalar(255, 0, 255), 2, cv::LINE_8);
	
	cv::namedWindow(name, 1);
	cv::imshow(name, img);
	cv::waitKey(0);

	return 1;
}

void callbackEvent(int event, int x, int y, int flaangs, void* userdata)
{	
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		cv::Point* p = (cv::Point*)userdata;
		p->x = x;
		p->y = y;
		std::cout << "[" << x << "," << y << "]" << std::endl;
	}	
}
