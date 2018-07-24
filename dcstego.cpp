/*------------------------------------------------------------------------------
-- DATE:        October , 2016
--
-- SOURCE FILE: main.go
--
-- REVISIONS:
--
-- DESIGNER:    Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
--
-- INTERFACE:
--    explicit DcStego(dcImage * , QFile * , dcCrypto *, QObject *parent = 0);
--    bool     checkFit();
--    QByteArray messageArray();
--    size_t messageSize();
--    void read(dcCrypto &);
--    QString getFileName(QByteArray &);
--    void setNumFromStr(QByteArray &, int *);
--    void process();
--    void read();
--
--
-- NOTES: This file is responsible for linking the functionality of the other
--        files
------------------------------------------------------------------------------*/
#include "dcstego.h"

/*-----------------------------------------------------------------------------
-- FUNCTION:    DcStego::DcStego
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   DcStego::DcStego(dcImage * carrier, QFile * message, dcCrypto * crypt, QObject *parent)
--
-- PARAMS:      dcImage     * carrier   the image to embed the message on
--              QFile       * message   the message to be embeded
--              dcCrypto    * crypt     the cryptographic object to save write files based on.
--              QObject     * parent    the parent object
--
-- RETURNS:
--
-- NOTES:
------------------------------------------------------------------------------*/
DcStego::DcStego(dcImage * carrier, QFile * message, dcCrypto * crypt, QObject *parent) :
    QObject(parent),
    _carrier(carrier),
    _message(message),
    _crypt(crypt)
{

}


/*-----------------------------------------------------------------------------
-- FUNCTION:    DcStego::checkFit
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   DcStego::checkFit()
--
-- PARAMS:
--
-- RETURNS:     bool true if carrier can fit the message.
--
-- NOTES:
------------------------------------------------------------------------------*/
bool DcStego::checkFit() {
    size_t c_size = _carrier->bytes();
    size_t m_size = messageSize();

    // each byte will take 8 bytes to store, each pixel is 3 bytes
    c_size = (c_size * 3) / 8;

    return (m_size < c_size);
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    DcStego::messageSize
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   size_t DcStego::messageSize()
--
-- PARAMS:
--
-- RETURNS:     the size of the message (including meta data)
--
-- NOTES:
------------------------------------------------------------------------------*/
size_t DcStego::messageSize() {
    return _message->fileName().length() + sizeof('\0') + sizeof(_message->size()) + _message->size();
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    DcStego::messageArray()
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   DcStego::messageArray()
--
-- PARAMS:
--
-- RETURNS:     QByteArray the full message including meta data.
--
-- NOTES:
------------------------------------------------------------------------------*/
QByteArray DcStego::messageArray() {
    QByteArray data;
    QFileInfo fileInfo(*_message);
    size_t length = _message->size();

    _message->open(QIODevice::ReadOnly);
    data.append(fileInfo.fileName());
    data.append('\0');
    data.append((char*) &length, sizeof(_message->size()));

    data.append(_message->readAll());
    _message->close();

    return data;
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    void DcStego::process()
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   void DcStego::process()
--
-- PARAMS:
--
-- RETURNS:     void
--
-- NOTES:       write in the design, changed it to process for Qtifying purposes
------------------------------------------------------------------------------*/
void DcStego::process()
{
    QByteArray data = messageArray();
    if(checkFit())
    {
        emit setCompletion(0);
        data = _crypt->encrypt(data);
        emit setCompletion(33);
        _carrier->write(data);
        emit setCompletion(100);
        emit finished();
        emit success();
    }
    else
    {
        emit error(QString("Image too small to hold file"));
        emit finished();
    }
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    DcStego::read
--
-- DATE:        October 7th, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   void DcStego::read()
--
-- PARAMS:
--
-- RETURNS:
--
-- NOTES:
------------------------------------------------------------------------------*/
void DcStego::read()
{
    QByteArray data = _crypt->decrypt(_carrier->readAll());
    int size;

    _message->setFileName(_message->fileName() + getFileName(data));
    if(!data.size())
    {
        emit error("Invalid: no message found!");
        return;
    }
    setNumFromStr(data, &size);
    _message->open(QIODevice::WriteOnly);
    _message->write(data.toStdString().c_str(), size);
    _message->close();
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    DcStego::getFileName
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   DcStego::getFileName(QByteArray & data)
--
-- PARAMS:      QByteArray & data: Full array to read the filename from.
--
-- RETURNS:     QString
--
-- NOTES:
------------------------------------------------------------------------------*/
QString DcStego::getFileName(QByteArray & data)
{
    char * filename = new char[strlen(data.toStdString().c_str())];

    strcpy(filename, data.toStdString().c_str());
    data.remove(0, strlen(data.toStdString().c_str()) + 1);

    return QString(filename);
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    DcStego::setNumFromStr
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   DcStego::setNumFromStr(QByteArray & data, int * size)
--
-- PARAMS:      QByteArray & data
--              int * size
--
-- RETURNS:     void
--
-- NOTES:       will read the size into the int*
------------------------------------------------------------------------------*/
void DcStego::setNumFromStr(QByteArray & data, int * size) {
    memcpy(size, data, sizeof(qint64));
    data.remove(0, sizeof(qint64));
    data.truncate(*size);
}
