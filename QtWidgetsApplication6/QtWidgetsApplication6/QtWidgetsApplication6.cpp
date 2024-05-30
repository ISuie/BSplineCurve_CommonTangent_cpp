#include "QtWidgetsApplication6.h"

//全局变量记录当前曲线索引
int curveIndex = 0;
//全局变量记录当前曲线总数
int curveCount = 0;
//是否绘制控制多边形
bool isPolygonVisible = true;
//是否显示BezierExtraction结果
bool isBezierVisible = true;
//绘制功能为B-Spline还是bezier，默认B-Spline
bool drawWhich = true;

//绘制mesh
bool drawMesh = false;

QtWidgetsApplication6::QtWidgetsApplication6(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//链接新控制定点信号与槽
	connect(ui.openGLWidget, &OpenGLWidget::pointTextReady, this, &QtWidgetsApplication6::handlePointText);
	//默认最开始绘制第一条曲线
	ui.lineEdit_3->setText("1");
	//默认最开始曲线总数为0
	ui.lineEdit_4->setText("1");
}

QtWidgetsApplication6::~QtWidgetsApplication6()
{}

//----------------------------信号与槽函数----------------------------

//求导数按钮
void QtWidgetsApplication6::on_pushButton_clicked() {

	//参数
	QString inputContent = ui.lineEdit->text();

	//当曲线还未绘制
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable() || inputContent == "") {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : Not derivable!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}

	std::vector<double> u = { inputContent.toDouble() };//得到参数
	//若求参数位置0.0处导数，修正结果，直接求0.0可能出现错误
	if (u[0] == 0.0) {
		u[0] = 1e-15;
	}

	std::vector<double> derivative = ui.openGLWidget->curve[curveIndex]->getDerivative(u);//调用求导接口
	//QString outputContent = "(" + QString::number(derivative[0]) + "," + QString::number(derivative[1]) + "," + QString::number(derivative[2]) + ")";

	//输出
	QString outputContent = "(";
	for (const double i : derivative) {
		outputContent.push_back(QString::number(i) + ",");
	}
	outputContent.chop(1);
	outputContent.push_back(")");
	ui.lineEdit_2->setText(outputContent);
}

//求索引按钮
void QtWidgetsApplication6::on_pushButton_2_clicked() {

	QString inputContent = ui.lineEdit->text();

	//当曲线还未绘制
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable() || inputContent == "") {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : Unable to ask index!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}

	std::vector<double> u = { inputContent.toDouble() };
	std::vector<int> index = ui.openGLWidget->curve[curveIndex]->getIndex(u);
	QString outputContent = "(";
	for (const int i : index) {
		outputContent.push_back(QString::number(i) + ",");
	}
	outputContent.chop(1);
	outputContent.push_back(")");
	ui.lineEdit_2->setText(outputContent);
}

//切换曲线按钮
void QtWidgetsApplication6::on_pushButton_3_clicked() {
	if (ui.lineEdit_5->text().toInt() - 1 > curveCount || ui.lineEdit_5->text().toInt() < 1) {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : No such curve!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}
	curveIndex = ui.lineEdit_5->text().toInt() - 1;
	//显示当前曲线已经切换
	ui.lineEdit_3->setText(QString::number(curveIndex + 1));
	//重置显示的控制定点坐标
	ui.plainTextEdit->clear();
	std::vector<Point> controlPoints = ui.openGLWidget->curve[curveIndex]->getControlPoints();
	for (const Point& point : controlPoints) {
		QString pointText = "(" + QString::number(point[0]) + "," + QString::number(point[1]) + "," + QString::number(point[2]) + ")";
		ui.plainTextEdit->appendPlainText(pointText);
	}
}

//清空当前曲线控制点按钮
void QtWidgetsApplication6::on_pushButton_4_clicked() {

	ui.openGLWidget->inflectionPoint[curveIndex]->clear();

	//ui.openGLWidget->commonTangent.clear();

	ui.openGLWidget->curve[curveIndex]->clearControlPoints();//清空controlPoints
	ui.openGLWidget->curve[curveIndex]->update();//清空其它参数

	//ui.openGLWidget->bezierCurve->clearBezierCurve();//清空bezier extraction结果

	ui.openGLWidget->update();//清空显示窗口

	ui.plainTextEdit->clear();//清空显示的控制顶点坐标
	ui.lineEdit_9->clear();//清空拐点参数位置

}

//求公切线按钮
void QtWidgetsApplication6::on_pushButton_5_clicked() {

	ui.openGLWidget->commonTangent.clear();

	//求公切线可能出现的异常处理************************************

	int curve_1 = ui.lineEdit_6->text().toInt() - 1;//得到第一条曲线索引
	int curve_2 = ui.lineEdit_7->text().toInt() - 1;//得到第二条曲线索引
	if (ui.lineEdit_6->text() != "" && ui.lineEdit_7->text() != "" && curve_1 <= curveCount && curve_2 <= curveCount &&
		ui.openGLWidget->curve[curve_1]->isCurveAvailable() && ui.openGLWidget->curve[curve_2]->isCurveAvailable()) {
		double numPoints = 500.0;
		std::vector<std::vector<double>> paramPoints;
		double deltaU = 1.0 / numPoints;//间隔
		for (int i = 0; i < numPoints; i++) {
			double u = i * deltaU;
			std::vector<double> param = { u }; // 参数点只有一个u值
			paramPoints.push_back(param);
		}
		//paramPoints.push_back({ 1.0 });
		for (const std::vector<double>& u_1 : paramPoints) {
			for (const std::vector<double>& u_2 : paramPoints) {

				//曲线1在u_1位置导数
				std::vector<double> derivative_1 = ui.openGLWidget->curve[curve_1]->getDerivative(u_1);
				//曲线2在u_2位置导数
				std::vector<double> derivative_2 = ui.openGLWidget->curve[curve_2]->getDerivative(u_2);
				//曲线1在u_1位置点坐标
				Point point_1 = ui.openGLWidget->curve[curve_1]->getPoint(u_1);
				//曲线2在u_2位置点坐标
				Point point_2 = ui.openGLWidget->curve[curve_2]->getPoint(u_2);
				//两点连线
				std::vector<double> lineVector(3);
				lineVector[0] = point_2[0] - point_1[0];
				lineVector[1] = point_2[1] - point_1[1];
				lineVector[2] = point_2[2] - point_1[2];

				double errorThreshold = 1; // 误差阈值

				//叉乘
				double crossProduct_1 = derivative_1[0] * derivative_2[1] -
					derivative_1[1] * derivative_2[0];
				double crossProduct_2 = derivative_1[0] * lineVector[1] -
					derivative_1[1] * lineVector[0];

				if (std::abs(crossProduct_1) < errorThreshold && std::abs(crossProduct_2) < errorThreshold) {
					// point_1 和 point_2 处于相同的切线上
					ui.openGLWidget->commonTangent.push_back(point_1);
					ui.openGLWidget->commonTangent.push_back(point_2);
				}
				else {
					continue;
					// 不在相同的切线上
				}
			}
		}
	}
	else {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : Unable to find the common tangent!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}
	ui.openGLWidget->update();
	ui.lineEdit_8->setText("Done");
}

//新建曲线按钮
void QtWidgetsApplication6::on_pushButton_6_clicked() {
	curveCount++;
	ui.openGLWidget->curve[curveCount] = new BSplineCurve();
	ui.openGLWidget->inflectionPoint[curveCount] = new std::vector<Point>();
	curveIndex = curveCount;
	ui.lineEdit_3->setText(QString::number(curveIndex + 1));//显示当前曲线索引
	ui.lineEdit_4->setText(QString::number(curveCount + 1));//显示当前曲线总数
	ui.plainTextEdit->clear();//清空显示的控制顶点坐标
}

//求坐标按钮
void QtWidgetsApplication6::on_pushButton_7_clicked() {

	QString inputContent = ui.lineEdit->text();

	//当曲线还未绘制
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable() || inputContent == "") {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : Unable to get the coordinates!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}

	std::vector<double> u = { ui.lineEdit->text().toDouble() };
	Point point = ui.openGLWidget->curve[curveIndex]->getPoint(u);
	QString outputContent = "(" + QString::number(point[0]) + "," + QString::number(point[1]) + "," + QString::number(point[2]) + ")";
	ui.lineEdit_2->setText(outputContent);
}

//是否显示控制多边形
void QtWidgetsApplication6::on_pushButton_8_clicked() {
	isPolygonVisible ? isPolygonVisible = false : isPolygonVisible = true;
	ui.openGLWidget->update();
}

