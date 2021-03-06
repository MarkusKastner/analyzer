/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#ifndef INTERPRETEREXCEPTION_H
#define INTERPRETEREXCEPTION_H

#include <exception>

namespace analyzer{
  namespace interpreter{
    class InterpreterException : public std::exception
    {
    public:
      InterpreterException()
        : std::exception()
      {
      }

      explicit InterpreterException(const std::string & message)
        : std::exception(message.c_str())
      {
      }

      virtual ~InterpreterException()
      {
      }
    };
  }
}
#endif