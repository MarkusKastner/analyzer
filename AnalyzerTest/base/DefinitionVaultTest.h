#ifndef DEFINITIONVAULTTEST_H
#define DEFINITIONVAULTTEST_H

#include <gtest\gtest.h>
#include <memory>

#include "AnalyzerLib\base\DefinitionVault.h"
#include "AnalyzerLib\base\error\DBException.h"
#include "AnalyzerLib\base\DefinitionSource.h"
#include "AnalyzerLib\base\LocalDefinition.h"

class DefinitionVaultTest : public testing::Test
{
public:

  void SetUp(){
    asciiSource.AddSource(std::shared_ptr<analyzer::base::DefinitionSource>(new analyzer::base::LocalDefinition("default_ascii")));
  }

  analyzer::base::DefinitionVault emptySource;
  analyzer::base::DefinitionVault asciiSource;
};

TEST_F(DefinitionVaultTest, init)
{
  ASSERT_FALSE(emptySource.HasDefinitions());
}

TEST_F(DefinitionVaultTest, AddSource)
{
  emptySource.AddSource(std::shared_ptr<analyzer::base::DefinitionSource>(new analyzer::base::LocalDefinition("default_ascii")));
  ASSERT_STREQ(emptySource.GetSourceAt(0)->GetName().c_str(), "default_ascii");
}

TEST_F(DefinitionVaultTest, NumSources)
{
  ASSERT_EQ(asciiSource.GetNumSources(), 1);
}

TEST_F(DefinitionVaultTest, GetSourceAt)
{
  ASSERT_STREQ(asciiSource.GetSourceAt(0)->GetName().c_str(), "default_ascii");
}

TEST_F(DefinitionVaultTest, GetSourceByName)
{
  ASSERT_STREQ(asciiSource.GetSource("default_ascii")->GetName().c_str(), "default_ascii");
}

TEST_F(DefinitionVaultTest, InvalidIndex)
{
  std::string errorMsg;

  try{
    ASSERT_STREQ(asciiSource.GetSourceAt(1)->GetName().c_str(), "default_ascii");
  }
  catch (analyzer::base::AnalyzerBaseException & ex){
    errorMsg = std::string(ex.what());
  }
  catch (...){}
  ASSERT_STREQ(errorMsg.c_str(), "Invalid index");
}

TEST_F(DefinitionVaultTest, InvalidSourceName)
{
  std::string errorMsg;

  try{
    ASSERT_STREQ(asciiSource.GetSource("unknown")->GetName().c_str(), "unknown");
  }
  catch (analyzer::base::AnalyzerBaseException & ex){
    errorMsg = std::string(ex.what());
  }
  catch (...){}
  ASSERT_STREQ(errorMsg.c_str(), "Unknown source name");
}

TEST_F(DefinitionVaultTest, HasSource)
{
  ASSERT_TRUE(asciiSource.HasSource("default_ascii"));
}
#endif