//求拐点按钮
void QtWidgetsApplication6::on_pushButton_9_clicked() {

	//当曲线还未绘制
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable()) {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : Unable to get the inflection points!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}

	//求拐点参数位置
	std::vector<double> inflectionPointU;
	std::vector<std::vector<double>> angleRange;
	ui.openGLWidget->curve[curveIndex]->getInflectionPoint(inflectionPointU, angleRange);

	std::vector<Point> tempP;//暂时存储拐点坐标数组
	for (const double ipu : inflectionPointU) {
		tempP.push_back(ui.openGLWidget->curve[curveIndex]->getPoint({ ipu }));
	}
	ui.openGLWidget->inflectionPoint[curveIndex] = new std::vector<Point>(tempP);//保存在OpenGLWidget

	//输出拐点参数数组
	QString tempUS = "{";//String
	for (const double ipu : inflectionPointU) {
		tempUS.push_back(QString::number(ipu) + ",");
	}
	tempUS.chop(1);
	tempUS.push_back("}");
	ui.lineEdit_9->setText(tempUS);

	//输出0.0到各个拐点到1.0参数位置角度范围
	for (const std::vector<double> a : angleRange) {
		ui.plainTextEdit_2->appendPlainText(QString::number(a[0]) + "," + QString::number(a[1]) + "," + QString::number(a[2]) + "\n");
	}

	ui.openGLWidget->update();//刷新窗口

}

//分出c型曲线
void QtWidgetsApplication6::on_pushButton_12_clicked() {

	//当曲线还未绘制
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable()) {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : Unable to get the C-shaped curves");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}

	//求拐点参数位置
	std::vector<double> inflectionPointU;
	std::vector<std::vector<double>> angleRange;
	ui.openGLWidget->curve[curveIndex]->getCShapedCurve(inflectionPointU, angleRange);

	std::vector<Point> tempP;//暂时存储拐点坐标数组
	for (const double ipu : inflectionPointU) {
		tempP.push_back(ui.openGLWidget->curve[curveIndex]->getPoint({ ipu }));
	}
	ui.openGLWidget->inflectionPoint[curveIndex] = new std::vector<Point>(tempP);//保存在OpenGLWidget

	//输出拐点参数数组
	QString tempUS = "{";//String
	for (const double ipu : inflectionPointU) {
		tempUS.push_back(QString::number(ipu) + ",");
	}
	tempUS.chop(1);
	tempUS.push_back("}");
	ui.lineEdit_11->setText(tempUS);

	//输出0.0到各个拐点到1.0参数位置角度范围
	for (const std::vector<double> a : angleRange) {
		ui.plainTextEdit_2->appendPlainText(QString::number(a[0]) + "," + QString::number(a[1]) + "," + QString::number(a[2]) + "\n");
	}

	ui.openGLWidget->update();//刷新窗口
}

//求重合角度
void QtWidgetsApplication6::on_pushButton_11_clicked() {

	std::vector<std::vector<double>> out;//输出

	int curve_1 = 0;//第一条曲线索引
	int curve_2 = 1;//第二条曲线索引

	std::vector<std::vector<double>> overlappingAngles;//存储重合的角度

	if (curve_1 <= curveCount && curve_2 <= curveCount && curve_1 != curve_2 &&
		ui.openGLWidget->curve[curve_1]->isCurveAvailable() && ui.openGLWidget->curve[curve_2]->isCurveAvailable()) {

		//求角度
		std::vector<double> inflectionPointU;//无用，为能调用getInflectionPoint函数
		std::vector<std::vector<double>> angleRange_1;//曲线1所有角度区间
		std::vector<std::vector<double>> angleRange_2;//曲线2所有角度区间
		ui.openGLWidget->curve[curve_1]->getCShapedCurve(inflectionPointU, angleRange_1);
		ui.openGLWidget->curve[curve_2]->getCShapedCurve(inflectionPointU, angleRange_2);

		int size_1 = angleRange_1.size();
		int size_2 = angleRange_2.size();

		for (int cIndex_1 = 0; cIndex_1 < size_1; cIndex_1++) {
			for (int cIndex_2 = 0; cIndex_2 < size_2; cIndex_2++) {

				if (angleRange_1[cIndex_1][2] == 1.0 && angleRange_2[cIndex_2][2] != 1.0) {//曲线1过360
					if (angleRange_2[cIndex_2][0] < angleRange_1[cIndex_1][1]) {//曲线2与曲线1上半部分有交集
						if (angleRange_2[cIndex_2][1] > angleRange_1[cIndex_1][1]) {//曲线2超出曲线1上半
							overlappingAngles.push_back({ angleRange_2[cIndex_2][0] ,angleRange_1[cIndex_1][1] ,0.0 });
						}
						else {//曲线2全部部分
							overlappingAngles.push_back({ angleRange_2[cIndex_2][0] ,angleRange_2[cIndex_2][1] ,0.0 });
						}
					}
					else if (angleRange_2[cIndex_2][1] > angleRange_1[cIndex_1][0]) {//曲线2与曲线1下半部分有交集
						if (angleRange_2[cIndex_2][0] < angleRange_1[cIndex_1][0]) {//曲线2超出曲线1下班
							overlappingAngles.push_back({ angleRange_1[cIndex_1][0] ,angleRange_2[cIndex_2][1] ,0.0 });
						}
						else {//曲线2全部部分
							overlappingAngles.push_back({ angleRange_2[cIndex_2][0] ,angleRange_2[cIndex_2][1] ,0.0 });
						}
					}
					continue;
				}
				else if (angleRange_1[cIndex_1][2] != 1.0 && angleRange_2[cIndex_2][2] == 1.0) {//曲线2过360
					if (angleRange_1[cIndex_1][0] < angleRange_2[cIndex_2][1]) {//曲线1与曲线2上半部分有交集
						if (angleRange_1[cIndex_1][1] > angleRange_2[cIndex_2][1]) {//曲线1超出曲线2上半
							overlappingAngles.push_back({ angleRange_1[cIndex_1][0] ,angleRange_2[cIndex_2][1] ,0.0 });
						}
						else {//曲线1全部部分
							overlappingAngles.push_back({ angleRange_1[cIndex_1][0] ,angleRange_1[cIndex_1][1] ,0.0 });
						}
					}
					else if (angleRange_1[cIndex_1][1] > angleRange_2[cIndex_2][0]) {//曲线1与曲线2下半部分有交集
						if (angleRange_1[cIndex_1][0] < angleRange_2[cIndex_2][0]) {//曲线1超出曲线2下班
							overlappingAngles.push_back({ angleRange_2[cIndex_2][0] ,angleRange_1[cIndex_1][1] ,0.0 });
						}
						else {//曲线1全部部分
							overlappingAngles.push_back({ angleRange_1[cIndex_1][0] ,angleRange_1[cIndex_1][1] ,0.0 });
						}
					}
					continue;
				}
				else {
					double max_0 = std::max(angleRange_1[cIndex_1][0], angleRange_2[cIndex_2][0]);
					double min_1 = std::min(angleRange_1[cIndex_1][1], angleRange_2[cIndex_2][1]);
					if (angleRange_1[cIndex_1][2] == 1.0 && angleRange_2[cIndex_2][2] == 1.0) {//两条线均经过360，则必有重合
						overlappingAngles.push_back({ max_0,min_1 ,1.0 });
					}
					else if (max_0 < min_1) {//都不经过360
						overlappingAngles.push_back({ max_0,min_1 ,0.0 });
					}
				}

			}
		}

	}
	else {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : Unable to find overlapping angle intervals!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}

	//输出一下
	for (const std::vector<double>& line : overlappingAngles) {
		ui.plainTextEdit_2->appendPlainText(
			QString::number(line[0]) + "," + QString::number(line[1]) + "," + QString::number(line[2]));
	}

	ui.openGLWidget->update();//刷新窗口
}


//BezierExtraction
void QtWidgetsApplication6::on_pushButton_13_clicked() {

	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable()) {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : Bezier Extraction cannot be implemented!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}

	ui.openGLWidget->curve[curveIndex]->bezierExtractCpts();//在BSplineCurve中准备CpbsA

	for (const std::vector<Point> line : ui.openGLWidget->curve[curveIndex]->getCpbsA()) {
		ui.plainTextEdit_2->appendPlainText("Bezier Extreaction:");
		for (const Point p : line) {
			ui.plainTextEdit_2->appendPlainText(QString::number(p[0]) + "," + QString::number(p[1]) + "," + QString::number(p[2]));
		}
		ui.plainTextEdit_2->appendPlainText(" ");
	}

	for (const std::vector<double> line : ui.openGLWidget->curve[curveIndex]->getIntervals()) {
		ui.plainTextEdit_2->appendPlainText("intervals:");
		ui.plainTextEdit_2->appendPlainText(QString::number(line[0]) + "," + QString::number(line[1]));
		ui.plainTextEdit_2->appendPlainText(" ");
	}

	ui.openGLWidget->update();//刷新窗口
}

