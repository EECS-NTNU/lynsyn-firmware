/******************************************************************************
 *
 *  This file is part of the TULIPP Analysis Utility
 *
 *  Copyright 2018 Asbjørn Djupdal, NTNU, TULIPP EU Project
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef PROFLINE_H
#define PROFLINE_H

#include "lynsyn.h"
#include "measurement.h"

class ProfLine {

  static bool lessThan(const Measurement &e1, const Measurement &e2) {
    return e1.time < e2.time;
  }

public:
  QString id;
  double runtime;
  double power;
  double energy;
  QVector<Measurement> measurements;

  ProfLine(QString id) {
    this->id = id;

    this->runtime = 0;
    this->power = 0;
    this->energy= 0;
  }

  void addMeasurement(Measurement m) {
    this->runtime += m.timeSinceLast;
    this->power = m.power;
    this->energy += m.power * m.timeSinceLast;
    measurements.push_back(m);
  }

  QVector<Measurement> *getMeasurements() {
    qSort(measurements.begin(), measurements.end(), lessThan);
    return &measurements;
  }

};

#endif
