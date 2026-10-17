#include "AlarmStateHandling.h"

void alarmSettingHourMinHandling() {
    // print draft alarm time
    lcd.setCursor(0, 1);
    
    // add '0' (0 is 48 in ascii) to get ascii value
    if (inputOption == 0) blinkChar('0' + alarmHourMin[0]);
    else lcd.write('0' + alarmHourMin[0]);

    if (inputOption == 1) blinkChar('0' + alarmHourMin[1]);
    else lcd.write('0' + alarmHourMin[1]);

    lcd.write(':');

    if (inputOption == 2) blinkChar('0' + alarmHourMin[2]);
    else lcd.write('0' + alarmHourMin[2]);

    if (inputOption == 3) blinkChar('0' + alarmHourMin[3]);
    else lcd.write('0' + alarmHourMin[3]);
}

void alarmSettingAmPmHandling() {
    lcd.setCursor(0, 1);

    if (inputOption == 0) blinkChar('*');
    else lcd.write('*');

    lcd.print("AM     ");

    if (inputOption == 1) blinkChar('*');
    else lcd.write('*');

    lcd.print("PM");
}

void alarmConfirmDeleteHandling() {
    lcd.setCursor(0, 1);
    lcd.print("Delete? ");

    if (inputOption == 0) blinkChar('Y');
    else lcd.write('Y');

    lcd.print("     ");

    if (inputOption == 1) blinkChar('N');
    else lcd.write('N');
}

void alarmSetHandling(const DateTime& now) {
    lcd.setCursor(0, 1);
    char alarmTime[16];
    const char *alarmAmOrPm = (alarm.isPM()) ? "PM" : "AM";
    sprintf(alarmTime, "Alarm: %02" PRIu8 ":%02" PRIu8 " %s", alarm.twelveHour(), alarm.minute(), alarmAmOrPm);
    lcd.print(alarmTime);

    // check if it is time for alarm
    if (now >= alarm) alarmState = AlarmState::Active;
}

void alarmActiveHandling() {
    playAlarm();

    // alarm message
    lcd.setCursor(0, 1);
    if (ledOn) lcd.print("RISE AND GRIND!");
    else lcd.print(SPACES_16);
}

void alarmSolvingMathHandling() {
    playAlarm();

    char problem[16];
    sprintf(problem, "%" PRIu8 " * %" PRIu8 " + %" PRIu16 " = ?", term1, term2, term3);

    lcd.setCursor(0, 0);
    lcd.print(problem);

    lcd.setCursor(0, 1);
    for (uint8_t i = 0; i <= MATH_ANSWER_RIGHT_BOUND; i++) {
        // add '0' (0 is 48 in ascii) to get ascii value
        if (i == inputOption) blinkChar('0' + mathUserAnswerInput[i]);
        else lcd.write('0' + mathUserAnswerInput[i]);
    }
}

void alarmMathIncorrectHandling() {
    playAlarm();

    lcd.setCursor(0, 1);

    if (millis() - mathEnteredAnswerTime >= mathFeedbackDur) {
        lcd.print(SPACES_16);
        alarmState = AlarmState::SolvingMath;
    } else lcd.print("Incorrect");
}

void alarmMathCorrectHandling(const DateTime& now) {
    lcd.setCursor(0, 1);

    if (millis() - mathEnteredAnswerTime >= mathFeedbackDur) {
        // reset alarm
        setAlarm(now, alarm.hour(), alarm.minute());
    } else lcd.print("CORRECT!");
}

void alarmSnoozeHandling() {
    const uint32_t elapsedMillis = millis() - snoozeStartTime;

    if (elapsedMillis >= SNOOZE_TIME) alarmState = AlarmState::Active;
    else {
        const uint16_t remainingTotalSecs = (SNOOZE_TIME - elapsedMillis) / 1000;
        const uint8_t mins = remainingTotalSecs / 60;
        const uint8_t secs = remainingTotalSecs % 60;

        lcd.setCursor(0, 1);
        char remainingTime[13];
        sprintf(remainingTime, "Snooze: %" PRIu8 ":%02" PRIu8, mins, secs);
        lcd.print(remainingTime);
    }
}