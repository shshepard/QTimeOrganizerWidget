#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QCheckBox>
#include <QDebug>
#include "qtimeorganizerwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

    void addBusyRange(const QDateTime&,
                      const QDateTime&,
                      const QString& text);

private slots:

    void timeChanged(const QDateTime &begin, const QDateTime &end);
    void onCheckBoxShowSettingsClicked(bool checked);

    //gui

    void on_checkBoxGrid_clicked();
    void on_spinBoxMinItemHeight_valueChanged(int arg1);
    void on_comboBoxGridSep_currentIndexChanged(int index);
    void on_checkBoxTracking_clicked();
    void on_checkBoxDrawGrid_clicked();
    void on_spinBoxHours_valueChanged(int arg1);
    void on_pushButtonSetRange_clicked();
    void on_pushButtonSetDate_clicked();
    void on_pushButtonClearSelection_clicked();
    void on_pushButtonShowHeader_clicked();
    void on_pushButtonHideHeader_clicked();
    void on_checkBoxShowHint_clicked();
    void on_comboBoxScrollMode_currentIndexChanged(int index);
    void on_pushButtonSetHour_clicked();
    void on_pushButtonSetBusyRange_clicked();
    void on_pushButtonClearBusyRange_clicked();

private:
    Ui::MainWindow *ui;

    QCheckBox* checkBoxShowSettings_ = { new QCheckBox(this) };
};

#endif // MAINWINDOW_H
