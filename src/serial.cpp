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

#include "serial.h"


Serial::Serial(const QString name)
{
    //QSerialPortInfo info = QSerialPortInfo(name);
    _packets = 0;
    _port_name = name;
    _timer = 0;

    open_port();

    _exit = false;
    _packet_in_process = false;
    _pending_write = false;
}


Serial::~Serial()
{
    _port.close();
    if (_timer) { _timer->deleteLater(); }
}

bool Serial::open_port()
{
    bool success = true;

    _port.close();
    _port.setPortName(_port_name);

    if (!_port.open(QIODevice::ReadWrite)) {
        qDebug() << "Could not open port, code" << _port.error();
        success = false;
    }

    //if (!_port.setBaudRate(2000000)) {
    if (!_port.setBaudRate(1000000)) {
        qDebug() << "Error setting baud rate, code" << _port.error();
        qDebug() << "Current rate is" << _port.baudRate();
        success = false;
    }

    if (!_port.setDataBits(QSerialPort::Data8)) {
        qDebug() << "Error setting up port databits, code" << _port.error();
        success = false;
    }

    _port.setParity(QSerialPort::NoParity);
    _port.setStopBits(QSerialPort::OneStop);
    _port.setFlowControl(QSerialPort::NoFlowControl);

    if (success) {
        _open = true;
    }

    return success;
}

void Serial::packet_start()
{
    _packet_in_process = true;
    _q.clear();
}


void Serial::packet_done()
{
    _packet_in_process = false;
    _pending_write = true;
}


void Serial::shutdown()
{
    _exit = true;
    _timer->stop();
    _timer->deleteLater();
}

void Serial::update_data(QByteArray *data)
{
    _next_frame = *data;
}

void Serial::write_data()
{
    //char reply[256];

    if (!_open) {
        if (!open_port()) {
            return;
        }
    }

    //_frame = QByteArray::fromRawData(_next_frame.data(), _next_frame.length());
    _frame = _next_frame;
    //qDebug() << _frame.toHex().left(16);

    if (_frame.length() == 0) {
        return;
    }

    int rc = _port.write(_frame);
    if (rc < 0) {
        qDebug() << "Write error";
    }
#if 0
    else {
        qDebug("wrote %d bytes", rc);
    }
#endif

    if (!_port.waitForBytesWritten(100)) {
        qDebug() << "Timeout!";
        _open = false;
        // Probably teensy power was pulled.  Let's try reopening the port.
    }
#if 0
    if (_port.waitForReadyRead(1000)) {
        _port.read(reply, 256);
        qDebug() << "Reply:" << reply;
    }
#endif
    _packets++;

    //_port.flush();
}

#if 0
void Serial::enqueue_data(QByteArray *data, bool force)
{
    // Hack.
    if (force || !_pending_write)
        _q.enqueue(*data);
}


unsigned long long Serial::get_pps_and_reset()
{
    unsigned long long p = _packets;
    _packets = 0;
    return p;
}


void Serial::print_stats()
{
    unsigned long long packets = get_pps_and_reset();
    qDebug("%0.2f packets/sec, _last_packet.size = %d", (double)packets / STATS_TIME, _last_packet.size());
    qDebug() << _last_packet.toHex();

}
#endif
