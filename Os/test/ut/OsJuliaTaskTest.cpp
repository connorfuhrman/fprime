#include "TaskString.hpp"
#include "gtest/gtest.h"
#include <Os/Task.hpp>

#include <jluna.hpp>

#include <fstream>
#include <string>


extern "C" {
  void startJuliaTaskTest();
}

static const char* jl_filename = "/tmp/fpime-OsJuliaTaskTest.jl";

static auto initialize()
{
  std::string jl_code = R"(
    ran = false

    function do_task()
       global ran = true
    end

    )";


  std::ofstream jl_file(jl_filename);
  jl_file << jl_code;
  jl_file.close();
  

  jluna::Main.safe_eval_file(jl_filename);
}

void runSomeTask(void* ptr)
{
  initialize();
  jluna::Main.safe_eval("do_task()");
  auto* ran = reinterpret_cast<bool*>(ptr);
  *ran = jluna::Main.safe_eval("return ran");
}

void startJuliaTaskTest()
{
  volatile bool taskRun = false;
  Os::Task testTask;
  Os::TaskString name("AJuliaTestTask");
  auto stat = testTask.start(name, runSomeTask, const_cast<bool*>(&taskRun));
  ASSERT_EQ(stat, Os::Task::TASK_OK);
  testTask.join(nullptr);
  ASSERT_EQ(taskRun, true);
}
