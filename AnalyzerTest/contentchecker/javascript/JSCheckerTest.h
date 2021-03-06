/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#ifndef JSCHECKERTEST_H
#define JSCHECKERTEST_H

#include <gtest/gtest.h>
#include "TestSupport.h"

#include <memory>
#include <vector>

#include "AnalyzerLib/contentchecker/javascript/JSChecker.h"

class JSCheckerTest : public testing::Test
{
public:
  JSCheckerTest()
    : testing::Test()
  {
  }
};

TEST_F(JSCheckerTest, data)
{
  std::shared_ptr<std::vector<unsigned char>> data(new std::vector<unsigned char>());
  data->push_back('x');

  analyzer::checker::JSChecker jsChecker;

  ASSERT_FALSE(jsChecker.HasData());
  jsChecker.SetData(data);
  ASSERT_TRUE(jsChecker.HasData());
  jsChecker.ReleaseData();
  ASSERT_FALSE(jsChecker.HasData());
}

TEST_F(JSCheckerTest, iCases)
{
  auto jsIfSyntax(TestSupport::GetInstance()->GetDataFromTestFilesDir("JavascriptSyntax/if.txt"));
  size_t offset = 20;

  analyzer::checker::JSChecker jsChecker;
  jsChecker.SetData(jsIfSyntax);
  ASSERT_TRUE(jsChecker.IsICaseSyntax(offset));
  ASSERT_EQ(jsChecker.GetLastFoundSyntaxOffset(), 2);
}

TEST_F(JSCheckerTest, fCases)
{
  auto jsForSyntax(TestSupport::GetInstance()->GetDataFromTestFilesDir("JavascriptSyntax/for.txt"));
  size_t offset = 22;

  analyzer::checker::JSChecker jsChecker;
  jsChecker.SetData(jsForSyntax);
  ASSERT_TRUE(jsChecker.IsFCaseSyntax(offset));
  ASSERT_EQ(jsChecker.GetLastFoundSyntaxOffset(), 3);
}

TEST_F(JSCheckerTest, vCases)
{
  auto jsVarSyntax(TestSupport::GetInstance()->GetDataFromTestFilesDir("JavascriptSyntax/var.txt"));
  size_t offset = 16;

  analyzer::checker::JSChecker jsChecker;
  jsChecker.SetData(jsVarSyntax);
  ASSERT_TRUE(jsChecker.IsVCaseSyntax(offset));
  ASSERT_EQ(jsChecker.GetLastFoundSyntaxOffset(), 3);
}
#endif