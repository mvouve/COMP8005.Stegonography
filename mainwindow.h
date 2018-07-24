#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QErrorMessage>
#include <QThread>
#include <string>
#include "dcimage.h"
#include "dcstego.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    ~MainWindow();

    void setCarrier(QString & filename);
    void decode(QString & filename);
    void encode(QString & filename);
    bool carrierSet();

public slots:
    void errorString(QString);
    void onDecodeButtonPressed();
    void setCompletion(int);
    void save();
private:
    dcImage         * _carrier;
    QFile           * _message;
    QErrorMessage   * _errorWindow;
    dcCrypto        * _crypt;
    Ui::MainWindow  * ui;

signals:
    void error(QString);
};

#endif // MAINWINDOW_H
