#define CHANNEL01  2
#define CHANNEL02  3
#define CHANNEL03  4
#define CHANNEL04  5
#define CHANNEL05  9
#define CHANNEL06  10

#define RANDOM_PIN A4
#define ON_PIN A3
#define OFF_PIN A2

#define AUDIO_PIN A0
#define RANDOM_MODE_PINOUT 53
#define RANDOM_MODE_SPEED 500
#define PIN_BUTTON  6
#define PIN_LED_BEAT  7
#define PIN_LED_STATUS  8

void light1();
void light2();
void light3();
void light4();
void light5();
void light6();
void none();
void allOnBeat();
void allOffBeat();
boolean inArray(int array[], int element, int arraySize);


// Channel Setup
int channels[] = {CHANNEL01, CHANNEL02, CHANNEL03, CHANNEL04, CHANNEL05, CHANNEL06};
#define CHANNEL_COUNT 6

// BPM Setup
unsigned int bpm = 284;
unsigned int MS_per_beat = 0;

// Counters
uint8_t section = 0;
uint8_t beatCounter = 0;
uint8_t volumeCouter = 0;

// Defaults
uint8_t volume;
uint8_t sectionLength;
bool ended = false;
bool musicStarted = false;

// Section Arrays
int chorus_a_sections[3] = {0, 9, 10};
int chorus_b_sections[7] = {1, 2, 4, 5, 6, 7, 12};
int chorus_c_sections[1] = {11};
int verse_sections[2] = {3, 8};
int last_sections[1] = {13};

// Light Sequences
typedef void (*LightSequences) ();
LightSequences chor_a[48] = {light1,none,light2,none,light3,none,allOnBeat,none,allOnBeat,none,allOnBeat,none,light3,none,light2,none,light1,none,allOnBeat,none,allOnBeat,none,allOnBeat,none,light2,none,light3,none,light4,none,allOnBeat,none,allOnBeat,none,light1,light2,light3,none,light4,none,allOnBeat,none,allOnBeat,none,none,none,none,none};
LightSequences chor_b[48] = {light2,none,light3,light4,light5,light6,allOnBeat,none,allOnBeat,none,allOnBeat,none,light3,none,light6,light1,light4,light5,allOnBeat,none,allOnBeat,none,allOnBeat,none,light6,none,light3,light4,light5,light6,allOnBeat,none,allOnBeat,none,light1,light2,light6,none,light4,none,allOnBeat,none,allOnBeat,none,none,none,none,none};
LightSequences chor_c[54] = {light2,none,light3,light4,light5,light6,allOnBeat,none,allOnBeat,none,allOnBeat,none,light3,none,light2,light1,light4,light5,allOnBeat,none,allOnBeat,none,allOnBeat,none,light2,none,light3,light4,light5,light6,allOnBeat,none,allOnBeat,none,light1,light2,light3,none,light4,none,allOnBeat,none,allOnBeat,none,none,none,none,none,none,none,none,none,none,none};
LightSequences ver[48] = {light2,none,light3,none,light4,none,allOnBeat,none,allOnBeat,none,allOnBeat,none,light3,none,light2,none,light4,none,allOnBeat,none,allOnBeat,none,light1,none,light2,none,light3,none,light4,none,light5,none,light6,none,light1,light2,light3,none,light4,none,allOnBeat,none,allOnBeat,none,none,none,none,none};
LightSequences last[52] = {light2,light1,light3,light5,light4,light6,allOnBeat,light3,light4,light2,light1,allOnBeat,light3,light1,light2,light5,light4,light6,light1,light2,light3,light4,light1,light5,light2,light6,light3,light1,light4,light2,light5,light3,light6,light4,light1,light2,light3,light1,light4,light6,allOnBeat,light3,light5,allOnBeat,light1,allOnBeat,light2,allOnBeat,light1,allOnBeat,light2,allOnBeat};


void setup() {
  unsigned int ms_per_minute = 1000 * 60;
  MS_per_beat = ms_per_minute / bpm;

  pinMode(PIN_LED_STATUS, OUTPUT);
  pinMode(PIN_LED_BEAT, OUTPUT);

  pinMode(CHANNEL01, OUTPUT);
  pinMode(CHANNEL02, OUTPUT);
  pinMode(CHANNEL03, OUTPUT);
  pinMode(CHANNEL04, OUTPUT);
  pinMode(CHANNEL05, OUTPUT);
  pinMode(CHANNEL06, OUTPUT);

  pinMode(RANDOM_PIN, INPUT);
  pinMode(ON_PIN, INPUT);
  pinMode(OFF_PIN, INPUT);
  pinMode(AUDIO_PIN, INPUT);

  Serial.begin(9600);

  turnLightsOff();
  powerOnSelfTest();

}

