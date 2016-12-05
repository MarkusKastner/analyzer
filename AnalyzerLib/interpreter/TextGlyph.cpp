#include "TextGlyph.h"

#include "strategy\DefaultPlainTextStrategy.h"
namespace analyzer{
  namespace interpreter{
    TextGlyph::TextGlyph()
      :data(), interpretStrategy(new strategy::DefaultPlainTextStrategy())
    {

    }

    TextGlyph::TextGlyph(const std::vector<std::shared_ptr<analyzer::core::Byte>> & data)
      :data(data), interpretStrategy(new strategy::DefaultPlainTextStrategy(data))
    {

    }

    TextGlyph::~TextGlyph()
    {
      if (this->interpretStrategy != nullptr){
        delete this->interpretStrategy;
        this->interpretStrategy = nullptr;
      }
    }

    bool TextGlyph::HasExpression()
    {
      return false;
    }

    size_t TextGlyph::NumBytes()
    {
      return this->data.size();
    }

    std::string TextGlyph::GetPlainText()
    {
      return this->interpretStrategy->GetPlainText();
    }
  }
}