#include "QtWidgetsApplication6.h"

//ȫ�ֱ�����¼��ǰ��������
int curveIndex = 0;
//ȫ�ֱ�����¼��ǰ��������
int curveCount = 0;
//�Ƿ���ƿ��ƶ����
bool isPolygonVisible = true;
//�Ƿ���ʾBezierExtraction���
bool isBezierVisible = true;
//���ƹ���ΪB-Spline����bezier��Ĭ��B-Spline
bool drawWhich = true;

//����mesh
bool drawMesh = false;

QtWidgetsApplication6::QtWidgetsApplication6(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//�����¿��ƶ����ź����
	connect(ui.openGLWidget, &OpenGLWidget::pointTextReady, this, &QtWidgetsApplication6::handlePointText);
	//Ĭ���ʼ���Ƶ�һ������
	ui.lineEdit_3->setText("1");
	//Ĭ���ʼ��������Ϊ0
	ui.lineEdit_4->setText("1");
}

QtWidgetsApplication6::~QtWidgetsApplication6()
{}

//----------------------------�ź���ۺ���----------------------------

//������ť
void QtWidgetsApplication6::on_pushButton_clicked() {

	//����
	QString inputContent = ui.lineEdit->text();

	//�����߻�δ����
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable() || inputContent == "") {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : Not derivable!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}

	std::vector<double> u = { inputContent.toDouble() };//�õ�����
	//�������λ��0.0�����������������ֱ����0.0���ܳ��ִ���
	if (u[0] == 0.0) {
		u[0] = 1e-15;
	}

	std::vector<double> derivative = ui.openGLWidget->curve[curveIndex]->getDerivative(u);//�����󵼽ӿ�
	//QString outputContent = "(" + QString::number(derivative[0]) + "," + QString::number(derivative[1]) + "," + QString::number(derivative[2]) + ")";

	//���
	QString outputContent = "(";
	for (const double i : derivative) {
		outputContent.push_back(QString::number(i) + ",");
	}
	outputContent.chop(1);
	outputContent.push_back(")");
	ui.lineEdit_2->setText(outputContent);
}

//��������ť
void QtWidgetsApplication6::on_pushButton_2_clicked() {

	QString inputContent = ui.lineEdit->text();

	//�����߻�δ����
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable() || inputContent == "") {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : Unable to ask index!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
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

//�л����߰�ť
void QtWidgetsApplication6::on_pushButton_3_clicked() {
	if (ui.lineEdit_5->text().toInt() - 1 > curveCount || ui.lineEdit_5->text().toInt() < 1) {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : No such curve!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}
	curveIndex = ui.lineEdit_5->text().toInt() - 1;
	//��ʾ��ǰ�����Ѿ��л�
	ui.lineEdit_3->setText(QString::number(curveIndex + 1));
	//������ʾ�Ŀ��ƶ�������
	ui.plainTextEdit->clear();
	std::vector<Point> controlPoints = ui.openGLWidget->curve[curveIndex]->getControlPoints();
	for (const Point& point : controlPoints) {
		QString pointText = "(" + QString::number(point[0]) + "," + QString::number(point[1]) + "," + QString::number(point[2]) + ")";
		ui.plainTextEdit->appendPlainText(pointText);
	}
}

//��յ�ǰ���߿��Ƶ㰴ť
void QtWidgetsApplication6::on_pushButton_4_clicked() {

	ui.openGLWidget->inflectionPoint[curveIndex]->clear();

	//ui.openGLWidget->commonTangent.clear();

	ui.openGLWidget->curve[curveIndex]->clearControlPoints();//���controlPoints
	ui.openGLWidget->curve[curveIndex]->update();//�����������

	//ui.openGLWidget->bezierCurve->clearBezierCurve();//���bezier extraction���

	ui.openGLWidget->update();//�����ʾ����

	ui.plainTextEdit->clear();//�����ʾ�Ŀ��ƶ�������
	ui.lineEdit_9->clear();//��չյ����λ��

}

//�����߰�ť
void QtWidgetsApplication6::on_pushButton_5_clicked() {

	ui.openGLWidget->commonTangent.clear();

	//�����߿��ܳ��ֵ��쳣����************************************

	int curve_1 = ui.lineEdit_6->text().toInt() - 1;//�õ���һ����������
	int curve_2 = ui.lineEdit_7->text().toInt() - 1;//�õ��ڶ�����������
	if (ui.lineEdit_6->text() != "" && ui.lineEdit_7->text() != "" && curve_1 <= curveCount && curve_2 <= curveCount &&
		ui.openGLWidget->curve[curve_1]->isCurveAvailable() && ui.openGLWidget->curve[curve_2]->isCurveAvailable()) {
		double numPoints = 500.0;
		std::vector<std::vector<double>> paramPoints;
		double deltaU = 1.0 / numPoints;//���
		for (int i = 0; i < numPoints; i++) {
			double u = i * deltaU;
			std::vector<double> param = { u }; // ������ֻ��һ��uֵ
			paramPoints.push_back(param);
		}
		//paramPoints.push_back({ 1.0 });
		for (const std::vector<double>& u_1 : paramPoints) {
			for (const std::vector<double>& u_2 : paramPoints) {

				//����1��u_1λ�õ���
				std::vector<double> derivative_1 = ui.openGLWidget->curve[curve_1]->getDerivative(u_1);
				//����2��u_2λ�õ���
				std::vector<double> derivative_2 = ui.openGLWidget->curve[curve_2]->getDerivative(u_2);
				//����1��u_1λ�õ�����
				Point point_1 = ui.openGLWidget->curve[curve_1]->getPoint(u_1);
				//����2��u_2λ�õ�����
				Point point_2 = ui.openGLWidget->curve[curve_2]->getPoint(u_2);
				//��������
				std::vector<double> lineVector(3);
				lineVector[0] = point_2[0] - point_1[0];
				lineVector[1] = point_2[1] - point_1[1];
				lineVector[2] = point_2[2] - point_1[2];

				double errorThreshold = 1; // �����ֵ

				//���
				double crossProduct_1 = derivative_1[0] * derivative_2[1] -
					derivative_1[1] * derivative_2[0];
				double crossProduct_2 = derivative_1[0] * lineVector[1] -
					derivative_1[1] * lineVector[0];

				if (std::abs(crossProduct_1) < errorThreshold && std::abs(crossProduct_2) < errorThreshold) {
					// point_1 �� point_2 ������ͬ��������
					ui.openGLWidget->commonTangent.push_back(point_1);
					ui.openGLWidget->commonTangent.push_back(point_2);
				}
				else {
					continue;
					// ������ͬ��������
				}
			}
		}
	}
	else {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : Unable to find the common tangent!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}
	ui.openGLWidget->update();
	ui.lineEdit_8->setText("Done");
}

//�½����߰�ť
void QtWidgetsApplication6::on_pushButton_6_clicked() {
	curveCount++;
	ui.openGLWidget->curve[curveCount] = new BSplineCurve();
	ui.openGLWidget->inflectionPoint[curveCount] = new std::vector<Point>();
	curveIndex = curveCount;
	ui.lineEdit_3->setText(QString::number(curveIndex + 1));//��ʾ��ǰ��������
	ui.lineEdit_4->setText(QString::number(curveCount + 1));//��ʾ��ǰ��������
	ui.plainTextEdit->clear();//�����ʾ�Ŀ��ƶ�������
}

//�����갴ť
void QtWidgetsApplication6::on_pushButton_7_clicked() {

	QString inputContent = ui.lineEdit->text();

	//�����߻�δ����
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable() || inputContent == "") {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : Unable to get the coordinates!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}

	std::vector<double> u = { ui.lineEdit->text().toDouble() };
	Point point = ui.openGLWidget->curve[curveIndex]->getPoint(u);
	QString outputContent = "(" + QString::number(point[0]) + "," + QString::number(point[1]) + "," + QString::number(point[2]) + ")";
	ui.lineEdit_2->setText(outputContent);
}

