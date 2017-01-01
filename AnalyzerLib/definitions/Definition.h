#ifndef DEFINITION_H
#define DEFINITION_H

#if _USRDLL
#define IMEX __declspec(dllexport)
#else
#define IMEX __declspec(dllimport)
#endif

#include <string>

namespace analyzer{
  namespace definition{
    class IMEX Definition
    {
    private:
      Definition(){}
    public:
      explicit Definition(const unsigned int & id);
      virtual ~Definition();

      const unsigned int & GetID();

      void SetDescription(const std::string & description);
      const std::string & GetDescription();

    private:
      unsigned int id;
      std::string * description;
    };
  }
}

#endif