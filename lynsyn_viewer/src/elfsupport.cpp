/******************************************************************************
 *
 *  This file is part of the Lynsyn host tools
 *
 *  Copyright 2019 Asbjørn Djupdal, NTNU
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

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QProcess>

#include "elfsupport.h"

///////////////////////////////////////////////////////////////////////////////

Addr2Line ElfSupport::addr2Line(QString elfFile, uint64_t addr) {
  QString fileName = "Unknown";
  QString function = "Unknown";
  uint64_t lineNumber = 0;

  // create command
  QString cmd =
    QString::fromStdString(qgetenv("CROSS_COMPILE").toStdString()) +
    "addr2line -C -f " + QString::number(addr, 16) + " -e " + elfFile;

  // run addr2line program
  QProcess p;
  p.start(cmd);
  p.waitForFinished();
  QString buf = QString::fromStdString(p.readAllStandardOutput().toStdString());
  QStringList lines = buf.split(QRegExp("\n|\r\n|\r"));

  // get function name
  if(lines.size() < 2) goto error;

  function = lines[0].simplified();
  if(function == "??") function = "Unknown";

  // get filename and linenumber
  {
    QString qbuf = lines[1];
    fileName = qbuf.left(qbuf.indexOf(':'));
    lineNumber = qbuf.mid(qbuf.indexOf(':') + 1).toULongLong();
  }

  if((function != "Unknown") || (lineNumber != 0)) {
    Addr2Line addr2line = Addr2Line(fileName, elfFile, function, lineNumber);
    addr2lineCache[addr] = addr2line;
    return addr2line;
  }

 error:
  return Addr2Line();
}

uint64_t ElfSupport::lookupSymbol(QString symbol) {
  for(auto elfFile : elfFiles) {
    // create command line
    QString cmd = QString("nm ") + elfFile;

    // run program
    QProcess p;
    p.start(cmd);
    p.waitForFinished();
    QString buf = QString::fromStdString(p.readAllStandardOutput().toStdString());
    QStringList lines = buf.split(QRegExp("\n|\r\n|\r"));

    for(auto line : lines) {
      QStringList tokens = line.split(' ');
      if(tokens.size() > 2) {
        if(tokens[2].trimmed() == symbol) {
          return tokens[0].toULongLong(0, 16);
        }
      }
    }
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////

char *ElfSupport::readLine(char *s, int size, FILE *stream) {
  char *ret = NULL;

  while(!ret) {
    ret = fgets(s, size, stream);
    if(feof(stream)) return NULL;
    if(ferror(stream)) {
      if(errno == EINTR) clearerr(stream);
      else return NULL;
    }
  }

  return ret;
}

void ElfSupport::setPc(uint64_t pc) {
  if (prevPc == pc) return;
    // check if pc exists in cache
  auto it = addr2lineCache.find(pc);
  if(it != addr2lineCache.end()) {
    addr2line = (*it).second;
    return;
  }

  // check if pc exists in elf files
  for(auto elfFile : elfFiles) {
    if(!elfFile.trimmed().isEmpty()) {
      addr2line = addr2Line(elfFile, pc);
      if(addr2line.valid) {
        addr2lineCache[pc] = addr2line;
        return;
      }
    }
  }

  // check if pc exists in kallsyms file
  if(!symsFile.trimmed().isEmpty()) {
    QFile file(symsFile);
    if(file.open(QIODevice::ReadOnly)) {

      QString lastSymbol;
      quint64 lastAddress = ~0;

      while(!file.atEnd()) {
        QString line = file.readLine();

        QStringList tokens = line.split(' ');

        if(tokens.size() >= 3) {
          quint64 address = tokens[0].toULongLong(nullptr, 16);
          //char symbolType = tokens[1][0].toLatin1();
          QString symbol = tokens[2].trimmed();

          if(pc < address) {
            file.close();
            if(lastAddress <= pc) {
              addr2line = Addr2Line("kallsyms", "kallsyms", symbol, 0);
              addr2lineCache[pc] = addr2line;
              return;
            } else goto error;
          }

          lastSymbol = symbol;
          lastAddress = address;
        }
      }
      file.close();
    }
  }

 error:
  addr2line = Addr2Line();
  addr2lineCache[pc] = addr2line;
}

QString ElfSupport::getFilename(uint64_t pc) {
  setPc(pc);
  return addr2line.filename;
}

QString ElfSupport::getElfName(uint64_t pc) {
  setPc(pc);
  return addr2line.elfName;
}

QString ElfSupport::getFunction(uint64_t pc) {
  setPc(pc);
  return addr2line.function;
}

uint64_t ElfSupport::getLineNumber(uint64_t pc) {
  setPc(pc);
  return addr2line.lineNumber;
}