//�Ƿ���ʾ���ƶ����
void QtWidgetsApplication6::on_pushButton_8_clicked() {
	isPolygonVisible ? isPolygonVisible = false : isPolygonVisible = true;
	ui.openGLWidget->update();
}

//��յ㰴ť
void QtWidgetsApplication6::on_pushButton_9_clicked() {

	//�����߻�δ����
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable()) {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : Unable to get the inflection points!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}

	//��յ����λ��
	std::vector<double> inflectionPointU;
	std::vector<std::vector<double>> angleRange;
	ui.openGLWidget->curve[curveIndex]->getInflectionPoint(inflectionPointU, angleRange);

	std::vector<Point> tempP;//��ʱ�洢�յ���������
	for (const double ipu : inflectionPointU) {
		tempP.push_back(ui.openGLWidget->curve[curveIndex]->getPoint({ ipu }));
	}
	ui.openGLWidget->inflectionPoint[curveIndex] = new std::vector<Point>(tempP);//������OpenGLWidget

	//����յ��������
	QString tempUS = "{";//String
	for (const double ipu : inflectionPointU) {
		tempUS.push_back(QString::number(ipu) + ",");
	}
	tempUS.chop(1);
	tempUS.push_back("}");
	ui.lineEdit_9->setText(tempUS);

	//���0.0�������յ㵽1.0����λ�ýǶȷ�Χ
	for (const std::vector<double> a : angleRange) {
		ui.plainTextEdit_2->appendPlainText(QString::number(a[0]) + "," + QString::number(a[1]) + "," + QString::number(a[2]) + "\n");
	}

	ui.openGLWidget->update();//ˢ�´���

}

//�ֳ�c������
void QtWidgetsApplication6::on_pushButton_12_clicked() {

	//�����߻�δ����
	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable()) {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : Unable to get the C-shaped curves");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}

	//��յ����λ��
	std::vector<double> inflectionPointU;
	std::vector<std::vector<double>> angleRange;
	ui.openGLWidget->curve[curveIndex]->getCShapedCurve(inflectionPointU, angleRange);

	std::vector<Point> tempP;//��ʱ�洢�յ���������
	for (const double ipu : inflectionPointU) {
		tempP.push_back(ui.openGLWidget->curve[curveIndex]->getPoint({ ipu }));
	}
	ui.openGLWidget->inflectionPoint[curveIndex] = new std::vector<Point>(tempP);//������OpenGLWidget

	//����յ��������
	QString tempUS = "{";//String
	for (const double ipu : inflectionPointU) {
		tempUS.push_back(QString::number(ipu) + ",");
	}
	tempUS.chop(1);
	tempUS.push_back("}");
	ui.lineEdit_11->setText(tempUS);

	//���0.0�������յ㵽1.0����λ�ýǶȷ�Χ
	for (const std::vector<double> a : angleRange) {
		ui.plainTextEdit_2->appendPlainText(QString::number(a[0]) + "," + QString::number(a[1]) + "," + QString::number(a[2]) + "\n");
	}

	ui.openGLWidget->update();//ˢ�´���
}

