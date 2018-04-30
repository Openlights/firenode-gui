// Copyright 2018 Jon Evans <jon@craftyjon.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <QApplication>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "networking.h"
#include "unpacker.h"
#include "serial.h"

#include "firenodewindow.h"


#define MAX_OUTPUTS 256


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FirenodeWindow w;
    w.show();

    QFile config_file("config.json");

    if (!config_file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open config.json, cannot continue");
        return 1;
    }

    QByteArray config_data = config_file.readAll();
    QJsonDocument config_doc(QJsonDocument::fromJson(config_data));

    int udp_port = config_doc.object()["port"].toInt();
    bool listen_all = config_doc.object()["listenAll"].toBool(false);

    QJsonArray outputs = config_doc.object()["outputs"].toArray();

    Serial* serials[MAX_OUTPUTS];
    Unpacker* unpackers[MAX_OUTPUTS];

    int num_serials = 0;

    if (outputs.size() > MAX_OUTPUTS) {
        qWarning("Cannot have more than %d output devices.", MAX_OUTPUTS);
        return 2;
    }

    Networking net(udp_port, listen_all);
    QTimer *serial_timer = new QTimer(&app);
    serial_timer->setInterval(1.0 / 25.0);

    for (int output_index = 0; output_index < outputs.size(); output_index++) {
        QJsonObject output_obj = outputs[output_index].toObject();
        //qDebug() << output_index << output_obj;

        QString serial_port = output_obj["port"].toString();
        int first_strand = output_obj["first-strand"].toInt();
        int last_strand = output_obj["last-strand"].toInt();

        serials[output_index] = new Serial(serial_port);
        unpackers[output_index] = new Unpacker(first_strand, last_strand);
        num_serials++;

        QObject::connect(&net, SIGNAL(data_ready(QByteArray)), unpackers[output_index], SLOT(unpack_data(QByteArray)));
        QObject::connect(unpackers[output_index], SIGNAL(frame_end()), unpackers[output_index], SLOT(assemble_data()));
        QObject::connect(unpackers[output_index], SIGNAL(data_ready(QByteArray*)), serials[output_index], SLOT(update_data(QByteArray*)));
        QObject::connect(serial_timer, SIGNAL(timeout()), serials[output_index], SLOT(write_data()));
    }

    QThread netThread;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &net, SLOT(stop()));
    QObject::connect(&app, SIGNAL(aboutToQuit()), &netThread, SLOT(quit()));

    netThread.start();
    net.moveToThread(&netThread);
    net.start();

    return app.exec();
}
