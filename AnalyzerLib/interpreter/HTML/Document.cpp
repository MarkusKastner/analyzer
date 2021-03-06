/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#include "Document.h"

namespace analyzer {
  namespace interpreter {
    namespace HTML {
      Document::Document()
        : docString(), title(), bodyElements()
      {
      }

      Document::~Document()
      {
      }

      std::string Document::GetString()
      {
        this->assemble();
        return this->docString;
      }

      void Document::SetTitle(const std::string & title)
      {
        this->title = title;
      }

      void Document::AddHeading(const analyzer::interpreter::HTML::Heading & heading)
      {
        this->bodyElements.push_back(std::shared_ptr<BodyElement>(new Heading(heading)));
      }

      void Document::AddParagraph(const analyzer::interpreter::HTML::Paragraph & paragraph)
      {
        this->bodyElements.push_back(std::shared_ptr<BodyElement>(new Paragraph(paragraph)));
      }

      void Document::AddTable(const analyzer::interpreter::HTML::Table & table)
      {
        this->bodyElements.push_back(std::shared_ptr<BodyElement>(new Table(table)));
      }

      void Document::AddLineFeed()
      {
        this->bodyElements.push_back(std::shared_ptr<BodyElement>(new LineFeed()));
      }

      void Document::assemble()
      {
        this->docString.clear();
        this->docString = "<!DOCTYPE html><html><head>";
        if (!title.empty()) {
          this->docString += "<title>" + this->title + "</title>";
        }
        this->docString += "</head><body>";

        for (auto& element : this->bodyElements) {
          this->docString += element->ToString();
        }
        this->docString += "</body></html>";
      }
    }
  }
}