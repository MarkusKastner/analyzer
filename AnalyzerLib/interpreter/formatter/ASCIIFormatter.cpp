#include "ASCIIFormatter.h"

#include <bitset>
#include <sstream>
#include <iomanip>

namespace analyzer {
  namespace interpreter {
    std::vector<std::string> ASCIIFormatter::Split(const std::string & text, const size_t & chunkSize)
    {
      std::vector<std::string> chunks;
      std::string chunk;
      for (auto& letter : text) {
        chunk += letter;
        if (chunk.size() == chunkSize) {
          chunks.push_back(chunk);
          chunk.clear();
        }
      }
      if (!chunk.empty()) {
        chunks.push_back(chunk);
      }
      return chunks;
    }

    std::string ASCIIFormatter::Text2BinaryExpression(const std::string & text)
    {
      std::string binString;
      for (auto& letter : text) {
        binString += std::bitset<8>(letter).to_string();
        binString += " ";
      }
      binString.pop_back();
      return binString;
    }

    std::string ASCIIFormatter::Text2NumericalExpression(const std::string & text)
    {
      std::string numString;
      for (auto& letter : text) {
        numString += std::to_string(static_cast<unsigned char>(letter));
        numString += " ";
      }
      numString.pop_back();
      return numString;
    }

    std::string ASCIIFormatter::Text2HexExpression(const std::string & text)
    {
      std::stringstream ss;
      ss << std::hex << std::setfill('0');
      for (int i = 0; i < text.size(); ++i){
        ss << std::setw(2) << static_cast<unsigned>(text[i]);
        ss << ' ';
      }
      std::string ret(ss.str());
      ret.pop_back();
      return ret;
    }
  }
}