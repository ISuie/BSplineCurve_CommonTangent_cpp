#pragma once

//#include <QString>

#include "Curve.h"

class BSplineCurve
{
public:
    //更新所有b样条曲线参数
    void update();

    //get函数
    int getDegree();//得到次数
    std::vector<double> getKnots();//得到节点向量
    std::vector<Point> getControlPoints();//得到控制顶点
    std::vector<Point> getPoints();//得到曲线上点的坐标
    std::vector<double> getDerivative(std::vector<double> u);//得到某参数位置曲线导数
    Point getPoint(std::vector<double> u);//得到某参数位置曲线点的坐标
    std::vector<int> getIndex(std::vector<double> u);//找到参数 u[0] 附近会影响曲线形状的基函数的索引
    std::vector<double> getSecDerivative(std::vector<double> u);//得到某参数位置曲线二阶导数
    void getInflectionPoint(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange);//求拐点参数位置和其角度范围
    void getCShapedCurve(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange);//求c型曲线和其角度范围

    //set函数
    void setDegree(int degree);
    //add函数
    void addControlPoint(Point point);

    //clear函数
    void clearControlPoints();

    //available函数
    bool isCurveAvailable();

    //Bezier Extraction
    void bezierExtraction(std::vector<std::vector<std::vector<double>>>& C, int& nb);
    void bezierExtractCpts();
    std::vector<std::vector<Point>> getCpbsA();
    std::vector<std::vector<double>> getIntervals();

private:
    //using Point = std::vector<double>;
    std::vector<Point> controlPoints;// 控制点
    std::vector<Point> points;       // 曲线上点的坐标
    std::vector<double> knots;       // 控制节点
    std::vector<double> weights;     // 权重

    int degree = 3;// 曲线次数
    int pSize;     // 控制点个数
    int kSize;     // 控制节点长度

    //Bezier Extraction
    std::vector<std::vector<Point>> cpbsA;
    std::vector<std::vector<double>> intervals;

};

