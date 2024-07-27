const int ledPin = 33;
const int buzPin = 2;

#define buzon digitalWrite(buzPin, HIGH)
#define buzoff digitalWrite(buzPin, LOW)

const int buttonPin[4] = { 25, 26, 27, 14 };
unsigned long lastDebounceTime[4] = { 0 };
unsigned long debounceButtonDelay = 100;

void setupInputOutput()
{
    for (int i = 0; i < 4; i++) {
        pinMode(buttonPin[i], INPUT_PULLUP);
        pinMode(buzPin, OUTPUT);
        pinMode(ledPin, OUTPUT);
    }

    for (int i = 0; i < 6; i++) {
        digitalWrite(ledPin, !digitalRead(ledPin)), delay(25);
        digitalWrite(buzPin, !digitalRead(buzPin)), delay(15);
    }

    delay(2000);
    beep(100);

    Serial.println("setup ok");
}

void beep(int duration) { buzon, delay(duration), buzoff; }

bool button(int ch)
{
    if (!digitalRead(buttonPin[ch])) {
        if (millis() - lastDebounceTime[ch] > debounceButtonDelay) {
            lastDebounceTime[ch] = millis();
            beep(10);
            return true;
        }
    } else {
        lastDebounceTime[ch] = 0;
        // return false;
    }
    return false;
}

unsigned long prevMillisBlink;
int stateBlinkOut;

void blinkIndicator(int pin, int offDuration)
{
    unsigned long currentMillis = millis();
    if (stateBlinkOut && (currentMillis - prevMillisBlink) >= 10)
        stateBlinkOut = LOW, prevMillisBlink = currentMillis;
    if (!stateBlinkOut && (currentMillis - prevMillisBlink) >= offDuration)
        stateBlinkOut = HIGH, prevMillisBlink = currentMillis;

    digitalWrite(pin, stateBlinkOut);
}