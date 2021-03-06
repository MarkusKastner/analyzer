/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#ifndef APPLICATIONSETTINGSTEST_H
#define APPLICATIONSETTINGSTEST_H

#include <gtest\gtest.h>
#include "TestSupport.h"

#include <filesystem>

#include "AnalyzerLib\base\ApplicationSettings.h"
#include "AnalyzerLib\base\error\AnalyzerBaseException.h"

namespace fs = std::tr2::sys;

class ApplicationSettingsTest : public testing::Test
{
public:
  ApplicationSettingsTest()
  :testing::Test(), applicationSettings(), lastOpenDir(TestSupport::GetInstance()->GetAppdir()), notExistingDir("C:/idontexist")
  {}

  virtual ~ApplicationSettingsTest() {}

  virtual void SetUp() {
    this->lastOpenDir += "/lastOpen";
    if (!fs::exists(fs::path(this->lastOpenDir))) {
      fs::create_directory(this->lastOpenDir);
    }

    if (fs::exists(fs::path(this->notExistingDir))) {
      fs::remove(fs::path(this->notExistingDir));
    }
  }

  analyzer::base::ApplicationSettings applicationSettings;
  std::string lastOpenDir;
  std::string notExistingDir;
};

TEST_F(ApplicationSettingsTest, init)
{
  analyzer::base::ApplicationSettings appSettings;
}

TEST_F(ApplicationSettingsTest, appPath)
{
  this->applicationSettings.SetAppDir(TestSupport::GetInstance()->GetAppdir());
  ASSERT_STREQ(TestSupport::GetInstance()->GetAppdir().c_str(), this->applicationSettings.GetAppDir().c_str());
}

TEST_F(ApplicationSettingsTest, appPathNotSet)
{
  analyzer::base::AnalyzerBaseException::ErrorCode errCode = analyzer::base::AnalyzerBaseException::ErrorCode::Empty;
  try {
    this->applicationSettings.GetAppDir();
  }
  catch (analyzer::base::AnalyzerBaseException & ex)
  {
    errCode = ex.GetErrorCode();
  }
  ASSERT_EQ(errCode, analyzer::base::AnalyzerBaseException::ErrorCode::ApplicationPathNotSet);
}

TEST_F(ApplicationSettingsTest, serialize)
{
  std::string path(TestSupport::GetInstance()->GetAppdir());
  path += "/analyzer.config";

  this->applicationSettings.SetAppDir(TestSupport::GetInstance()->GetAppdir());
  this->applicationSettings.Serialize();

  ASSERT_TRUE(fs::exists(fs::path(path)));
}

TEST_F(ApplicationSettingsTest, lastOpenDir)
{
  this->applicationSettings.SetAppDir(TestSupport::GetInstance()->GetAppdir());

  this->applicationSettings.SetLastOpenDir(this->lastOpenDir);
  ASSERT_STREQ(this->lastOpenDir.c_str(), this->applicationSettings.GetLastOpenDir().c_str());
}

TEST_F(ApplicationSettingsTest, deserialize)
{
  std::string lastOpenDirSerialized;
  {
    analyzer::base::ApplicationSettings appSettings;
    appSettings.SetAppDir(TestSupport::GetInstance()->GetAppdir());
    appSettings.SetLastOpenDir(this->lastOpenDir);
    appSettings.Serialize();
  }

  {
    analyzer::base::ApplicationSettings appSettings;
    appSettings.SetAppDir(TestSupport::GetInstance()->GetAppdir());
    appSettings.Deserialize();
    lastOpenDirSerialized = appSettings.GetLastOpenDir();
  }
  
  ASSERT_STREQ(this->lastOpenDir.c_str(), lastOpenDirSerialized.c_str());
}

TEST_F(ApplicationSettingsTest, tryGetLastOpenDirThatDoesntExisit)
{
  this->applicationSettings.SetAppDir(TestSupport::GetInstance()->GetAppdir());
  this->applicationSettings.SetLastOpenDir(this->notExistingDir);

  ASSERT_STREQ("", this->applicationSettings.GetLastOpenDir().c_str());

}
#endif