//���غϽǶ�
void QtWidgetsApplication6::on_pushButton_11_clicked() {

	std::vector<std::vector<double>> out;//���

	int curve_1 = 0;//��һ����������
	int curve_2 = 1;//�ڶ�����������

	std::vector<std::vector<double>> overlappingAngles;//�洢�غϵĽǶ�

	if (curve_1 <= curveCount && curve_2 <= curveCount && curve_1 != curve_2 &&
		ui.openGLWidget->curve[curve_1]->isCurveAvailable() && ui.openGLWidget->curve[curve_2]->isCurveAvailable()) {

		//��Ƕ�
		std::vector<double> inflectionPointU;//���ã�Ϊ�ܵ���getInflectionPoint����
		std::vector<std::vector<double>> angleRange_1;//����1���нǶ�����
		std::vector<std::vector<double>> angleRange_2;//����2���нǶ�����
		ui.openGLWidget->curve[curve_1]->getCShapedCurve(inflectionPointU, angleRange_1);
		ui.openGLWidget->curve[curve_2]->getCShapedCurve(inflectionPointU, angleRange_2);

		int size_1 = angleRange_1.size();
		int size_2 = angleRange_2.size();

		for (int cIndex_1 = 0; cIndex_1 < size_1; cIndex_1++) {
			for (int cIndex_2 = 0; cIndex_2 < size_2; cIndex_2++) {

				if (angleRange_1[cIndex_1][2] == 1.0 && angleRange_2[cIndex_2][2] != 1.0) {//����1��360
					if (angleRange_2[cIndex_2][0] < angleRange_1[cIndex_1][1]) {//����2������1�ϰ벿���н���
						if (angleRange_2[cIndex_2][1] > angleRange_1[cIndex_1][1]) {//����2��������1�ϰ�
							overlappingAngles.push_back({ angleRange_2[cIndex_2][0] ,angleRange_1[cIndex_1][1] ,0.0 });
						}
						else {//����2ȫ������
							overlappingAngles.push_back({ angleRange_2[cIndex_2][0] ,angleRange_2[cIndex_2][1] ,0.0 });
						}
					}
					else if (angleRange_2[cIndex_2][1] > angleRange_1[cIndex_1][0]) {//����2������1�°벿���н���
						if (angleRange_2[cIndex_2][0] < angleRange_1[cIndex_1][0]) {//����2��������1�°�
							overlappingAngles.push_back({ angleRange_1[cIndex_1][0] ,angleRange_2[cIndex_2][1] ,0.0 });
						}
						else {//����2ȫ������
							overlappingAngles.push_back({ angleRange_2[cIndex_2][0] ,angleRange_2[cIndex_2][1] ,0.0 });
						}
					}
					continue;
				}
				else if (angleRange_1[cIndex_1][2] != 1.0 && angleRange_2[cIndex_2][2] == 1.0) {//����2��360
					if (angleRange_1[cIndex_1][0] < angleRange_2[cIndex_2][1]) {//����1������2�ϰ벿���н���
						if (angleRange_1[cIndex_1][1] > angleRange_2[cIndex_2][1]) {//����1��������2�ϰ�
							overlappingAngles.push_back({ angleRange_1[cIndex_1][0] ,angleRange_2[cIndex_2][1] ,0.0 });
						}
						else {//����1ȫ������
							overlappingAngles.push_back({ angleRange_1[cIndex_1][0] ,angleRange_1[cIndex_1][1] ,0.0 });
						}
					}
					else if (angleRange_1[cIndex_1][1] > angleRange_2[cIndex_2][0]) {//����1������2�°벿���н���
						if (angleRange_1[cIndex_1][0] < angleRange_2[cIndex_2][0]) {//����1��������2�°�
							overlappingAngles.push_back({ angleRange_2[cIndex_2][0] ,angleRange_1[cIndex_1][1] ,0.0 });
						}
						else {//����1ȫ������
							overlappingAngles.push_back({ angleRange_1[cIndex_1][0] ,angleRange_1[cIndex_1][1] ,0.0 });
						}
					}
					continue;
				}
				else {
					double max_0 = std::max(angleRange_1[cIndex_1][0], angleRange_2[cIndex_2][0]);
					double min_1 = std::min(angleRange_1[cIndex_1][1], angleRange_2[cIndex_2][1]);
					if (angleRange_1[cIndex_1][2] == 1.0 && angleRange_2[cIndex_2][2] == 1.0) {//�����߾�����360��������غ�
						overlappingAngles.push_back({ max_0,min_1 ,1.0 });
					}
					else if (max_0 < min_1) {//��������360
						overlappingAngles.push_back({ max_0,min_1 ,0.0 });
					}
				}

			}
		}

	}
	else {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : Unable to find overlapping angle intervals!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}

	//���һ��
	for (const std::vector<double>& line : overlappingAngles) {
		ui.plainTextEdit_2->appendPlainText(
			QString::number(line[0]) + "," + QString::number(line[1]) + "," + QString::number(line[2]));
	}

	ui.openGLWidget->update();//ˢ�´���
}


//BezierExtraction
void QtWidgetsApplication6::on_pushButton_13_clicked() {

	if (!ui.openGLWidget->curve[curveIndex]->isCurveAvailable()) {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : Bezier Extraction cannot be implemented!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}

	ui.openGLWidget->curve[curveIndex]->bezierExtractCpts();//��BSplineCurve��׼��CpbsA

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

	ui.openGLWidget->update();//ˢ�´���
}

