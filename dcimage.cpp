/*------------------------------------------------------------------------------
-- DATE:        October, 2016
--
-- Source File:	 mainwindow.cpp
--
-- REVISIONS: 	(Date and Description)
--
-- DESIGNER:	 Marc Vouve
--
-- PROGRAMMER:	 Marc Vouve
--
--
-- INTERFACE:
--    dcImage(QString, QImage, QObject *);
--    QString filename();
--    QImage  image();
--    void  hideData(QPoint start, QFile data);
--    void write(QByteArray message);
--    void write(char c);
--    void writeBit(bool bit);
--    void nextPixel();
--    void nextBit();
--
--    char read();
--    bool readBit();
--    QByteArray readAll();
--    size_t bytes();
--
-- NOTES: This file contains functions that handle images and image writing which
--        aren't handled by Qt. The general purpose of this would be writing to LSBs
--
------------------------------------------------------------------------------*/

#include "dcimage.h"

dcImage::dcImage(QString s, QImage i, QObject * parent = 0) : QObject(parent),
    _filename(s),
    _image(i),
    _bit(0),
    _currentPlace(0,0)
{

}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::filename
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   dcImage::filename()
--
-- PARAMS:
--
-- RETURNS:     Filename of the image currently loaded.
--
-- NOTES:       This is not contained with the image.
------------------------------------------------------------------------------*/
QString dcImage::filename() {
    return _filename;
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    QImage dcImage::image()
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   QImage dcImage::image()
--
-- PARAMS:
--
-- RETURNS:     The QImage handle of the image.
--
-- NOTES:       XOR encrypt
------------------------------------------------------------------------------*/
QImage dcImage::image() {
    return _image;
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::readAll
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   dcImage::readAll()
--
-- PARAMS:
--
-- RETURNS:     QByteArray the message contained in the LSBs of the image.
--
-- NOTES:       this is the main read function, most read functions in this
--              file are intended as helpers for this one.
------------------------------------------------------------------------------*/
QByteArray dcImage::readAll() {
    QByteArray message;
    int i = 0;
    while(_currentPlace.x() < _image.width() && _currentPlace.y() < _image.height()) {
        try
        {
            message.append(read());
        }
        catch(std::exception &e)
        {
            return message;
        }
    }

    return message;
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::read
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   dcImage::read()
--
-- PARAMS:
--
-- RETURNS:     char: a single character read from the image
--
-- NOTES:
------------------------------------------------------------------------------*/
char dcImage::read() {
    char c = 0;
    char b;

    for(int i = 0; i < 8; i++) {
        b = readBit() << i;
        c |= b;
    }
    return c;
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::readBit()
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   dcCrypto::encrypt(QByteArray & plainText)
--
-- PARAMS:      QByteArray & plainText: The plaintext string to encrypt.
--
-- RETURNS:     QByteArray the encrypted string
--
-- NOTES:       reads the current bit that the _currentPlace is pointed to
--              advances current place if out of bits.
------------------------------------------------------------------------------*/
bool dcImage::readBit() {
    bool read;
    switch(_bit) {
    case 0:
        read = (QColor(_image.pixel(_currentPlace)).red()   & 1);
        break;
    case 1:
        read = (QColor(_image.pixel(_currentPlace)).green() & 1);
        break;
    case 2:
        read = (QColor(_image.pixel(_currentPlace)).blue()  & 1);
        break;
    }
    nextBit();
    return read;
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::bytes
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   size_t dcImage::bytes()
--
-- PARAMS:
--
-- RETURNS:     the total pixels in the image.
--
-- NOTES:
------------------------------------------------------------------------------*/
size_t dcImage::bytes() {
    return _image.width() * _image.height();
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::write(QByteArray message)
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   void dcImage::write(QByteArray message)
--
-- PARAMS:      message: a multibyte string to write to the file.
--
-- RETURNS:     void
--
-- NOTES:
------------------------------------------------------------------------------*/
void dcImage::write(QByteArray message) {
    for(int i = 0; i < message.size(); i++) {
        write(message.at(i));
    }
    _currentPlace.setX(0);
    _currentPlace.setY(0);
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    void dcImage::write(char c)
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   void dcImage::write(char c)
--
-- PARAMS:      char c
--
-- RETURNS:     void
--
-- NOTES:       Write a single character into the LSB of colour channels.
------------------------------------------------------------------------------*/
void dcImage::write(char c) {
    for(int i = 0; i < 8; i++) {
        writeBit(c & 1);
        c >>= 1;
    }
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::writeBit
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   dcImage::writeBit(bool bit)
--
-- PARAMS:      bool bit: the value to assign the LSB to.
--
-- RETURNS:     void
--
-- NOTES:       Advances current bit once complete
------------------------------------------------------------------------------*/
void dcImage::writeBit(bool bit) {
    QColor c = _image.pixel(_currentPlace);

    switch(_bit) {
    case 0:
        _image.setPixel(_currentPlace, QColor((bit)?c.red()|1:c.red()&~1, c.green(), c.blue()).rgb());
        break;
    case 1:
        _image.setPixel(_currentPlace, QColor(c.red(), (bit)?c.green()|1:c.green()&~1, c.blue()).rgb());
        break;
    case 2:
        _image.setPixel(_currentPlace, QColor(c.red(), c.green(), (bit)?c.blue()|1:c.blue()&~1).rgb());
        break;
    default:
        break;
    }
    nextBit();
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::nextBit
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   dcImage::nextBit()
--
-- PARAMS:
--
-- RETURNS:     void
--
-- NOTES:       advance the current bit, if out of color channels,
------------------------------------------------------------------------------*/
void dcImage::nextBit() {
    if(_bit < 2) {
        _bit++;
    } else {
        _bit = 0;
        nextPixel();
    }
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcImage::nextPixel
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   dcImage::nextPixel()
--
-- PARAMS:
--
-- RETURNS:     void
--
-- NOTES:       advances the pixel by 1, if width is reached move down 1 line.
------------------------------------------------------------------------------*/
void dcImage::nextPixel() {
    if(_currentPlace.x() + 1 < _image.width()) {
        _currentPlace.setX(_currentPlace.x() + 1);
    } else if(_currentPlace.y() + 1 < _image.height()) {
        _currentPlace.setX(0);
        _currentPlace.setY(_currentPlace.y() + 1);
    } else {
        throw std::exception("Pixel out of bounds");
    }

}
