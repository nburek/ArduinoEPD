#include <epd.h>

using namespace EPD;

Display disp(Serial1, 2, 3); //These inputs are based off pluggin the panel into pins 0-3 of the Arduino Yun

void setup() {
  Serial.begin(115200);
  while(!Serial);
}

bool assertTrue(const char *errorMsg, bool success) {
  if (!success) {
    Serial.println(errorMsg);
  }
  return success;
}

bool assertEqual(const char *errorMsg, long expected, long actual) {
  bool success = expected == actual;
  if (!success) {
    Serial.print(errorMsg);
    Serial.print(" Expected: ");
    Serial.print(expected);
    Serial.print(", Actual: ");
    Serial.println(actual);
  }
  return success;
}

void beforeAll() {
  disp.reset();
  disp.wakeUp();
  while(!disp.handshake()); //wait for display to be ready
}

void beforeTest() {
  disp.setBaudRate(57600);
}

void afterTest() {
  
}

void afterAll() {
  disp.enterSleep();
}

bool testHandshake() {
  bool isSuccess = true;
  isSuccess &= assertTrue("Handshake failed when it should have succeeded.", disp.handshake());
  return isSuccess;
}

bool testSetGetBaudRate() {
  bool isSuccess = assertTrue("The setBaudRate function did not return true", disp.setBaudRate(19200));
  long baudRate = disp.getBaudRate();
  isSuccess &= assertEqual("The Baud Rate returned by getBaudRate did not equal 19200", 19200, baudRate);
  return isSuccess;
}

bool testSetGetStorageArea() {
  bool isSuccess = assertTrue("The setStorageArea function did not return true", disp.setStorageArea(StorageArea::MICRO_SD));
  StorageArea storageArea = disp.getStorageArea();
  isSuccess &= assertEqual("The Storage Area returned by getStorageArea did not equal StorageArea::MICRO_SD", StorageArea::MICRO_SD, storageArea);
  return isSuccess;
}


bool (* tests [])() = {
  testHandshake, //Test 1
  testSetGetBaudRate,
  testSetGetStorageArea
  
};

void loop() {
  unsigned int numberOfTests = sizeof(tests) / sizeof(void*);
  unsigned int failureCount = 0;
  
  Serial.println("Starting Integration tests");
  Serial.println("==========================\n\n");
  beforeAll();

  for (unsigned int i = 0; i < numberOfTests; ++i) {
    Serial.print("Running test number ");
    Serial.println(i + 1);
    beforeTest();
    if (!tests[i]()) {
      ++failureCount;
      Serial.print("Test number ");
      Serial.print(i + 1);
      Serial.println(" has failed.");
    }
    afterTest();
    Serial.println("");
  }

  afterAll();
  Serial.println("\n\n==========================");
  Serial.println("Finished Running All Tests");
  Serial.print(failureCount);
  Serial.print(" test(s) failed out of ");
  Serial.println(numberOfTests);

  
  while (1) {
    delay(500);
  }
}