#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <ezBuzzer.h>
#include <EEPROM.h>
#include "RemoteCodes.h"

inline constexpr uint32_t DAY_IN_SEC = 86400;
inline constexpr char SPACES_16[] = "                ";
inline constexpr uint8_t EEPROM_ADDRESS = 0;

inline constexpr int melody[] = {
    NOTE_D6,
    NOTE_D6,
    NOTE_A6,
    NOTE_D6,
    NOTE_FS6,
    NOTE_FS6,
    NOTE_A6,
    NOTE_FS6,
    NOTE_CS6,
    NOTE_CS6,
    NOTE_CS6,
    NOTE_E6,
    NOTE_B5,
    NOTE_B5,
    NOTE_D6,
    NOTE_B5
};

inline constexpr int noteDurations[] = {
    3,
    4,
    8,
    4,
    3,
    3,
    8,
    8,
    3,
    3,
    8,
    8,
    3,
    4,
    8,
    4
};

inline constexpr int melodyLength = sizeof(melody) / sizeof(int);

enum class DateTimeDisplay : uint8_t {
    First,
    Time,
    Date,
    DayOfTheWeek,
    Last
};
inline DateTimeDisplay dateTimeDisplay = DateTimeDisplay::Time;

// lcd
inline constexpr uint8_t rs = 2;
inline constexpr uint8_t en = 3;
inline constexpr uint8_t d4 = 4;
inline constexpr uint8_t d5 = 5;
inline constexpr uint8_t d6 = 6;
inline constexpr uint8_t d7 = 7;
inline LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// led
inline constexpr uint8_t ledPin = 8;
inline constexpr uint16_t flashInterval = 500;
inline unsigned long prevFlashTime = 0;
inline bool ledOn = false;

// buzzer
inline constexpr uint8_t buzzerPin = 9;
inline ezBuzzer buzzer(buzzerPin, BUZZER_TYPE_PASSIVE);

// ir
inline constexpr uint32_t IrPin = 10;

// rtc
inline RTC_DS3231 rtc;

// alarm
enum class AlarmState : uint8_t {
    NotSet,
    SettingHourMin,
    SettingAmPm,
    Set,
    ConfirmDelete,
    Active,
    SolvingMath,
    MathCorrect,
    MathIncorrect,
    Snooze
};
inline AlarmState alarmState = AlarmState::NotSet;
// right bound for input (highest valid inputOption)
inline constexpr uint8_t SETTING_HOUR_MIN_RIGHT_BOUND = 3;
inline constexpr uint8_t SETTING_AM_PM_RIGHT_BOUND = 1;
inline constexpr uint8_t CONFIRM_DELETE_RIGHT_BOUND = 1;
inline uint8_t alarmHourMin[4];
// tracks the option currently selected by the user starting at 0
inline uint8_t inputOption = 0;
inline DateTime alarm;
// 9 minutes
inline constexpr uint32_t SNOOZE_TIME = 540000;
inline unsigned long snoozeStartTime = 0;

// math
inline uint8_t term1;
inline uint8_t term2;
inline uint8_t term3;
// answer for math problem can only be 3 digits max because highest answer is:
// 99 * 9 + 99 = 990
inline constexpr uint8_t MATH_ANSWER_RIGHT_BOUND = 2;
inline uint8_t mathUserAnswerInput[MATH_ANSWER_RIGHT_BOUND + 1];
inline constexpr uint16_t mathFeedbackDur = 4000;
inline unsigned long mathEnteredAnswerTime = 0;

// char blink
inline constexpr uint16_t blinkDur = 500;
inline unsigned long prevBlinkTime = 0;
inline bool printBlock = false;

// helper functions
uint16_t getMathUserAnswer();

void getDayOfTheWeekStr(const DateTime& now, char *dayOfTheWeek);

void getDateStr(const DateTime& now, char *date);

void insertStr(char *dest, char *src, uint8_t len);

void setAlarm(const DateTime& now, uint8_t hour, uint8_t min);

void deleteAlarm();

void playAlarm();

void stopAlarm();

void resetInputCursor(bool printBlockAtStart);

void blinkChar(char blinkingChar);

void mathCancelIncorrect();

void handleMathNumInput(uint8_t num);

void handleAlarmHourMinNumInput(uint8_t num);

bool alarmTimeIsValid();

void handleDigitIncDec(uint8_t *arr, uint8_t index, bool inc);

#endif