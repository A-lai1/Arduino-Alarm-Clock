#include "GlobalsAndHelpers.h"

uint16_t getMathUserAnswer() {
    uint16_t mathUserAnswer = 0;
    for (uint8_t i = 0; i <= MATH_ANSWER_RIGHT_BOUND; i++) {
        mathUserAnswer *= 10;
        mathUserAnswer += mathUserAnswerInput[i];
    }
    return mathUserAnswer;
}

void getDayOfTheWeekStr(const DateTime& now, char *dayOfTheWeek) {
    uint8_t d = now.dayOfTheWeek();

    if (d == 0) sprintf(dayOfTheWeek, "%s", "Sunday");
    else if (d == 1) sprintf(dayOfTheWeek, "%s", "Monday");
    else if (d == 2) sprintf(dayOfTheWeek, "%s", "Tuesday");
    else if (d == 3) sprintf(dayOfTheWeek, "%s", "Wednesday");
    else if (d == 4) sprintf(dayOfTheWeek, "%s", "Thursday");
    else if (d == 5) sprintf(dayOfTheWeek, "%s", "Friday");
    else if (d == 6) sprintf(dayOfTheWeek, "%s", "Saturday");
}

void getDateStr(const DateTime& now, char *date) {
    uint8_t len = 4;
    uint8_t m = now.month();

    if (m == 1) insertStr(date, "Jan ", len);
    else if (m == 2) insertStr(date, "Feb ", len);
    else if (m == 3) insertStr(date, "Mar ", len);
    else if (m == 4) insertStr(date, "Apr ", len);
    else if (m == 5) insertStr(date, "May ", len);
    else if (m == 6) insertStr(date, "Jun ", len);
    else if (m == 7) insertStr(date, "Jul ", len);
    else if (m == 8) insertStr(date, "Aug ", len);
    else if (m == 9) insertStr(date, "Sep ", len);
    else if (m == 10) insertStr(date, "Oct ", len);
    else if (m == 11) insertStr(date, "Nov ", len);
    else if (m == 12) insertStr(date, "Dec ", len);

    date += len;

    char dayStr[len + 1];
    sprintf(dayStr, "%02" PRIu8 ", ", now.day());
    insertStr(date, dayStr, len);
    date += len;

    char yearStr[len + 1];
    sprintf(yearStr, "%" PRIu16, now.year());
    insertStr(date, yearStr, len);
    date[len] = '\0';
}

void insertStr(char *dest, char *src, uint8_t len) {
    for (uint8_t i = 0; i < len; i++)
        dest[i] = src[i];
}

void setAlarm(const DateTime& now, uint8_t hour, uint8_t min) {
    alarm = DateTime(now.year(), now.month(), now.day(), hour, min, 0);

    // if alarm time is less than current time, add a day
    if (alarm <= now) alarm = DateTime(alarm.unixtime() + DAY_IN_SEC);

    // save in eeprom
    EEPROM.put(EEPROM_ADDRESS, alarm.unixtime());
        
    // update alarmState
    alarmState = AlarmState::Set;
}

void deleteAlarm() {
    // save in eeprom
    const uint32_t zero32 = 0;
    EEPROM.put(EEPROM_ADDRESS, zero32);

    // update alarmState
    alarmState = AlarmState::NotSet;
}

void playAlarm() {
    // led
    if ((millis() - prevFlashTime) >= flashInterval) {
        ledOn = !ledOn;
        digitalWrite(ledPin, ledOn);
        prevFlashTime = millis();
    }

    // buzzer
    if (buzzer.getState() == BUZZER_IDLE) buzzer.playMelody(melody, noteDurations, melodyLength);
    buzzer.loop();
}

void stopAlarm() {
    ledOn = false;
    digitalWrite(ledPin, ledOn);
    buzzer.stop();
}

void resetInputCursor(bool printBlockAtStart) {
    printBlock = printBlockAtStart;
    prevBlinkTime = millis();
}

void blinkChar(char blinkingChar) {
    if (millis() - prevBlinkTime >= blinkDur) {
        printBlock = !printBlock;
        prevBlinkTime = millis();
    }
    
    if (printBlock) lcd.write(255);
    else lcd.write(blinkingChar);
}

void mathCancelIncorrect() {
    if (alarmState == AlarmState::MathIncorrect) {
        alarmState = AlarmState::SolvingMath;
        lcd.setCursor(0, 1);
        lcd.print(SPACES_16);
    }
}

void handleMathNumInput(uint8_t num) {
    mathCancelIncorrect();

    mathUserAnswerInput[inputOption] = num;
    // move on to next number if not at the last
    if (inputOption != MATH_ANSWER_RIGHT_BOUND){
        inputOption++;
        resetInputCursor(true);
    } else resetInputCursor(false);
}

void handleAlarmHourMinNumInput(uint8_t num) {
    alarmHourMin[inputOption] = num;
    // move on to next number if not at the last
    if (inputOption != SETTING_HOUR_MIN_RIGHT_BOUND){
        inputOption++;
        resetInputCursor(true);
    } else resetInputCursor(false);
}

bool alarmTimeIsValid() {
    // if 1st digit is 1, 2nd digit must be 0, 1, or 2 (e.g. 13:xx is invalid 12 hr time)
    if (alarmHourMin[0] == 1 && alarmHourMin[1] > 2) return false;

    // 00:xx is invalid 12 hr time
    if (alarmHourMin[0] == 0 && alarmHourMin[1] == 0) return false;

    return true;
}

void handleDigitIncDec(uint8_t *arr, uint8_t index, bool inc) {
    if (inc) {
        if (arr[index] == 9) arr[index] = 0;
        else arr[index]++;
    } else {
        if (arr[index] == 0) arr[index] = 9;
        else arr[index]--;
    }
    resetInputCursor(false);
}