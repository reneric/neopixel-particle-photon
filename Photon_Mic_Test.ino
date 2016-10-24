
#define MICROPHONE_PIN A5
#define AUDIO_BUFFER_MAX 8192

int audioStartIdx = 0, audioEndIdx = 0;
uint16_t audioBuffer[AUDIO_BUFFER_MAX];
uint16_t txBuffer[AUDIO_BUFFER_MAX];

// version without timers
unsigned long lastRead = micros();

void setup() {
    Serial.begin(115200);
    pinMode(MICROPHONE_PIN, INPUT);


    lastRead = micros();
}

void loop() {
    listenAndSend(100);
}

void listenAndSend(int delay) {
    unsigned long startedListening = millis();

    while ((millis() - startedListening) < delay) {
        unsigned long time = micros();

        if (lastRead > time) {
            // time wrapped?
            //lets just skip a beat for now, whatever.
            lastRead = time;
        }

        //125 microseconds is 1/8000th of a second
        if ((time - lastRead) > 125) {
            lastRead = time;
            readMic();
        }
    }
}


// Callback for Timer 1
void readMic(void) {
    uint16_t value = analogRead(MICROPHONE_PIN);
    Serial.print(value);
    if (audioEndIdx >= AUDIO_BUFFER_MAX) {
        audioEndIdx = 0;
    }
    audioBuffer[audioEndIdx++] = value;
}

