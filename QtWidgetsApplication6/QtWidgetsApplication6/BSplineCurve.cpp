#include "BSplineCurve.h"

void BSplineCurve::update() {
    //清空原值
    knots.clear();
    weights.clear();
    points.clear();

    pSize = controlPoints.size();
    kSize = pSize + degree + 1;

    if (pSize > degree) {

        //更新权重，个数同控制点个数
        //weights.resize(pSize);
        for (int i = 0; i < pSize; i++) {
            weights.push_back(1.0);
        }

        //************************创建节点************************

        //（1）开b样条曲线
        //double distance = 1.0 / (kSize - 1.0);
        ////更新控制节点，个数同次数+控制点个数+1
        ////单数
        ////knots.resize(kSize);
        //knots.push_back(0.0);//首为0.0
        //for (int i = 0; i < kSize - 2; i++) {
        //    knots.push_back(distance * (i + 1));
        //}
        //knots.push_back(1.0);//尾为1.0

        //----------------------------------------------------

        //（2）Clamped B样条
        double distance = 1.0 / (kSize - 2.0 * (degree + 1.0) + 1.0);
        for (int i = 0; i < degree + 1; i++) {
            knots.push_back(0.0);
        }
        for (int i = 0; i < kSize - 2 * (degree + 1); i++) {
            knots.push_back(distance * (i + 1));
        }
        for (int i = 0; i < degree + 1; i++) {
            knots.push_back(1.0);
        }

        //************************绘制节点************************

        Curve curve(degree, controlPoints, knots, weights);

        int numPoints = 500; // 曲线上的点的数量
        std::vector<std::vector<double>> paramPoints;

        //（1）开B样条

        //double deltaU = (1.0 - distance * degree * 2.0) / numPoints;

        //for (int i = 0; i < numPoints; i++) {
        //    double u = i * deltaU + distance * degree;
        //    std::vector<double> param = { u }; // 参数点只有一个u值
        //    paramPoints.push_back(param);
        //}

        //----------------------------------------------------

        //（2）clamped
        double deltaU = 1.0 / numPoints;//间隔
        for (int i = 0; i < numPoints - 1; i++) {
            double u = i * deltaU;
            std::vector<double> param = { u }; // 参数点只有一个u值
            paramPoints.push_back(param);
        }
        paramPoints.push_back({ 1.0 });

        //************************得到曲线************************

        //std::vector<Point> points;
        curve.getPointAtParam(paramPoints, points);

    }
}

//get函数----------------------------------

int BSplineCurve::getDegree() {
    return this->degree;
}

std::vector<double> BSplineCurve::getKnots() {
    return this->knots;
}

std::vector<Point> BSplineCurve::getControlPoints() {
    return  this->controlPoints;
}

std::vector<Point> BSplineCurve::getPoints() {
    return  this->points;
}

//得到某参数位置曲线导数
std::vector<double> BSplineCurve::getDerivative(std::vector<double> u) {
    Curve curve(degree, controlPoints, knots, weights);

    std::vector<std::vector<double>> basisValue; // 基函数和以及导数
    curve.evalDers(u, basisValue);

    std::vector<int> index;// 对求导参数位置有影响的基函数索引
    curve.getConnectIndex(u, index);

    // 求累加得到最终一阶导数值
    std::vector<double> derivative(3, 0.0);
    for (int i = 0; i < degree + 1; i++) {
        derivative[0] += controlPoints[index[i]][0] * basisValue[1][i];
        derivative[1] += controlPoints[index[i]][1] * basisValue[1][i];
        derivative[2] += controlPoints[index[i]][2] * basisValue[1][i];
    }
    return derivative;
}

//得到某参数位置曲线点的坐标
Point BSplineCurve::getPoint(std::vector<double> u) {
    Curve curve(degree, controlPoints, knots, weights);
    return curve.getPointAtParam(u);
}

