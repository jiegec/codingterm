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

#include "worker.h"
#include "algo.h"
#include <QDebug>

Worker::Worker(Chip *chip) : chip(chip) {}

void Worker::calculate() {
  std::vector<double> result;
  std::vector<double> length;
  for (int i = 0; i <= chip->side; i++) {
    for (int j = 0; j < chip->side; j++) {
      if (chip->disabled_v[i][j]) {
        length.push_back(0);
      } else {
        length.push_back(chip->width_v[i][j]);
      }
    }
  }
  for (int i = 0; i < chip->side; i++) {
    for (int j = 0; j <= chip->side; j++) {
      if (chip->disabled_h[i][j]) {
        length.push_back(0);
      } else {
        length.push_back(chip->width_h[i][j]);
      }
    }
  }
  for (int i = 0; i < INPUT_NUM; i++) {
    length.push_back(chip->inputWidth[i]);
  }
  for (int i = 0; i < OUTPUT_NUM; i++) {
    length.push_back(chip->outputWidth[i]);
  }
  qWarning() << "length of size" << length.size();
  result =
      caluconspeed(chip->side + 1, length, chip->inputCol[0], chip->inputCol[1],
                   chip->outputCol[0], chip->outputCol[1], chip->outputCol[2]);
  qWarning() << "Got result" << result;
  emit finished(QVector<double>::fromStdVector(result));
}