//求两条B-Spline中的第一段Bezier是否有切线
void QtWidgetsApplication6::on_pushButton_14_clicked() {

	//两条b样条已绘制
	if (curveCount > 0
		&& ui.openGLWidget->curve[0]->isCurveAvailable()
		&& ui.openGLWidget->curve[1]->isCurveAvailable()) {

		//调用bezier extraction的最终封装函数，CpbsA和Intervals会在BSplineCurve类属性中准备好
		ui.openGLWidget->curve[0]->bezierExtractCpts();
		ui.openGLWidget->curve[1]->bezierExtractCpts();

	}
	else {
		return;
	}

	//Bezier Extraction
	//std::vector<std::vector<double>> intervals = ui.openGLWidget->curve[1]->getIntervals();
	std::vector<std::vector<Point>> cpbsA_1 = ui.openGLWidget->curve[0]->getCpbsA();
	std::vector<std::vector<Point>> cpbsA_2 = ui.openGLWidget->curve[1]->getCpbsA();

	double errorThreshold = 1.0; // 误差阈值

	int size_1 = cpbsA_1.size();
	int size_2 = cpbsA_2.size();
	//遍历所有bezier
	for (int i = 0; i < size_1; i++) {
		for (int j = 0; j < size_2; j++) {

			//cpbsA_1和cpbsA_2中分别有size_1和size_2组控制顶点
			std::vector<Point> controlPoints_1 = cpbsA_1[i];
			std::vector<Point> controlPoints_2 = cpbsA_2[j];

			//两种设置controlPoints方式
			BezierCurve bezierCurve_1(controlPoints_1);
			BezierCurve bezierCurve_2(controlPoints_2);
			//bezierCurve_1.setBezierControlPoints(controlPoints_1);
			//bezierCurve_2.setBezierControlPoints(controlPoints_2);

			//求切线
			//自创产物，公式见图片（手机）
			for (double s = 0.0; s <= 1.0; s += 0.005) {
				for (double t = 0.0; t <= 1.0; t += 0.005) {
					Point PsQt = { 0.0,0.0,0.0 };
					PsQt[0] = bezierCurve_1.getPoint(s)[0] - bezierCurve_2.getPoint(t)[0];
					PsQt[1] = bezierCurve_1.getPoint(s)[1] - bezierCurve_2.getPoint(t)[1];
					Point PsDT = bezierCurve_1.getDerT(s);
					Point PsD = bezierCurve_1.getDerivative(s);
					Point QtDT = bezierCurve_2.getDerT(t);
					if (std::abs(PsQt[0] * PsDT[0] + PsQt[1] * PsDT[1]) < errorThreshold &&
						std::abs(PsD[0] * QtDT[0] + PsD[1] * QtDT[1]) < errorThreshold) {
						ui.openGLWidget->tantan.push_back(bezierCurve_1.getPoint(s));
						ui.openGLWidget->tantan.push_back(bezierCurve_2.getPoint(t));
					}
				}
			}

		}
	}

	//输出所有切线端点在曲线上的坐标
	for (const Point& p : ui.openGLWidget->tantan) {
		ui.plainTextEdit_2->appendPlainText("tantan:" + QString::number(p[0]) + "," + QString::number(p[1]) + "," + QString::number(p[2]));
	}

	ui.openGLWidget->update();//刷新窗口
}

//隐藏或者显示Bezier
void QtWidgetsApplication6::on_pushButton_15_clicked() {

	isBezierVisible ? isBezierVisible = false : isBezierVisible = true;//修改bool变量isBezierVisible的值
	ui.openGLWidget->update();//刷新窗口

}

//把bezier曲线分割成两部分
void QtWidgetsApplication6::on_pushButton_16_clicked() {

	BezierCurve bezier;
	bezier.setBezierControlPoints(ui.openGLWidget->curve[0]->getControlPoints());

	//清空
	ui.openGLWidget->control_1.clear();
	ui.openGLWidget->control_2.clear();

	//分割设置分隔位置为0.3
	bezier.extract(ui.openGLWidget->control_1, ui.openGLWidget->control_2, 0.3);

	ui.openGLWidget->update();//刷新窗口
}

//功能：B-Spline相关包括extract之后的绘制/bezier直接绘制
void QtWidgetsApplication6::on_pushButton_17_clicked() {
	drawWhich ? drawWhich = false : drawWhich = true;//修改bool变量drawWhich的值
	ui.openGLWidget->update();//刷新窗口
}

//求bezier曲线首末点（为了验证bezier绘制的问题，在结尾处会出现一小段空白问题，已解决）
void QtWidgetsApplication6::on_pushButton_18_clicked() {
	Point p1 = ui.openGLWidget->bezierCurve.getPoint(0.0);//曲线首点
	Point p2 = ui.openGLWidget->bezierCurve.getPoint(1.0);//曲线末点

	std::vector<Point> pa = ui.openGLWidget->bezierCurve.getControlPoints();
	Point p3 = pa.front();//控制顶点首点
	Point p4 = pa.back();//控制顶点末点

	ui.plainTextEdit_2->appendPlainText("p1:" + QString::number(p1[0]) + "," + QString::number(p1[1]) + "," + QString::number(p1[2]));
	ui.plainTextEdit_2->appendPlainText("p2:" + QString::number(p2[0]) + "," + QString::number(p2[1]) + "," + QString::number(p2[2]));

	ui.plainTextEdit_2->appendPlainText("p3:" + QString::number(p3[0]) + "," + QString::number(p3[1]) + "," + QString::number(p3[2]));
	ui.plainTextEdit_2->appendPlainText("p4:" + QString::number(p4[0]) + "," + QString::number(p4[1]) + "," + QString::number(p4[2]));
}

//************************测试输出************************

//清空测试区域数据
void QtWidgetsApplication6::on_pushButton_10_clicked() {
	ui.plainTextEdit_2->clear();//清空显示的控制顶点坐标
}

//************************测试输出************************

//-------------------------------------------------------------

//处理控制顶点信号
void QtWidgetsApplication6::handlePointText(const QString& pointText) {
	ui.plainTextEdit->appendPlainText(pointText);//点了屏幕就会出现
	//可能会有一些逻辑上的问题
	//B-Spline先关应该还好
	//Bezier绘制肯定大有问题
}