//找到参数 u[0] 附近会影响曲线形状的基函数的索引
std::vector<int> BSplineCurve::getIndex(std::vector<double> u) {
    Curve curve(degree, controlPoints, knots, weights);
    std::vector<int> index;// 索引值
    curve.getConnectIndex(u, index);
    return index;
}

//得到某参数位置曲线二阶导数
std::vector<double> BSplineCurve::getSecDerivative(std::vector<double> u) {
    Curve curve(degree, controlPoints, knots, weights);

    std::vector<double> basisValue; // 基函数和以及导数
    curve.evalSecDers(u, basisValue);

    std::vector<int> index;// 对求导参数位置有影响的基函数索引
    curve.getConnectIndex(u, index);

    // 求累加得到最终一阶导数值
    std::vector<double> derivative(3, 0.0);
    for (int i = 0; i < degree + 1; i++) {
        derivative[0] += controlPoints[index[i]][0] * basisValue[i];
        derivative[1] += controlPoints[index[i]][1] * basisValue[i];
        derivative[2] += controlPoints[index[i]][2] * basisValue[i];
    }
    return derivative;
}

//求拐点参数位置（单纯求出两拐点端点和参数中点位置的角度）
//void BSplineCurve::getInflectionPoint(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange) {
//
//    inflectionPointU.clear();
//    angleRange.clear();
//
//    Curve curve(degree, controlPoints, knots, weights);
//
//    double numPoints = 1000.0;
//    std::vector<std::vector<double>> paramPoints;
//    double deltaU = 1.0 / numPoints;//间隔
//    for (int i = 0; i < numPoints; i++) {
//        double u = i * deltaU;
//        std::vector<double> param = { u }; // 参数点只有一个u值
//        paramPoints.push_back(param);
//    }
//
//    double errorThreshold = 0.15; // 误差阈值
//    std::vector<double> derivative;
//    std::vector<double> secDerivative;
//
//    //std::vector<Point> tempP;//暂时存储拐点坐标数组
//    std::vector<std::vector<double>> tempU;//暂时存储拐点参数值数组
//    int i = 0;//有效的拐点个数为i+1
//
//    //遍历0.0到1.0参数值
//    for (const std::vector<double>& u : paramPoints) {
//        derivative = this->getDerivative(u);
//        secDerivative = this->getSecDerivative(u);
//
//        //判断是否为拐点
//        if (std::abs((derivative[0] * secDerivative[1] + derivative[1] * secDerivative[0])
//            / sqrt(derivative[0] * derivative[0] + derivative[1] * derivative[1])
//            / sqrt(secDerivative[0] * secDerivative[0] + secDerivative[1] * secDerivative[1])) < errorThreshold) {
//
//            //某一凹凸性变化位置第一个拐点
//            if (tempU.empty()) {
//                tempU.push_back(std::vector<double>(1, u[0]));
//            }
//            else {
//                int i = tempU.size() - 1; //当前 tempU 的索引
//
//                //某一凹凸性变化位置其他等价拐点
//                if (std::abs(u[0] - tempU[i].back()) < 2 * deltaU) {
//                    tempU[i].push_back(u[0]);
//                }
//
//                //出现另一凹凸性变化位置第一个拐点
//                else {
//                    tempU.push_back(std::vector<double>(1, u[0]));
//                }
//            }
//        }
//
//    }
//
//    std::vector<double> parameterArray = { 1e-15 };//用来后续求角度的参数数组,值为0.0,inflectionPointU,1.0
//    //取中
//    for (const std::vector<double> uArray : tempU) {
//        int size = uArray.size();
//        std::vector<double> u = { uArray[size / 2] };
//        inflectionPointU.push_back(u[0]);
//        parameterArray.push_back(u[0]);
//        //tempP.push_back(ui.openGLWidget->curve[curveIndex]->getPoint(u));
//    }
//    parameterArray.push_back(1.0);
//
//    //求参数0.0，到每个拐点，到1.0位置的角度范围(parameterArray)---------------------------------------------------------
//    angleRange.resize(parameterArray.size() - 1);
//
//    if (parameterArray.size() > 1) {
//
//        for (int i = 1; i < parameterArray.size(); i++) {
//
//            double pi = 3.141592653589793;
//            // 第一个拐点的切线
//            std::vector<double> derivativeA = this->getDerivative({ parameterArray[i - 1] });
//            // 两个拐点中点切线
//            std::vector<double> derivativeH = this->getDerivative({ (parameterArray[i - 1] + parameterArray[i]) / 2 });
//            // 第二个拐点的切线
//            std::vector<double> derivativeB = this->getDerivative({ parameterArray[i] });
//            //x轴
//            std::vector<double> derivativeO = { 1.0,0.0 };
//
//            //std::vector<double> angle;//存储角度的向量
//            //angle.resize(3);//分配内存
//            
//            //将弧度转化为角度
//            angleRange[i - 1].push_back(acos(derivativeA[0]                                                  //(derivativeA[0] * derivativeO[0] + derivativeA[1] * derivativeO[1]
//                / sqrt(derivativeA[0] * derivativeA[0] + derivativeA[1] * derivativeA[1]))  //已知 |derivativeO| = 1.0
//                * 180 / pi);
//            angleRange[i - 1].push_back(acos(derivativeH[0]
//                / sqrt(derivativeH[0] * derivativeH[0] + derivativeH[1] * derivativeH[1]))
//                * 180 / pi);
//            angleRange[i - 1].push_back(acos(derivativeB[0]
//                / sqrt(derivativeB[0] * derivativeB[0] + derivativeB[1] * derivativeB[1]))
//                * 180 / pi);
//
//            //调整角度范围为0到360
//            if (derivativeA[0] > 0 && derivativeA[1] < 0 || derivativeA[0] < 0 && derivativeA[1] < 0) {
//                angleRange[i - 1][0] = 360.0 - angleRange[i - 1][0];
//            }
//            if (derivativeH[0] > 0 && derivativeH[1] < 0 || derivativeH[0] < 0 && derivativeH[1] < 0) {
//                angleRange[i - 1][1] = 360.0 - angleRange[i - 1][1];
//            }
//            if (derivativeB[0] > 0 && derivativeB[1] < 0 || derivativeB[0] < 0 && derivativeB[1] < 0) {
//                angleRange[i - 1][2] = 360.0 - angleRange[i - 1][2];
//            }
//
//        }
//
//    }
//
//}

