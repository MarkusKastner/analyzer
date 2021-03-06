/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#ifndef HEXTABLEWIDGET_H
#define HEXTABLEWIDGET_H

#include "ViewOutput.h"
#include <QTableWidget>

#include <vector>
#include <string>
#include <queue>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

#include "AnalyzerLib\core\File.h"
#include "AnalyzerLib\base\BaseData.h"
#include "AnalyzerLib\interpreter\HEXInterpreter.h"

namespace analyzer {
  namespace interpreter {
    class HEXInterpreter;
  }
  namespace core {
    class File;
  }
  namespace gui {
    namespace display {
      class HexBrowser;
      class HexTableWidgetItem;
      class HexTableWidget : public QTableWidget, public ViewOutput
      {
        Q_OBJECT
      public:
        HexTableWidget(HexBrowser * parent);
        virtual ~HexTableWidget();

        virtual void SetFile(core::File * file);
        virtual void ClearFile();

        void AddHexRow(const interpreter::HEXInterpreter::HexRow & hexExp);
        void MarkIndex(const analyzer::base::Marking & marking);
        void DeleteColor(const analyzer::base::AnalyzerRGB & color);
        void MarkSuspectRange(const size_t & index, const size_t offset);

      private:
        core::File * file;
        HexBrowser * browser;
        std::vector<HexTableWidgetItem*> hexTableWidgetItems;

        std::queue<analyzer::base::Marking> newMarkings;
        std::queue<analyzer::base::AnalyzerRGB> colorToDelete;
        std::queue<size_t> suspected;

        std::unique_ptr<std::thread> markerThread;
        std::atomic<bool> runMarker;
        std::recursive_mutex newMarkingsLock;
        std::recursive_mutex colorToDeleteLock;
        std::recursive_mutex suspectedLock;

        void onSelection();
        void setup();
        void setDetailOutput(const std::vector<unsigned char> & bytes);

        void markingRoutine();
        bool hasColorToDelete();
        analyzer::base::AnalyzerRGB fetchColorToDelete();
        void deleteColor();

        bool hasNewMarking();
        void setNewMarking();
        analyzer::base::Marking fetchNextMarking();

        void markSuspected();
        bool hasSuspected();
        size_t fetchSuspected();

        interpreter::HEXInterpreter * getInterpreter();
      };
    }
  }
}
#endif
