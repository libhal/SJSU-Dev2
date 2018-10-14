#include <cstdarg>

#include "L0_LowLevel/uart0.hpp"
#include "L2_Utilities/debug_print.hpp"
#include "L4_Application/task.hpp"

class PrinterTask : public rtos::Task<1024>
{
 public:
  PrinterTask(const char * task_name, const char * message)
      : Task(task_name, rtos::Priority::kMedium),
        message_(message),
        run_count_(0){};
  bool Setup() override
  {
    DEBUG_PRINT("Setup() for: %s", kName);
    return true;
  };
  bool PreRun() override
  {
    DEBUG_PRINT("PreRun() for: %s", kName);
    return true;
  };
  bool Run() override
  {
    run_count_ += 1;
    DEBUG_PRINT("%s: %ld", message_, run_count_);
    if (run_count_ == 10)
    {
      Delete();
    }
    return true;
  };

 private:
  const char * message_;
  uint32_t run_count_;
};

int main()
{
  DEBUG_PRINT("Starting TaskScheduler example...");
  // TODO(#210): Should remove the use of new and use static allocation for
  //             constructing tasks when the issue is fixed.
  PrinterTask * printer_one =
      new PrinterTask("Printer A", "I am a printer, I am faster");
  printer_one->SetDelayTime(500);
  PrinterTask * printer_two =
      new PrinterTask("Printer B", "I am also a printer");
  printer_two->SetDelayTime(1000);

  DEBUG_PRINT("Attempting to search for Printer A in the scheduler...");
  rtos::TaskInterface & task =
      *(rtos::TaskScheduler::Instance().GetTask("Printer A"));
  DEBUG_PRINT("Found task: %s", task.GetName());

  rtos::TaskScheduler::Instance().Start();
  DEBUG_PRINT("This point should not be reached!");
  delete printer_one;
  delete printer_two;
  return 0;
}
