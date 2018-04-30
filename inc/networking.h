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

#ifndef _NETWORKING_H
#define _NETWORKING_H

#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtNetwork/QUdpSocket>

#define MAX_PACKET_SIZE 16384

//#define USE_ZMQ

#ifdef USE_ZMQ
#include "zmq.h"
#endif

class Networking : public QObject
{
    Q_OBJECT

public:
    Networking(int port, bool listen_all);
    ~Networking();

    bool open(void);
    bool close(void);

public slots:
    void start(void);
    void run(void);
    void stop(void);
    void get_data(void);

private slots:
    void read_pending_packets(void);

signals:
    void data_ready(QByteArray data);

private:

    void *context;
    void *subscriber;
    int port;
    bool running;

    QTimer *_timer;
    QUdpSocket *_socket;
};

#endif
