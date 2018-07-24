#ifndef DCSTEGO_H
#define DCSTEGO_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include "dcstego.h"
#include "dcimage.h"
#include "dccrypto.h"

class DcStego : public QObject
{
    Q_OBJECT
public:
    explicit DcStego(dcImage * , QFile * , dcCrypto *, QObject *parent = 0);
    bool     checkFit();
    QByteArray messageArray();
    size_t messageSize();

    void read(dcCrypto &);
private:
    QString getFileName(QByteArray &);
    void setNumFromStr(QByteArray &, int *);
    dcImage  * _carrier;
    QFile    * _message;
    dcCrypto * _crypt;

public slots:
    void process();
    void read();
signals:
    void error(QString);
    void finished();
    void success();
    void setCompletion(int);

public slots:
};

#endif // DCSTEGO_H
