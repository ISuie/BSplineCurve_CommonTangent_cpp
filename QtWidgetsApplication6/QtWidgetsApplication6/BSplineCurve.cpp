#include "BSplineCurve.h"

void BSplineCurve::update() {
    //���ԭֵ
    knots.clear();
    weights.clear();
    points.clear();

    pSize = controlPoints.size();
    kSize = pSize + degree + 1;

    if (pSize > degree) {

        //����Ȩ�أ�����ͬ���Ƶ����
        //weights.resize(pSize);
        for (int i = 0; i < pSize; i++) {
            weights.push_back(1.0);
        }

        //************************�����ڵ�************************

        //��1����b��������
        //double distance = 1.0 / (kSize - 1.0);
        ////���¿��ƽڵ㣬����ͬ����+���Ƶ����+1
        ////����
        ////knots.resize(kSize);
        //knots.push_back(0.0);//��Ϊ0.0
        //for (int i = 0; i < kSize - 2; i++) {
        //    knots.push_back(distance * (i + 1));
        //}
        //knots.push_back(1.0);//βΪ1.0

        //----------------------------------------------------

        //��2��Clamped B����
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

        //************************���ƽڵ�************************

        Curve curve(degree, controlPoints, knots, weights);

        int numPoints = 500; // �����ϵĵ������
        std::vector<std::vector<double>> paramPoints;

        //��1����B����

        //double deltaU = (1.0 - distance * degree * 2.0) / numPoints;

        //for (int i = 0; i < numPoints; i++) {
        //    double u = i * deltaU + distance * degree;
        //    std::vector<double> param = { u }; // ������ֻ��һ��uֵ
        //    paramPoints.push_back(param);
        //}

        //----------------------------------------------------

        //��2��clamped
        double deltaU = 1.0 / numPoints;//���
        for (int i = 0; i < numPoints - 1; i++) {
            double u = i * deltaU;
            std::vector<double> param = { u }; // ������ֻ��һ��uֵ
            paramPoints.push_back(param);
        }
        paramPoints.push_back({ 1.0 });

        //************************�õ�����************************

        //std::vector<Point> points;
        curve.getPointAtParam(paramPoints, points);

    }
}

//get����----------------------------------

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

//�õ�ĳ����λ�����ߵ���
std::vector<double> BSplineCurve::getDerivative(std::vector<double> u) {
    Curve curve(degree, controlPoints, knots, weights);

    std::vector<std::vector<double>> basisValue; // ���������Լ�����
    curve.evalDers(u, basisValue);

    std::vector<int> index;// ���󵼲���λ����Ӱ��Ļ���������
    curve.getConnectIndex(u, index);

    // ���ۼӵõ�����һ�׵���ֵ
    std::vector<double> derivative(3, 0.0);
    for (int i = 0; i < degree + 1; i++) {
        derivative[0] += controlPoints[index[i]][0] * basisValue[1][i];
        derivative[1] += controlPoints[index[i]][1] * basisValue[1][i];
        derivative[2] += controlPoints[index[i]][2] * basisValue[1][i];
    }
    return derivative;
}

//�õ�ĳ����λ�����ߵ������
Point BSplineCurve::getPoint(std::vector<double> u) {
    Curve curve(degree, controlPoints, knots, weights);
    return curve.getPointAtParam(u);
}

//�ҵ����� u[0] ������Ӱ��������״�Ļ�����������
std::vector<int> BSplineCurve::getIndex(std::vector<double> u) {
    Curve curve(degree, controlPoints, knots, weights);
    std::vector<int> index;// ����ֵ
    curve.getConnectIndex(u, index);
    return index;
}

//�õ�ĳ����λ�����߶��׵���
std::vector<double> BSplineCurve::getSecDerivative(std::vector<double> u) {
    Curve curve(degree, controlPoints, knots, weights);

    std::vector<double> basisValue; // ���������Լ�����
    curve.evalSecDers(u, basisValue);

    std::vector<int> index;// ���󵼲���λ����Ӱ��Ļ���������
    curve.getConnectIndex(u, index);

    // ���ۼӵõ�����һ�׵���ֵ
    std::vector<double> derivative(3, 0.0);
    for (int i = 0; i < degree + 1; i++) {
        derivative[0] += controlPoints[index[i]][0] * basisValue[i];
        derivative[1] += controlPoints[index[i]][1] * basisValue[i];
        derivative[2] += controlPoints[index[i]][2] * basisValue[i];
    }
    return derivative;
}