//求拐点参数位置（保证角度区间为逆时针区间，同时标记是否经过360）
void BSplineCurve::getInflectionPoint(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange) {

    inflectionPointU.clear();
    angleRange.clear();

    Curve curve(degree, controlPoints, knots, weights);

    double numPoints = 1000.0;
    std::vector<std::vector<double>> paramPoints;
    double deltaU = 1.0 / numPoints;//间隔
    for (int i = 0; i < numPoints; i++) {
        double u = i * deltaU;
        std::vector<double> param = { u }; // 参数点只有一个u值
        paramPoints.push_back(param);
    }

    double errorThreshold = 0.15; // 误差阈值
    std::vector<double> derivative;
    std::vector<double> secDerivative;

    //std::vector<Point> tempP;//暂时存储拐点坐标数组
    std::vector<std::vector<double>> tempU;//暂时存储拐点参数值数组
    int i = 0;//有效的拐点个数为i+1

    //遍历0.0到1.0参数值
    for (const std::vector<double>& u : paramPoints) {
        derivative = this->getDerivative(u);
        secDerivative = this->getSecDerivative(u);

        //判断是否为拐点
        if (std::abs((derivative[0] * secDerivative[1] + derivative[1] * secDerivative[0])
            / sqrt(derivative[0] * derivative[0] + derivative[1] * derivative[1])
            / sqrt(secDerivative[0] * secDerivative[0] + secDerivative[1] * secDerivative[1])) < errorThreshold) {

            //某一凹凸性变化位置第一个拐点
            if (tempU.empty()) {
                tempU.push_back(std::vector<double>(1, u[0]));
            }
            else {
                int i = tempU.size() - 1; //当前 tempU 的索引

                //某一凹凸性变化位置其他等价拐点
                if (std::abs(u[0] - tempU[i].back()) < 2 * deltaU) {
                    tempU[i].push_back(u[0]);
                }

                //出现另一凹凸性变化位置第一个拐点
                else {
                    tempU.push_back(std::vector<double>(1, u[0]));
                }
            }
        }

    }

    std::vector<double> parameterArray = { 1e-15 };//用来后续求角度的参数数组,值为0.0,inflectionPointU,1.0
    //取中
    for (const std::vector<double> uArray : tempU) {
        int size = uArray.size();
        std::vector<double> u = { uArray[size / 2] };
        inflectionPointU.push_back(u[0]);
        parameterArray.push_back(u[0]);
        //tempP.push_back(ui.openGLWidget->curve[curveIndex]->getPoint(u));
    }
    parameterArray.push_back(1.0);

    //求参数0.0，到每个拐点，到1.0位置的角度范围(parameterArray)---------------------------------------------------------
    angleRange.resize(parameterArray.size() - 1);

    if (parameterArray.size() > 1) {

        //double range;//记录是否超出180度

        for (int i = 1; i < parameterArray.size(); i++) {

            double flag = 0.0;//标记位，标记是否经过360，默认没有

            double pi = 3.141592653589793;
            // 第一个拐点的切线
            std::vector<double> derivativeA = this->getDerivative({ parameterArray[i - 1] });
            // 两个拐点中点切线
            std::vector<double> derivativeH = this->getDerivative({ (parameterArray[i - 1] + parameterArray[i]) / 2 });
            // 第二个拐点的切线
            std::vector<double> derivativeB = this->getDerivative({ parameterArray[i] });

            std::vector<double> angle;//存储角度的向量
            angle.push_back(acos(derivativeA[0]                                                  //(derivativeA[0] * derivativeO[0] + derivativeA[1] * derivativeO[1]
                / sqrt(derivativeA[0] * derivativeA[0] + derivativeA[1] * derivativeA[1]))  //已知 |derivativeO| = 1.0
                * 180 / pi);
            angle.push_back(acos(derivativeH[0]
                / sqrt(derivativeH[0] * derivativeH[0] + derivativeH[1] * derivativeH[1]))
                * 180 / pi);
            angle.push_back(acos(derivativeB[0]
                / sqrt(derivativeB[0] * derivativeB[0] + derivativeB[1] * derivativeB[1]))
                * 180 / pi);

            //调整角度范围为0到360
            if (derivativeA[0] > 0 && derivativeA[1] < 0 || derivativeA[0] < 0 && derivativeA[1] < 0) {
                angle[0] = 360.0 - angle[0];
            }
            if (derivativeH[0] > 0 && derivativeH[1] < 0 || derivativeH[0] < 0 && derivativeH[1] < 0) {
                angle[1] = 360.0 - angle[1];
            }
            if (derivativeB[0] > 0 && derivativeB[1] < 0 || derivativeB[0] < 0 && derivativeB[1] < 0) {
                angle[2] = 360.0 - angle[2];
            }

            //角度区间经过360
			if ((angle[1] - angle[0]) * (angle[2] - angle[1]) < 0) {
				flag = 1.0;//标记经过360度
				std::vector<double> derivativeC = this->getDerivative({ (3.0 * parameterArray[i - 1] + parameterArray[i]) / 2.0 });
				angle.push_back(acos(derivativeC[0]
					/ sqrt(derivativeC[0] * derivativeC[0] + derivativeC[1] * derivativeC[1]))
					* 180 / pi);
				if (derivativeC[0] > 0 && derivativeC[1] < 0 || derivativeC[0] < 0 && derivativeC[1] < 0) {
                    angle[3] = 360.0 - angle[3];
                }
                if ((angle[2] - angle[1]) * (angle[1] - angle[3]) * (angle[3] - angle[0]) < 0) {
                    double tempAngle = angle[0];
                    angle[0] = angle[2];
                    angle[2] = tempAngle;
                }
                //range = angle[2] + 360.0 - angle[0];
            }
            //角度区间不经过360
            else {
                flag = 0.0;
                //保证逆时针顺序
                if (angle[2] < angle[0]) {
                    double tempAngle = angle[0];
                    angle[0] = angle[2];
                    angle[2] = tempAngle;
                }
                //range = angle[2] - angle[0];
            }
            //每结束一次

            angleRange[i - 1] = { angle[0],angle[2],flag };

        }

    }

}

