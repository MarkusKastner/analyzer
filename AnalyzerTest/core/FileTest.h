/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#ifndef FILETEST_H
#define FILETEST_H

#include <gtest\gtest.h>

#include <vector>
#include <string>
#include <memory>

#include "AnalyzerLib\core\File.h"
#include "AnalyzerLib\core\PrimaryFile.h"
#include "AnalyzerLib\core\InternalFile.h"
#include "AnalyzerLib\core\FileInfo.h"
#include "AnalyzerLib\core\FileObserver.h"
#include "AnalyzerLib\interpreter\XMLInterpreter.h"
#include "AnalyzerLib\interpreter\HEXInterpreter.h"

class FileTest : public testing::Test
{
public:
  class SomeIntFile : public analyzer::core::InternalFile
  {
  public:
    SomeIntFile(const std::string & name, const std::shared_ptr<std::vector<unsigned char>> & data, const size_t & indexBegin, const size_t & offset)
      : analyzer::core::InternalFile(name, data, indexBegin, offset)
    {}

    virtual ~SomeIntFile() {}
  };

  class SomeFileObserver : public analyzer::core::FileObserver
  {
  public:
    SomeFileObserver() : file() {}
    virtual ~SomeFileObserver() {}
    virtual void AddInternalFile(const std::shared_ptr<analyzer::core::File> & file) {
      this->file = file;
    }
    const std::shared_ptr<analyzer::core::File> & GetInternalFileTest() const {
      return this->file;
    }
  private:
    std::shared_ptr<analyzer::core::File> file;
  };

  FileTest()
    : file1(), charVector(), fileName1("test.txt"), someFileObserver(), 
    parentFileName("parent"), internalFileName("newinternal"), 
    internalDataIndex(19), internalDataOffset(38), primFile(), intFile()
  {}
  virtual ~FileTest(){
  }

