#include <FreeRTOS.h>
#include <semphr.h>

#include <cstdarg>

#include "L3_Application/task.hpp"
#include "utility/log.hpp"

class PrinterTask : public rtos::Task<512>
{
 public:
  constexpr PrinterTask(const char * task_name, const char * message)
      : Task(task_name, rtos::Priority::kMedium),
        message_(message),
        run_count_(0)
  {
    print_mutex = { 0 };
  };
  bool Setup() override
  {
    if (print_mutex == NULL)
    {
      print_mutex = xSemaphoreCreateMutex();
    }
    LOG_INFO("Setup() for: %s", kName);
    return true;
  };
  bool PreRun() override
  {
    xSemaphoreTake(print_mutex, portMAX_DELAY);
    LOG_INFO("PreRun() for: %s", kName);
    xSemaphoreGive(print_mutex);
    return true;
  };
  bool Run() override
  {
    run_count_ += 1;
    xSemaphoreTake(print_mutex, portMAX_DELAY);
    LOG_INFO("%s: %ld", message_, run_count_);
    xSemaphoreGive(print_mutex);
    if (run_count_ == 10)
    {
      Delete();
    }
    return true;
  };

 private:
  inline static SemaphoreHandle_t print_mutex;
  const char * message_;
  uint32_t run_count_;
};

PrinterTask printer_one("Printer A", "I am a printer, I am faster");
PrinterTask printer_two("Printer B", "I am also a printer");

int main()
{
  LOG_INFO("Starting TaskScheduler example...");
  printer_one.SetDelayTime(500);
  printer_two.SetDelayTime(1000);

  LOG_INFO("Attempting to search for Printer A in the scheduler...");
  rtos::TaskInterface & task =
      *(rtos::TaskScheduler::Instance().GetTask("Printer A"));
  LOG_INFO("Found task: %s", task.GetName());

  LOG_INFO("Starting scheduler");
  rtos::TaskScheduler::Instance().Start();
  LOG_INFO("This point should not be reached!");
  return 0;
}