////######################针对f(s,t)得到f1, f2, f1 ? cf2, f1 + cf2######################
////求fij
//void QtWidgetsApplication5::on_pushButton_19_clicked() {
//	//f(s,t) = (P(s) - Q(t)) * P'(s)T = P(s)*P'(s)T - Q(t)*P'(s)T = 0.0
//	//f1(s,t) = P(s) * P'(s)T
//	//f2(s,t) = Q(t) * P'(s)T两者相等
//	
//	if (ui.openGLWidget->curve[0]->isCurveAvailable()&& ui.openGLWidget->curve[1]->isCurveAvailable()) {//将第一组控制顶点最为P(s)的控制顶点
//
//		//对于s
//		double s0 = 0.0;
//		double s1 = 1.0;
//		//对于t
//		double t0 = 0.0;
//		double t1 = 1.0;
//
//		//-------------------------------------------------------------------
//		
//		//f1(s,t)的系数fij，其中i为s的次数，j为t的次数，j恒为0
//		//对于f1(s,t) = P(s) * P'(s)T只要处理P'(s)T
//		std::vector<Point> P = ui.openGLWidget->curve[0]->getControlPoints();//P(s)的控制顶点
//		int pS = P.size();//P的size
//		double m = pS - 1;//P(s)的次数，类型double方便后续计算
//
//		//BezierCurve bezierCurveP(P);//创建P(s)的bezier曲线
//		//bezierCurveP.update();
//
//		std::vector<double> fi0(m + 1);
//
//		//对于fi0的前m个
//		//f1(s,t) = P(s) * P'(s)T
//		//dP0
//		fi0[0] = P[0][0] * (-P[1][1] + P[0][1]) + P[0][1] * (P[1][0] - P[0][0]);
//		for (int i = 1; i < m; i++) {
//			//Qi[0] = i/m*(P[i][0]-P[i-1][0])+(1.0-i/m)*(P[i+1][0]-P[i][0])
//			fi0[i] = P[i][0] * ((double)i / m * (-P[i][1] + P[i - 1][1]) + (1.0 - (double)i / m) * (-P[i + 1][1] + P[i][1])) +
//				P[i][1] * ((double)i / m * (P[i][0] - P[i - 1][0]) + (1.0 - (double)i / m) * (P[i + 1][0] - P[i][0]));
//		}
//		//fi0第m+1个
//		fi0[m] = P[m][0] * (-P[m][1] + P[m - 1][1]) + P[m][1] * (P[m][0] - P[m - 1][0]);
//
//		for (const double& d : fi0) {
//			ui.plainTextEdit_2->appendPlainText(QString::number(d));
//		}
//
//		//-------------------------------------------------------------------
//		//对于f2(s,t) = Q(t) * P'(s)T也只要处理P'(s)T
//		//f2(s,t)的系数fij，其中i为s的次数，j为t的次数
//
//		std::vector<Point> Q = ui.openGLWidget->curve[1]->getControlPoints();//Q(t)的控制顶点
//		int qS = Q.size();//Q的size
//		double n = qS - 1;//Q(t)的次数，类型double方便后续计算
//
//		//BezierCurve bezierCurveQ(Q);//创建Q(t)的bezier曲线
//		//bezierCurveQ.update();
//
//		std::vector<std::vector<double>> fij;
//		fij.resize(m + 1, std::vector<double>(n + 1));//m+1行n+1列
//
//		for (int j = 0; j < n+1; j++) {
//			fij[0][j] = Q[j][0] * (-P[1][1] + P[0][1]) + Q[j][1] * (P[1][0] - P[0][0]);
//			for (int i = 1; i < m; i++) {
//				fij[i][j] = Q[j][0] * ((double)i / m * (-P[i][1] + P[i - 1][1]) + (1.0 - (double)i / m) * (-P[i + 1][1] + P[i][1])) +
//					Q[j][1] * ((double)i / m * (P[i][0] - P[i - 1][0]) + (1.0 - (double)i / m) * (P[i + 1][0] - P[i][0]));
//			}
//			fij[m][j] = Q[j][0] * (-P[m][1] + P[m - 1][1]) + Q[j][1] * (P[m][0] - P[m - 1][0]);
//		}
//
//		for (const std::vector<double>& l : fij) {
//			QString string;
//			for (int i = 0; i < n + 1; i++) {
//				string = string + QString::number(l[i]) + ",";
//			}
//			ui.plainTextEdit_2->appendPlainText(string);
//		}
//
//	}
//
//}
////g(s,t) = P'(s) * Q'(t)T = 0

//定义阶乘函数
int fac(int x)
{
	int s = 1;
	for (int i = 1; i <= x; i++)
		s *= i;
	return s;
}

//定义组合数
int C(int m, int n) {
	return fac(m) / (fac(m - n) * fac(n));
}

//定义Berstein基函数
double B(int n, int i, float u) {
	//Bernstein基公式：B(n,i) = C(n,i)*u^i*(i.0-u)^(n-i)
	int sum = 1;
	if (i == n || i == 0) return pow(u, i) * pow(1 - u, n - i);
	int j;
	for (j = n - i + 1; j <= n; j++)
		sum *= j;
	for (j = 1; j <= i; j++)
		sum /= j;
	return sum * pow(u, i) * pow(1 - u, n - i);
}

