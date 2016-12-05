#ifndef WORD_H
#define WORD_H

#if _USRDLL
#define IMEX __declspec(dllexport)
#else
#define IMEX __declspec(dllimport)
#endif

#include "BasicType.h"
#include "Byte.h"

namespace analyzer{
  namespace core{
    class IMEX Word : public BasicType
    {
    public:
      Word();
      explicit Word(const Byte & highByte, const Byte & lowByte);
      virtual ~Word();

      virtual unsigned int GetBitAt(const unsigned int & index);
      virtual std::string GetBitsAsString();

      unsigned short GetValue() const;

    private:
      Byte lowByte;
      Byte highByte;
    };
  }
}

#endif