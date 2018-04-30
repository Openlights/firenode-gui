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

#ifndef _UNPACKER_H
#define _UNPACKER_H

#include <QtCore/QObject>
#include <QtCore/QDebug>

#define MAX_STRANDS 128


//! Unpacks data received over the network
class Unpacker : public QObject
{
    Q_OBJECT

public:
    Unpacker(int first, int last);
    ~Unpacker();

public slots:
    void unpack_data(QByteArray data);
    void assemble_data(void);

signals:
    void data_ready(QByteArray *data);
    void frame_begin(void);
    void frame_end(void);

private:
    QByteArray strand_data[MAX_STRANDS];
    int first_strand;
    int last_strand;

};

#endif
