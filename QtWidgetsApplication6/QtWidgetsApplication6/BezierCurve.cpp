#include "BezierCurve.h"

//构造函数
BezierCurve::BezierCurve() {

}

//构造函数（控制顶点）
BezierCurve::BezierCurve(std::vector<Point> c) {
	this->bezierControlPoints = c;
}

/**
 * \brief 计算 Bernstein基
 *
 * \param n 共n+1个控制点
 * \param i 第i+1个控制点对应的基
 * \param u 参数位置
 */
double BezierCurve::B(int n, int i, float u) {
	//Bernstein基公式：B(n,i) = C(n,i)*u^i*(1.0-u)^(n-i)
	int sum = 1;
	if (i == n || i == 0) return pow(u, i) * pow(1 - u, n - i);
	int j;
	for (j = n - i + 1; j <= n; j++)
		sum *= j;
	for (j = 1; j <= i; j++)
		sum /= j;
	return sum * pow(u, i) * pow(1 - u, n - i);
}

//更新有Bezier曲线参数
void BezierCurve::update() {

	bezierPoints.clear();

	int num = 500; // 曲线上的点的数量
	std::vector<double> param;
	double deltaU = 1.0 / num;//间隔
	for (int i = 0; i < num - 1; i++) {
		double u = i * deltaU;
		param.push_back(u);
	}
	param.push_back({ 1.0 });

	for (const double& u : param) {
		bezierPoints.push_back(getPoint(u));
	}
	
	//貌似有不严谨的地方，末尾会有一段曲线没有显示
	/*for (double u = 0.0; u <= 1.0; u += 0.05) {
		bezierPoints.push_back(getPoint(u));
	}*/

}

//清空所有参数
void BezierCurve::clearBezierCurve() {
	this->bezierControlPoints.clear();
	this->bezierPoints.clear();
	this->intervals.clear();
}

//bool BezierCurve::isCurveAvailable() {
//	return !this->bezierControlPoints.empty();
//}

//get函数
//得到控制顶点
std::vector<Point> BezierCurve::getControlPoints() {
	return this->bezierControlPoints;
}

//获得此Bezier曲线上的点
std::vector<Point> BezierCurve::getPoints() {
	return this->bezierPoints;
}

//得到某参数位置bezier曲线上的点坐标
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

	if (u - 0.0 < 1e-15) {//对参数位置接近0.0的处理
		u = 1e-15;
	}

	std::vector<double> derivative = { 0.0, 0.0, 0.0 };
	for (int i = 0; i < n - 1; i++) {
		Point p0 = this->bezierControlPoints[i];
		Point p1 = this->bezierControlPoints[i + 1];
		derivative[0] += 3.0 * (p1[0] - p0[0]) * B(n - 2, i, u);//求导公式
		derivative[1] += 3.0 * (p1[1] - p0[1]) * B(n - 2, i, u);
	}

	return derivative;
}

//求某参数位置导数的转置
Point BezierCurve::getDerT(double u) {
	int n = this->bezierControlPoints.size();

	if (u - 0.0 < 1e-15) {//对参数位置接近0.0的处理
		u = 1e-15;
	}

	Point pDerT = { 0.0,0.0,0.0 };
	for (int i = 0; i < n - 1; i++) {
		Point p0 = this->bezierControlPoints[i];
		Point p1 = this->bezierControlPoints[i + 1];
		pDerT[0] -= 3.0 * (p1[1] - p0[1]) * B(n - 2, i, u);//求导公式
		pDerT[1] += 3.0 * (p1[0] - p0[0]) * B(n - 2, i, u);
	}

	return pDerT;
}

//set函数
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

	//求树形结构每个节点的值
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