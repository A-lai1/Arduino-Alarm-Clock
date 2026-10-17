#define IR_USE_AVR_TIMER1
#include <IRremote.hpp>
#include "GlobalsAndHelpers.h"
#include "IrInputHandling.h"
#include "AlarmStateHandling.h"

void setup() {
    lcd.begin(16, 2);
    pinMode(ledPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    IrReceiver.begin(IrPin, ENABLE_LED_FEEDBACK);

    // halt program until rtc is detected
    while (!rtc.begin()) {
        lcd.print("RTC NOT FOUND");
        lcd.setCursor(0, 0);
        delay(3000);
    }

    // check if rtc lost power
    if (rtc.lostPower()) {
        lcd.print("REPLACE BATTERY");
        lcd.setCursor(0, 0);
        delay(5000);

        // adjust the time
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    uint32_t alarmRetrieved;
    EEPROM.get(EEPROM_ADDRESS, alarmRetrieved);
    if (alarmRetrieved != 0 && alarmRetrieved != 0xFFFFFFFF) {
        alarm = DateTime(alarmRetrieved);
        alarmState = AlarmState::Set;
    }

    for (uint8_t i = 0; i <= MATH_ANSWER_RIGHT_BOUND; i++) mathUserAnswerInput[i] = 0;

    randomSeed(analogRead(0));

    lcd.clear();
}

void loop() {
    const DateTime now = rtc.now();

    if ((alarmState != AlarmState::SolvingMath) && (alarmState != AlarmState::MathIncorrect)) {
        if (dateTimeDisplay == DateTimeDisplay::Time) {
            char time[12];
            const char *amOrPm = (now.isPM()) ? "PM" : "AM";
            sprintf(time, "%02" PRIu8 ":%02" PRIu8 ":%02" PRIu8 " %s", now.twelveHour(), now.minute(), now.second(), amOrPm);
            lcd.print(time);
        } else if (dateTimeDisplay == DateTimeDisplay::Date) {
            char date[13];
            getDateStr(now, date);
            lcd.print(date);
        } else if (dateTimeDisplay == DateTimeDisplay::DayOfTheWeek) {
            char dayOfTheWeek[10];
            getDayOfTheWeekStr(now, dayOfTheWeek);
            lcd.print(dayOfTheWeek);
        }
    }

    handleIrInput(now);

    handleAlarmState(now);

    lcd.setCursor(0, 0);
}

void handleIrInput(const DateTime& now) {
    if (IrReceiver.decode()) {
        const uint32_t input = IrReceiver.decodedIRData.decodedRawData;

        // 0 means button was held down, so ignore it
        if (input != 0) {
            if (input == Remote::STAR || input == Remote::POUND) irStarPoundHandling(now);
            else if (input == Remote::OK) irOkHandling(now);
            else if (input == Remote::LEFT) irLeftHandling();
            else if (input == Remote::RIGHT) irRightHandling();
            else if (input == Remote::UP) irUpHandling();
            else if (input == Remote::DOWN) irDownHandling();
            else if (input == Remote::ZERO) irZeroHandling();
            else if (input == Remote::ONE) irOneHandling();
            else if (input == Remote::TWO) irTwoHandling();
            else if (input == Remote::THREE) irThreeHandling();
            else if (input == Remote::FOUR) irFourHandling();
            else if (input == Remote::FIVE) irFiveHandling();
            else if (input == Remote::SIX) irSixHandling();
            else if (input == Remote::SEVEN) irSevenHandling();
            else if (input == Remote::EIGHT) irEightHandling();
            else if (input == Remote::NINE) irNineHandling();
        }
            
        IrReceiver.resume();
    }
}

void handleAlarmState(const DateTime& now) {
    if (alarmState == AlarmState::SettingHourMin) alarmSettingHourMinHandling();
    else if (alarmState == AlarmState::SettingAmPm) alarmSettingAmPmHandling();
    else if (alarmState == AlarmState::ConfirmDelete) alarmConfirmDeleteHandling();
    else if (alarmState == AlarmState::Set) alarmSetHandling(now);
    else if (alarmState == AlarmState::Active) alarmActiveHandling();
    else if (alarmState == AlarmState::SolvingMath) alarmSolvingMathHandling();
    else if (alarmState == AlarmState::MathIncorrect) alarmMathIncorrectHandling();
    else if (alarmState == AlarmState::MathCorrect) alarmMathCorrectHandling(now);
    else if (alarmState == AlarmState::Snooze) alarmSnoozeHandling();
}