//######################针对f(s,t)得到f1, f2, f1 ? cf2, f1 + cf2######################
//求fij
void QtWidgetsApplication6::on_pushButton_19_clicked() {
	//f(s,t) = (P(s) - Q(t)) * P'(s)T = P(s)*P'(s)T - Q(t)*P'(s)T = 0.0
	//f1(s,t) = P(s) * P'(s)T
	//f2(s,t) = Q(t) * P'(s)T两者相等


	if (ui.openGLWidget->curve[1]
		&& ui.openGLWidget->curve[0]->isCurveAvailable()
		&& ui.openGLWidget->curve[1]->isCurveAvailable()) {//将第一组控制顶点最为P(s)的控制顶点

		//对于s
		double s0 = 0.0;
		double s1 = 1.0;
		double s = 0.5;
		double s_ = (s1 - s) / (s1 - s0);
		//对于t
		double t0 = 0.0;
		double t1 = 1.0;
		double t = 0.5;
		double t_ = (t1 - t) / (t1 - t0);

		//-------------------------------------------------------------------

		//f1(s,t)的系数fij，其中i为s的次数，j为t的次数
		//对于f1(s,t) = P(s) * P'(s)T
		//通过B升次是两B次数相差1，在将B合并
		std::vector<Point> P = ui.openGLWidget->curve[0]->getControlPoints();//P(s)的控制顶点
		int pS = P.size();//P的size
		double m = pS - 1;//P(s)的次数，类型double方便后续计算

		std::vector<Point> Q = ui.openGLWidget->curve[1]->getControlPoints();//Q(t)的控制顶点
		int qS = Q.size();//Q的size
		double n = qS - 1;//Q(t)的次数，类型double方便后续计算

		//BezierCurve bezierCurveP(P);//创建P(s)的bezier曲线
		//bezierCurveP.update();

		std::vector<double> tempij1;//先求新的Pi之后再求Qij
		tempij1.resize(2 * m, 0.0);//(2*m-1,0)注意个数要是次数加1

		for (int i = 0; i < m + 1; i++) {//计算的时候针对的是P(s)*P'(s)T
			for (int j = 0; j < m; j++) {
				tempij1[i + j] += (double)C(m, i) * (double)C(m - 1, j) / (double)C(2 * m - 1, i + j)//B由两个相乘化为2*m-1
					* (double)m / (s1 - s0)//求导前系数
					* (P[i][0] * (-P[j + 1][1] + P[j][1]) +
						P[i][1] * (P[j + 1][0] - P[j][0]));
			}
		}

		std::vector<std::vector<double>> fij1;//Qij

		//(2*m-1,0)->(2*m-1,n)
		for (int i = 0; i < 2 * m; i++) {//共2m行
			fij1.push_back(std::vector<double>(n + 1, tempij1[i]));//每行都一样
		}

		//P(s)P'(S)T
		double f1xy = 0.0;
		for (int i = 0; i < 2 * m; i++) {
			for (int j = 0; j < n + 1; j++) {
				f1xy += fij1[i][j] * B(2 * m - 1, i, (s1 - s) / (s1 - s0)) * B(n, j, (t1 - t) / (t1 - t0));
			}
		}

		//打印
		ui.plainTextEdit_2->appendPlainText("fij1(2*m-1,0)");
		for (const double& d : tempij1) {
			ui.plainTextEdit_2->appendPlainText(QString::number(d));
		}

		ui.plainTextEdit_2->appendPlainText("\nfij1");
		for (const std::vector<double> l : fij1) {
			QString string;
			for (int i = 0; i < n + 1; i++) {
				string = string + QString::number(l[i]) + ",";
			}
			ui.plainTextEdit_2->appendPlainText(string);
		}

		//-------------------------------------------------------------------
		//对于f2(s,t) = Q(t) * P'(s)T
		//f2(s,t)的系数fij，其中i为s的次数，j为t的次数

		//BezierCurve bezierCurveQ(Q);//创建Q(t)的bezier曲线
		//bezierCurveQ.update();

		//std::vector<std::vector<double>> tempij2(m, std::vector<double>(n + 1));//(m-1,n)m行n+1列
		//只使用(m-1,n)m行n+1列，其他为0.0
		std::vector<std::vector<double>> beforeij2(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n + 1; j++) {
				beforeij2[i][j] = (double)m / (s1 - s0)
					* (Q[j][0] * (-P[i + 1][1] + P[i][1]) + Q[j][1] * (P[i + 1][0] - P[i][0]));
			}
		}

		//打印
		ui.plainTextEdit_2->appendPlainText("\nfij2(m-1,n)");
		for (int i = 0; i < m; i++) {
			QString string;
			for (int j = 0; j < n + 1; j++) {
				string = string + QString::number(beforeij2[i][j]) + ",";
			}
			ui.plainTextEdit_2->appendPlainText(string);
		}

		std::vector<std::vector<double>> fij2(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

		for (int t = 0; t < m; t++) {
			for (int j = 0; j < n + 1; j++) {
				fij2[0][j] = beforeij2[0][j];
				for (int i = 1; i < m + t; i++) {
					fij2[i][j] = (double)i / ((double)m + (double)t) * beforeij2[i - 1][j]
						+ (1.0 - (double)i / ((double)m + (double)t)) * beforeij2[i][j];
				}
				fij2[m + t][j] = beforeij2[m + t - 1][j];
			}
			beforeij2 = fij2;
		}

		//Q(t)P'(S)T
		double f2xy = 0.0;
		for (int i = 0; i < 2 * m; i++) {
			for (int j = 0; j < n + 1; j++) {
				f2xy += fij2[i][j] * B(2 * m - 1, i, (s1 - s) / (s1 - s0)) * B(n, j, (t1 - t) / (t1 - t0));
			}
		}

		//打印
		ui.plainTextEdit_2->appendPlainText("\nfij2");
		for (const std::vector<double>& l : fij2) {
			QString string;
			for (int i = 0; i < n + 1; i++) {
				string = string + QString::number(l[i]) + ",";
			}
			ui.plainTextEdit_2->appendPlainText(string);
		}

		//打印P(s)P'(S)T-Q(t)P'(S)T（直接算版本）
		BezierCurve bezierCurve_1(P);
		BezierCurve bezierCurve_2(Q);
		Point PsDT = bezierCurve_1.getDerT(s_);
		Point Ps = bezierCurve_1.getPoint(s_);
		Point Qt = bezierCurve_2.getPoint(t_);

		ui.plainTextEdit_2->appendPlainText("\n##P(s)P'(S)T##");
		ui.plainTextEdit_2->appendPlainText(QString::number(f1xy));
		ui.plainTextEdit_2->appendPlainText(QString::number(Ps[0] * PsDT[0] + Ps[1] * PsDT[1]));
		ui.plainTextEdit_2->appendPlainText("\n##Q(t)P'(S)T##");
		ui.plainTextEdit_2->appendPlainText(QString::number(f2xy));
		ui.plainTextEdit_2->appendPlainText(QString::number(Qt[0] * PsDT[0] + Qt[1] * PsDT[1]));

		std::vector<std::vector<double>> fij(2 * m, std::vector<double>(n + 1));
		for (int i = 0; i < 2 * m; i++) {
			for (int j = 0; j < n + 1; j++) {
				fij[i][j] = fij1[i][j] - fij2[i][j];
			}
		}
		ui.plainTextEdit_2->appendPlainText("\nfij");
		for (const std::vector<double>& l : fij) {
			QString string;
			for (int i = 0; i < n + 1; i++) {
				string = string + QString::number(l[i]) + ",";
			}
			ui.plainTextEdit_2->appendPlainText(string);
		}

		//g(s,t) = P'(s) * Q'(t)T = 0=============================================================
		//(m-1,n-1)
		std::vector<std::vector<double>> beforeij3(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				beforeij3[i][j] = (double)m / (s1 - s0) * (double)n / (t1 - t0)
					* ((P[i + 1][0] - P[i][0]) * (-Q[j + 1][1] + Q[j][1]) + (P[i + 1][1] - P[i][1]) * (Q[j + 1][0] - Q[j][0]));
			}
		}

		//(m-1,n-1)->(2*m-1,n)
		std::vector<std::vector<double>> gij(2 * m, std::vector<double>(n + 1, 0.0));

		//1：(m-1,n-1)->(m-1,n)
		for (int i = 0; i < m; i++) {
			gij[i][0] = beforeij3[i][0];
			for (int j = 1; j < n; j++) {
				gij[i][j] = (double)j / (double)n * beforeij3[i][j - 1]
					+ (1.0 - (double)j / (double)n) * beforeij3[i][j];
			}
			gij[i][n] = beforeij3[i][n - 1];
		}
		beforeij3 = gij;

		//2：(m-1,n)->(2*m-1,n)
		for (int t = 0; t < m; t++) {
			for (int j = 0; j < n + 1; j++) {
				gij[0][j] = beforeij3[0][j];
				for (int i = 1; i < m + t; i++) {
					gij[i][j] = (double)i / ((double)m + (double)t) * beforeij3[i - 1][j]
						+ (1.0 - (double)i / ((double)m + (double)t)) * beforeij3[i][j];
				}
				gij[m + t][j] = beforeij3[m + t - 1][j];
			}
			beforeij3 = gij;
		}

		//打印
		ui.plainTextEdit_2->appendPlainText("\ngij");
		for (const std::vector<double>& l : gij) {
			QString string;
			for (int i = 0; i < n + 1; i++) {
				string = string + QString::number(l[i]) + ",";
			}
			ui.plainTextEdit_2->appendPlainText(string);
		}

		double gxy = 0.0;
		for (int i = 0; i < 2 * m; i++) {
			for (int j = 0; j < n + 1; j++) {
				gxy += gij[i][j] * B(2 * m - 1, i, (s1 - s) / (s1 - s0)) * B(n, j, (t1 - t) / (t1 - t0));
			}
		}

		//验证
		Point PsD = bezierCurve_1.getDerivative(s_);
		Point QtDT = bezierCurve_2.getDerT(t_);
		ui.plainTextEdit_2->appendPlainText("\n##P'(S)Q'(t)T##");
		ui.plainTextEdit_2->appendPlainText(QString::number(gxy));
		ui.plainTextEdit_2->appendPlainText(QString::number(PsD[0] * QtDT[0] + PsD[1] * QtDT[1]));

	}

}

//绘制网格
void QtWidgetsApplication6::on_pushButton_20_clicked() {
	if (!drawMesh) {//只需要初始化一次
		drawMesh = true;
		InitialMesh(ui.openGLWidget->M, 2, 2, -8.0, 8.0, -8.0, 8.0);//初始化得到一个mesh，只包含一个face
	}
	ui.openGLWidget->update();//刷新窗口
}

//cross指定面
void QtWidgetsApplication6::on_pushButton_21_clicked() {

	if (ui.lineEdit_12->text().toInt() >= ui.openGLWidget->M.m_Faces.size() || ui.lineEdit_12->text().toInt() < 0) {
		QMessageBox errorMsgBox;// 创建一个错误消息对话框
		errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
		errorMsgBox.setText("An error occurred : No such face!");// 设置错误消息文本
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
		errorMsgBox.exec();//显示消息对话框
		return;
	}

	QString crossText = ui.lineEdit_12->text();
	QStringList dataList = crossText.split(","); // 使用逗号分割字符串

	vector<int> crossFaces;
	foreach(const QString & str, dataList) {
		bool ok;
		crossFaces.push_back(str.toInt(&ok)); // 将每个子串转换为整数
		/*if (!ok) {
			qDebug() << "Extracted Number:" << number;
		}*/
	}

	myRefine_QQ_Cross(ui.openGLWidget->M, crossFaces);

	ui.openGLWidget->update();//刷新窗口
}

//求联通指定面
void QtWidgetsApplication6::on_pushButton_22_clicked() {

	QString remainText = ui.lineEdit_13->text();
	QStringList dataList = remainText.split(","); // 使用逗号分割字符串

	vector<int> remain;
	foreach(const QString & str, dataList) {
		bool ok;
		remain.push_back(str.toInt(&ok)); // 将每个子串转换为整数，push到remian数组
		/*if (!ok) {
			qDebug() << "Extracted Number:" << number;
		}*/
	}

	ui.openGLWidget->connected.clear();
	vector<int> nextStack;
	//只要remain不为空，就pop出back，并遍历找到所有连通的，内部有!remain.empty()迭代判断
	//while (!remain.empty()) {
	Connected(ui.openGLWidget->M, remain, ui.openGLWidget->connected, nextStack);
	//}

	ui.openGLWidget->update();//刷新窗口
}

