#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QScrollBar>
#include <channel.h>
#include <server.h>
#include <qdebug.h>
#include <line.h>
#include <vector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    socket(new QTcpSocket(this)),
	servers(new std::list<Server*>())
{
    ui->setupUi(this);
    connectToServer("irc.freenode.net", 6667);
}

MainWindow::~MainWindow()
{
	servers->remove_if(
		[](Server *element)
	{
		delete element;
		return true;
	}
	);
	delete servers;
    delete ui;
}

void MainWindow::connectToServer(QString server, int port)
{
    connect(socket, &QIODevice::readyRead, this, &MainWindow::readStream);
    socket->connectToHost(server, port);
	//add a wait for connection someday
	Server* s = new Server(server, port);
	servers->push_back(s);
	ui->serverCombo->addItem(s->getAddress());
    socket->write("NICK TEST444999\r\n");
    socket->write("USER TEST444999 0 * :TEST\r\n");
}

QString MainWindow::parsedLine(QString line)
{
	QString prefixEnd = line.section(':', 1);
	QString prefixMiddle = prefixEnd.split(" :").first();
	//qDebug() << line;
	QString prefix = prefixMiddle.section(' ', 0, 0);
	QString command = prefixMiddle.section(' ', 1, 1);
	QString middle = prefixMiddle.section(' ', 2);
	QStringList middleParams = middle.split(QRegExp("\\s+"),
		QString::SkipEmptyParts);
	//qDebug() << middleParams.count();
	//QString trailing = line.section(' :', -1, -1);
	QString result;
	if (line.contains(" :"))
	{
		QString trailing = prefixEnd.split(" :").last();
		result = trailing;
	}
	else
	{
		result = command + " " + middle;
	}
	//Line  l(prefix, command, middleParams, trailing);
	return result;
}

void MainWindow::readStream()
{
	while (socket->canReadLine())
	{
		QString line = socket->readLine();
		addText(parsedLine(line));
	}
}

void MainWindow::on_lineEdit_returnPressed()
{
    QString text = ui->lineEdit->text() + "\r\n";
    if (text.length() > 0)
    {
        socket->write(text.toUtf8());
        addText(text);
        ui->lineEdit->clear();
    }
}

QStringList MainWindow::splitLine(QString line)
{
	return	line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
}

void MainWindow::addText(QString text)
{
    totalText += text;
    QScrollBar *scrollBar = ui->textBrowser->verticalScrollBar();
    bool isMax = scrollBar->value() == scrollBar->maximum();

    ui->textBrowser->setText(totalText);

    if (isMax)
    {
        scrollBar->setValue(scrollBar->maximum());
    }
}

void MainWindow::on_channelCombo_activated(int index)
{
	qDebug() << "channel act";
}

void MainWindow::on_serverCombo_activated(int index)
{
	qDebug() << "server act";
}

void MainWindow::on_channelCombo_editTextChanged(const QString &arg1)
{

}
