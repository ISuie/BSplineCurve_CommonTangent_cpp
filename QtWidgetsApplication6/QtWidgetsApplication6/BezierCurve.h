#pragma once

#include <iostream>
#include <vector>
using namespace std;
using Point = std::vector<double>;

class BezierCurve
{
public:
	//构造函数
	BezierCurve();
	BezierCurve::BezierCurve(std::vector<Point> c);

	//求基函数
	double B(int n, int i, float u);

	//更新有Bezier曲线参数
	void update();

	//清空所有参数
	void clearBezierCurve();
	//是否已经绘制出bezier曲线
	//bool isCurveAvailable();

	//get函数
	std::vector<Point> getControlPoints();//得到控制顶点
	std::vector<Point> getPoints();//得到曲线上点的坐标
	Point getPoint(double u);//得到某参数位置bezier曲线上的点坐标
	Point getDerivative(double u);//求某参数位置导数
	Point getDerT(double u);//求某参数位置导数的转置

	//set函数
	void setBezierControlPoints(std::vector<Point> bezierControlPoints);
	void setIntervals(std::vector<double> intervals);

	//extract
	void extract(std::vector<Point>& control_1, std::vector<Point>& control_2, double t);

private:
	std::vector<Point> bezierPoints;//曲线上500个点
	std::vector<Point> bezierControlPoints;//控制定点
	std::vector<double> intervals;//间隔，是对于B-Spline的，bezier用不到

};



