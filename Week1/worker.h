// Copyright (C) 2018 Jiajie Chen
// 
// This file is part of Week1.
// 
// Week1 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Week1 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Week1.  If not, see <http://www.gnu.org/licenses/>.
// 

#ifndef WORKER_H
#define WORKER_H

#include "chip.h"
#include <QVector>

class Worker : public QObject {
    Q_OBJECT
public:
    Worker(Chip *chip);
public slots:
    void calculate();
signals:
    void finished(QVector<double>);
private:
    Chip *chip;
};

#endif

