#ifndef DCIMAGE_H
#define DCIMAGE_H

#include <QObject>
#include <QFile>
#include <QImage>
#include <QPoint>
#include <QString>
#include <QColor>
#include <exception>
#include <memory>

class dcImage : QObject
{
    Q_OBJECT
public:
    dcImage(QString, QImage, QObject *);
    QString filename();
    QImage  image();
    void  hideData(QPoint start, QFile data);
    void write(QByteArray message);
    void write(char c);
    void writeBit(bool bit);
    void nextPixel();
    void nextBit();

    char read();
    bool readBit();
    QByteArray readAll();
    size_t bytes();
private:
    QColor makeColor(char * data, char bit, QColor color);
    QPoint nextPixel(QPoint init);
    QString _filename;
    QImage  _image;
    QPoint  _currentPlace;
    quint8  _bit;

signals:

public slots:
};

#endif // DCIMAGE_H
