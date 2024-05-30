#include "BezierCurve.h"

//���캯��
BezierCurve::BezierCurve() {

}

//���캯�������ƶ��㣩
BezierCurve::BezierCurve(std::vector<Point> c) {
	this->bezierControlPoints = c;
}

/**
 * \brief ���� Bernstein��
 *
 * \param n ��n+1�����Ƶ�
 * \param i ��i+1�����Ƶ��Ӧ�Ļ�
 * \param u ����λ��
 */
double BezierCurve::B(int n, int i, float u) {
	//Bernstein����ʽ��B(n,i) = C(n,i)*u^i*(1.0-u)^(n-i)
	int sum = 1;
	if (i == n || i == 0) return pow(u, i) * pow(1 - u, n - i);
	int j;
	for (j = n - i + 1; j <= n; j++)
		sum *= j;
	for (j = 1; j <= i; j++)
		sum /= j;
	return sum * pow(u, i) * pow(1 - u, n - i);
}

//������Bezier���߲���
void BezierCurve::update() {

	bezierPoints.clear();

	int num = 500; // �����ϵĵ������
	std::vector<double> param;
	double deltaU = 1.0 / num;//���
	for (int i = 0; i < num - 1; i++) {
		double u = i * deltaU;
		param.push_back(u);
	}
	param.push_back({ 1.0 });

	for (const double& u : param) {
		bezierPoints.push_back(getPoint(u));
	}
	
	//ò���в��Ͻ��ĵط���ĩβ����һ������û����ʾ
	/*for (double u = 0.0; u <= 1.0; u += 0.05) {
		bezierPoints.push_back(getPoint(u));
	}*/

}

//������в���
void BezierCurve::clearBezierCurve() {
	this->bezierControlPoints.clear();
	this->bezierPoints.clear();
	this->intervals.clear();
}

//bool BezierCurve::isCurveAvailable() {
//	return !this->bezierControlPoints.empty();
//}

//get����
//�õ����ƶ���
std::vector<Point> BezierCurve::getControlPoints() {
	return this->bezierControlPoints;
}

//��ô�Bezier�����ϵĵ�
std::vector<Point> BezierCurve::getPoints() {
	return this->bezierPoints;
}

//�õ�ĳ����λ��bezier�����ϵĵ�����
Point BezierCurve::getPoint(double u) {
	int n = bezierControlPoints.size();
	Point p = { 0.0,0.0,0.0 };
	for (int i = 0; i < n; i++) {
		p[0] += this->bezierControlPoints[i][0] * B(n - 1, i, u);
		p[1] += this->bezierControlPoints[i][1] * B(n - 1, i, u);
	}
	return p;
}

Point BezierCurve::getDerivative(double u) {

	int n = this->bezierControlPoints.size();

	if (u - 0.0 < 1e-15) {//�Բ���λ�ýӽ�0.0�Ĵ���
		u = 1e-15;
	}

	std::vector<double> derivative = { 0.0, 0.0, 0.0 };
	for (int i = 0; i < n - 1; i++) {
		Point p0 = this->bezierControlPoints[i];
		Point p1 = this->bezierControlPoints[i + 1];
		derivative[0] += 3.0 * (p1[0] - p0[0]) * B(n - 2, i, u);//�󵼹�ʽ
		derivative[1] += 3.0 * (p1[1] - p0[1]) * B(n - 2, i, u);
	}

	return derivative;
}

//��ĳ����λ�õ�����ת��
Point BezierCurve::getDerT(double u) {
	int n = this->bezierControlPoints.size();

	if (u - 0.0 < 1e-15) {//�Բ���λ�ýӽ�0.0�Ĵ���
		u = 1e-15;
	}

	Point pDerT = { 0.0,0.0,0.0 };
	for (int i = 0; i < n - 1; i++) {
		Point p0 = this->bezierControlPoints[i];
		Point p1 = this->bezierControlPoints[i + 1];
		pDerT[0] -= 3.0 * (p1[1] - p0[1]) * B(n - 2, i, u);//�󵼹�ʽ
		pDerT[1] += 3.0 * (p1[0] - p0[0]) * B(n - 2, i, u);
	}

	return pDerT;
}

//set����
void BezierCurve::setBezierControlPoints(std::vector<Point> bezierControlPoints) {
	this->bezierControlPoints = bezierControlPoints;
}

void BezierCurve::setIntervals(std::vector<double> intervals) {
	this->intervals = intervals;
}

//extract
void BezierCurve::extract(std::vector<Point>& control_1, std::vector<Point>& control_2, double t) {

	int n = bezierControlPoints.size();
	std::vector<std::vector<Point>> TreeP;

	TreeP.push_back(bezierControlPoints);

	//�����νṹÿ���ڵ��ֵ
	for (int i = 0; i < n - 1; i++) {
		std::vector<Point> lineP;
		std::vector<Point> P = TreeP.back();
		for (int j = 0; j < n - i - 1; j++) {

			lineP.push_back({ P[j][0] * (1.0 - t) + P[j + 1][0] * t ,
			P[j][1] * (1.0 - t) + P[j + 1][1] * t ,
			0.0 });

		}
		TreeP.push_back(lineP);
	}

	for (int i = 0; i < n; i++) {
		control_1.push_back(TreeP[i][0]);
		control_2.push_back(TreeP[i][n - i - 1]);
	}

}