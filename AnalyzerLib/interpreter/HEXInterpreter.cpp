/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#include "AnalyzerLib/interpreter/HEXInterpreter.h"
#include "AnalyzerLib/interpreter/formatter/ASCIITable.h"

#include <sstream>
#include <bitset>
#include <iomanip>

namespace analyzer {
  namespace interpreter {
    HEXInterpreter::HEXInterpreter()
      :Interpreter(), data(), hex(), text()
    {
    }

    HEXInterpreter::HEXInterpreter(const std::shared_ptr<std::vector<unsigned char>>& data)
      : Interpreter(), data(data), hex(), text()
    {
      this->data2Hex();
    }

    HEXInterpreter::HEXInterpreter(const std::shared_ptr<std::vector<unsigned char>>& data, const size_t & indexBegin, const size_t & offset)
      : Interpreter(indexBegin, offset), data(data), hex(), text()
    {
      this->data2Hex();
    }

    HEXInterpreter::~HEXInterpreter()
    {
    }

    bool HEXInterpreter::HasData()
    {
      return !(!data);
    }

    void HEXInterpreter::SetData(const std::shared_ptr<std::vector<unsigned char>> & data)
    {
      this->data = data;
      this->data2Hex();
    }

    void HEXInterpreter::SetData(const std::shared_ptr<std::vector<unsigned char>>& data, const size_t & indexBegin, const size_t & offset)
    {
      this->setLimits(indexBegin, offset);
      this->SetData(data);
    }

    const std::string & HEXInterpreter::GetText()
    {
      return this->text;
    }

    core::FileFormat HEXInterpreter::GetFileFormat()
    {
      return core::FileFormat::hex;
    }

    bool HEXInterpreter::UseRichText()
    {
      return true;
    }

    const std::vector<std::pair<std::string, char>> & HEXInterpreter::GetHexExpressions()
    {
      return this->hex;
    }

    std::vector<HEXInterpreter::HexRow> HEXInterpreter::GetHexRows()
    {
      std::vector<HEXInterpreter::HexRow> rows;
      HEXInterpreter::HexRow row;

      size_t size = this->hex.size();

      for (size_t i = 0; i < size; ++i) {
        row.HexValues.push_back({ this->hex[i].first, i });
        row.ASCII += this->hex[i].second;
        if (row.HexValues.size() >= 16) {
          rows.push_back(row);
          row.HexValues.clear();
          row.ASCII.clear();
        }
      }
      if (!row.HexValues.empty()) {
        rows.push_back(row);
      }
      return rows;
    }

    std::vector<unsigned char> HEXInterpreter::GetBytesByIndex(const std::vector<size_t> & indexes)
    {
      std::vector<unsigned char> bytes;
      if (this->data->size() == 0) {
        return bytes;
      }
      size_t offset = this->findOffset();

      for (auto& index : indexes) {
        if (index < this->getIndexBegin() + offset) {
          size_t resIndex = this->getIndexBegin() + index;
          bytes.push_back(this->data->at(resIndex));
        }
      }
      return bytes;
    }

    void HEXInterpreter::data2Hex()
    {
      if (this->data->size() == 0) {
        return;
      }
      size_t offset = this->findOffset();

      for (size_t i = this->getIndexBegin(); i < this->getIndexBegin() + offset; ++i) {
        this->hex.push_back({ this->char2Hex(this->data.get()->at(i)), ASCIITable::Byte2Printable(this->data.get()->at(i)) });
      }
    }

    std::string HEXInterpreter::char2Hex(const unsigned char & value)
    {
      std::stringstream stream;
      stream << std::setw(2) << std::setfill('0') << std::hex << +value;
      return stream.str();
    }

    size_t HEXInterpreter::findOffset()
    {
      size_t offset = this->data->size();
      if (this->hasLimits()) {
        offset = this->getOffset();
      }
      return offset;
    }
  }
}