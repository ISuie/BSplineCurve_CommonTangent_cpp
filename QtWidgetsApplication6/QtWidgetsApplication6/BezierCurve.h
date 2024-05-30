#pragma once

#include <iostream>
#include <vector>
using namespace std;
using Point = std::vector<double>;

class BezierCurve
{
public:
	//���캯��
	BezierCurve();
	BezierCurve::BezierCurve(std::vector<Point> c);

	//�������
	double B(int n, int i, float u);

	//������Bezier���߲���
	void update();

	//������в���
	void clearBezierCurve();
	//�Ƿ��Ѿ����Ƴ�bezier����
	//bool isCurveAvailable();

	//get����
	std::vector<Point> getControlPoints();//�õ����ƶ���
	std::vector<Point> getPoints();//�õ������ϵ������
	Point getPoint(double u);//�õ�ĳ����λ��bezier�����ϵĵ�����
	Point getDerivative(double u);//��ĳ����λ�õ���
	Point getDerT(double u);//��ĳ����λ�õ�����ת��

	//set����
	void setBezierControlPoints(std::vector<Point> bezierControlPoints);
	void setIntervals(std::vector<double> intervals);

	//extract
	void extract(std::vector<Point>& control_1, std::vector<Point>& control_2, double t);

private:
	std::vector<Point> bezierPoints;//������500����
	std::vector<Point> bezierControlPoints;//���ƶ���
	std::vector<double> intervals;//������Ƕ���B-Spline�ģ�bezier�ò���

};