//求c型曲线和其角度范围
void BSplineCurve::getCShapedCurve(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange) {

    inflectionPointU.clear();
    angleRange.clear();

    Curve curve(degree, controlPoints, knots, weights);

    double numPoints = 1000.0;
    std::vector<std::vector<double>> paramPoints;
    double deltaU = 1.0 / numPoints;//间隔
    for (int i = 0; i < numPoints; i++) {
        double u = i * deltaU;
        std::vector<double> param = { u }; // 参数点只有一个u值
        paramPoints.push_back(param);
    }

    double errorThreshold = 0.15; // 误差阈值
    std::vector<double> derivative;
    std::vector<double> secDerivative;

    std::vector<std::vector<double>> tempU;//暂时存储拐点参数值数组
    //int i = 0;//有效的拐点个数为i+1

    //遍历0.0到1.0参数值
    for (const std::vector<double>& u : paramPoints) {
        derivative = this->getDerivative(u);
        secDerivative = this->getSecDerivative(u);

        //判断是否为拐点
        if (std::abs((derivative[0] * secDerivative[1] + derivative[1] * secDerivative[0])
            / sqrt(derivative[0] * derivative[0] + derivative[1] * derivative[1])
            / sqrt(secDerivative[0] * secDerivative[0] + secDerivative[1] * secDerivative[1])) < errorThreshold) {

            //某一凹凸性变化位置第一个拐点
            if (tempU.empty()) {
                tempU.push_back(std::vector<double>(1, u[0]));
            }
            else {
                int i = tempU.size() - 1; //当前 tempU 的索引

                //某一凹凸性变化位置其他等价拐点
                if (std::abs(u[0] - tempU[i].back()) < 2 * deltaU) {
                    tempU[i].push_back(u[0]);
                }

                //出现另一凹凸性变化位置第一个拐点
                else {
                    tempU.push_back(std::vector<double>(1, u[0]));
                }
            }
        }

    }
    tempU.push_back(std::vector<double>(1, 1.0));

    double pi = 3.141592653589793;//pi
    std::vector<double> derivativeO = { 1.0,0.0 };//x轴

    inflectionPointU = { 1e-15 };//用来后续求角度的参数数组,值为0.0,inflectionPointU,1.0
    //取中
    for (const std::vector<double> uArray : tempU) {
        int size = uArray.size();
        std::vector<double> u = { uArray[size / 2] };

        double range;//记录是否超出180度
        double flag = 0.0;//标记位，标记是否经过360，默认没有
        
        //循环
        while (!u.empty()) {

            // 第一个拐点的切线
            std::vector<double> derivativeA = this->getDerivative({ inflectionPointU.back() });
            // 两个拐点中点切线
            std::vector<double> derivativeH = this->getDerivative({ (inflectionPointU.back() + u.back()) / 2.0 });
            // 第二个拐点的切线
            std::vector<double> derivativeB = this->getDerivative({ u.back()});
            //x轴

            std::vector<double> angle;
            angle.push_back(acos(derivativeA[0]                                                  //(derivativeA[0] * derivativeO[0] + derivativeA[1] * derivativeO[1]
                / sqrt(derivativeA[0] * derivativeA[0] + derivativeA[1] * derivativeA[1]))  //已知 |derivativeO| = 1.0
                * 180 / pi);
            angle.push_back(acos(derivativeH[0]
                / sqrt(derivativeH[0] * derivativeH[0] + derivativeH[1] * derivativeH[1]))
                * 180 / pi);
            angle.push_back(acos(derivativeB[0]
                / sqrt(derivativeB[0] * derivativeB[0] + derivativeB[1] * derivativeB[1]))
                * 180 / pi);

            //调整角度范围为0到360
            if (derivativeA[0] > 0 && derivativeA[1] < 0 || derivativeA[0] < 0 && derivativeA[1] < 0) {
                angle[0] = 360.0 - angle[0];
            }
            if (derivativeH[0] > 0 && derivativeH[1] < 0 || derivativeH[0] < 0 && derivativeH[1] < 0) {
                angle[1] = 360.0 - angle[1];
            }
            if (derivativeB[0] > 0 && derivativeB[1] < 0 || derivativeB[0] < 0 && derivativeB[1] < 0) {
                angle[2] = 360.0 - angle[2];
            }

            //角度区间经过360
            if ((angle[1] - angle[0]) * (angle[2] - angle[1]) < 0) {
                flag = 1.0;//标记经过360度
                std::vector<double> derivativeC = this->getDerivative({ (3.0 * inflectionPointU.back() + u.back()) / 2.0 });
                angle.push_back(acos(derivativeC[0]
                    / sqrt(derivativeC[0] * derivativeC[0] + derivativeC[1] * derivativeC[1]))
                    * 180 / pi);
                if (derivativeC[0] > 0 && derivativeC[1] < 0 || derivativeC[0] < 0 && derivativeC[1] < 0) {
                    angle[3] = 360.0 - angle[3];
                }
                if ((angle[2] - angle[1]) * (angle[1] - angle[3]) * (angle[3] - angle[0]) < 0) {
                    double tempAngle = angle[0];
                    angle[0] = angle[2];
                    angle[2] = tempAngle;
                }
                range = angle[2] + 360.0 - angle[0];
            }
            //角度区间不经过360
            else {
                flag = 0.0;
                //保证逆时针顺序
                if (angle[2] < angle[0]) {
                    double tempAngle = angle[0];
                    angle[0] = angle[2];
                    angle[2] = tempAngle;
                }
                range = angle[2] - angle[0];
            }
            //每结束一次
            if (range >= 180) {
                u.push_back((inflectionPointU.back() + u.back()) / 2.0);
            }
            else {
                inflectionPointU.push_back(u.back());
                u.pop_back();
                angleRange.push_back(std::vector<double>({angle[0],angle[2],flag}));
            }

        }
        
    }

}


