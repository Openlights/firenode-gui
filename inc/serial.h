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

#ifndef _SERIAL_H
#define _SERIAL_H

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtCore/QQueue>
#include <QtCore/QTimer>


#define STATS_TIME 1.0


//! Writes data to strand controller connected to a virtual serial port.
class Serial : public QObject
{
    Q_OBJECT

public:
    Serial(const QString name);
    ~Serial();
    //unsigned long long get_pps_and_reset(void);
    void run(void);

public slots:
    void update_data(QByteArray *data);
    void write_data(void);
    //void enqueue_data(QByteArray *data, bool force=false);
    //void print_stats(void);
    //void process_loop(void);
    void shutdown(void);
    void packet_start(void);
    void packet_done(void);

signals:
    void data_written();

private:
    bool open_port(void);

    QString _port_name;
    QSerialPort _port;
    bool _open;
    QTimer *_timer;
    bool _packet_in_process;
    bool _pending_write;

    unsigned long long _packets;
    bool _exit;
    QQueue<QByteArray> _q;
    QByteArray _frame;
    QByteArray _next_frame;

    QByteArray _packet_start_frame, _packet_end_frame;

};

#endif
