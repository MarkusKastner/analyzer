/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#include "DefinitionVault.h"

#include "LocalDefinition.h"
#include "AnalyzerLib\base\error\AnalyzerBaseException.h"

namespace analyzer{
  namespace definition{
    DefinitionVault::DefinitionVault()
      :sources(new std::vector<std::shared_ptr<DefinitionSource>>())
    {
    }
    
    DefinitionVault::~DefinitionVault()
    {
      delete this->sources;
    }

    bool DefinitionVault::HasDefinitions()
    {
      return !this->sources->empty();
    }

    size_t DefinitionVault::GetNumSources()
    {
      return this->sources->size();
    }

    void DefinitionVault::AddSource(const std::shared_ptr<DefinitionSource> & source)
    {
      this->sources->push_back(source);
    }

    std::shared_ptr<DefinitionSource> DefinitionVault::GetSourceAt(const size_t index)
    {
      if (index > this->sources->size() - 1){
        throw base::AnalyzerBaseException("Invalid index");
      }
      return this->sources->at(0);
    }

    std::shared_ptr<DefinitionSource> DefinitionVault::GetSource(const std::string & sourceName)
    {
      for (auto& source : *this->sources){
        if (source->GetName().compare(sourceName) == 0){
          return source;
        }
      }
      throw base::AnalyzerBaseException("Unknown source name");
    }

    bool DefinitionVault::HasSource(const std::string & sourceName)
    {
      for (auto& source : *this->sources){
        if (source->GetName().compare(sourceName) == 0){
          return true;;
        }
      }
      return false;
    }
  }
}