char inervalJudgement(BSplineCurve* curve0, BSplineCurve* curve1, double s0, double s1, double t0, double t1) {
	//if (ui.openGLWidget->curve[1]
	//	&& ui.openGLWidget->curve[0]->isCurveAvailable()
	//	&& ui.openGLWidget->curve[1]->isCurveAvailable()) {//将第一组控制顶点最为P(s)的控制顶点

	//对于s
	/*double s0 = 0.0;
	double s1 = 1.0;*/
	double s = (s0 + s1) / 2.0;
	double s_ = (s1 - s) / (s1 - s0);
	//对于t
	/*double t0 = 0.0;
	double t1 = 1.0;*/
	double t = (t0 + t1) / 2.0;
	double t_ = (t1 - t) / (t1 - t0);

	//-------------------------------------------------------------------

	//f1(s,t)的系数fij，其中i为s的次数，j为t的次数
	//对于f1(s,t) = P(s) * P'(s)T
	//通过B升次是两B次数相差1，在将B合并
	std::vector<Point> P = curve0->getControlPoints();//P(s)的控制顶点
	int pS = P.size();//P的size
	double m = pS - 1;//P(s)的次数，类型double方便后续计算

	std::vector<Point> Q = curve1->getControlPoints();//Q(t)的控制顶点
	int qS = Q.size();//Q的size
	double n = qS - 1;//Q(t)的次数，类型double方便后续计算

	std::vector<double> tempij1;//先求新的Pi之后再求Qij
	tempij1.resize(2 * m, 0.0);//(2*m-1,0)注意个数要是次数加1

	for (int i = 0; i < m + 1; i++) {//计算的时候针对的是P(s)*P'(s)T
		for (int j = 0; j < m; j++) {
			tempij1[i + j] += (double)C(m, i) * (double)C(m - 1, j) / (double)C(2 * m - 1, i + j)//B由两个相乘化为2*m-1
				* (double)m / (s1 - s0)//求导前系数
				* (P[i][0] * (-P[j + 1][1] + P[j][1]) +
					P[i][1] * (P[j + 1][0] - P[j][0]));
		}
	}

	std::vector<std::vector<double>> fij1;//Qij

	//(2*m-1,0)->(2*m-1,n)
	for (int i = 0; i < 2 * m; i++) {//共2m行
		fij1.push_back(std::vector<double>(n + 1, tempij1[i]));//每行都一样
	}

	//-------------------------------------------------------------------
	//对于f2(s,t) = Q(t) * P'(s)T
	//f2(s,t)的系数fij，其中i为s的次数，j为t的次数

	//BezierCurve bezierCurveQ(Q);//创建Q(t)的bezier曲线
	//bezierCurveQ.update();

	//std::vector<std::vector<double>> tempij2(m, std::vector<double>(n + 1));//(m-1,n)m行n+1列
	//只使用(m-1,n)m行n+1列，其他为0.0
	std::vector<std::vector<double>> beforeij2(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n + 1; j++) {
			beforeij2[i][j] = (double)m / (s1 - s0)
				* (Q[j][0] * (-P[i + 1][1] + P[i][1]) + Q[j][1] * (P[i + 1][0] - P[i][0]));
		}
	}

	std::vector<std::vector<double>> fij2(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

	for (int t = 0; t < m; t++) {
		for (int j = 0; j < n + 1; j++) {
			fij2[0][j] = beforeij2[0][j];
			for (int i = 1; i < m + t; i++) {
				fij2[i][j] = (double)i / ((double)m + (double)t) * beforeij2[i - 1][j]
					+ (1.0 - (double)i / ((double)m + (double)t)) * beforeij2[i][j];
			}
			fij2[m + t][j] = beforeij2[m + t - 1][j];
		}
		beforeij2 = fij2;
	}

	std::vector<std::vector<double>> fij(2 * m, std::vector<double>(n + 1));
	for (int i = 0; i < 2 * m; i++) {
		for (int j = 0; j < n + 1; j++) {
			fij[i][j] = fij1[i][j] - fij2[i][j];
		}
	}

	//打印fij
	/*ui.plainTextEdit_2->appendPlainText("\nfij");
	for (const std::vector<double>& l : fij) {
		QString string;
		for (int i = 0; i < n + 1; i++) {
			string = string + QString::number(l[i]) + ",";
		}
		ui.plainTextEdit_2->appendPlainText(string);
	}*/

	//g(s,t) = P'(s) * Q'(t)T = 0=============================================================
	//(m-1,n-1)
	std::vector<std::vector<double>> beforeij3(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			beforeij3[i][j] = (double)m / (s1 - s0) * (double)n / (t1 - t0)
				* ((P[i + 1][0] - P[i][0]) * (-Q[j + 1][1] + Q[j][1]) + (P[i + 1][1] - P[i][1]) * (Q[j + 1][0] - Q[j][0]));
		}
	}

	//(m-1,n-1)->(2*m-1,n)
	std::vector<std::vector<double>> gij(2 * m, std::vector<double>(n + 1, 0.0));

	//1：(m-1,n-1)->(m-1,n)
	for (int i = 0; i < m; i++) {
		gij[i][0] = beforeij3[i][0];
		for (int j = 1; j < n; j++) {
			gij[i][j] = (double)j / (double)n * beforeij3[i][j - 1]
				+ (1.0 - (double)j / (double)n) * beforeij3[i][j];
		}
		gij[i][n] = beforeij3[i][n - 1];
	}
	beforeij3 = gij;

	//2：(m-1,n)->(2*m-1,n)
	for (int t = 0; t < m; t++) {
		for (int j = 0; j < n + 1; j++) {
			gij[0][j] = beforeij3[0][j];
			for (int i = 1; i < m + t; i++) {
				gij[i][j] = (double)i / ((double)m + (double)t) * beforeij3[i - 1][j]
					+ (1.0 - (double)i / ((double)m + (double)t)) * beforeij3[i][j];
			}
			gij[m + t][j] = beforeij3[m + t - 1][j];
		}
		beforeij3 = gij;
	}

	//打印gij
	/*ui.plainTextEdit_2->appendPlainText("\ngij");
	for (const std::vector<double>& l : gij) {
		QString string;
		for (int i = 0; i < n + 1; i++) {
			string = string + QString::number(l[i]) + ",";
		}
		ui.plainTextEdit_2->appendPlainText(string);
	}*/

	//判断st在0.0和1.0参数之间是否存在切线-------------------------------
	bool flag1;
	fij[0][0] > 0.0 ? flag1 = true : flag1 = false;//正数则true，负数则false
	bool flag2;
	gij[0][0] > 0.0 ? flag2 = true : flag2 = false;
	bool shouldbreakf = false;//如果有异号或0则true
	bool shouldbreakg = false;

	double max1 = std::abs(fij[0][0]);//设置初始值，后续求c
	double max2 = std::abs(gij[0][0]);

	for (int i = 0; i < 2 * m; i++) {
		for (int j = 0; j < n + 1; j++) {
			if (fij[i][j] >= 0.0 && !flag1 || fij[i][j] <= 0.0 && flag1) {//有0或者异号
				shouldbreakf = true;//区间判断通过
			}
			if (gij[i][j] >= 0.0 && !flag2 || gij[i][j] <= 0.0 && flag2) {
				shouldbreakg = true;//区间判断通过
			}
			//有更大绝对值则更新max
			if (abs(fij[i][j]) > max1) {
				max1 = abs(fij[i][j]);
			}
			if (abs(gij[i][j]) > max2) {
				max2 = abs(gij[i][j]);
			}
		}
	}

	//fij+c*gij
	std::vector<std::vector<double>> fc(2 * m, std::vector<double>(n + 1));
	//fij-c*gij
	std::vector<std::vector<double>> f_c(2 * m, std::vector<double>(n + 1));

	//前面通过才有后续，否则直接跳到词if的else输出无公切
	if (shouldbreakf && shouldbreakg) {

		double c = max1 / max2;//系数
		//打印系数
		/*ui.plainTextEdit_2->appendPlainText("\nc = " + QString::number(max1) + " / " + QString::number(max2)
			+ " = " + QString::number(c));*/

			//求fij+c*gij和fij-c*gij
		for (int i = 0; i < 2 * m; i++) {
			for (int j = 0; j < n + 1; j++) {
				fc[i][j] = fij[i][j] + c * gij[i][j];
				f_c[i][j] = fij[i][j] - c * gij[i][j];
			}
		}

		//打印fc和f_c
		/*ui.plainTextEdit_2->appendPlainText("\nfc");
		for (const std::vector<double>& l : fc) {
			QString string;
			for (int i = 0; i < n + 1; i++) {
				string = string + QString::number(l[i]) + ",";
			}
			ui.plainTextEdit_2->appendPlainText(string);
		}
		ui.plainTextEdit_2->appendPlainText("\nf_c");
		for (const std::vector<double>& l : f_c) {
			QString string;
			for (int i = 0; i < n + 1; i++) {
				string = string + QString::number(l[i]) + ",";
			}
			ui.plainTextEdit_2->appendPlainText(string);
		}*/

		//同样的二重循环判断
		bool flag3;
		fc[0][0] > 0.0 ? flag3 = true : flag3 = false;//正数则true，负数则false
		bool flag4;
		f_c[0][0] > 0.0 ? flag4 = true : flag4 = false;
		bool shouldbreakfc = false;//如果有异号或0则true
		bool shouldbreakf_c = false;//如果有异号或0则true

		for (int i = 0; i < 2 * m && !(shouldbreakfc && shouldbreakf_c); i++) {//shouldbreak2为true之后也直接break
			for (int j = 0; j < n + 1; j++) {
				if (fc[i][j] >= 0.0 && !flag3 || fc[i][j] <= 0.0 && flag3) {//满足有符号不一样
					shouldbreakfc = true;
				}
				if (f_c[i][j] >= 0.0 && !flag4 || f_c[i][j] <= 0.0 && flag4) {
					shouldbreakf_c = true;//区间判断通过
				}
				if (shouldbreakfc && shouldbreakf_c) {
					return 'y';//输出
					//return;//这里没有max需要算，直接退出
				}
			}
		}

		//这里输出
		//if (shouldbreakfc && shouldbreakf_c) {
		//	//有公切线
		//}
		//else {
		return '2';//没有
		//}

	}
	else {//直接输出没有公切线
		return '1';
	}

	//}
}

