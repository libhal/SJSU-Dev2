#include <FreeRTOS.h>
#include <task.h>
#include <iterator>
// Implementation of vApplicationGetIdleTaskMemory required when
// The function is called to statically create the idle task when
// vTaskStartScheduler is invoked.
static StaticTask_t idle_task_tcb;
static StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
extern "C" void vApplicationGetIdleTaskMemory(  // NOLINT
    StaticTask_t ** ppx_idle_task_tcb_buffer,
    StackType_t ** ppx_idle_task_stack_buffer,
    uint32_t * pul_idle_task_stack_size)
{
  *ppx_idle_task_tcb_buffer   = &idle_task_tcb;
  *ppx_idle_task_stack_buffer = idle_task_stack;
  *pul_idle_task_stack_size   = std::size(idle_task_stack);
}