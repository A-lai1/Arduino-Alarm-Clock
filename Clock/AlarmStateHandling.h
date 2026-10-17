#ifndef ALARM_STATE_HANDLING_H
#define ALARM_STATE_HANDLING_H

#include "GlobalsAndHelpers.h"

void alarmSettingHourMinHandling();

void alarmSettingAmPmHandling();

void alarmConfirmDeleteHandling();

void alarmSetHandling(const DateTime& now);

void alarmActiveHandling();

void alarmSolvingMathHandling();

void alarmMathIncorrectHandling();

void alarmMathCorrectHandling(const DateTime& now);

void alarmSnoozeHandling();

#endif