//判断是否有切线
//可以整个改成一个函数，输入输出为曲线指针和参数范围
void QtWidgetsApplication6::on_pushButton_23_clicked() {
	if (ui.openGLWidget->curve[1]
		&& ui.openGLWidget->curve[0]->isCurveAvailable()
		&& ui.openGLWidget->curve[1]->isCurveAvailable()) {
		char res0 = inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], 0.0, 1.0, 0.0, 1.0);
		char res1 = inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], 0.0, 0.5, 0.0, 0.5);
		char res2 = inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], 0.0, 0.5, 0.5, 1.0);
		char res3 = inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], 0.5, 1.0, 0.0, 0.5);
		char res4 = inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], 0.5, 1.0, 0.5, 1.0);
		ui.lineEdit_14->setText(QString(res0)+":"+ QString(res1)+ QString(res2)+QString(res3)+QString(res4));
	}

	//if (ui.openGLWidget->curve[1]
	//	&& ui.openGLWidget->curve[0]->isCurveAvailable()
	//	&& ui.openGLWidget->curve[1]->isCurveAvailable()) {//将第一组控制顶点最为P(s)的控制顶点

	//	//对于s
	//	double s0 = 0.0;
	//	double s1 = 1.0;
	//	double s = 0.5;
	//	double s_ = (s1 - s) / (s1 - s0);
	//	//对于t
	//	double t0 = 0.0;
	//	double t1 = 1.0;
	//	double t = 0.5;
	//	double t_ = (t1 - t) / (t1 - t0);

	//	//-------------------------------------------------------------------

	//	//f1(s,t)的系数fij，其中i为s的次数，j为t的次数
	//	//对于f1(s,t) = P(s) * P'(s)T
	//	//通过B升次是两B次数相差1，在将B合并
	//	std::vector<Point> P = ui.openGLWidget->curve[0]->getControlPoints();//P(s)的控制顶点
	//	int pS = P.size();//P的size
	//	double m = pS - 1;//P(s)的次数，类型double方便后续计算

	//	std::vector<Point> Q = ui.openGLWidget->curve[1]->getControlPoints();//Q(t)的控制顶点
	//	int qS = Q.size();//Q的size
	//	double n = qS - 1;//Q(t)的次数，类型double方便后续计算

	//	std::vector<double> tempij1;//先求新的Pi之后再求Qij
	//	tempij1.resize(2 * m, 0.0);//(2*m-1,0)注意个数要是次数加1

	//	for (int i = 0; i < m + 1; i++) {//计算的时候针对的是P(s)*P'(s)T
	//		for (int j = 0; j < m; j++) {
	//			tempij1[i + j] += (double)C(m, i) * (double)C(m - 1, j) / (double)C(2 * m - 1, i + j)//B由两个相乘化为2*m-1
	//				* (double)m / (s1 - s0)//求导前系数
	//				* (P[i][0] * (-P[j + 1][1] + P[j][1]) +
	//					P[i][1] * (P[j + 1][0] - P[j][0]));
	//		}
	//	}

	//	std::vector<std::vector<double>> fij1;//Qij

	//	//(2*m-1,0)->(2*m-1,n)
	//	for (int i = 0; i < 2 * m; i++) {//共2m行
	//		fij1.push_back(std::vector<double>(n + 1, tempij1[i]));//每行都一样
	//	}

	//	//-------------------------------------------------------------------
	//	//对于f2(s,t) = Q(t) * P'(s)T
	//	//f2(s,t)的系数fij，其中i为s的次数，j为t的次数

	//	//BezierCurve bezierCurveQ(Q);//创建Q(t)的bezier曲线
	//	//bezierCurveQ.update();

	//	//std::vector<std::vector<double>> tempij2(m, std::vector<double>(n + 1));//(m-1,n)m行n+1列
	//	//只使用(m-1,n)m行n+1列，其他为0.0
	//	std::vector<std::vector<double>> beforeij2(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

	//	for (int i = 0; i < m; i++) {
	//		for (int j = 0; j < n + 1; j++) {
	//			beforeij2[i][j] = (double)m / (s1 - s0)
	//				* (Q[j][0] * (-P[i + 1][1] + P[i][1]) + Q[j][1] * (P[i + 1][0] - P[i][0]));
	//		}
	//	}

	//	std::vector<std::vector<double>> fij2(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

	//	for (int t = 0; t < m; t++) {
	//		for (int j = 0; j < n + 1; j++) {
	//			fij2[0][j] = beforeij2[0][j];
	//			for (int i = 1; i < m + t; i++) {
	//				fij2[i][j] = (double)i / ((double)m + (double)t) * beforeij2[i - 1][j]
	//					+ (1.0 - (double)i / ((double)m + (double)t)) * beforeij2[i][j];
	//			}
	//			fij2[m + t][j] = beforeij2[m + t - 1][j];
	//		}
	//		beforeij2 = fij2;
	//	}

	//	std::vector<std::vector<double>> fij(2 * m, std::vector<double>(n + 1));
	//	for (int i = 0; i < 2 * m; i++) {
	//		for (int j = 0; j < n + 1; j++) {
	//			fij[i][j] = fij1[i][j] - fij2[i][j];
	//		}
	//	}

	//	//打印fij
	//	ui.plainTextEdit_2->appendPlainText("\nfij");
	//	for (const std::vector<double>& l : fij) {
	//		QString string;
	//		for (int i = 0; i < n + 1; i++) {
	//			string = string + QString::number(l[i]) + ",";
	//		}
	//		ui.plainTextEdit_2->appendPlainText(string);
	//	}

	//	//g(s,t) = P'(s) * Q'(t)T = 0=============================================================
	//	//(m-1,n-1)
	//	std::vector<std::vector<double>> beforeij3(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

	//	for (int i = 0; i < m; i++) {
	//		for (int j = 0; j < n; j++) {
	//			beforeij3[i][j] = (double)m / (s1 - s0) * (double)n / (t1 - t0)
	//				* ((P[i + 1][0] - P[i][0]) * (-Q[j + 1][1] + Q[j][1]) + (P[i + 1][1] - P[i][1]) * (Q[j + 1][0] - Q[j][0]));
	//		}
	//	}

	//	//(m-1,n-1)->(2*m-1,n)
	//	std::vector<std::vector<double>> gij(2 * m, std::vector<double>(n + 1, 0.0));

	//	//1：(m-1,n-1)->(m-1,n)
	//	for (int i = 0; i < m; i++) {
	//		gij[i][0] = beforeij3[i][0];
	//		for (int j = 1; j < n; j++) {
	//			gij[i][j] = (double)j / (double)n * beforeij3[i][j - 1]
	//				+ (1.0 - (double)j / (double)n) * beforeij3[i][j];
	//		}
	//		gij[i][n] = beforeij3[i][n - 1];
	//	}
	//	beforeij3 = gij;

	//	//2：(m-1,n)->(2*m-1,n)
	//	for (int t = 0; t < m; t++) {
	//		for (int j = 0; j < n + 1; j++) {
	//			gij[0][j] = beforeij3[0][j];
	//			for (int i = 1; i < m + t; i++) {
	//				gij[i][j] = (double)i / ((double)m + (double)t) * beforeij3[i - 1][j]
	//					+ (1.0 - (double)i / ((double)m + (double)t)) * beforeij3[i][j];
	//			}
	//			gij[m + t][j] = beforeij3[m + t - 1][j];
	//		}
	//		beforeij3 = gij;
	//	}

	//	//打印gij
	//	ui.plainTextEdit_2->appendPlainText("\ngij");
	//	for (const std::vector<double>& l : gij) {
	//		QString string;
	//		for (int i = 0; i < n + 1; i++) {
	//			string = string + QString::number(l[i]) + ",";
	//		}
	//		ui.plainTextEdit_2->appendPlainText(string);
	//	}

	//	//判断st在0.0和1.0参数之间是否存在切线-------------------------------
	//	bool flag1;
	//	fij[0][0] > 0.0 ? flag1 = true : flag1 = false;//正数则true，负数则false
	//	bool flag2;
	//	gij[0][0] > 0.0 ? flag2 = true : flag2 = false;
	//	bool shouldbreakf = false;//如果有异号或0则true
	//	bool shouldbreakg = false;

	//	double max1 = std::abs(fij[0][0]);//设置初始值，后续求c
	//	double max2 = std::abs(gij[0][0]);

	//	for (int i = 0; i < 2 * m; i++) {
	//		for (int j = 0; j < n + 1; j++) {
	//			if (fij[i][j] >= 0.0 && !flag1 || fij[i][j] <= 0.0 && flag1) {//有0或者异号
	//				shouldbreakf = true;//区间判断通过
	//			}
	//			if (gij[i][j] >= 0.0 && !flag2 || gij[i][j] <= 0.0 && flag2) {
	//				shouldbreakg = true;//区间判断通过
	//			}
	//			//有更大绝对值则更新max
	//			if (abs(fij[i][j]) > max1) {
	//				max1 = abs(fij[i][j]);
	//			}
	//			if (abs(gij[i][j]) > max2) {
	//				max2 = abs(gij[i][j]);
	//			}
	//		}
	//	}

	//	//fij+c*gij
	//	std::vector<std::vector<double>> fc(2 * m, std::vector<double>(n + 1));
	//	//fij-c*gij
	//	std::vector<std::vector<double>> f_c(2 * m, std::vector<double>(n + 1));

	//	//前面通过才有后续，否则直接跳到词if的else输出无公切
	//	if (shouldbreakf&&shouldbreakg) {

	//		double c = max1 / max2;//系数
	//		ui.plainTextEdit_2->appendPlainText("\nc = " + QString::number(max1) + " / " + QString::number(max2)
	//			+ " = " + QString::number(c));
	//		 
	//		//求fij+c*gij和fij-c*gij
	//		for (int i = 0; i < 2 * m; i++) {
	//			for (int j = 0; j < n + 1; j++) {
	//				fc[i][j] = fij[i][j] + c * gij[i][j];
	//				f_c[i][j] = fij[i][j] - c * gij[i][j];
	//			}
	//		}

	//		//打印fc和f_c
	//		ui.plainTextEdit_2->appendPlainText("\nfc");
	//		for (const std::vector<double>& l : fc) {
	//			QString string;
	//			for (int i = 0; i < n + 1; i++) {
	//				string = string + QString::number(l[i]) + ",";
	//			}
	//			ui.plainTextEdit_2->appendPlainText(string);
	//		}
	//		ui.plainTextEdit_2->appendPlainText("\nf_c");
	//		for (const std::vector<double>& l : f_c) {
	//			QString string;
	//			for (int i = 0; i < n + 1; i++) {
	//				string = string + QString::number(l[i]) + ",";
	//			}
	//			ui.plainTextEdit_2->appendPlainText(string);
	//		}

	//		//同样的二重循环判断
	//		bool flag3;
	//		fc[0][0] > 0.0 ? flag3 = true : flag3 = false;//正数则true，负数则false
	//		bool flag4;
	//		f_c[0][0] > 0.0 ? flag4 = true : flag4 = false;
	//		bool shouldbreakfc = false;//如果有异号或0则true
	//		bool shouldbreakf_c = false;//如果有异号或0则true

	//		for (int i = 0; i < 2 * m && !(shouldbreakfc&& shouldbreakf_c); i++) {//shouldbreak2为true之后也直接break
	//			for (int j = 0; j < n + 1; j++) {
	//				if (fc[i][j] >= 0.0 && !flag3 || fc[i][j] <= 0.0 && flag3) {//满足有符号不一样
	//					shouldbreakfc = true;
	//				}
	//				if (f_c[i][j] >= 0.0 && !flag4 || f_c[i][j] <= 0.0 && flag4) {
	//					shouldbreakf_c = true;//区间判断通过
	//				}
	//				if (shouldbreakfc && shouldbreakf_c) {
	//					ui.lineEdit_14->setText("maybeyes");//输出
	//					return;//这里没有max需要算，直接退出
	//				}
	//			}
	//		}

	//		//这里输出
	//		//if (shouldbreakfc && shouldbreakf_c) {
	//		//	//有公切线
	//		//}
	//		//else {
	//		ui.lineEdit_14->setText("no2");//没有
	//		//}

	//	}
	//	else {//直接输出没有公切线
	//		ui.lineEdit_14->setText("no1");
	//	}

	//}
}