//��յ����λ�ã�����������յ�˵�Ͳ����е�λ�õĽǶȣ�
//void BSplineCurve::getInflectionPoint(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange) {
//
//    inflectionPointU.clear();
//    angleRange.clear();
//
//    Curve curve(degree, controlPoints, knots, weights);
//
//    double numPoints = 1000.0;
//    std::vector<std::vector<double>> paramPoints;
//    double deltaU = 1.0 / numPoints;//���
//    for (int i = 0; i < numPoints; i++) {
//        double u = i * deltaU;
//        std::vector<double> param = { u }; // ������ֻ��һ��uֵ
//        paramPoints.push_back(param);
//    }
//
//    double errorThreshold = 0.15; // �����ֵ
//    std::vector<double> derivative;
//    std::vector<double> secDerivative;
//
//    //std::vector<Point> tempP;//��ʱ�洢�յ���������
//    std::vector<std::vector<double>> tempU;//��ʱ�洢�յ����ֵ����
//    int i = 0;//��Ч�Ĺյ����Ϊi+1
//
//    //����0.0��1.0����ֵ
//    for (const std::vector<double>& u : paramPoints) {
//        derivative = this->getDerivative(u);
//        secDerivative = this->getSecDerivative(u);
//
//        //�ж��Ƿ�Ϊ�յ�
//        if (std::abs((derivative[0] * secDerivative[1] + derivative[1] * secDerivative[0])
//            / sqrt(derivative[0] * derivative[0] + derivative[1] * derivative[1])
//            / sqrt(secDerivative[0] * secDerivative[0] + secDerivative[1] * secDerivative[1])) < errorThreshold) {
//
//            //ĳһ��͹�Ա仯λ�õ�һ���յ�
//            if (tempU.empty()) {
//                tempU.push_back(std::vector<double>(1, u[0]));
//            }
//            else {
//                int i = tempU.size() - 1; //��ǰ tempU ������
//
//                //ĳһ��͹�Ա仯λ�������ȼ۹յ�
//                if (std::abs(u[0] - tempU[i].back()) < 2 * deltaU) {
//                    tempU[i].push_back(u[0]);
//                }
//
//                //������һ��͹�Ա仯λ�õ�һ���յ�
//                else {
//                    tempU.push_back(std::vector<double>(1, u[0]));
//                }
//            }
//        }
//
//    }
//
//    std::vector<double> parameterArray = { 1e-15 };//����������ǶȵĲ�������,ֵΪ0.0,inflectionPointU,1.0
//    //ȡ��
//    for (const std::vector<double> uArray : tempU) {
//        int size = uArray.size();
//        std::vector<double> u = { uArray[size / 2] };
//        inflectionPointU.push_back(u[0]);
//        parameterArray.push_back(u[0]);
//        //tempP.push_back(ui.openGLWidget->curve[curveIndex]->getPoint(u));
//    }
//    parameterArray.push_back(1.0);
//
//    //�����0.0����ÿ���յ㣬��1.0λ�õĽǶȷ�Χ(parameterArray)---------------------------------------------------------
//    angleRange.resize(parameterArray.size() - 1);
//
//    if (parameterArray.size() > 1) {
//
//        for (int i = 1; i < parameterArray.size(); i++) {
//
//            double pi = 3.141592653589793;
//            // ��һ���յ������
//            std::vector<double> derivativeA = this->getDerivative({ parameterArray[i - 1] });
//            // �����յ��е�����
//            std::vector<double> derivativeH = this->getDerivative({ (parameterArray[i - 1] + parameterArray[i]) / 2 });
//            // �ڶ����յ������
//            std::vector<double> derivativeB = this->getDerivative({ parameterArray[i] });
//            //x��
//            std::vector<double> derivativeO = { 1.0,0.0 };
//
//            //std::vector<double> angle;//�洢�Ƕȵ�����
//            //angle.resize(3);//�����ڴ�
//            
//            //������ת��Ϊ�Ƕ�
//            angleRange[i - 1].push_back(acos(derivativeA[0]                                                  //(derivativeA[0] * derivativeO[0] + derivativeA[1] * derivativeO[1]
//                / sqrt(derivativeA[0] * derivativeA[0] + derivativeA[1] * derivativeA[1]))  //��֪ |derivativeO| = 1.0
//                * 180 / pi);
//            angleRange[i - 1].push_back(acos(derivativeH[0]
//                / sqrt(derivativeH[0] * derivativeH[0] + derivativeH[1] * derivativeH[1]))
//                * 180 / pi);
//            angleRange[i - 1].push_back(acos(derivativeB[0]
//                / sqrt(derivativeB[0] * derivativeB[0] + derivativeB[1] * derivativeB[1]))
//                * 180 / pi);
//
//            //�����Ƕȷ�ΧΪ0��360
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