void loop() {
  if (digitalRead(RANDOM_PIN) == HIGH) {
    resetCounters();
    doRandomLightsWithDelay();
  } else if (digitalRead(ON_PIN) == HIGH) {
    resetCounters();
    allOn();
  } else if (digitalRead(OFF_PIN) == HIGH) {
    resetCounters();
    allOff();
  } else {
    doMusic();
  }
}

void doMusic() {
  volume = analogRead(AUDIO_PIN);

  if (!musicStarted && section == 0 && beatCounter == 0) {
    (volume < 50) ? volumeCouter++ : (volumeCouter = 0);
    if (volumeCouter > 20) {
      ended = false;
      musicStarted = true;
      volumeCouter = 0;
      delay(2800);
    }
  }
  if (ended) {
    return;
  }
  if (musicStarted) {
    sectionLength = 47;

    digitalWrite(PIN_LED_BEAT, digitalRead(PIN_LED_BEAT) != HIGH);

    if (inArray(chorus_a_sections, section, 3)) {
      chor_a[beatCounter]();
    }
    if (inArray(chorus_b_sections, section, 7)) {
      chor_a[beatCounter]();
    }
    if (inArray(chorus_c_sections, section, 1)) {
      sectionLength = 53;
      chor_c[beatCounter]();
    }
    if (inArray(verse_sections, section, 2)) {
      ver[beatCounter]();
    }
    if (inArray(last_sections, section, 1)) {
      sectionLength = 51;
      last[beatCounter]();
    }
    Serial.println(section);
    if (section == 13 && beatCounter == sectionLength) {
      resetCounters();
    }
    if (beatCounter == sectionLength) {
      beatCounter = 0;
      section++;
    } else {
      beatCounter++;
    }


    delay(MS_per_beat);
  }
}

void doRandomLights() {
  unsigned int ms_per_minute = 1000 * 60;
  MS_per_beat = ms_per_minute / bpm;

  randomSeed(analogRead(0));
  for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
    int randNumber = random(255);
    randNumber = map(randNumber, 0, 255, 255, 0);
    analogWrite(channels[channelIndex], randNumber);
  }
  delay(MS_per_beat);
}

void doRandomLightsWithDelay() {
  randomSeed(analogRead(0));
  for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
    int randNumber = random(255);
    randNumber = map(randNumber, 0, 255, 255, 0);
    analogWrite(channels[channelIndex], randNumber);
  }
  delay(random(100, RANDOM_MODE_SPEED));
}

void allOn() {
  for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
    analogWrite(channels[channelIndex], 0);
  }
}

void allOff() {
  for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
    analogWrite(channels[channelIndex], 255);
  }
}

void powerOnSelfTest() {
  Serial.println("Power on self test running.");
  for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
    analogWrite(channels[channelIndex], 0); // turn on one channel at a time
    delay(100);
    analogWrite(channels[channelIndex], 255);
  }
  allOn();
}

void turnLightsOff() {
  for (int channelIndex = 0; channelIndex < CHANNEL_COUNT; channelIndex++) {
    analogWrite(channels[channelIndex], 255);
  }
}

void light1() {
  Serial.println("light1");
  allOffBeat();
  digitalWrite(channels[0], HIGH);
}

void light2() {
  Serial.println("light2");
  allOffBeat();
  digitalWrite(channels[1], HIGH);
}

void light3() {
  Serial.println("light3");
  allOffBeat();
  digitalWrite(channels[2], HIGH);
}

void light4() {
  Serial.println("light4");
  allOffBeat();
  digitalWrite(channels[3], HIGH);
}

void light5() {
  Serial.println("light5");
  allOffBeat();
  digitalWrite(channels[4], HIGH);
}

void light6() {
  Serial.println("light6");
  allOffBeat();
  digitalWrite(channels[5], HIGH);
}

void allOnBeat() {
  allOffBeat();
  Serial.println("All on beat.");
  digitalWrite(channels[4], HIGH);

}

void allOffBeat() {
  Serial.println("All off beat.");
  digitalWrite(channels[0], LOW);
  digitalWrite(channels[1], LOW);
  digitalWrite(channels[2], LOW);
  digitalWrite(channels[3], LOW);
  digitalWrite(channels[4], LOW);
  digitalWrite(channels[5], LOW);
}

void none() {
  Serial.println("None.");
  digitalWrite(channels[0], LOW);
  digitalWrite(channels[1], LOW);
  digitalWrite(channels[2], LOW);
  digitalWrite(channels[3], LOW);
  digitalWrite(channels[4], LOW);
  digitalWrite(channels[5], LOW);
}

boolean inArray(int array[], int element, int arraySize) {
  for (int i = 0; i < arraySize; i++) {
    if (array[i] == element) {
      return true;
    }
  }
  return false;
}

void resetCounters() {
  ended = true;
  sectionLength = 47;
  musicStarted = false;
  volumeCouter = 0;
  beatCounter = 0;
  section = 0;
}

