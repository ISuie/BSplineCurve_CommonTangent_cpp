#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication6.h"

//#include "OpenGLWidget.h"
#include "BSplineCurve.h"
#include "BezierCurve.h"
#include "Tmesh.h"

#include <math.h>

class QtWidgetsApplication6 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication6(QWidget* parent = nullptr);
    ~QtWidgetsApplication6();

private:
    Ui::QtWidgetsApplication6Class ui;

public slots:
    //��ť���
    void on_pushButton_clicked();  //����
    void on_pushButton_2_clicked();//������
    void on_pushButton_3_clicked();//�л�����
    void on_pushButton_4_clicked();//��յ�ǰ���߿��Ƶ�
    void on_pushButton_5_clicked();//������
    void on_pushButton_6_clicked();//�½�����
    void on_pushButton_7_clicked();//������
    void on_pushButton_8_clicked();//�Ƿ���ʾ���ƶ����
    void on_pushButton_9_clicked();//��յ㰴ť
    void on_pushButton_11_clicked();//���غϽǶ�
    void on_pushButton_12_clicked();//�ֳ�c������
    void on_pushButton_13_clicked();//BezierExtration
    void on_pushButton_14_clicked();//������B-Spline�еĵ�һ��Bezier�Ƿ�������
    void on_pushButton_15_clicked();//���ػ�����ʾBezier
    void on_pushButton_16_clicked();//��bezier���߷ָ��������

    void on_pushButton_17_clicked();//���ܣ�B-Spline��ذ���extract֮��Ļ���/bezierֱ�ӻ���
    void on_pushButton_18_clicked();//��bezier������ĩ��

    void on_pushButton_10_clicked();//��ղ�����������

    //�����¿��ƶ��������ź�
    void handlePointText(const QString& pointText);

    //���f(s,t)�õ�f1, f2, f1 + cf2, f1 + cf2
    void on_pushButton_19_clicked();//����fac��C��B����

    //drawMesh
    void on_pushButton_20_clicked();//��������
    void on_pushButton_21_clicked();//crossָ��id��face
    void on_pushButton_22_clicked();//����ָͨ��id��face

    //���������߽��д�����һ��f1��f2���жϣ�ֻ��ʼ��һ�Σ�
    void on_pushButton_23_clicked();//�ж��Ƿ�������
    //���������߽��д���������f1��f2���ж�
    void on_pushButton_24_clicked();//ϸ�����������ж������Ƿ�������
};