//set函数----------------------------------

void BSplineCurve::setDegree(int degree) {
    this->degree = degree;
}

//add函数----------------------------------

void BSplineCurve::addControlPoint(Point point) {
    this->controlPoints.push_back(point);
}

//clear函数---------------------------------

void BSplineCurve::clearControlPoints() {
    this->controlPoints.clear();
}

//available函数----------------------------

bool BSplineCurve::isCurveAvailable() {
    return this->pSize > this->degree;
}

//=====================================BezierExtraction=====================================

/**
 * \brief 根据给定的节点矢量和次数计算B-样条的bezier提取算子
 *
 * \param knot 节点矢量
 * \param p B-样条的次数
 * \param C 输出参数，用于存储bezier提取算子
 * \param nb 输出参数，用于存储bezier提取算子数目
 */
void BSplineCurve::bezierExtraction(std::vector<std::vector<std::vector<double>>>& C, int& nb) {

    int m = knots.size() - degree - 1;
    int a = degree + 1;
    int b = a + 1;
    nb = 1;

    std::vector<std::vector<double>> identityMatrix(degree + 1, std::vector<double>(degree + 1, 0.0));
    for (int i = 0; i < degree + 1; i++) {
        identityMatrix[i][i] = 1.0;
    }
    C.push_back(identityMatrix);

    while (b <= m) {

        C.push_back(identityMatrix);
        int i = b;
        while (b <= m && knots[b] == knots[b - 1]) {
            b++;
        }

        int multiplicity = b - i + 1;
        if (multiplicity < degree) {
            double numerator = knots[b - 1] - knots[a - 1];
            std::vector<double> alphas(degree - multiplicity);
            for (int j = degree; j >= multiplicity + 1; j--) {
                alphas[j - multiplicity - 1] = numerator / (knots[a + j - 1] - knots[a - 1]);
            }
            int r = degree - multiplicity;
            for (int j = 1; j <= r; j++) {
                int save = r - j + 1;
                int s = multiplicity + j;
                for (int k = degree + 1; k >= s + 1; k--) {
                    double alpha = alphas[k - s - 1];
                    for (int row = 0; row < degree + 1; row++) {
                        C[nb - 1][row][k - 1] = alpha * C[nb - 1][row][k - 1] + (1 - alpha) * C[nb - 1][row][k - 2];
                    }
                }
                if (b <= m) {
                    for (int row = 0; row <= j; row++) {
                        C[nb][save - 1 + row][save - 1] = C[nb - 1][degree - j + row][degree];
                    }
                }
            }
            nb++;
            if (b <= m) {
                a = b;
                b++;
            }
        }
        else if (multiplicity == degree) {
            if (b <= m) {
                nb++;
                a = b;
                b++;
            }
        }
    }
}

