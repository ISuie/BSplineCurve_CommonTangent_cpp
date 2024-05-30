#pragma once

//#include <QString>

#include "Curve.h"

class BSplineCurve
{
public:
    //��������b�������߲���
    void update();

    //get����
    int getDegree();//�õ�����
    std::vector<double> getKnots();//�õ��ڵ�����
    std::vector<Point> getControlPoints();//�õ����ƶ���
    std::vector<Point> getPoints();//�õ������ϵ������
    std::vector<double> getDerivative(std::vector<double> u);//�õ�ĳ����λ�����ߵ���
    Point getPoint(std::vector<double> u);//�õ�ĳ����λ�����ߵ������
    std::vector<int> getIndex(std::vector<double> u);//�ҵ����� u[0] ������Ӱ��������״�Ļ�����������
    std::vector<double> getSecDerivative(std::vector<double> u);//�õ�ĳ����λ�����߶��׵���
    void getInflectionPoint(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange);//��յ����λ�ú���Ƕȷ�Χ
    void getCShapedCurve(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange);//��c�����ߺ���Ƕȷ�Χ

    //set����
    void setDegree(int degree);
    //add����
    void addControlPoint(Point point);

    //clear����
    void clearControlPoints();

    //available����
    bool isCurveAvailable();

    //Bezier Extraction
    void bezierExtraction(std::vector<std::vector<std::vector<double>>>& C, int& nb);
    void bezierExtractCpts();
    std::vector<std::vector<Point>> getCpbsA();
    std::vector<std::vector<double>> getIntervals();

private:
    //using Point = std::vector<double>;
    std::vector<Point> controlPoints;// ���Ƶ�
    std::vector<Point> points;       // �����ϵ������
    std::vector<double> knots;       // ���ƽڵ�
    std::vector<double> weights;     // Ȩ��

    int degree = 3;// ���ߴ���
    int pSize;     // ���Ƶ����
    int kSize;     // ���ƽڵ㳤��

    //Bezier Extraction
    std::vector<std::vector<Point>> cpbsA;
    std::vector<std::vector<double>> intervals;

};