//细分三次区间判断区间是否有切线
void QtWidgetsApplication6::on_pushButton_24_clicked() {

	//前提：两条曲线
	if (ui.openGLWidget->curve[1]
		&& ui.openGLWidget->curve[0]->isCurveAvailable()
		&& ui.openGLWidget->curve[1]->isCurveAvailable()) {

		drawMesh = true;

		//网格初始化（放大和位移网格到左上角：x:s*5.0-5.0，y:t*5.0）
		double s0 = 0.0;
		double s1 = 1.0;
		double t0 = 0.0;
		double t1 = 1.0;
		InitialMesh(ui.openGLWidget->M, 2, 2, s0 * 8.0, s1 * 8.0, t0 * 8.0, t1 * 8.0);
		//vector<int> facesRemained;//保留的面
		//vector<int> crossFaces;//记录每次要十字剖分的面

		//判断（0）：s、t0.0到1.0判断
		//ui.lineEdit_14->setText(QString(res0));
		if (inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], s0, s1, t0, t1) == 'y') {//保留
			//facesRemained.push_back(0);//第一个面为0
			//crossFaces.push_back(0);//后续需要十字剖分
		}
		else {
			return;//否则直接结束
		}

		//判断（1）：上次判断之后保留的继续cross
		vector<int> facesDeleted;//已删除的面
		vector<CFace> allFaces;//现有的所有面
		vector<CVertex> allVers;//现有的所有点
		vector<int> crossFaces = {0};//需要十字剖分的面，事先保存则不需额外循环

		for (int depth = 0; depth < 3; depth++) {
			myRefine_QQ_Cross(ui.openGLWidget->M, crossFaces);//第一次剖分id为0的face即可
			crossFaces.clear();
			//更新面、点
			allFaces = ui.openGLWidget->M.m_Faces;//现有的所有面
			allVers = ui.openGLWidget->M.m_Vertices;//现有的所有点
			for (const CFace& f : allFaces) {//遍历所有现有面
				bool flag = true;
				for (const int& d : facesDeleted) {
					if (f.m_nFaceid == d) {//删除
						flag = false;
					}
				}
				if (flag) {//没有删除
					CVertex cv = allVers[f.m_nFourIndex[0]];
					s0 = cv.m_dVerx / 8.0;
					s1 = cv.m_dVerx / 8.0;
					t0 = cv.m_dVery / 8.0;
					t1 = cv.m_dVery / 8.0;
					for (int j = 1; j < 4; j++) {//更新此处s0、s1、t0、t1
						//int index = f.m_nFourIndex[j];
						CVertex cv = allVers[f.m_nFourIndex[j]];
						if (cv.m_dVerx / 8.0 < s0) s0 = cv.m_dVerx / 8.0;
						if (cv.m_dVerx / 8.0 > s1) s1 = cv.m_dVerx / 8.0;
						if (cv.m_dVery / 8.0 < t0) t0 = cv.m_dVery / 8.0;
						if (cv.m_dVery / 8.0 > t1) t1 = cv.m_dVery / 8.0;
					}
					//若区间保留
					if (inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], s0, s1, t0, t1) == 'y') {
						crossFaces.push_back(f.m_nFaceid);
					}
					else {
						facesDeleted.push_back(f.m_nFaceid);
					}
				}
			}
		}
		
		for (const CFace& f : allFaces) {//遍历所有现有面
			for (const int& d : facesDeleted) {
				if (f.m_nFaceid != d) {//未删除
					ui.openGLWidget->remainid.push_back(f.m_nFaceid);
				}
			}
		}

		ui.openGLWidget->update();//刷新窗口
	}

}