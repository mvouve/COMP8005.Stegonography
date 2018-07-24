#ifndef DCCRYPTO_H
#define DCCRYPTO_H

#include <QString>
#include <QByteArray>

class dcCrypto
{
public:
    dcCrypto(QString);
    QByteArray encrypt(QByteArray &);
    QByteArray decrypt(QByteArray &);
private:
    QString _key;
};

#endif // DCCRYPTO_H
