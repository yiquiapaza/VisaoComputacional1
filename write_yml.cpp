// WriteAndRead.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <time.h>

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core/persistence.hpp>

#pragma warning(disable : 4996)

std::vector <cv::Point2d> points;

void callBackFunction(int event, int x, int y, int flangs, void* userdata);

int main()
{
	std::string name;
	std::cin >> name;	
	std::string dir_img = "D:/OpenCV/visao_trabalho1/" + name + ".jpg";
	std::string dir_yml = "D:/" + name + ".yml";

	cv::Mat image = cv::imread(dir_img);	
	
	if (image.empty()) 
	{
		std::cout << "Cant open or find Imagen" << std::endl;
		return -1;
	}

	cv::namedWindow(name, 1);
	cv::setMouseCallback(name, callBackFunction, NULL);
	cv::imshow(name, image);
	std::cout << "There are our Points" << std::endl;
	cv::waitKey(0);

	cv::FileStorage fs(dir_yml, cv::FileStorage::WRITE);

	time_t rawtime; time(&rawtime);

	fs << "calibrationDate" << asctime(localtime(&rawtime));
	fs << "nameImage" << name;
	fs << "nameDirImage" << dir_img;
	fs << "nameDirYaml" << dir_yml;

	auto num = points[0];

	fs << "origin" << "[";
	for (cv::Point2d point : points)
	{
		fs << "{:" << "x" << point.x << "y" << point.y << "}";
	}	
	fs << "]";

	fs << "points2d" << "[";
	for (cv::Point2d point : points) {
		fs << "{:" << "x" << point.x - num.x << "y" << point.y - num.y << "}";
	}
	fs << "]";

	fs << "points3d" << "[";
	
	// First points option for first image
	/*
	fs << "{:" << "x" << 0.0 << "y" << 0.0 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 7.32 << "y" << 0.0 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 12.82 << "y" << 0.0 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 12.82 << "y" << -5.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -5.5 << "y" << -5.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -5.5 << "y" << 0.0 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 0.0 << "y" << 0.0 << "z" << 2.44 << "}";
	fs << "{:" << "x" << 7.32 << "y" << 0.0 << "z" << 2.44 << "}";
	*/

	// Second points option for firts image
	/*
	fs << "{:" << "x" << 0 << "y" << 0 << "z" << 0 << "}";
	fs << "{:" << "x" << 0 << "y" << 0 << "z" << 2.44 << "}";
	fs << "{:" << "x" << 7.32 << "y" << 0 << "z" << 0 << "}";
	fs << "{:" << "x" << 12.82 << "y" << 5.5 << "z" << 0 << "}";
	fs << "{:" << "x" << 12.82 << "y" << 0 << "z" << 0 << "}";
	fs << "{:" << "x" << 23.82 << "y" << 0 << "z" << 0 << "}";
	*/
	
	// Second Imagen
	
	fs << "{:" << "x" << 0.0 << "y" << 0.0 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 0.0 << "y" << 1 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -24.84 << "y" << 0 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -24.84 << "y" << 16.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -35.84 << "y" << 0 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -35.84 << "y" << 5.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -54.16 << "y" << 5.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -65.16 << "y" << 16.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -45 << "y" << 11 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -1 << "y" << 0 << "z" << 0.0 << "}";
	
	/*
	fs << "{:" << "x" << 0.0 << "y" << 0.0 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -9.16 << "y" << -4.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 9.16 << "y" << -4.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 9.16 << "y" << -11 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 20.16 << "y" << -11 << "z" << 0.0 << "}";
	fs << "{:" << "x" << 20.16 << "y" << 5.5 << "z" << 0.0 << "}";
	fs << "{:" << "x" << -20.16 << "y" << 5.5 << "z" << 0.0 << "}";
	*/
	fs << "]";

	std::cout << points << std::endl;

	fs.release();
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

void callBackFunction(int event, int x, int y, int flangs, void* userdata)
{	
	if (points.size() < 10 ) 
	{
		if (event == cv::EVENT_LBUTTONDOWN) 
		{
			points.push_back(cv::Point2d(x, y));
			std::cout << "[" << x << "," << y << "]" << std::endl;
		}
	}
}
