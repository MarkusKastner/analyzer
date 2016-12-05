#ifndef BINARYSTYLEINTERPRETER_H
#define BINARYSTYLEINTERPRETER_H

#if _USRDLL
#define IMEX __declspec(dllexport)
#else
#define IMEX __declspec(dllimport)
#endif

#include <memory>

#include "InterpreterObserverImpl.h"
#include "TextGlyph.h"

namespace analyzer{
  namespace interpreter{
    class IMEX BinaryStyleInterpreter : public InterpreterObserverImpl
    {
    public:
      BinaryStyleInterpreter();
      explicit BinaryStyleInterpreter(const std::shared_ptr<analyzer::core::ByteCollection> & byteCollection);
      virtual ~BinaryStyleInterpreter();

      virtual bool HasData();
      virtual void ResetData(const std::shared_ptr<analyzer::core::ByteCollection> & data);
      virtual std::string GetPlainText();

      size_t NumGlyphs() const;
      std::shared_ptr<TextGlyph> GetGlyphAt(const size_t & index);

    private:
      std::shared_ptr<analyzer::core::ByteCollection> * byteCollection;
      std::vector<std::shared_ptr<TextGlyph>> * glyphs;

      void createGlyphs();

      void throwGlyphIndex(const size_t & index);
      
    };
  }
}

#endif