//��յ����λ�ã���֤�Ƕ�����Ϊ��ʱ�����䣬ͬʱ����Ƿ񾭹�360��
void BSplineCurve::getInflectionPoint(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange) {

    inflectionPointU.clear();
    angleRange.clear();

    Curve curve(degree, controlPoints, knots, weights);

    double numPoints = 1000.0;
    std::vector<std::vector<double>> paramPoints;
    double deltaU = 1.0 / numPoints;//���
    for (int i = 0; i < numPoints; i++) {
        double u = i * deltaU;
        std::vector<double> param = { u }; // ������ֻ��һ��uֵ
        paramPoints.push_back(param);
    }

    double errorThreshold = 0.15; // �����ֵ
    std::vector<double> derivative;
    std::vector<double> secDerivative;

    //std::vector<Point> tempP;//��ʱ�洢�յ���������
    std::vector<std::vector<double>> tempU;//��ʱ�洢�յ����ֵ����
    int i = 0;//��Ч�Ĺյ����Ϊi+1

    //����0.0��1.0����ֵ
    for (const std::vector<double>& u : paramPoints) {
        derivative = this->getDerivative(u);
        secDerivative = this->getSecDerivative(u);

        //�ж��Ƿ�Ϊ�յ�
        if (std::abs((derivative[0] * secDerivative[1] + derivative[1] * secDerivative[0])
            / sqrt(derivative[0] * derivative[0] + derivative[1] * derivative[1])
            / sqrt(secDerivative[0] * secDerivative[0] + secDerivative[1] * secDerivative[1])) < errorThreshold) {

            //ĳһ��͹�Ա仯λ�õ�һ���յ�
            if (tempU.empty()) {
                tempU.push_back(std::vector<double>(1, u[0]));
            }
            else {
                int i = tempU.size() - 1; //��ǰ tempU ������

                //ĳһ��͹�Ա仯λ�������ȼ۹յ�
                if (std::abs(u[0] - tempU[i].back()) < 2 * deltaU) {
                    tempU[i].push_back(u[0]);
                }

                //������һ��͹�Ա仯λ�õ�һ���յ�
                else {
                    tempU.push_back(std::vector<double>(1, u[0]));
                }
            }
        }

    }

    std::vector<double> parameterArray = { 1e-15 };//����������ǶȵĲ�������,ֵΪ0.0,inflectionPointU,1.0
    //ȡ��
    for (const std::vector<double> uArray : tempU) {
        int size = uArray.size();
        std::vector<double> u = { uArray[size / 2] };
        inflectionPointU.push_back(u[0]);
        parameterArray.push_back(u[0]);
        //tempP.push_back(ui.openGLWidget->curve[curveIndex]->getPoint(u));
    }
    parameterArray.push_back(1.0);

    //�����0.0����ÿ���յ㣬��1.0λ�õĽǶȷ�Χ(parameterArray)---------------------------------------------------------
    angleRange.resize(parameterArray.size() - 1);

    if (parameterArray.size() > 1) {

        //double range;//��¼�Ƿ񳬳�180��

        for (int i = 1; i < parameterArray.size(); i++) {

            double flag = 0.0;//���λ������Ƿ񾭹�360��Ĭ��û��

            double pi = 3.141592653589793;
            // ��һ���յ������
            std::vector<double> derivativeA = this->getDerivative({ parameterArray[i - 1] });
            // �����յ��е�����
            std::vector<double> derivativeH = this->getDerivative({ (parameterArray[i - 1] + parameterArray[i]) / 2 });
            // �ڶ����յ������
            std::vector<double> derivativeB = this->getDerivative({ parameterArray[i] });

            std::vector<double> angle;//�洢�Ƕȵ�����
            angle.push_back(acos(derivativeA[0]                                                  //(derivativeA[0] * derivativeO[0] + derivativeA[1] * derivativeO[1]
                / sqrt(derivativeA[0] * derivativeA[0] + derivativeA[1] * derivativeA[1]))  //��֪ |derivativeO| = 1.0
                * 180 / pi);
            angle.push_back(acos(derivativeH[0]
                / sqrt(derivativeH[0] * derivativeH[0] + derivativeH[1] * derivativeH[1]))
                * 180 / pi);
            angle.push_back(acos(derivativeB[0]
                / sqrt(derivativeB[0] * derivativeB[0] + derivativeB[1] * derivativeB[1]))
                * 180 / pi);

            //�����Ƕȷ�ΧΪ0��360
            if (derivativeA[0] > 0 && derivativeA[1] < 0 || derivativeA[0] < 0 && derivativeA[1] < 0) {
                angle[0] = 360.0 - angle[0];
            }
            if (derivativeH[0] > 0 && derivativeH[1] < 0 || derivativeH[0] < 0 && derivativeH[1] < 0) {
                angle[1] = 360.0 - angle[1];
            }
            if (derivativeB[0] > 0 && derivativeB[1] < 0 || derivativeB[0] < 0 && derivativeB[1] < 0) {
                angle[2] = 360.0 - angle[2];
            }

            //�Ƕ����侭��360
			if ((angle[1] - angle[0]) * (angle[2] - angle[1]) < 0) {
				flag = 1.0;//��Ǿ���360��
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
            //�Ƕ����䲻����360
            else {
                flag = 0.0;
                //��֤��ʱ��˳��
                if (angle[2] < angle[0]) {
                    double tempAngle = angle[0];
                    angle[0] = angle[2];
                    angle[2] = tempAngle;
                }
                //range = angle[2] - angle[0];
            }
            //ÿ����һ��

            angleRange[i - 1] = { angle[0],angle[2],flag };

        }

    }

}

//��c�����ߺ���Ƕȷ�Χ
void BSplineCurve::getCShapedCurve(std::vector<double>& inflectionPointU, std::vector<std::vector<double>>& angleRange) {

    inflectionPointU.clear();
    angleRange.clear();

    Curve curve(degree, controlPoints, knots, weights);

    double numPoints = 1000.0;
    std::vector<std::vector<double>> paramPoints;
    double deltaU = 1.0 / numPoints;//���
    for (int i = 0; i < numPoints; i++) {
        double u = i * deltaU;
        std::vector<double> param = { u }; // ������ֻ��һ��uֵ
        paramPoints.push_back(param);
    }

    double errorThreshold = 0.15; // �����ֵ
    std::vector<double> derivative;
    std::vector<double> secDerivative;

    std::vector<std::vector<double>> tempU;//��ʱ�洢�յ����ֵ����
    //int i = 0;//��Ч�Ĺյ����Ϊi+1

    //����0.0��1.0����ֵ
    for (const std::vector<double>& u : paramPoints) {
        derivative = this->getDerivative(u);
        secDerivative = this->getSecDerivative(u);

        //�ж��Ƿ�Ϊ�յ�
        if (std::abs((derivative[0] * secDerivative[1] + derivative[1] * secDerivative[0])
            / sqrt(derivative[0] * derivative[0] + derivative[1] * derivative[1])
            / sqrt(secDerivative[0] * secDerivative[0] + secDerivative[1] * secDerivative[1])) < errorThreshold) {

            //ĳһ��͹�Ա仯λ�õ�һ���յ�
            if (tempU.empty()) {
                tempU.push_back(std::vector<double>(1, u[0]));
            }
            else {
                int i = tempU.size() - 1; //��ǰ tempU ������

                //ĳһ��͹�Ա仯λ�������ȼ۹յ�
                if (std::abs(u[0] - tempU[i].back()) < 2 * deltaU) {
                    tempU[i].push_back(u[0]);
                }

                //������һ��͹�Ա仯λ�õ�һ���յ�
                else {
                    tempU.push_back(std::vector<double>(1, u[0]));
                }
            }
        }

    }
    tempU.push_back(std::vector<double>(1, 1.0));

    double pi = 3.141592653589793;//pi
    std::vector<double> derivativeO = { 1.0,0.0 };//x��

    inflectionPointU = { 1e-15 };//����������ǶȵĲ�������,ֵΪ0.0,inflectionPointU,1.0
    //ȡ��
    for (const std::vector<double> uArray : tempU) {
        int size = uArray.size();
        std::vector<double> u = { uArray[size / 2] };

        double range;//��¼�Ƿ񳬳�180��
        double flag = 0.0;//���λ������Ƿ񾭹�360��Ĭ��û��
        
        //ѭ��
        while (!u.empty()) {

            // ��һ���յ������
            std::vector<double> derivativeA = this->getDerivative({ inflectionPointU.back() });
            // �����յ��е�����
            std::vector<double> derivativeH = this->getDerivative({ (inflectionPointU.back() + u.back()) / 2.0 });
            // �ڶ����յ������
            std::vector<double> derivativeB = this->getDerivative({ u.back()});
            //x��

            std::vector<double> angle;
            angle.push_back(acos(derivativeA[0]                                                  //(derivativeA[0] * derivativeO[0] + derivativeA[1] * derivativeO[1]
                / sqrt(derivativeA[0] * derivativeA[0] + derivativeA[1] * derivativeA[1]))  //��֪ |derivativeO| = 1.0
                * 180 / pi);
            angle.push_back(acos(derivativeH[0]
                / sqrt(derivativeH[0] * derivativeH[0] + derivativeH[1] * derivativeH[1]))
                * 180 / pi);
            angle.push_back(acos(derivativeB[0]
                / sqrt(derivativeB[0] * derivativeB[0] + derivativeB[1] * derivativeB[1]))
                * 180 / pi);

            //�����Ƕȷ�ΧΪ0��360
            if (derivativeA[0] > 0 && derivativeA[1] < 0 || derivativeA[0] < 0 && derivativeA[1] < 0) {
                angle[0] = 360.0 - angle[0];
            }
            if (derivativeH[0] > 0 && derivativeH[1] < 0 || derivativeH[0] < 0 && derivativeH[1] < 0) {
                angle[1] = 360.0 - angle[1];
            }
            if (derivativeB[0] > 0 && derivativeB[1] < 0 || derivativeB[0] < 0 && derivativeB[1] < 0) {
                angle[2] = 360.0 - angle[2];
            }

            //�Ƕ����侭��360
            if ((angle[1] - angle[0]) * (angle[2] - angle[1]) < 0) {
                flag = 1.0;//��Ǿ���360��
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
            //�Ƕ����䲻����360
            else {
                flag = 0.0;
                //��֤��ʱ��˳��
                if (angle[2] < angle[0]) {
                    double tempAngle = angle[0];
                    angle[0] = angle[2];
                    angle[2] = tempAngle;
                }
                range = angle[2] - angle[0];
            }
            //ÿ����һ��
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


//set����----------------------------------

void BSplineCurve::setDegree(int degree) {
    this->degree = degree;
}

//add����----------------------------------

void BSplineCurve::addControlPoint(Point point) {
    this->controlPoints.push_back(point);
}

//clear����---------------------------------

void BSplineCurve::clearControlPoints() {
    this->controlPoints.clear();
}

//available����----------------------------

bool BSplineCurve::isCurveAvailable() {
    return this->pSize > this->degree;
}

//=====================================BezierExtraction=====================================

/**
 * \brief ���ݸ����Ľڵ�ʸ���ʹ�������B-������bezier��ȡ����
 *
 * \param knot �ڵ�ʸ��
 * \param p B-�����Ĵ���
 * \param C ������������ڴ洢bezier��ȡ����
 * \param nb ������������ڴ洢bezier��ȡ������Ŀ
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
 * \brief ����B�������Ƶ㡢�����ͽڵ�ʸ������Bezier���Ƶ�
 *
 * \param knot �ڵ�ʸ��
 * \param p B-�����Ĵ���
 * \param cpx B-�����Ŀ��Ƶ㼯
 * \param cpbsA ������������ڴ洢Bezier���ߵĿ��Ƶ�
 * \param intervals ������������ڴ洢Bezier���ߵ����䷶Χ
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
    int ptdim = controlPoints[0].size();//��ά��


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
                    cpbsA[j][x][y] += cbs[j][z][x] * cpn[z][y];//cbs[j][x][z]ת�ý��cbs[j][z][x]
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