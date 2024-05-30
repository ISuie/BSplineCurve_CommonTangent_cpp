#include "OpenGLWidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget()
{
}

//OpenGL上下文初始化
void OpenGLWidget::initializeGL() {

    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    //glLoadIdentity();//当前矩阵设置为单位矩阵
    glLineWidth(4.0);
    gluOrtho2D(-10.0, 10.0, -10.0, 10.0);//设置投影参数
    glPointSize(5.0);

    curve[0] = new BSplineCurve();
    inflectionPoint[0] = new std::vector<Point>();
    commonTangent.clear();

    //bezierCurve[0] = new BezierCurve();

}

//处理 OpenGL 窗口大小变化
void OpenGLWidget::resizeGL(int w, int h) {
    glViewport(0.0f, 0.0f, w, h);
}

//绘制 OpenGL 场景
void OpenGLWidget::paintGL() {

    //清空绘图缓存区
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    //#########################绘制B-Spline相关#########################
    if (drawWhich) {
        //遍历已经创建的B-Spline
        for (int i = 0; i < curveCount + 1; i++) {

            //是否绘制控制多变形和控制定点
            if (isPolygonVisible) {

                //----------------------控制多边形----------------------

                glColor3f(0.0f, 0.0f, 1.0f);//设置控制多边形颜色为蓝色
                glBegin(GL_LINE_STRIP);
                for (const Point& point : curve[i]->getControlPoints()) {
                    glVertex3d(point[0], point[1], point[2]);
                }
                glEnd();

                //----------------------绘制控制点----------------------

                glColor3f(0.0f, 1.0f, 0.0f);//设置控制点为绿色
                glBegin(GL_POINTS);
                for (const Point& point : curve[i]->getControlPoints()) {
                    glVertex3d(point[0], point[1], point[2]);
                }
                glEnd();

            }

            //----------------------绘制曲线----------------------

            glColor3f(1.0f, 0.0f, 0.0f);//设置曲线为红色
            glBegin(GL_LINE_STRIP);
            for (const Point& point : curve[i]->getPoints()) {
                glVertex3d(point[0], point[1], point[2]);
            }
            glEnd();

            //----------------------绘制拐点----------------------

            glColor3f(0.0f, 0.0f, 0.0f);//设置拐点为黑色
            glBegin(GL_POINTS);
            for (const Point& point : *inflectionPoint[i]) {
                glVertex3d(point[0], point[1], point[2]);
            }
            glEnd();

            //----------------------绘制test----------------------

            //测试b-spline曲线的切线
       //     if (curve[curveIndex]->isCurveAvailable()) {
                //glColor3f(0.0f, 0.0f, 0.0f);//设置切线为黑色
                //Point p1 = curve[curveIndex]->getPoint({ 1.0 });
                //Point tan = curve[curveIndex]->getDerivative({ 1.0 });
                //glBegin(GL_LINE_STRIP);
                //glVertex3d(p1[0], p1[1], p1[2]);
                //glVertex3d(p1[0] + tan[0], p1[1] + tan[1], p1[2] + tan[2]);
                //glEnd();
       //     }

        }

        //----------------------绘制切线----------------------

        //最简单的遍历求B-Spline的切线
        glColor3f(0.0f, 0.0f, 0.0f);//设置切线为黑色
        glBegin(GL_LINES);
        for (const Point& point : commonTangent) {
            glVertex3d(point[0], point[1], point[2]);
        }
        glEnd();

        //----------------------绘制Bezier（由B-Spline曲线bezier extract得到）----------------------

        for (int i = 0; i < curveCount + 1 && !(curve[i]->getCpbsA()).empty(); i++) {

            std::vector<std::vector<std::vector<double>>> cpbsA = curve[i]->getCpbsA();//得到bezier曲线的控制定点
            std::vector<std::vector<double>> intervals = curve[i]->getIntervals();//关于S-Spline的

            if (isBezierVisible) {//是否显示bezier曲线的控制多边形和顶点

                //----------------------绘制Bezier控制多边形----------------------

                glColor3f(1.0f, 1.0f, 0.0f);//设置曲线为黄色
                for (const std::vector<Point>& points : cpbsA) {
                    glBegin(GL_LINE_STRIP);
                    for (const Point& point : points) {
                        glVertex3d(point[0], point[1], point[2]);
                    }
                    glEnd();
                }

                //----------------------绘制Bezier控制点----------------------

                glColor3f(0.0f, 0.0f, 0.0f);//设置曲线为黑色
                glBegin(GL_POINTS);
                for (const std::vector<Point>& points : cpbsA) {
                    for (const Point& point : points) {
                        glVertex3d(point[0], point[1], point[2]);
                    }
                }
                glEnd();

            }

            //----------------------绘制Bezier曲线----------------------

            int num = cpbsA.size();
            for (int j = 0; j < num; j++) {

                BezierCurve bezierCurve;
                bezierCurve.setBezierControlPoints(cpbsA[j]);
                bezierCurve.setIntervals(intervals[j]);
                bezierCurve.update();

                glColor3f(1.0f, 0.0f, 1.0f);//设置曲线为紫色
                glBegin(GL_LINE_STRIP);
                for (const Point& point : bezierCurve.getPoints()) {
                    glVertex3d(point[0], point[1], point[2]);
                }
                glEnd();

                //测试切线
                //glColor3f(0.0f, 0.0f, 0.0f); // 设置切线为黑色
                //Point pp1 = bezierCurve.getPoint(0.5);
                //Point pp2 = bezierCurve.getDerivative(0.5);
                //glBegin(GL_LINE_STRIP);
                //glVertex3d(pp1[0], pp1[1], pp1[2]); // 曲线上的点
                //glVertex3d(pp1[0] + pp2[0] * 100.0, pp1[1] + pp2[1] * 100.0, pp1[2] + pp2[2]); // 切线的终点
                //glEnd();

            }

        }

        //绘制通过bezier extract求得的的切线
        glColor3f(0.0f, 0.0f, 0.0f);//设置切线为黑色
        glBegin(GL_LINES);
        for (const Point& point : tantan) {
            glVertex3d(point[0], point[1], point[2]);
        }
        glEnd();

        //测试BezierCurve类中的extract函数（把一条bezier曲线分成两半，后续在cell分割中进一步使用）
        //但是这里的显示有点问题，针对的是当前B-Spline控制点绘制的bezier的extract
        if (curve[0]->isCurveAvailable() && !control_1.empty() && !control_2.empty()) {

            BezierCurve bezier_1;
            BezierCurve bezier_2;
            bezier_1.setBezierControlPoints(control_1);
            bezier_2.setBezierControlPoints(control_2);

            bezier_1.update();
            bezier_2.update();

            //控制点绘制
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

            //曲线绘制
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
    //#########################直接显示bezier#########################
    else {
        bezierCurve.setBezierControlPoints(curve[0]->getControlPoints());
        bezierCurve.update();

        //----------------------绘制Bezier控制多边形----------------------

        glColor3f(1.0f, 1.0f, 0.0f);//设置曲线为黄色
		glBegin(GL_LINE_STRIP);
		for (const Point& point : bezierCurve.getControlPoints()) {
			glVertex3d(point[0], point[1], point[2]);
		}
        glEnd();

        //----------------------绘制Bezier控制点----------------------

        glColor3f(0.0f, 0.0f, 0.0f);//设置曲线为黑色
		glBegin(GL_POINTS);
		for (const Point& point : bezierCurve.getControlPoints()) {
			glVertex3d(point[0], point[1], point[2]);
		}
        glEnd();

        //----------------------绘制Bezier曲线----------------------

        glColor3f(1.0f, 0.0f, 1.0f);//设置曲线为紫色
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
        for (const CFace& f : facs) {//绘制每个面
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

        //框出连通
        glColor3f(1.0f, 0.0f, 1.0f);//设置曲线为紫色
        for (const vector<int> connectedComponent : connected) {
            double x0 = vers[facs[connectedComponent[0]].m_nFourIndex[0]].m_dVerx;
            double y0 = vers[facs[connectedComponent[0]].m_nFourIndex[0]].m_dVery;
            double minX = x0, maxX = x0, minY = y0, maxY = y0;
            for (const int& faceid : connectedComponent) {//查询更新矩形框边界
                for (int j = 0; j < 4; j++) {
                    int index = facs[faceid].m_nFourIndex[j];
                    CVertex cv = vers[index];
                    if (cv.m_dVerx < minX) minX = cv.m_dVerx;
                    if (cv.m_dVerx > maxX) maxX = cv.m_dVerx;
                    if (cv.m_dVery < minY) minY = cv.m_dVery;
                    if (cv.m_dVery > maxY) maxY = cv.m_dVery;
                }
            }
            //绘制矩形框
            glBegin(GL_LINE_STRIP);
            glVertex3d(minX, minY, 0.0);
            glVertex3d(minX, maxY, 0.0);
            glVertex3d(maxX, maxY, 0.0);
            glVertex3d(maxX, minY, 0.0);
            glVertex3d(minX, minY, 0.0);
            glEnd();
        }

        //标注每个face的id
        QPainter painter(this);
        for (const CFace& f : facs) {

            double x1 = vers[f.m_nFourIndex[0]].m_dVerx;
            double x2 = vers[f.m_nFourIndex[2]].m_dVerx;
            double y1 = vers[f.m_nFourIndex[0]].m_dVery;
            double y2 = vers[f.m_nFourIndex[2]].m_dVery;
			double xpos = ((x1 + x2) / 2.0 + 10.0) / 20.0 * width();
			double ypos = -((y1 + y2) / 2.0 - 10.0) / 20.0 * height();

			bool found = false;  //标记是否找到匹配的元素
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

//当鼠标被按下时，Qt 框架会自动调用这个函数
void OpenGLWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {

        //还未新建任何曲线错误弹框处理
        if (curveCount == -1) {
            QMessageBox errorMsgBox;// 创建一个错误消息对话框
            errorMsgBox.setWindowTitle("Error"); // 设置对话框标题
            errorMsgBox.setText("An error occurred :\nNo curves have been created yet!");// 设置错误消息文本
            errorMsgBox.setStandardButtons(QMessageBox::Ok); // 设置按钮为OK按钮
            errorMsgBox.exec();//显示消息对话框
            return;
        }

        // 将鼠标点击坐标转换为OpenGL视口坐标
        //int viewport[4];
        //glGetIntegerv(GL_VIEWPORT, viewport);

        double xpos = event->x();
        double ypos = event->y();

        // 将视口坐标转换为世界坐标
        //20.0（确定）和17.0则图像偏左上，0.0则偏向右下，8.0偏右下较少，9.0右下
        double worldX = (xpos / width()) * 20.0 - 10.0;
        double worldY = -(ypos / height()) * 20.0 + 10.0;

        // 添加控制点
        curve[curveIndex]->addControlPoint(Point({ worldX, worldY, 0.0 }));
        curve[curveIndex]->update();

        QString pointText = "(" + QString::number(worldX) + "," + QString::number(worldY) + "," + QString::number(0.0) + ")";
        // 发射信号并传递数据
        emit pointTextReady(pointText);

        // 重新绘制场景
        update();
    }
}

