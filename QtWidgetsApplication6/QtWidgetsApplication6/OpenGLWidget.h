#pragma once//C2011����

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

extern bool drawMesh;//����mesh����

class OpenGLWidget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT

signals:
    // ����һ���źţ����ڷ��Ϳ��Ƶ�����
    void pointTextReady(const QString& data);

public:
    OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget();

protected:
    //��Ҫ��cpp�ļ���ʵ��
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    virtual void mousePressEvent(QMouseEvent* event);

public:
    //B-��������
    BSplineCurve* curve[100] = { nullptr };

    //ÿ�����߹յ�����
    std::vector<Point> *inflectionPoint[100] = { nullptr };

    //���߶˵�
    std::vector<Point> commonTangent;

    //Bezier����������
    std::vector<Point> tantan;

    //����BezierCurve���е�extract����
    std::vector<Point> control_1;
    std::vector<Point> control_2;

    //����
    BezierCurve bezierCurve;

    //mesh�������
    CMesh M;
    vector<vector<int>> connected;
    vector<int> remainid;
};

