#include "OpenGLWidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget()
{
}

//OpenGL�����ĳ�ʼ��
void OpenGLWidget::initializeGL() {

    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    //glLoadIdentity();//��ǰ��������Ϊ��λ����
    glLineWidth(4.0);
    gluOrtho2D(-10.0, 10.0, -10.0, 10.0);//����ͶӰ����
    glPointSize(5.0);

    curve[0] = new BSplineCurve();
    inflectionPoint[0] = new std::vector<Point>();
    commonTangent.clear();

    //bezierCurve[0] = new BezierCurve();

}

//���� OpenGL ���ڴ�С�仯
void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0.0f, 0.0f, w, h);
}

//���� OpenGL ����
void OpenGLWidget::paintGL() {

    //��ջ�ͼ������
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    //#########################����B-Spline���#########################
    if (drawWhich) {
        //�����Ѿ�������B-Spline
        for (int i = 0; i < curveCount + 1; i++) {

            //�Ƿ���ƿ��ƶ���κͿ��ƶ���
            if (isPolygonVisible) {

                //----------------------���ƶ����----------------------

                glColor3f(0.0f, 0.0f, 1.0f);//���ÿ��ƶ������ɫΪ��ɫ
                glBegin(GL_LINE_STRIP);
                for (const Point& point : curve[i]->getControlPoints()) {
                    glVertex3d(point[0], point[1], point[2]);
                }
                glEnd();

                //----------------------���ƿ��Ƶ�----------------------

                glColor3f(0.0f, 1.0f, 0.0f);//���ÿ��Ƶ�Ϊ��ɫ
                glBegin(GL_POINTS);
                for (const Point& point : curve[i]->getControlPoints()) {
                    glVertex3d(point[0], point[1], point[2]);
                }
                glEnd();

            }

            //----------------------��������----------------------

            glColor3f(1.0f, 0.0f, 0.0f);//��������Ϊ��ɫ
            glBegin(GL_LINE_STRIP);
            for (const Point& point : curve[i]->getPoints()) {
                glVertex3d(point[0], point[1], point[2]);
            }
            glEnd();

            //----------------------���ƹյ�----------------------

            glColor3f(0.0f, 0.0f, 0.0f);//���ùյ�Ϊ��ɫ
            glBegin(GL_POINTS);
            for (const Point& point : *inflectionPoint[i]) {
                glVertex3d(point[0], point[1], point[2]);
            }
            glEnd();

            //----------------------����test----------------------

            //����b-spline���ߵ�����
       //     if (curve[curveIndex]->isCurveAvailable()) {
                //glColor3f(0.0f, 0.0f, 0.0f);//��������Ϊ��ɫ
                //Point p1 = curve[curveIndex]->getPoint({ 1.0 });
                //Point tan = curve[curveIndex]->getDerivative({ 1.0 });
                //glBegin(GL_LINE_STRIP);
                //glVertex3d(p1[0], p1[1], p1[2]);
                //glVertex3d(p1[0] + tan[0], p1[1] + tan[1], p1[2] + tan[2]);
                //glEnd();
       //     }

        }

        //----------------------��������----------------------

        //��򵥵ı�����B-Spline������
        glColor3f(0.0f, 0.0f, 0.0f);//��������Ϊ��ɫ
        glBegin(GL_LINES);
        for (const Point& point : commonTangent) {
            glVertex3d(point[0], point[1], point[2]);
        }
        glEnd();

        //----------------------����Bezier����B-Spline����bezier extract�õ���----------------------

        for (int i = 0; i < curveCount + 1 && !(curve[i]->getCpbsA()).empty(); i++) {

            std::vector<std::vector<std::vector<double>>> cpbsA = curve[i]->getCpbsA();//�õ�bezier���ߵĿ��ƶ���
            std::vector<std::vector<double>> intervals = curve[i]->getIntervals();//����S-Spline��

            if (isBezierVisible) {//�Ƿ���ʾbezier���ߵĿ��ƶ���κͶ���

                //----------------------����Bezier���ƶ����----------------------

                glColor3f(1.0f, 1.0f, 0.0f);//��������Ϊ��ɫ
                for (const std::vector<Point>& points : cpbsA) {
                    glBegin(GL_LINE_STRIP);
                    for (const Point& point : points) {
                        glVertex3d(point[0], point[1], point[2]);
                    }
                    glEnd();
                }

                //----------------------����Bezier���Ƶ�----------------------

                glColor3f(0.0f, 0.0f, 0.0f);//��������Ϊ��ɫ
                glBegin(GL_POINTS);
                for (const std::vector<Point>& points : cpbsA) {
                    for (const Point& point : points) {
                        glVertex3d(point[0], point[1], point[2]);
                    }
                }
                glEnd();

            }

            //----------------------����Bezier����----------------------

            int num = cpbsA.size();
            for (int j = 0; j < num; j++) {

                BezierCurve bezierCurve;
                bezierCurve.setBezierControlPoints(cpbsA[j]);
                bezierCurve.setIntervals(intervals[j]);
                bezierCurve.update();

                glColor3f(1.0f, 0.0f, 1.0f);//��������Ϊ��ɫ
                glBegin(GL_LINE_STRIP);
                for (const Point& point : bezierCurve.getPoints()) {
                    glVertex3d(point[0], point[1], point[2]);
                }
                glEnd();

                //��������
                //glColor3f(0.0f, 0.0f, 0.0f); // ��������Ϊ��ɫ
                //Point pp1 = bezierCurve.getPoint(0.5);
                //Point pp2 = bezierCurve.getDerivative(0.5);
                //glBegin(GL_LINE_STRIP);
                //glVertex3d(pp1[0], pp1[1], pp1[2]); // �����ϵĵ�
                //glVertex3d(pp1[0] + pp2[0] * 100.0, pp1[1] + pp2[1] * 100.0, pp1[2] + pp2[2]); // ���ߵ��յ�
                //glEnd();

            }

        }

        //����ͨ��bezier extract��õĵ�����
        glColor3f(0.0f, 0.0f, 0.0f);//��������Ϊ��ɫ
        glBegin(GL_LINES);
        for (const Point& point : tantan) {
            glVertex3d(point[0], point[1], point[2]);
        }
        glEnd();

        //����BezierCurve���е�extract��������һ��bezier���߷ֳ����룬������cell�ָ��н�һ��ʹ�ã�
        //�����������ʾ�е����⣬��Ե��ǵ�ǰB-Spline���Ƶ���Ƶ�bezier��extract
        if (curve[0]->isCurveAvailable() && !control_1.empty() && !control_2.empty()) {

            BezierCurve bezier_1;
            BezierCurve bezier_2;
            bezier_1.setBezierControlPoints(control_1);
            bezier_2.setBezierControlPoints(control_2);

            bezier_1.update();
            bezier_2.update();

            //���Ƶ����
            glColor3f(0.0f, 1.0f, 0.0f);
            glBegin(GL_POINTS);
            for (const Point& point : control_1) {
                glVertex3d(point[0], point[1], point[2]);
            }
            glEnd();
            glBegin(GL_POINTS);
            for (const Point& point : control_2) {
                glVertex3d(point[0], point[1], point[2]);
            }
            glEnd();

            //���߻���
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_LINE_STRIP);
            for (const Point& point : bezier_1.getPoints()) {
                glVertex3d(point[0], point[1], point[2]);
            }
            glEnd();
            glBegin(GL_LINE_STRIP);
            for (const Point& point : bezier_2.getPoints()) {
                glVertex3d(point[0], point[1], point[2]);
            }
            glEnd();

        }

        glFlush();

    }
    //#########################ֱ����ʾbezier#########################
    else {
        bezierCurve.setBezierControlPoints(curve[0]->getControlPoints());
        bezierCurve.update();

        //----------------------����Bezier���ƶ����----------------------

        glColor3f(1.0f, 1.0f, 0.0f);//��������Ϊ��ɫ
		glBegin(GL_LINE_STRIP);
		for (const Point& point : bezierCurve.getControlPoints()) {
			glVertex3d(point[0], point[1], point[2]);
		}
        glEnd();

        //----------------------����Bezier���Ƶ�----------------------

        glColor3f(0.0f, 0.0f, 0.0f);//��������Ϊ��ɫ
		glBegin(GL_POINTS);
		for (const Point& point : bezierCurve.getControlPoints()) {
			glVertex3d(point[0], point[1], point[2]);
		}
        glEnd();

        //----------------------����Bezier����----------------------

        glColor3f(1.0f, 0.0f, 1.0f);//��������Ϊ��ɫ
        glBegin(GL_LINE_STRIP);
        for (const Point& point : bezierCurve.getPoints()) {
            glVertex3d(point[0], point[1], point[2]);
        }
        glEnd();

        glFlush();

    }

    if (drawMesh) {

        vector<CVertex> vers = M.m_Vertices;
        vector<CFace> facs = M.m_Faces;
        for (const CFace& f : facs) {//����ÿ����
            vector<CVertex> drawVers;
            for (const int& i : f.m_nFourIndex) {
                drawVers.push_back(vers[i]);
            }
            glBegin(GL_LINE_STRIP);
            glVertex3d(drawVers[0].m_dVerx, drawVers[0].m_dVery, 0.0);
            glVertex3d(drawVers[1].m_dVerx, drawVers[1].m_dVery, 0.0);
            glVertex3d(drawVers[2].m_dVerx, drawVers[2].m_dVery, 0.0);
            glVertex3d(drawVers[3].m_dVerx, drawVers[3].m_dVery, 0.0);
            glVertex3d(drawVers[0].m_dVerx, drawVers[0].m_dVery, 0.0);
            glEnd();
        }

        //�����ͨ
        glColor3f(1.0f, 0.0f, 1.0f);//��������Ϊ��ɫ
        for (const vector<int> connectedComponent : connected) {
            double x0 = vers[facs[connectedComponent[0]].m_nFourIndex[0]].m_dVerx;
            double y0 = vers[facs[connectedComponent[0]].m_nFourIndex[0]].m_dVery;
            double minX = x0, maxX = x0, minY = y0, maxY = y0;
            for (const int& faceid : connectedComponent) {//��ѯ���¾��ο�߽�
                for (int j = 0; j < 4; j++) {
                    int index = facs[faceid].m_nFourIndex[j];
                    CVertex cv = vers[index];
                    if (cv.m_dVerx < minX) minX = cv.m_dVerx;
                    if (cv.m_dVerx > maxX) maxX = cv.m_dVerx;
                    if (cv.m_dVery < minY) minY = cv.m_dVery;
                    if (cv.m_dVery > maxY) maxY = cv.m_dVery;
                }
            }
            //���ƾ��ο�
            glBegin(GL_LINE_STRIP);
            glVertex3d(minX, minY, 0.0);
            glVertex3d(minX, maxY, 0.0);
            glVertex3d(maxX, maxY, 0.0);
            glVertex3d(maxX, minY, 0.0);
            glVertex3d(minX, minY, 0.0);
            glEnd();
        }

        //��עÿ��face��id
        QPainter painter(this);
        for (const CFace& f : facs) {

            double x1 = vers[f.m_nFourIndex[0]].m_dVerx;
            double x2 = vers[f.m_nFourIndex[2]].m_dVerx;
            double y1 = vers[f.m_nFourIndex[0]].m_dVery;
            double y2 = vers[f.m_nFourIndex[2]].m_dVery;
			double xpos = ((x1 + x2) / 2.0 + 10.0) / 20.0 * width();
			double ypos = -((y1 + y2) / 2.0 - 10.0) / 20.0 * height();

			bool found = false;  //����Ƿ��ҵ�ƥ���Ԫ��
			for (const int& id : remainid) {
				if (f.m_nFaceid == id) {
					found = true;
					break;
				}
			}
			if (found) {
				painter.drawText(xpos, ypos, QString::number(f.m_nFaceid) + "_remain");
			}
			else {
				painter.drawText(xpos, ypos, QString::number(f.m_nFaceid));
			}


		}

	}

}

