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
--	explicit MainWindow(QWidget *parent)
--  void dragEnterEvent(QDragEventEvent *event) (overloaded)
--  void dropEvent(QDropEvent *event);
--  void setCarrier(QString &)
--  void decode(QString &)
--  void carrierSet()
--  (SLOTS)
--  void errorString(QString)
--  void onDecodeButtonPressed();
--  void setCompletion(int)
--  void save()
--
-- NOTES: This file handles the main window of the stego program.
--
------------------------------------------------------------------------------*/
#include "mainwindow.h"
#include "ui_mainwindow.h"


/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::MainWindow
--
-- DATE:        October 1st, 2016
--
-- REVISIONS:   October 7,  2016 - added decode button handler
--              October 14, 2016 - Added error window handler && Progress bar
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   MainWindow::MainWindow(QWidget *parent)
--
-- PARAMS:      QWidget *parent: the parent object.[5
--
-- RETURNS:     constructor
--
-- NOTES:   	this function sets the port into non blocking mode.
------------------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _carrier(NULL),
    _message(NULL),
    _errorWindow(new QErrorMessage),
    _crypt(NULL)
{
    ui->setupUi(this);
    connect(ui->decodeButton, SIGNAL(released()), this, SLOT(onDecodeButtonPressed()));
    connect(this, SIGNAL(error(QString)), _errorWindow, SLOT(showMessage(QString)));
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _carrier;
    delete _message;
    delete _crypt;
    delete _errorWindow;
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::onDecodeButtonPressed
--
-- DATE:        October 7, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   MainWindow::onDecodeButtonPressed()
--
-- RETURNS:     void
--
-- NOTES:   	This function is triggered when the decode button is pressed.
------------------------------------------------------------------------------*/
void MainWindow::onDecodeButtonPressed() {
    if(carrierSet()) {
        QFileDialog dialog;
        QStringList filenames;
        _message = new QFile;
        dialog.setFileMode(QFileDialog::Directory);

        if(dialog.exec()) {
            filenames = dialog.selectedFiles();  // just the save location for the image.
            decode(filenames.first());
        }
    } else {
        emit error(QString("No carrier image currently set"));
    }

}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::MainWindow
--
-- DATE:        October 2, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   MainWindow::MainWindow(QWidget *parent)
--
-- RETURNS:     void
--
-- NOTES:   	Saves the image in BMP format.
------------------------------------------------------------------------------*/
void MainWindow::save() {
    QFileDialog f;
    QStringList filenames;
    f.setFileMode(QFileDialog::AnyFile);
    f.setAcceptMode(QFileDialog::AcceptSave);

    if(f.exec()) {
        filenames = f.selectedFiles();
        _carrier->image().save(filenames.first(), "BMP");
    }
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::MainWindow
--
-- DATE:        October 2, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   void MainWindow::dragEnterEvent(QDragEnterEvent *event)
--
-- PARAMS:      QDragEnterEvent *event info associated with the event
--
-- RETURNS:     void
--
-- NOTES:   	This function is required for drag and drop
------------------------------------------------------------------------------*/
void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();

}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::dropEvent
--
-- DATE:        October 2, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   void MainWindow::dropEvent(QDropEvent *event)
--
-- PARAMS:      QDropEvent *event: info associated with the event
--
-- RETURNS:     void
--
-- NOTES:   	This function is overloaded from QMainWindow
------------------------------------------------------------------------------*/
void MainWindow::dropEvent(QDropEvent *event) {
    if(event->mimeData()->hasUrls()) {
        QString filename(event->mimeData()->urls().last().toLocalFile());
        delete _crypt;
        delete _message;
        if(carrierSet()) {
            encode(filename);
        } else {
            setCarrier(filename);
        }
    }
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::carrierSet
--
-- DATE:        October 2, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   MainWindow::carrierSet()
--
-- PARAMS:
--
-- RETURNS:     bool true if carrier is set.
--
-- NOTES:   	This function simplifies _carrier != null.
------------------------------------------------------------------------------*/
bool MainWindow::carrierSet() {
    return (_carrier != NULL);
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::setCarrier
--
-- DATE:        October 2, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   MainWindow::setCarrier(QString & filename)
--
-- PARAMS:      filename: the path to the file to use as the carrier.
--
-- RETURNS:     void
--
-- NOTES:   	Set the carrier image.
------------------------------------------------------------------------------*/
void MainWindow::setCarrier(QString & filename)
{
    QImage image(filename);
    if(image.format() != QImage::Format_Invalid)
    {
        image = image.convertToFormat(QImage::Format_RGB888);
        QGraphicsScene * scene = new QGraphicsScene(ui->imageView);
        scene->addPixmap(QPixmap::fromImage(image));
        ui->imageView->setScene(scene);
        _carrier = new dcImage(filename, image, 0);
    }
    else
    {
        emit error("Invalid image format! Please try a BMP");
    }

}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::decode
--
-- DATE:        October 7, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   MainWindow::decode(QString & filename)
--
-- PARAMS:      filename: directory to save file to.
--
-- RETURNS:     void
--
-- NOTES:   	Read the file out of the carrier image.
------------------------------------------------------------------------------*/
void MainWindow::decode(QString & filename)
{
    _message->setFileName(filename + QDir::separator());
    _crypt = new dcCrypto(ui->password->text());
    DcStego * worker = new DcStego(_carrier, _message, _crypt);
    QThread * thread = new QThread(this);

    connect(worker, SIGNAL (error(QString)), this, SLOT (errorString(QString)));
    connect(thread, SIGNAL (started()),  worker, SLOT (read()));
    connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    connect(worker, SIGNAL (setCompletion(int)), this, SLOT (setCompletion(int)));
    thread->start();
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::encode
--
-- DATE:        October 1, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   void MainWindow::encode(QString & filename)
--
-- PARAMS:      filename: directory to save file to.
--
-- RETURNS:     void
--
-- NOTES:   	Read the file out of the carrier image.
------------------------------------------------------------------------------*/
void MainWindow::encode(QString & filename) {
    _message = new QFile(filename);
    _crypt = new dcCrypto(ui->password->text());
    DcStego * worker = new DcStego(_carrier, _message, _crypt);
    QThread * thread = new QThread(this);
    connect(worker, SIGNAL (error(QString)), this, SLOT (errorString(QString)));
    connect(thread, SIGNAL (started()),  worker, SLOT (process()));
    connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    connect(worker, SIGNAL (setCompletion(int)), this, SLOT (setCompletion(int)));
    connect(worker, SIGNAL (success()), this, SLOT (save()));
    thread->start();

}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::errorString(QString msg)
--
-- DATE:        October 14, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   MainWindow::errorString(QString msg)
--
-- PARAMS:      msg: Message to show in the error window.
--
-- RETURNS:     void
--
-- NOTES:   	Displays an error message.
------------------------------------------------------------------------------*/
void MainWindow::errorString(QString msg) {
    emit error(msg);
}

/*-----------------------------------------------------------------------------
-- FUNCTION:    MainWindow::setCompletion
--
-- DATE:        October 14, 2016
--
-- REVISIONS:
--
-- DESIGNER:	Marc Vouve
--
-- PROGRAMMER:  Marc Vouve
--
-- INTERFACE:   MainWindow::setCompletion(int progress)
--
-- PARAMS:      progress: the % complete to show on the progress bar.
--
-- RETURNS:     void
--
-- NOTES:   	Changes the progress bar.
------------------------------------------------------------------------------*/
void MainWindow::setCompletion(int progress) {
    ui->progressBar->setValue(progress);
}
