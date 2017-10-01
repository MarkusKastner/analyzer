/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#ifndef CONTENTCHECKER_H
#define CONTENTCHECKER_H

#include <vector>
#include <memory>
#include <atomic>
#include <thread>

#include "AnalyzerLib/base/BaseData.h"

namespace analyzer {
  namespace checker {
    class CheckObserver;
    class ContentChecker
    {
    public:
      virtual ~ContentChecker();

      size_t GetNumCheckObservers() const;
      void RegisterCheckObserver(CheckObserver * observer);
      void UnregisterCheckObserver(CheckObserver * observer);

      void SetWorkingColor(const base::AnalyzerRGB & color);
      const base::AnalyzerRGB & GetWorkingColor() const;

      bool HasData();
      void SetData(const std::shared_ptr<std::vector<unsigned char>> & data);
      void ReleaseData();

      void SetCheckRange(const size_t & startOffset, const size_t & checkOffset);
      const size_t & GetStartOffest() const;
      const size_t & GetCheckOffest() const;

      bool IsChecking() const;
      void StartCheck();
      void StopCheck();
      bool IsFinished() const;

    protected:
      ContentChecker();

      virtual void checkData() = 0;
      
      const std::shared_ptr<std::vector<unsigned char>> & getData();
      void notifyCurrentIndex(const size_t index);
      void notifyMarkedIndex(const size_t index);

    private:
      std::vector<CheckObserver*> checkObservers;
      base::AnalyzerRGB workingColor;
      std::shared_ptr<std::vector<unsigned char>> data;
      size_t startOffest;
      size_t checkOffest;
      std::atomic<bool> runCheck;
      std::atomic<bool> finished;
      std::unique_ptr<std::thread> checkThread;

      void checkRoutine();
      void notifyCheckFinished();
    };
  }
}
#endif