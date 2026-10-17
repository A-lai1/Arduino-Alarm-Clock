#include "IrInputHandling.h"

void irStarPoundHandling(const DateTime& now) {
    if (alarmState == AlarmState::SettingHourMin || alarmState == AlarmState::SettingAmPm) {
    // exit if currently setting alarm
        alarmState = AlarmState::NotSet;
        lcd.setCursor(0, 1);
        lcd.print(SPACES_16);
    } else if (alarmState == AlarmState::Set) {
    // if alarm is set, prompt to delete alarm
        alarmState = AlarmState::ConfirmDelete;
        // no is selected by default
        inputOption = 1;
    } else if (alarmState == AlarmState::ConfirmDelete) {
    // cancel ConfirmDelete
        alarmState = AlarmState::Set;
    } else if (alarmState == AlarmState::Active) {
    // snooze
        stopAlarm();

        lcd.setCursor(0, 1);
        lcd.print(SPACES_16);

        alarmState = AlarmState::Snooze;
        snoozeStartTime = millis();
    } else if (alarmState == AlarmState::NotSet) {
    // otherwise, prompt to enter hours and mins
        lcd.setCursor(0, 1);
        lcd.print(SPACES_16);
        alarmState = AlarmState::SettingHourMin;
        inputOption = 0;

        // set current time as default time
        alarmHourMin[0] = now.twelveHour() / 10;
        alarmHourMin[1] = now.twelveHour() % 10;
        alarmHourMin[2] = now.minute() / 10;
        alarmHourMin[3] = now.minute() % 10;
    }
}

void irOkHandling(const DateTime& now) {
    if ((alarmState == AlarmState::SettingHourMin) && (alarmTimeIsValid())) {
    // if alarm draft is valid, prompt user to specify am or pm
        alarmState = AlarmState::SettingAmPm;
        inputOption = 0;
    } else if (alarmState == AlarmState::SettingAmPm) {
    // save alarm
        uint8_t alarmHour = alarmHourMin[0] * 10 + alarmHourMin[1];

        // change 12 hr time to 24 hr time
        // 12 AM is 00
        if (inputOption == 0 && alarmHour == 12) alarmHour -= 12;
        // add 12 to all PM times except 12 (12 PM is still 12)
        else if (inputOption == 1 && alarmHour != 12) alarmHour += 12;

        uint8_t alarmMin = alarmHourMin[2] * 10 + alarmHourMin[3];

        setAlarm(now, alarmHour, alarmMin);
        
        // if alarm is invalid, halt program to debug
        if (!alarm.isValid()) {
            lcd.clear();
            lcd.setCursor(0, 0);
            char failedAlarm[6];
            sprintf(failedAlarm, "%" PRIu8 "%" PRIu8 ":%" PRIu8 "%" PRIu8, alarmHourMin[0], alarmHourMin[1], alarmHourMin[2], alarmHourMin[3]);
            lcd.print(failedAlarm);

            lcd.setCursor(0, 1);
            lcd.print("INVALID ALARM");
            while (1);
        }
    } else if (alarmState == AlarmState::ConfirmDelete) {
    // confirm alarm deletion or cancel
        if (inputOption == 0) {
            deleteAlarm();

            lcd.setCursor(0, 1);
            lcd.print(SPACES_16);
        } else if (inputOption == 1) alarmState = AlarmState::Set;
    } else if (alarmState == AlarmState::Active) {
    // set up math problem
        alarmState = AlarmState::SolvingMath;

        // set up problem. problem takes the form of:
        // xx * x + xx = ?
        term1 = random(0, 100);
        term2 = random(0, 10);
        term3 = random(0, 100);

        inputOption = 0;

        lcd.clear();
    } else if (alarmState == AlarmState::SolvingMath) {
    // check if user answer is right
        const uint16_t mathTrueAnswer = term1 * term2 + term3;
        const uint16_t mathUserAnswer = getMathUserAnswer();

        // correct answer
        if (mathUserAnswer == mathTrueAnswer) {
            // clear first row so time is displayed again properly
            lcd.setCursor(0, 0);
            lcd.print(SPACES_16);

            // turn off alarm
            stopAlarm();

            alarmState = AlarmState::MathCorrect;
        // incorrect answer
        } else alarmState = AlarmState::MathIncorrect;

        // reset user answer
        for (uint8_t i = 0; i <= MATH_ANSWER_RIGHT_BOUND; i++) mathUserAnswerInput[i] = 0;

        mathEnteredAnswerTime = millis();
    } else if (alarmState == AlarmState::Snooze) {
    // turn alarm back on
        alarmState = AlarmState::Active;
    }
}

