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

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QProcess>
#include "elfsupport.h"

static char *readLine(char *s, int size, FILE *stream) {
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
      QString fileName = "Unknown";
      QString function = "Unknown";
      uint64_t lineNumber = 0;
      char buf[1024];
      FILE *fp;
      std::stringstream pcStream;
      std::string cmd;

      // create command
      pcStream << std::hex << pc;
      cmd = std::string(qgetenv("CROSS_COMPILE").constData()) + "addr2line -C -f -a " + pcStream.str() + " -e " + elfFile.toUtf8().constData();
      // run addr2line program
      if((fp = popen(cmd.c_str(), "r")) == NULL) goto error;

      // discard first output line
      if(readLine(buf, 1024, fp) == NULL) goto error;

      // get function name
      if(readLine(buf, 1024, fp) == NULL) goto error;
      function = QString::fromUtf8(buf).simplified();
      if(function == "??") function = "Unknown";

      // get filename and linenumber
      if(readLine(buf, 1024, fp) == NULL) goto error;

      {
        QString qbuf = QString::fromUtf8(buf);
        fileName = qbuf.left(qbuf.indexOf(':'));
        lineNumber = qbuf.mid(qbuf.indexOf(':') + 1).toULongLong();
      }

      // close stream
      pclose(fp);

      if((function != "Unknown") || (lineNumber != 0)) {
        addr2line = Addr2Line(fileName, elfFile, function, lineNumber);
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
  addr2line = Addr2Line("Unknown", "Unknown", "Unknown", 0);
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

uint64_t ElfSupport::lookupSymbol(QString symbol) {
  FILE *fp;
  char buf[1024];

  for(auto elfFile : elfFiles) {
    // create command line
    QString cmd = QString("nm ") + elfFile;

    // run program
    if((fp = popen(cmd.toUtf8().constData(), "r")) == NULL) goto error;

    while(!feof(fp) && !ferror(fp)) {
      if(readLine(buf, 1024, fp)) {
        QStringList line = QString::fromUtf8(buf).split(' ');
        if(line[2].trimmed() == symbol) {
          return line[0].toULongLong(0, 16);
        }
      }
    }
  }

 error:
  return 0;
}

bool ElfSupport::isStatic(QString elf) {
  QString elfFileName = QFileInfo(elf).fileName();

  if(elfStatic.find(elfFileName) == elfStatic.end()) {
    QString cmd = "readelf -h " + elf;

    QProcess process;
    process.start(cmd);
    process.waitForFinished(-1);

    QString stdout = process.readAllStandardOutput();
    bool stat = stdout.contains("EXEC");
    elfStatic[elfFileName] = stat;

    return stat;
  }

  return elfStatic[elfFileName];
}