//������B-Spline�еĵ�һ��Bezier�Ƿ�������
void QtWidgetsApplication6::on_pushButton_14_clicked() {

	//����b�����ѻ���
	if (curveCount > 0
		&& ui.openGLWidget->curve[0]->isCurveAvailable()
		&& ui.openGLWidget->curve[1]->isCurveAvailable()) {

		//����bezier extraction�����շ�װ������CpbsA��Intervals����BSplineCurve��������׼����
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

	double errorThreshold = 1.0; // �����ֵ

	int size_1 = cpbsA_1.size();
	int size_2 = cpbsA_2.size();
	//��������bezier
	for (int i = 0; i < size_1; i++) {
		for (int j = 0; j < size_2; j++) {

			//cpbsA_1��cpbsA_2�зֱ���size_1��size_2����ƶ���
			std::vector<Point> controlPoints_1 = cpbsA_1[i];
			std::vector<Point> controlPoints_2 = cpbsA_2[j];

			//��������controlPoints��ʽ
			BezierCurve bezierCurve_1(controlPoints_1);
			BezierCurve bezierCurve_2(controlPoints_2);
			//bezierCurve_1.setBezierControlPoints(controlPoints_1);
			//bezierCurve_2.setBezierControlPoints(controlPoints_2);

			//������
			//�Դ������ʽ��ͼƬ���ֻ���
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

	//����������߶˵��������ϵ�����
	for (const Point& p : ui.openGLWidget->tantan) {
		ui.plainTextEdit_2->appendPlainText("tantan:" + QString::number(p[0]) + "," + QString::number(p[1]) + "," + QString::number(p[2]));
	}

	ui.openGLWidget->update();//ˢ�´���
}

//���ػ�����ʾBezier
void QtWidgetsApplication6::on_pushButton_15_clicked() {

	isBezierVisible ? isBezierVisible = false : isBezierVisible = true;//�޸�bool����isBezierVisible��ֵ
	ui.openGLWidget->update();//ˢ�´���

}

//��bezier���߷ָ��������
void QtWidgetsApplication6::on_pushButton_16_clicked() {

	BezierCurve bezier;
	bezier.setBezierControlPoints(ui.openGLWidget->curve[0]->getControlPoints());

	//���
	ui.openGLWidget->control_1.clear();
	ui.openGLWidget->control_2.clear();

	//�ָ����÷ָ�λ��Ϊ0.3
	bezier.extract(ui.openGLWidget->control_1, ui.openGLWidget->control_2, 0.3);

	ui.openGLWidget->update();//ˢ�´���
}

//���ܣ�B-Spline��ذ���extract֮��Ļ���/bezierֱ�ӻ���
void QtWidgetsApplication6::on_pushButton_17_clicked() {
	drawWhich ? drawWhich = false : drawWhich = true;//�޸�bool����drawWhich��ֵ
	ui.openGLWidget->update();//ˢ�´���
}

//��bezier������ĩ�㣨Ϊ����֤bezier���Ƶ����⣬�ڽ�β�������һС�οհ����⣬�ѽ����
void QtWidgetsApplication6::on_pushButton_18_clicked() {
	Point p1 = ui.openGLWidget->bezierCurve.getPoint(0.0);//�����׵�
	Point p2 = ui.openGLWidget->bezierCurve.getPoint(1.0);//����ĩ��

	std::vector<Point> pa = ui.openGLWidget->bezierCurve.getControlPoints();
	Point p3 = pa.front();//���ƶ����׵�
	Point p4 = pa.back();//���ƶ���ĩ��

	ui.plainTextEdit_2->appendPlainText("p1:" + QString::number(p1[0]) + "," + QString::number(p1[1]) + "," + QString::number(p1[2]));
	ui.plainTextEdit_2->appendPlainText("p2:" + QString::number(p2[0]) + "," + QString::number(p2[1]) + "," + QString::number(p2[2]));

	ui.plainTextEdit_2->appendPlainText("p3:" + QString::number(p3[0]) + "," + QString::number(p3[1]) + "," + QString::number(p3[2]));
	ui.plainTextEdit_2->appendPlainText("p4:" + QString::number(p4[0]) + "," + QString::number(p4[1]) + "," + QString::number(p4[2]));
}

//************************�������************************

//��ղ�����������
void QtWidgetsApplication6::on_pushButton_10_clicked() {
	ui.plainTextEdit_2->clear();//�����ʾ�Ŀ��ƶ�������
}

//************************�������************************

//-------------------------------------------------------------

//������ƶ����ź�
void QtWidgetsApplication6::handlePointText(const QString& pointText) {
	ui.plainTextEdit->appendPlainText(pointText);//������Ļ�ͻ����
	//���ܻ���һЩ�߼��ϵ�����
	//B-Spline�ȹ�Ӧ�û���
	//Bezier���ƿ϶���������
}

////######################���f(s,t)�õ�f1, f2, f1 ? cf2, f1 + cf2######################
////��fij
//void QtWidgetsApplication5::on_pushButton_19_clicked() {
//	//f(s,t) = (P(s) - Q(t)) * P'(s)T = P(s)*P'(s)T - Q(t)*P'(s)T = 0.0
//	//f1(s,t) = P(s) * P'(s)T
//	//f2(s,t) = Q(t) * P'(s)T�������
//	
//	if (ui.openGLWidget->curve[0]->isCurveAvailable()&& ui.openGLWidget->curve[1]->isCurveAvailable()) {//����һ����ƶ�����ΪP(s)�Ŀ��ƶ���
//
//		//����s
//		double s0 = 0.0;
//		double s1 = 1.0;
//		//����t
//		double t0 = 0.0;
//		double t1 = 1.0;
//
//		//-------------------------------------------------------------------
//		
//		//f1(s,t)��ϵ��fij������iΪs�Ĵ�����jΪt�Ĵ�����j��Ϊ0
//		//����f1(s,t) = P(s) * P'(s)TֻҪ����P'(s)T
//		std::vector<Point> P = ui.openGLWidget->curve[0]->getControlPoints();//P(s)�Ŀ��ƶ���
//		int pS = P.size();//P��size
//		double m = pS - 1;//P(s)�Ĵ���������double�����������
//
//		//BezierCurve bezierCurveP(P);//����P(s)��bezier����
//		//bezierCurveP.update();
//
//		std::vector<double> fi0(m + 1);
//
//		//����fi0��ǰm��
//		//f1(s,t) = P(s) * P'(s)T
//		//dP0
//		fi0[0] = P[0][0] * (-P[1][1] + P[0][1]) + P[0][1] * (P[1][0] - P[0][0]);
//		for (int i = 1; i < m; i++) {
//			//Qi[0] = i/m*(P[i][0]-P[i-1][0])+(1.0-i/m)*(P[i+1][0]-P[i][0])
//			fi0[i] = P[i][0] * ((double)i / m * (-P[i][1] + P[i - 1][1]) + (1.0 - (double)i / m) * (-P[i + 1][1] + P[i][1])) +
//				P[i][1] * ((double)i / m * (P[i][0] - P[i - 1][0]) + (1.0 - (double)i / m) * (P[i + 1][0] - P[i][0]));
//		}
//		//fi0��m+1��
//		fi0[m] = P[m][0] * (-P[m][1] + P[m - 1][1]) + P[m][1] * (P[m][0] - P[m - 1][0]);
//
//		for (const double& d : fi0) {
//			ui.plainTextEdit_2->appendPlainText(QString::number(d));
//		}
//
//		//-------------------------------------------------------------------
//		//����f2(s,t) = Q(t) * P'(s)TҲֻҪ����P'(s)T
//		//f2(s,t)��ϵ��fij������iΪs�Ĵ�����jΪt�Ĵ���
//
//		std::vector<Point> Q = ui.openGLWidget->curve[1]->getControlPoints();//Q(t)�Ŀ��ƶ���
//		int qS = Q.size();//Q��size
//		double n = qS - 1;//Q(t)�Ĵ���������double�����������
//
//		//BezierCurve bezierCurveQ(Q);//����Q(t)��bezier����
//		//bezierCurveQ.update();
//
//		std::vector<std::vector<double>> fij;
//		fij.resize(m + 1, std::vector<double>(n + 1));//m+1��n+1��
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

//����׳˺���
int fac(int x)
{
	int s = 1;
	for (int i = 1; i <= x; i++)
		s *= i;
	return s;
}

//���������
int C(int m, int n) {
	return fac(m) / (fac(m - n) * fac(n));
}

//����Berstein������
double B(int n, int i, float u) {
	//Bernstein����ʽ��B(n,i) = C(n,i)*u^i*(i.0-u)^(n-i)
	int sum = 1;
	if (i == n || i == 0) return pow(u, i) * pow(1 - u, n - i);
	int j;
	for (j = n - i + 1; j <= n; j++)
		sum *= j;
	for (j = 1; j <= i; j++)
		sum /= j;
	return sum * pow(u, i) * pow(1 - u, n - i);
}

//######################���f(s,t)�õ�f1, f2, f1 ? cf2, f1 + cf2######################
//��fij
void QtWidgetsApplication6::on_pushButton_19_clicked() {
	//f(s,t) = (P(s) - Q(t)) * P'(s)T = P(s)*P'(s)T - Q(t)*P'(s)T = 0.0
	//f1(s,t) = P(s) * P'(s)T
	//f2(s,t) = Q(t) * P'(s)T�������


	if (ui.openGLWidget->curve[1]
		&& ui.openGLWidget->curve[0]->isCurveAvailable()
		&& ui.openGLWidget->curve[1]->isCurveAvailable()) {//����һ����ƶ�����ΪP(s)�Ŀ��ƶ���

		//����s
		double s0 = 0.0;
		double s1 = 1.0;
		double s = 0.5;
		double s_ = (s1 - s) / (s1 - s0);
		//����t
		double t0 = 0.0;
		double t1 = 1.0;
		double t = 0.5;
		double t_ = (t1 - t) / (t1 - t0);

		//-------------------------------------------------------------------

		//f1(s,t)��ϵ��fij������iΪs�Ĵ�����jΪt�Ĵ���
		//����f1(s,t) = P(s) * P'(s)T
		//ͨ��B��������B�������1���ڽ�B�ϲ�
		std::vector<Point> P = ui.openGLWidget->curve[0]->getControlPoints();//P(s)�Ŀ��ƶ���
		int pS = P.size();//P��size
		double m = pS - 1;//P(s)�Ĵ���������double�����������

		std::vector<Point> Q = ui.openGLWidget->curve[1]->getControlPoints();//Q(t)�Ŀ��ƶ���
		int qS = Q.size();//Q��size
		double n = qS - 1;//Q(t)�Ĵ���������double�����������

		//BezierCurve bezierCurveP(P);//����P(s)��bezier����
		//bezierCurveP.update();

		std::vector<double> tempij1;//�����µ�Pi֮������Qij
		tempij1.resize(2 * m, 0.0);//(2*m-1,0)ע�����Ҫ�Ǵ�����1

		for (int i = 0; i < m + 1; i++) {//�����ʱ����Ե���P(s)*P'(s)T
			for (int j = 0; j < m; j++) {
				tempij1[i + j] += (double)C(m, i) * (double)C(m - 1, j) / (double)C(2 * m - 1, i + j)//B��������˻�Ϊ2*m-1
					* (double)m / (s1 - s0)//��ǰϵ��
					* (P[i][0] * (-P[j + 1][1] + P[j][1]) +
						P[i][1] * (P[j + 1][0] - P[j][0]));
			}
		}

		std::vector<std::vector<double>> fij1;//Qij

		//(2*m-1,0)->(2*m-1,n)
		for (int i = 0; i < 2 * m; i++) {//��2m��
			fij1.push_back(std::vector<double>(n + 1, tempij1[i]));//ÿ�ж�һ��
		}

		//P(s)P'(S)T
		double f1xy = 0.0;
		for (int i = 0; i < 2 * m; i++) {
			for (int j = 0; j < n + 1; j++) {
				f1xy += fij1[i][j] * B(2 * m - 1, i, (s1 - s) / (s1 - s0)) * B(n, j, (t1 - t) / (t1 - t0));
			}
		}

		//��ӡ
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
		//����f2(s,t) = Q(t) * P'(s)T
		//f2(s,t)��ϵ��fij������iΪs�Ĵ�����jΪt�Ĵ���

		//BezierCurve bezierCurveQ(Q);//����Q(t)��bezier����
		//bezierCurveQ.update();

		//std::vector<std::vector<double>> tempij2(m, std::vector<double>(n + 1));//(m-1,n)m��n+1��
		//ֻʹ��(m-1,n)m��n+1�У�����Ϊ0.0
		std::vector<std::vector<double>> beforeij2(2 * m, std::vector<double>(n + 1, 0.0));//(2*m-1,n)

		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n + 1; j++) {
				beforeij2[i][j] = (double)m / (s1 - s0)
					* (Q[j][0] * (-P[i + 1][1] + P[i][1]) + Q[j][1] * (P[i + 1][0] - P[i][0]));
			}
		}

		//��ӡ
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

		//��ӡ
		ui.plainTextEdit_2->appendPlainText("\nfij2");
		for (const std::vector<double>& l : fij2) {
			QString string;
			for (int i = 0; i < n + 1; i++) {
				string = string + QString::number(l[i]) + ",";
			}
			ui.plainTextEdit_2->appendPlainText(string);
		}

		//��ӡP(s)P'(S)T-Q(t)P'(S)T��ֱ����汾��
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

		//1��(m-1,n-1)->(m-1,n)
		for (int i = 0; i < m; i++) {
			gij[i][0] = beforeij3[i][0];
			for (int j = 1; j < n; j++) {
				gij[i][j] = (double)j / (double)n * beforeij3[i][j - 1]
					+ (1.0 - (double)j / (double)n) * beforeij3[i][j];
			}
			gij[i][n] = beforeij3[i][n - 1];
		}
		beforeij3 = gij;

		//2��(m-1,n)->(2*m-1,n)
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

		//��ӡ
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

		//��֤
		Point PsD = bezierCurve_1.getDerivative(s_);
		Point QtDT = bezierCurve_2.getDerT(t_);
		ui.plainTextEdit_2->appendPlainText("\n##P'(S)Q'(t)T##");
		ui.plainTextEdit_2->appendPlainText(QString::number(gxy));
		ui.plainTextEdit_2->appendPlainText(QString::number(PsD[0] * QtDT[0] + PsD[1] * QtDT[1]));

	}

}

//��������
void QtWidgetsApplication6::on_pushButton_20_clicked() {
	if (!drawMesh) {//ֻ��Ҫ��ʼ��һ��
		drawMesh = true;
		InitialMesh(ui.openGLWidget->M, 2, 2, -8.0, 8.0, -8.0, 8.0);//��ʼ���õ�һ��mesh��ֻ����һ��face
	}
	ui.openGLWidget->update();//ˢ�´���
}

//crossָ����
void QtWidgetsApplication6::on_pushButton_21_clicked() {

	if (ui.lineEdit_12->text().toInt() >= ui.openGLWidget->M.m_Faces.size() || ui.lineEdit_12->text().toInt() < 0) {
		QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
		errorMsgBox.setWindowTitle("Error"); // ���öԻ������
		errorMsgBox.setText("An error occurred : No such face!");// ���ô�����Ϣ�ı�
		errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
		errorMsgBox.exec();//��ʾ��Ϣ�Ի���
		return;
	}

	QString crossText = ui.lineEdit_12->text();
	QStringList dataList = crossText.split(","); // ʹ�ö��ŷָ��ַ���

	vector<int> crossFaces;
	foreach(const QString & str, dataList) {
		bool ok;
		crossFaces.push_back(str.toInt(&ok)); // ��ÿ���Ӵ�ת��Ϊ����
		/*if (!ok) {
			qDebug() << "Extracted Number:" << number;
		}*/
	}

	myRefine_QQ_Cross(ui.openGLWidget->M, crossFaces);

	ui.openGLWidget->update();//ˢ�´���
}

//����ָͨ����
void QtWidgetsApplication6::on_pushButton_22_clicked() {

	QString remainText = ui.lineEdit_13->text();
	QStringList dataList = remainText.split(","); // ʹ�ö��ŷָ��ַ���

	vector<int> remain;
	foreach(const QString & str, dataList) {
		bool ok;
		remain.push_back(str.toInt(&ok)); // ��ÿ���Ӵ�ת��Ϊ������push��remian����
		/*if (!ok) {
			qDebug() << "Extracted Number:" << number;
		}*/
	}

	ui.openGLWidget->connected.clear();
	vector<int> nextStack;
	//ֻҪremain��Ϊ�գ���pop��back���������ҵ�������ͨ�ģ��ڲ���!remain.empty()�����ж�
	//while (!remain.empty()) {
	Connected(ui.openGLWidget->M, remain, ui.openGLWidget->connected, nextStack);
	//}

	ui.openGLWidget->update();//ˢ�´���
}

char inervalJudgement(BSplineCurve* curve0, BSplineCurve* curve1, double s0, double s1, double t0, double t1) {
	//if (ui.openGLWidget->curve[1]
	//	&& ui.openGLWidget->curve[0]->isCurveAvailable()
	//	&& ui.openGLWidget->curve[1]->isCurveAvailable()) {//����һ����ƶ�����ΪP(s)�Ŀ��ƶ���

	//����s
	/*double s0 = 0.0;
	double s1 = 1.0;*/
	double s = (s0 + s1) / 2.0;
	double s_ = (s1 - s) / (s1 - s0);
	//����t
	/*double t0 = 0.0;
	double t1 = 1.0;*/
	double t = (t0 + t1) / 2.0;
	double t_ = (t1 - t) / (t1 - t0);

	//-------------------------------------------------------------------

	//f1(s,t)��ϵ��fij������iΪs�Ĵ�����jΪt�Ĵ���
	//����f1(s,t) = P(s) * P'(s)T
	//ͨ��B��������B�������1���ڽ�B�ϲ�
	std::vector<Point> P = curve0->getControlPoints();//P(s)�Ŀ��ƶ���
	int pS = P.size();//P��size
	double m = pS - 1;//P(s)�Ĵ���������double�����������

	std::vector<Point> Q = curve1->getControlPoints();//Q(t)�Ŀ��ƶ���
	int qS = Q.size();//Q��size
	double n = qS - 1;//Q(t)�Ĵ���������double�����������

	std::vector<double> tempij1;//�����µ�Pi֮������Qij
	tempij1.resize(2 * m, 0.0);//(2*m-1,0)ע�����Ҫ�Ǵ�����1

	for (int i = 0; i < m + 1; i++) {//�����ʱ����Ե���P(s)*P'(s)T
		for (int j = 0; j < m; j++) {
			tempij1[i + j] += (double)C(m, i) * (double)C(m - 1, j) / (double)C(2 * m - 1, i + j)//B��������˻�Ϊ2*m-1
				* (double)m / (s1 - s0)//��ǰϵ��
				* (P[i][0] * (-P[j + 1][1] + P[j][1]) +
					P[i][1] * (P[j + 1][0] - P[j][0]));
		}
	}

	std::vector<std::vector<double>> fij1;//Qij

	//(2*m-1,0)->(2*m-1,n)
	for (int i = 0; i < 2 * m; i++) {//��2m��
		fij1.push_back(std::vector<double>(n + 1, tempij1[i]));//ÿ�ж�һ��
	}

	//-------------------------------------------------------------------
	//����f2(s,t) = Q(t) * P'(s)T
	//f2(s,t)��ϵ��fij������iΪs�Ĵ�����jΪt�Ĵ���

	//BezierCurve bezierCurveQ(Q);//����Q(t)��bezier����
	//bezierCurveQ.update();

	//std::vector<std::vector<double>> tempij2(m, std::vector<double>(n + 1));//(m-1,n)m��n+1��
	//ֻʹ��(m-1,n)m��n+1�У�����Ϊ0.0
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

	//��ӡfij
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

	//1��(m-1,n-1)->(m-1,n)
	for (int i = 0; i < m; i++) {
		gij[i][0] = beforeij3[i][0];
		for (int j = 1; j < n; j++) {
			gij[i][j] = (double)j / (double)n * beforeij3[i][j - 1]
				+ (1.0 - (double)j / (double)n) * beforeij3[i][j];
		}
		gij[i][n] = beforeij3[i][n - 1];
	}
	beforeij3 = gij;

	//2��(m-1,n)->(2*m-1,n)
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

	//��ӡgij
	/*ui.plainTextEdit_2->appendPlainText("\ngij");
	for (const std::vector<double>& l : gij) {
		QString string;
		for (int i = 0; i < n + 1; i++) {
			string = string + QString::number(l[i]) + ",";
		}
		ui.plainTextEdit_2->appendPlainText(string);
	}*/

	//�ж�st��0.0��1.0����֮���Ƿ��������-------------------------------
	bool flag1;
	fij[0][0] > 0.0 ? flag1 = true : flag1 = false;//������true��������false
	bool flag2;
	gij[0][0] > 0.0 ? flag2 = true : flag2 = false;
	bool shouldbreakf = false;//�������Ż�0��true
	bool shouldbreakg = false;

	double max1 = std::abs(fij[0][0]);//���ó�ʼֵ��������c
	double max2 = std::abs(gij[0][0]);

	for (int i = 0; i < 2 * m; i++) {
		for (int j = 0; j < n + 1; j++) {
			if (fij[i][j] >= 0.0 && !flag1 || fij[i][j] <= 0.0 && flag1) {//��0�������
				shouldbreakf = true;//�����ж�ͨ��
			}
			if (gij[i][j] >= 0.0 && !flag2 || gij[i][j] <= 0.0 && flag2) {
				shouldbreakg = true;//�����ж�ͨ��
			}
			//�и������ֵ�����max
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

	//ǰ��ͨ�����к���������ֱ��������if��else����޹���
	if (shouldbreakf && shouldbreakg) {

		double c = max1 / max2;//ϵ��
		//��ӡϵ��
		/*ui.plainTextEdit_2->appendPlainText("\nc = " + QString::number(max1) + " / " + QString::number(max2)
			+ " = " + QString::number(c));*/

			//��fij+c*gij��fij-c*gij
		for (int i = 0; i < 2 * m; i++) {
			for (int j = 0; j < n + 1; j++) {
				fc[i][j] = fij[i][j] + c * gij[i][j];
				f_c[i][j] = fij[i][j] - c * gij[i][j];
			}
		}

		//��ӡfc��f_c
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

		//ͬ���Ķ���ѭ���ж�
		bool flag3;
		fc[0][0] > 0.0 ? flag3 = true : flag3 = false;//������true��������false
		bool flag4;
		f_c[0][0] > 0.0 ? flag4 = true : flag4 = false;
		bool shouldbreakfc = false;//�������Ż�0��true
		bool shouldbreakf_c = false;//�������Ż�0��true

		for (int i = 0; i < 2 * m && !(shouldbreakfc && shouldbreakf_c); i++) {//shouldbreak2Ϊtrue֮��Ҳֱ��break
			for (int j = 0; j < n + 1; j++) {
				if (fc[i][j] >= 0.0 && !flag3 || fc[i][j] <= 0.0 && flag3) {//�����з��Ų�һ��
					shouldbreakfc = true;
				}
				if (f_c[i][j] >= 0.0 && !flag4 || f_c[i][j] <= 0.0 && flag4) {
					shouldbreakf_c = true;//�����ж�ͨ��
				}
				if (shouldbreakfc && shouldbreakf_c) {
					return 'y';//���
					//return;//����û��max��Ҫ�㣬ֱ���˳�
				}
			}
		}

		//�������
		//if (shouldbreakfc && shouldbreakf_c) {
		//	//�й�����
		//}
		//else {
		return '2';//û��
		//}

	}
	else {//ֱ�����û�й�����
		return '1';
	}

	//}
}

//�ж��Ƿ�������
//���������ĳ�һ���������������Ϊ����ָ��Ͳ�����Χ
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
	//	&& ui.openGLWidget->curve[1]->isCurveAvailable()) {//����һ����ƶ�����ΪP(s)�Ŀ��ƶ���

	//	//����s
	//	double s0 = 0.0;
	//	double s1 = 1.0;
	//	double s = 0.5;
	//	double s_ = (s1 - s) / (s1 - s0);
	//	//����t
	//	double t0 = 0.0;
	//	double t1 = 1.0;
	//	double t = 0.5;
	//	double t_ = (t1 - t) / (t1 - t0);

	//	//-------------------------------------------------------------------

	//	//f1(s,t)��ϵ��fij������iΪs�Ĵ�����jΪt�Ĵ���
	//	//����f1(s,t) = P(s) * P'(s)T
	//	//ͨ��B��������B�������1���ڽ�B�ϲ�
	//	std::vector<Point> P = ui.openGLWidget->curve[0]->getControlPoints();//P(s)�Ŀ��ƶ���
	//	int pS = P.size();//P��size
	//	double m = pS - 1;//P(s)�Ĵ���������double�����������

	//	std::vector<Point> Q = ui.openGLWidget->curve[1]->getControlPoints();//Q(t)�Ŀ��ƶ���
	//	int qS = Q.size();//Q��size
	//	double n = qS - 1;//Q(t)�Ĵ���������double�����������

	//	std::vector<double> tempij1;//�����µ�Pi֮������Qij
	//	tempij1.resize(2 * m, 0.0);//(2*m-1,0)ע�����Ҫ�Ǵ�����1

	//	for (int i = 0; i < m + 1; i++) {//�����ʱ����Ե���P(s)*P'(s)T
	//		for (int j = 0; j < m; j++) {
	//			tempij1[i + j] += (double)C(m, i) * (double)C(m - 1, j) / (double)C(2 * m - 1, i + j)//B��������˻�Ϊ2*m-1
	//				* (double)m / (s1 - s0)//��ǰϵ��
	//				* (P[i][0] * (-P[j + 1][1] + P[j][1]) +
	//					P[i][1] * (P[j + 1][0] - P[j][0]));
	//		}
	//	}

	//	std::vector<std::vector<double>> fij1;//Qij

	//	//(2*m-1,0)->(2*m-1,n)
	//	for (int i = 0; i < 2 * m; i++) {//��2m��
	//		fij1.push_back(std::vector<double>(n + 1, tempij1[i]));//ÿ�ж�һ��
	//	}

	//	//-------------------------------------------------------------------
	//	//����f2(s,t) = Q(t) * P'(s)T
	//	//f2(s,t)��ϵ��fij������iΪs�Ĵ�����jΪt�Ĵ���

	//	//BezierCurve bezierCurveQ(Q);//����Q(t)��bezier����
	//	//bezierCurveQ.update();

	//	//std::vector<std::vector<double>> tempij2(m, std::vector<double>(n + 1));//(m-1,n)m��n+1��
	//	//ֻʹ��(m-1,n)m��n+1�У�����Ϊ0.0
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

	//	//��ӡfij
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

	//	//1��(m-1,n-1)->(m-1,n)
	//	for (int i = 0; i < m; i++) {
	//		gij[i][0] = beforeij3[i][0];
	//		for (int j = 1; j < n; j++) {
	//			gij[i][j] = (double)j / (double)n * beforeij3[i][j - 1]
	//				+ (1.0 - (double)j / (double)n) * beforeij3[i][j];
	//		}
	//		gij[i][n] = beforeij3[i][n - 1];
	//	}
	//	beforeij3 = gij;

	//	//2��(m-1,n)->(2*m-1,n)
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

	//	//��ӡgij
	//	ui.plainTextEdit_2->appendPlainText("\ngij");
	//	for (const std::vector<double>& l : gij) {
	//		QString string;
	//		for (int i = 0; i < n + 1; i++) {
	//			string = string + QString::number(l[i]) + ",";
	//		}
	//		ui.plainTextEdit_2->appendPlainText(string);
	//	}

	//	//�ж�st��0.0��1.0����֮���Ƿ��������-------------------------------
	//	bool flag1;
	//	fij[0][0] > 0.0 ? flag1 = true : flag1 = false;//������true��������false
	//	bool flag2;
	//	gij[0][0] > 0.0 ? flag2 = true : flag2 = false;
	//	bool shouldbreakf = false;//�������Ż�0��true
	//	bool shouldbreakg = false;

	//	double max1 = std::abs(fij[0][0]);//���ó�ʼֵ��������c
	//	double max2 = std::abs(gij[0][0]);

	//	for (int i = 0; i < 2 * m; i++) {
	//		for (int j = 0; j < n + 1; j++) {
	//			if (fij[i][j] >= 0.0 && !flag1 || fij[i][j] <= 0.0 && flag1) {//��0�������
	//				shouldbreakf = true;//�����ж�ͨ��
	//			}
	//			if (gij[i][j] >= 0.0 && !flag2 || gij[i][j] <= 0.0 && flag2) {
	//				shouldbreakg = true;//�����ж�ͨ��
	//			}
	//			//�и������ֵ�����max
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

	//	//ǰ��ͨ�����к���������ֱ��������if��else����޹���
	//	if (shouldbreakf&&shouldbreakg) {

	//		double c = max1 / max2;//ϵ��
	//		ui.plainTextEdit_2->appendPlainText("\nc = " + QString::number(max1) + " / " + QString::number(max2)
	//			+ " = " + QString::number(c));
	//		 
	//		//��fij+c*gij��fij-c*gij
	//		for (int i = 0; i < 2 * m; i++) {
	//			for (int j = 0; j < n + 1; j++) {
	//				fc[i][j] = fij[i][j] + c * gij[i][j];
	//				f_c[i][j] = fij[i][j] - c * gij[i][j];
	//			}
	//		}

	//		//��ӡfc��f_c
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

	//		//ͬ���Ķ���ѭ���ж�
	//		bool flag3;
	//		fc[0][0] > 0.0 ? flag3 = true : flag3 = false;//������true��������false
	//		bool flag4;
	//		f_c[0][0] > 0.0 ? flag4 = true : flag4 = false;
	//		bool shouldbreakfc = false;//�������Ż�0��true
	//		bool shouldbreakf_c = false;//�������Ż�0��true

	//		for (int i = 0; i < 2 * m && !(shouldbreakfc&& shouldbreakf_c); i++) {//shouldbreak2Ϊtrue֮��Ҳֱ��break
	//			for (int j = 0; j < n + 1; j++) {
	//				if (fc[i][j] >= 0.0 && !flag3 || fc[i][j] <= 0.0 && flag3) {//�����з��Ų�һ��
	//					shouldbreakfc = true;
	//				}
	//				if (f_c[i][j] >= 0.0 && !flag4 || f_c[i][j] <= 0.0 && flag4) {
	//					shouldbreakf_c = true;//�����ж�ͨ��
	//				}
	//				if (shouldbreakfc && shouldbreakf_c) {
	//					ui.lineEdit_14->setText("maybeyes");//���
	//					return;//����û��max��Ҫ�㣬ֱ���˳�
	//				}
	//			}
	//		}

	//		//�������
	//		//if (shouldbreakfc && shouldbreakf_c) {
	//		//	//�й�����
	//		//}
	//		//else {
	//		ui.lineEdit_14->setText("no2");//û��
	//		//}

	//	}
	//	else {//ֱ�����û�й�����
	//		ui.lineEdit_14->setText("no1");
	//	}

	//}
}

//ϸ�����������ж������Ƿ�������
void QtWidgetsApplication6::on_pushButton_24_clicked() {

	//ǰ�᣺��������
	if (ui.openGLWidget->curve[1]
		&& ui.openGLWidget->curve[0]->isCurveAvailable()
		&& ui.openGLWidget->curve[1]->isCurveAvailable()) {

		drawMesh = true;

		//�����ʼ�����Ŵ��λ���������Ͻǣ�x:s*5.0-5.0��y:t*5.0��
		double s0 = 0.0;
		double s1 = 1.0;
		double t0 = 0.0;
		double t1 = 1.0;
		InitialMesh(ui.openGLWidget->M, 2, 2, s0 * 8.0, s1 * 8.0, t0 * 8.0, t1 * 8.0);
		//vector<int> facesRemained;//��������
		//vector<int> crossFaces;//��¼ÿ��Ҫʮ���ʷֵ���

		//�жϣ�0����s��t0.0��1.0�ж�
		//ui.lineEdit_14->setText(QString(res0));
		if (inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], s0, s1, t0, t1) == 'y') {//����
			//facesRemained.push_back(0);//��һ����Ϊ0
			//crossFaces.push_back(0);//������Ҫʮ���ʷ�
		}
		else {
			return;//����ֱ�ӽ���
		}

		//�жϣ�1�����ϴ��ж�֮�����ļ���cross
		vector<int> facesDeleted;//��ɾ������
		vector<CFace> allFaces;//���е�������
		vector<CVertex> allVers;//���е����е�
		vector<int> crossFaces = {0};//��Ҫʮ���ʷֵ��棬���ȱ����������ѭ��

		for (int depth = 0; depth < 3; depth++) {
			myRefine_QQ_Cross(ui.openGLWidget->M, crossFaces);//��һ���ʷ�idΪ0��face����
			crossFaces.clear();
			//�����桢��
			allFaces = ui.openGLWidget->M.m_Faces;//���е�������
			allVers = ui.openGLWidget->M.m_Vertices;//���е����е�
			for (const CFace& f : allFaces) {//��������������
				bool flag = true;
				for (const int& d : facesDeleted) {
					if (f.m_nFaceid == d) {//ɾ��
						flag = false;
					}
				}
				if (flag) {//û��ɾ��
					CVertex cv = allVers[f.m_nFourIndex[0]];
					s0 = cv.m_dVerx / 8.0;
					s1 = cv.m_dVerx / 8.0;
					t0 = cv.m_dVery / 8.0;
					t1 = cv.m_dVery / 8.0;
					for (int j = 1; j < 4; j++) {//���´˴�s0��s1��t0��t1
						//int index = f.m_nFourIndex[j];
						CVertex cv = allVers[f.m_nFourIndex[j]];
						if (cv.m_dVerx / 8.0 < s0) s0 = cv.m_dVerx / 8.0;
						if (cv.m_dVerx / 8.0 > s1) s1 = cv.m_dVerx / 8.0;
						if (cv.m_dVery / 8.0 < t0) t0 = cv.m_dVery / 8.0;
						if (cv.m_dVery / 8.0 > t1) t1 = cv.m_dVery / 8.0;
					}
					//�����䱣��
					if (inervalJudgement(ui.openGLWidget->curve[0], ui.openGLWidget->curve[1], s0, s1, t0, t1) == 'y') {
						crossFaces.push_back(f.m_nFaceid);
					}
					else {
						facesDeleted.push_back(f.m_nFaceid);
					}
				}
			}
		}
		
		for (const CFace& f : allFaces) {//��������������
			for (const int& d : facesDeleted) {
				if (f.m_nFaceid != d) {//δɾ��
					ui.openGLWidget->remainid.push_back(f.m_nFaceid);
				}
			}
		}

		ui.openGLWidget->update();//ˢ�´���
	}

}