void irLeftHandling() {
    // cancel MathIncorrect if alarmState is set to it
    mathCancelIncorrect();

    if (inputOption == 0) {
        if (alarmState == AlarmState::SettingHourMin) inputOption = SETTING_HOUR_MIN_RIGHT_BOUND;
        else if (alarmState == AlarmState::SettingAmPm) inputOption = SETTING_AM_PM_RIGHT_BOUND;
        else if (alarmState == AlarmState::ConfirmDelete) inputOption = CONFIRM_DELETE_RIGHT_BOUND;
        else if (alarmState == AlarmState::SolvingMath) inputOption = MATH_ANSWER_RIGHT_BOUND;
    } else inputOption--;

    resetInputCursor(true);
}

void irRightHandling() {
    // cancel MathIncorrect if alarmState is set to it
    mathCancelIncorrect();

    if (
        ((alarmState == AlarmState::SettingHourMin) && (inputOption == SETTING_HOUR_MIN_RIGHT_BOUND)) ||
        ((alarmState == AlarmState::SettingAmPm) && (inputOption == SETTING_AM_PM_RIGHT_BOUND)) ||
        ((alarmState == AlarmState::ConfirmDelete) && (inputOption == CONFIRM_DELETE_RIGHT_BOUND)) ||
        ((alarmState == AlarmState::SolvingMath) && (inputOption == MATH_ANSWER_RIGHT_BOUND))
    ) inputOption = 0;
    else inputOption++;

    resetInputCursor(true);
}

void irUpHandling() {
    if (alarmState == AlarmState::SettingHourMin){
        handleDigitIncDec(alarmHourMin, inputOption, true);
    } else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect){
        mathCancelIncorrect();
        handleDigitIncDec(mathUserAnswerInput, inputOption, true);
    } else {
        dateTimeDisplay = static_cast<DateTimeDisplay>(static_cast<uint8_t>(dateTimeDisplay) - 1);
        if (dateTimeDisplay == DateTimeDisplay::First) dateTimeDisplay = static_cast<DateTimeDisplay>(static_cast<uint8_t>(DateTimeDisplay::Last) - 1);

        lcd.setCursor(0, 0);
        lcd.print(SPACES_16);
    }
}

void irDownHandling() {
    if (alarmState == AlarmState::SettingHourMin){
        handleDigitIncDec(alarmHourMin, inputOption, false);
    } else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) {
        mathCancelIncorrect();
        handleDigitIncDec(mathUserAnswerInput, inputOption, false);
    } else {
        dateTimeDisplay = static_cast<DateTimeDisplay>(static_cast<uint8_t>(dateTimeDisplay) + 1);
        if (dateTimeDisplay == DateTimeDisplay::Last) dateTimeDisplay = static_cast<DateTimeDisplay>(static_cast<uint8_t>(DateTimeDisplay::First) + 1);

        lcd.setCursor(0, 0);
        lcd.print(SPACES_16);
    }
}

void irZeroHandling() {
    if (alarmState == AlarmState::SettingHourMin) handleAlarmHourMinNumInput(0);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(0);
}

void irOneHandling() {
    if (alarmState == AlarmState::SettingHourMin) handleAlarmHourMinNumInput(1);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(1);
}

void irTwoHandling() {
    if (alarmState == AlarmState::SettingHourMin && inputOption != 0) handleAlarmHourMinNumInput(2);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(2);
}

void irThreeHandling() {
    if (alarmState == AlarmState::SettingHourMin && inputOption != 0) handleAlarmHourMinNumInput(3);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(3);
}

void irFourHandling() {
    if (alarmState == AlarmState::SettingHourMin && inputOption != 0) handleAlarmHourMinNumInput(4);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(4);
}

void irFiveHandling() {
    if (alarmState == AlarmState::SettingHourMin && inputOption != 0) handleAlarmHourMinNumInput(5);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(5);
}

void irSixHandling() {
    if (alarmState == AlarmState::SettingHourMin && inputOption != 0) handleAlarmHourMinNumInput(6);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(6);
}

void irSevenHandling() {
    if (alarmState == AlarmState::SettingHourMin && inputOption != 0 && inputOption != 2) handleAlarmHourMinNumInput(7);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(7);
}

void irEightHandling() {
    if (alarmState == AlarmState::SettingHourMin && inputOption != 0 && inputOption != 2) handleAlarmHourMinNumInput(8);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(8);
}

void irNineHandling() {
    if (alarmState == AlarmState::SettingHourMin && inputOption != 0 && inputOption != 2) handleAlarmHourMinNumInput(9);
    else if (alarmState == AlarmState::SolvingMath || alarmState == AlarmState::MathIncorrect) handleMathNumInput(9);
}