/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#ifndef TYPEANALYZERTEST_H
#define TYPEANALYZERTEST_H

#include <gtest\gtest.h>
#include <memory>
#include <vector>

#include "AnalyzerLib\core\TypeAnalyzer.h"
#include "AnalyzerLib\core\FileInfo.h"
#include "TestSupport.h"

class TypeAnalyzerTest : public testing::Test
{
public:
  TypeAnalyzerTest() {}
  virtual ~TypeAnalyzerTest() {}


};

TEST_F(TypeAnalyzerTest, emptyData)
{
  auto fileInfo = analyzer::core::TypeAnalyzer::GetInstance()->GetFileInfo(std::shared_ptr<std::vector<unsigned char>>());
  ASSERT_EQ(fileInfo.Format, analyzer::core::FileFormat::empty);
}

TEST_F(TypeAnalyzerTest, recXML)
{
  auto data = TestSupport::GetInstance()->GetDataFromTestFilesDir("test.xml");
  auto fileInfo = analyzer::core::TypeAnalyzer::GetInstance()->GetFileInfo(data);
  ASSERT_EQ(fileInfo.Format, analyzer::core::FileFormat::xml);
}

TEST_F(TypeAnalyzerTest, recASCII)
{
  auto data = TestSupport::GetInstance()->GetDataFromTestFilesDir("test.txt");
  auto fileInfo = analyzer::core::TypeAnalyzer::GetInstance()->GetFileInfo(data);
  ASSERT_EQ(fileInfo.Format, analyzer::core::FileFormat::ascii);
}

TEST_F(TypeAnalyzerTest, recPDF)
{
  auto data = TestSupport::GetInstance()->GetDataFromTestFilesDir("test.pdf");
  auto fileInfo = analyzer::core::TypeAnalyzer::GetInstance()->GetFileInfo(data);
  ASSERT_EQ(fileInfo.Format, analyzer::core::FileFormat::pdf);
}

TEST_F(TypeAnalyzerTest, recMSExec)
{
  auto data = TestSupport::GetInstance()->GetDataFromTestFilesDir("test.exe");
  auto fileInfo = analyzer::core::TypeAnalyzer::GetInstance()->GetFileInfo(data);
  ASSERT_EQ(fileInfo.Format, analyzer::core::FileFormat::winExec);
}

TEST_F(TypeAnalyzerTest, recBMP)
{
  auto data = TestSupport::GetInstance()->GetDataFromTestFilesDir("test.bmp");
  auto fileInfo = analyzer::core::TypeAnalyzer::GetInstance()->GetFileInfo(data);
  ASSERT_EQ(fileInfo.Format, analyzer::core::FileFormat::bmp);
}

TEST_F(TypeAnalyzerTest, limits)
{
  std::string fileData("00000<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n<tag1>\n  <tag2>some text</tag2>\n  <tag3/>\n</tag1>0000");
  std::shared_ptr<std::vector<unsigned char>> data(new std::vector<unsigned char>(fileData.begin(), fileData.end()));
  auto fileInfo = analyzer::core::TypeAnalyzer::GetInstance()->GetFileInfo(data, 5, 106);
  ASSERT_EQ(fileInfo.Format, analyzer::core::FileFormat::xml);
}

#endif