#pragma once//C2011错误

#include <QtWidgets/QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>

#include <iostream>
using namespace std;

#include "Curve.h"

#include <GL/glut.h>

#include "BSplineCurve.h"
#include "BezierCurve.h"
#include "Tmesh.h"

extern int curveIndex;
extern int curveCount;
extern bool isPolygonVisible;
extern bool isBezierVisible;
extern bool drawWhich;

extern bool drawMesh;//测试mesh绘制

class OpenGLWidget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT

signals:
    // 定义一个信号，用于发送控制点坐标
    void pointTextReady(const QString& data);

public:
    OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();

protected:
    //需要在cpp文件中实现
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    virtual void mousePressEvent(QMouseEvent* event);

public:
    //B-样条曲线
    BSplineCurve* curve[100] = { nullptr };

    //每条曲线拐点坐标
    std::vector<Point> *inflectionPoint[100] = { nullptr };

    //切线端点
    std::vector<Point> commonTangent;

    //Bezier曲线求切线
    std::vector<Point> tantan;

    //测试BezierCurve类中的extract函数
    std::vector<Point> control_1;
    std::vector<Point> control_2;

    //测试
    BezierCurve bezierCurve;

    //mesh方面测试
    CMesh M;
    vector<vector<int>> connected;
    vector<int> remainid;
};