/**
 * \brief 根据B样条控制点、次数和节点矢量计算Bezier控制点
 *
 * \param knot 节点矢量
 * \param p B-样条的次数
 * \param cpx B-样条的控制点集
 * \param cpbsA 输出参数，用于存储Bezier曲线的控制点
 * \param intervals 输出参数，用于存储Bezier曲线的区间范围
 */
void BSplineCurve::bezierExtractCpts() {

    cpbsA.clear();
    intervals.clear();

    //uknot = unique(knot);
    std::vector<double> uknot = { knots[0] };
    for (int i = 1; i < knots.size(); i++) {
        if (knots[i] != uknot.back()) {
            uknot.push_back(knots[i]);
        }
    }

    std::vector<std::vector<std::vector<double>>> cbs;
    int nb;
    bezierExtraction(cbs, nb);

    int n = knots.size() - degree - 2;
    int ptdim = controlPoints[0].size();//点维度


    for (int j = 0; j < uknot.size() - 1; ++j) {

        cpbsA.push_back(std::vector<std::vector<double>>(degree + 1, std::vector<double>(ptdim, 0.0)));

        std::vector<int> indexTemp;
        std::vector<int> bi;
        for (int i = 0; i < uknot.size(); i++) {
            indexTemp = this->getIndex({ uknot[i] });
            bi.push_back(indexTemp[0]+this->degree);
        }

        std::vector<std::vector<double>> cpn;
        for (int i = bi[j] - degree; i <= bi[j]; i++) {
            cpn.push_back(controlPoints[i]);
        }

        //cpbsA(:,:,j) = (cbs(:,:,j))'*cpn;
        for (int x = 0; x < cpbsA[0].size(); x++) {
            for (int y = 0; y < cpbsA[0][0].size(); y++) {
                for (int z = 0; z < cpn.size(); z++) {
                    cpbsA[j][x][y] += cbs[j][z][x] * cpn[z][y];//cbs[j][x][z]转置结果cbs[j][z][x]
                }
            }
        }

        intervals.push_back({ uknot[j], uknot[j + 1] });
    }
}

std::vector<std::vector<Point>> BSplineCurve::getCpbsA(){
    return this->cpbsA;
}

std::vector<std::vector<double>> BSplineCurve::getIntervals() {
    return this->intervals;
}