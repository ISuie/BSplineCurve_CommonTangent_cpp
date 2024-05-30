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
    //按钮点击
    void on_pushButton_clicked();  //求导数
    void on_pushButton_2_clicked();//求索引
    void on_pushButton_3_clicked();//切换曲线
    void on_pushButton_4_clicked();//清空当前曲线控制点
    void on_pushButton_5_clicked();//求公切线
    void on_pushButton_6_clicked();//新建曲线
    void on_pushButton_7_clicked();//求坐标
    void on_pushButton_8_clicked();//是否显示控制多边形
    void on_pushButton_9_clicked();//求拐点按钮
    void on_pushButton_11_clicked();//求重合角度
    void on_pushButton_12_clicked();//分出c型曲线
    void on_pushButton_13_clicked();//BezierExtration
    void on_pushButton_14_clicked();//求两条B-Spline中的第一段Bezier是否有切线
    void on_pushButton_15_clicked();//隐藏或者显示Bezier
    void on_pushButton_16_clicked();//把bezier曲线分割成两部分

    void on_pushButton_17_clicked();//功能：B-Spline相关包括extract之后的绘制/bezier直接绘制
    void on_pushButton_18_clicked();//求bezier曲线首末点

    void on_pushButton_10_clicked();//清空测试区域数据

    //接受新控制顶点坐标信号
    void handlePointText(const QString& pointText);

    //针对f(s,t)得到f1, f2, f1 + cf2, f1 + cf2
    void on_pushButton_19_clicked();//附加fac、C、B函数

    //drawMesh
    void on_pushButton_20_clicked();//创建网格
    void on_pushButton_21_clicked();//cross指定id的face
    void on_pushButton_22_clicked();//求连通指定id的face

    //对两条曲线进行处理做一次f1，f2的判断（只初始第一次）
    void on_pushButton_23_clicked();//判断是否有切线
    //对两条曲线进行处理做三次f1，f2的判断
    void on_pushButton_24_clicked();//细分三次区间判断区间是否有切线
};
