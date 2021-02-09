#pragma once

#include "third_party/fff/fff.h"
#include "utility/rtos/freertos/rtos.hpp"

DECLARE_FAKE_VOID_FUNC(vTaskStartScheduler);
DECLARE_FAKE_VOID_FUNC(vTaskSuspend, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskResume, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskDelete, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskDelayUntil, TickType_t *, TickType_t);
DECLARE_FAKE_VOID_FUNC(vApplicationGetIdleTaskMemory,
                       StaticTask_t **,
                       StackType_t **,
                       uint32_t *);

DECLARE_FAKE_VALUE_FUNC(TickType_t, xTaskGetTickCount);
DECLARE_FAKE_VALUE_FUNC(TaskHandle_t,
                        xTaskCreateStatic,
                        TaskFunction_t,
                        const char *,
                        uint32_t,
                        void *,
                        UBaseType_t,
                        StackType_t *,
                        StaticTask_t *);

DECLARE_FAKE_VALUE_FUNC(EventGroupHandle_t,
                        xEventGroupCreateStatic,
                        StaticEventGroup_t *);
DECLARE_FAKE_VALUE_FUNC(EventBits_t,
                        xEventGroupSync,
                        EventGroupHandle_t,
                        EventBits_t,
                        EventBits_t,
                        TickType_t);

DECLARE_FAKE_VALUE_FUNC(QueueHandle_t,
                        xQueueGenericCreateStatic,
                        UBaseType_t,
                        UBaseType_t,
                        uint8_t *,
                        StaticQueue_t *,
                        uint8_t);
DECLARE_FAKE_VALUE_FUNC(BaseType_t,
                        xQueueGenericSend,
                        QueueHandle_t,
                        const void *,
                        TickType_t,
                        BaseType_t);
DECLARE_FAKE_VALUE_FUNC(BaseType_t,
                        xQueueSemaphoreTake,
                        QueueHandle_t,
                        TickType_t);

DECLARE_FAKE_VALUE_FUNC(TimerHandle_t,
                        xTimerCreateStatic,
                        const char *,
                        TickType_t,
                        UBaseType_t,
                        void *,
                        TimerCallbackFunction_t,
                        StaticTimer_t *);
DECLARE_FAKE_VALUE_FUNC(BaseType_t,
                        xTimerGenericCommand,
                        TimerHandle_t,
                        BaseType_t,
                        TickType_t,
                        BaseType_t *,
                        TickType_t);
DECLARE_FAKE_VALUE_FUNC(void *, pvTimerGetTimerID, TimerHandle_t);
DECLARE_FAKE_VOID_FUNC(vTimerSetTimerID, TimerHandle_t, void *);