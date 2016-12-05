#ifndef INTERPRETSTRATEGYIMPL_H
#define INTERPRETSTRATEGYIMPL_H

#include <string>
#include <vector>
#include <memory>

#include "AnalyzerLib\core\Byte.h"
#include "InterpretStrategy.h"

namespace analyzer{
  namespace interpreter{
    namespace strategy{
      class InterpretStrategyImpl : public InterpretStrategy
      {
      public:
        virtual ~InterpretStrategyImpl();
        virtual std::string GetPlainText();

      protected:
        InterpretStrategyImpl();
        explicit InterpretStrategyImpl(const std::vector<std::shared_ptr<analyzer::core::Byte>> & data);
        std::vector<std::shared_ptr<analyzer::core::Byte>> & currentData();

      private:
        std::vector<std::shared_ptr<analyzer::core::Byte>> data;
      };
    }
  }
}

#endif