  void SetUp(){
    for (int i = 0; i < 4; i++){
      this->charVector.push_back('a');
    }
    std::string prevData("some previouse data");
    std::string xmlHeaderStr("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    std::string postData("some data after");

    internalXMLData.assign(prevData.begin(), prevData.end());
    internalXMLData.insert(internalXMLData.end(), xmlHeaderStr.begin(), xmlHeaderStr.end());
    internalXMLData.insert(internalXMLData.end(), postData.begin(), postData.end());

    std::shared_ptr<std::vector<unsigned char>> data(std::make_shared<std::vector<unsigned char>>(this->internalXMLData));
    std::vector<unsigned char> empty;
    this->primFile.reset(new analyzer::core::PrimaryFile(parentFileName, empty));
    this->intFile.reset(new SomeIntFile(internalFileName, data, this->internalDataIndex, this->internalDataOffset));

    this->primFile->RegisterFileObserver(&this->someFileObserver);
    this->primFile->AddInternalFile(intFile);
  }

  analyzer::core::PrimaryFile file1;
  std::vector<unsigned char> charVector;
  std::string fileName1;
  SomeFileObserver someFileObserver;
  std::vector<unsigned char> internalXMLData;
  std::string parentFileName;
  std::string internalFileName;
  size_t internalDataIndex;
  size_t internalDataOffset;
  std::shared_ptr<analyzer::core::File> primFile;
  std::shared_ptr<analyzer::core::File> intFile;
};

TEST_F(FileTest, Init)
{
  ASSERT_FALSE(file1.IsLoaded());
}

TEST_F(FileTest, InitWithData)
{
  analyzer::core::PrimaryFile file(fileName1, charVector);
  ASSERT_TRUE(file.IsLoaded());
}

TEST_F(FileTest, SetFileData)
{
  this->file1.SetFileData(fileName1, charVector);

  ASSERT_STREQ(this->file1.GetFileName().c_str(), this->fileName1.c_str());
  ASSERT_EQ(this->file1.GetSize(), charVector.size());
}

TEST_F(FileTest, SetFileNameWithPath)
{
  std::string fileName("testDir/subTestDir/test.txt");
  this->file1.SetFileData(fileName, charVector);

  ASSERT_STREQ(this->file1.GetPath()[0].c_str(), "testDir");
  ASSERT_STREQ(this->file1.GetPath()[1].c_str(), "subTestDir");
}

TEST_F(FileTest, CpyCtor)
{
  this->file1.SetFileData(fileName1, charVector);

  analyzer::core::PrimaryFile theNewfile(this->file1);

  ASSERT_STREQ(theNewfile.GetFileName().c_str(), this->fileName1.c_str());
  ASSERT_EQ(theNewfile.GetSize(), this->charVector.size());
}

TEST_F(FileTest, AssignOperator)
{
  analyzer::core::PrimaryFile theNewfile;
  {
    this->file1.SetFileData(fileName1, charVector);
    theNewfile = this->file1;
  }
  ASSERT_STREQ(theNewfile.GetFileName().c_str(), this->fileName1.c_str());
  ASSERT_EQ(theNewfile.GetSize(), this->charVector.size());
}

TEST_F(FileTest, UseRichTextEmpty)
{
  ASSERT_FALSE(this->file1.UseRichText());
}

TEST_F(FileTest, UseRichTextBMP)
{
  std::string testFile(TestSupport::GetInstance()->GetTestFilesDir());
  std::string bmpFile("bmp/test16_1.bmp");
  testFile += "/" + bmpFile;
  auto dataPtr = TestSupport::GetInstance()->GetDataFromTestFilesDir(bmpFile);
  std::vector<unsigned char> data(dataPtr->begin(), dataPtr->end());

  this->file1.SetFileData(testFile, data);
  ASSERT_TRUE(this->file1.UseRichText());
}

TEST_F(FileTest, getText)
{
  std::string testFile(TestSupport::GetInstance()->GetTestFilesDir());
  std::string bmpFile("bmp/test16_1.bmp");
  testFile += "/" + bmpFile;
  auto dataPtr = TestSupport::GetInstance()->GetDataFromTestFilesDir(bmpFile);
  std::vector<unsigned char> data(dataPtr->begin(), dataPtr->end());
  this->file1.SetFileData(testFile, data);
  std::string text(this->file1.GetText());
  std::string compTxt("<!DOCTYPE html><html><head><title>Windows Bitmap</title></head>");
  std::string testTxt(text.substr(0, 63));
  ASSERT_STREQ(compTxt.c_str(), testTxt.c_str());
  ASSERT_EQ(this->file1.GetFileFormat(), analyzer::core::FileFormat::bmp);
}

TEST_F(FileTest, handleInternalFile)
{
  std::string fileName(parentFileName + "/" + internalFileName);

  ASSERT_TRUE(primFile->HasInternalFiles());
  ASSERT_STREQ(fileName.c_str(), this->someFileObserver.GetInternalFileTest()->GetFileName().c_str());
  bool isXML = false;
  if (dynamic_cast<analyzer::interpreter::XMLInterpreter*>(this->someFileObserver.GetInternalFileTest()->GetInterpreter().get())) {
    isXML = true;
  }
  ASSERT_TRUE(isXML);
  primFile->UnregisterFileObserver(&this->someFileObserver);
}

TEST_F(FileTest, internalFileSize)
{
  ASSERT_EQ(this->intFile->GetSize(), this->internalDataOffset);
}

TEST_F(FileTest, cloneToHexfileFromPrimFile)
{
  std::string testFile(TestSupport::GetInstance()->GetTestFilesDir());
  std::string bmpFile("bmp/test16_1.bmp");
  testFile += "/" + bmpFile;
  auto dataPtr = TestSupport::GetInstance()->GetDataFromTestFilesDir(bmpFile);
  std::vector<unsigned char> data(dataPtr->begin(), dataPtr->end());
  this->file1.SetFileData(testFile, data);

  std::shared_ptr<analyzer::core::File> file(this->file1.CloneToHexFile());

  bool isHex = false;
  if (dynamic_cast<analyzer::interpreter::HEXInterpreter*>(file->GetInterpreter().get())) {
    isHex = true;
  }
  ASSERT_TRUE(isHex);
  ASSERT_EQ(dataPtr->size(), file->GetSize());
}

TEST_F(FileTest, cloneToHexfileFromInternalFile)
{
  std::shared_ptr<analyzer::core::File> file(this->intFile->CloneToHexFile());

  bool isHex = false;
  if (dynamic_cast<analyzer::interpreter::HEXInterpreter*>(file->GetInterpreter().get())) {
    isHex = true;
  }
  ASSERT_TRUE(isHex);
  ASSERT_EQ(this->internalDataOffset, file->GetSize());
}
#endif