//����걻����ʱ��Qt ��ܻ��Զ������������
void OpenGLWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {

        //��δ�½��κ����ߴ��󵯿���
        if (curveCount == -1) {
            QMessageBox errorMsgBox;// ����һ��������Ϣ�Ի���
            errorMsgBox.setWindowTitle("Error"); // ���öԻ������
            errorMsgBox.setText("An error occurred :\nNo curves have been created yet!");// ���ô�����Ϣ�ı�
            errorMsgBox.setStandardButtons(QMessageBox::Ok); // ���ð�ťΪOK��ť
            errorMsgBox.exec();//��ʾ��Ϣ�Ի���
            return;
        }

        // �����������ת��ΪOpenGL�ӿ�����
        //int viewport[4];
        //glGetIntegerv(GL_VIEWPORT, viewport);

        double xpos = event->x();
        double ypos = event->y();

        // ���ӿ�����ת��Ϊ��������
        //20.0��ȷ������17.0��ͼ��ƫ���ϣ�0.0��ƫ�����£�8.0ƫ���½��٣�9.0����
        double worldX = (xpos / width()) * 20.0 - 10.0;
        double worldY = -(ypos / height()) * 20.0 + 10.0;

        // ��ӿ��Ƶ�
        curve[curveIndex]->addControlPoint(Point({ worldX, worldY, 0.0 }));
        curve[curveIndex]->update();

        QString pointText = "(" + QString::number(worldX) + "," + QString::number(worldY) + "," + QString::number(0.0) + ")";
        // �����źŲ���������
        emit pointTextReady(pointText);

        // ���»��Ƴ���
        update();
    }
}

