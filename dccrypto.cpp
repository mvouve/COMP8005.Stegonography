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
--	dcCrypto
--  QByteArray encrypt(QByteArray &)
--  QByteArray decrypt(QByteArray &)
--
-- NOTES: This file contains cryptographic functions. (that aren't very good)
--
------------------------------------------------------------------------------*/
#include "dccrypto.h"

dcCrypto::dcCrypto(QString key) :
    _key(key)
{
    if(key.length() < 1) {
        _key = "THIS IS AN UNSECURE KEY";
    }
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcCrypto::encrypt
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
-- NOTES:       XOR encrypt
------------------------------------------------------------------------------*/
QByteArray dcCrypto::encrypt(QByteArray & plainText)
{
    QByteArray cypher;

    for(int j = 0; j < plainText.length(); j++) {
        cypher[j] = plainText.at(j) ^ _key.toLatin1().at(j % _key.length());
    }

    return cypher;
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    dcCrypto::decrypt
--
-- DATE:        October 7st, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   dcCrypto::decrypt(QByteArray & cypherString)
--
-- PARAMS:      QByteArray & cypherString: The cypherString string to decrypt.
--
-- RETURNS:     QByteArray Plaintext version of the string
--
-- NOTES:   	XOR decrypt
------------------------------------------------------------------------------*/
QByteArray dcCrypto::decrypt(QByteArray & cypherString)
{
    QByteArray plainText;

    for(int j = 0; j < cypherString.length(); j++) {
        plainText[j] = cypherString.at(j) ^ _key.toLatin1().at(j % _key.length());
    }

    return plainText;
}
