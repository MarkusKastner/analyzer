/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#ifndef OUTPUTDOCK_H
#define OUTPUTDOCK_H

#include <QDockWidget>
#include <QTextEdit>
#include <string>

namespace analyzer {
  namespace gui {
    class OutputDock : public QDockWidget
    {
    public:
      OutputDock(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());
      virtual ~OutputDock();

      void AddMessage(const std::string & message);
      void Clear();

    private:
      QTextEdit * output;
      void setup();
    };
  }
}

#endif