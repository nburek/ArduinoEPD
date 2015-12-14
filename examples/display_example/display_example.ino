#include <epd.h>

using namespace EPD;

Display disp(Serial1, 2, 3); //These inputs are based off pluggin the panel into pins 0-3 of the Arduino Yun

void setup() {
  disp.reset();
  disp.wakeUp();
  disp.setBaudRate(57600);
  disp.setStorageArea(StorageArea::NAND_FLASH);

  Serial.begin(115200);
  Serial.println("Waiting for handshake");
  while (!disp.handshake()); //wait for the display to be ready
  Serial.println("Received Handshake");
}

void baseDraw() {
  int i, j;

  Serial.println("Starting to draw pixels");
  
  // Draw pixel Grid
  disp.clearScreen();
  for (j = 0; j < 600; j += 50)
  {
    for (i = 0; i < 800; i += 50)
    {
      disp.drawPoint(i, j);
      disp.drawPoint(i, j + 1);
      disp.drawPoint(i + 1, j);
      disp.drawPoint(i + 1, j + 1);
    }
  }
  disp.refresh();
  delay(5000);

  // Draw Lines
  disp.clearScreen();
  for (i = 0; i < 800; i += 100)
  {
    disp.drawLine(0, 0, i, 599);
    disp.drawLine(799, 0, i, 599);
  }
  disp.refresh();
  delay(5000);

  // Fill Rectangles
  disp.clearScreen();
  disp.setDrawingColor(Color::BLACK, Color::WHITE);
  disp.fillRectangle(10, 10, 100, 100);

  disp.setDrawingColor(Color::DARK_GREY, Color::WHITE);
  disp.fillRectangle(110, 10, 200, 100);

  disp.setDrawingColor(Color::LIGHT_GREY, Color::WHITE);
  disp.fillRectangle(210, 10, 300, 100);

  disp.refresh();
  delay(5000);

  // Draw Circles
  disp.setDrawingColor(Color::BLACK, Color::WHITE);
  disp.clearScreen();
  for (i = 0; i < 300; i += 40)
  {
    disp.drawCircle(399, 299, i);
  }
  disp.refresh();
  delay(5000);

  // Fill Circles
  disp.clearScreen();
  for (j = 0; j < 6; j++)
  {
    for (i = 0; i < 8; i++)
    {
      disp.fillCircle(50 + i * 100, 50 + j * 100, 50);
    }
  }
  disp.refresh();
  delay(5000);

  // Draw Triangles
  disp.clearScreen();
  for (i = 1; i < 5; i++)
  {
    disp.drawTriangle(399, 249 - i * 50, 349 - i * 50, 349 + i * 50, 449 + i * 50, 349 + i * 50);
  }
  disp.refresh();
  delay(5000);
}

void drawTextDemo() {
  char buff[16] = {'G', 'B', 'K', '3', '2', ':', ' ', 0xc4, 0xe3, 0xba, 0xc3, 0xca, 0xc0, 0xbd, 0xe7, 0x00};
  disp.setDrawingColor(Color::BLACK, Color::WHITE);
  disp.clearScreen();
  disp.setChineseFontSize(FontSize::DOTS_MATRIX_32);
  disp.setEnglishFontSize(FontSize::DOTS_MATRIX_32);
  disp.displayText(0, 50, buff);
  disp.displayText(0, 300, "ASCII32: Hello, World!");

  disp.setChineseFontSize(FontSize::DOTS_MATRIX_48);
  disp.setEnglishFontSize(FontSize::DOTS_MATRIX_48);
  buff[3] = '4';
  buff[4] = '8';
  disp.displayText(0, 100, buff);
  disp.displayText(0, 350, "ASCII48: Hello, World!");

  disp.setChineseFontSize(FontSize::DOTS_MATRIX_64);
  disp.setEnglishFontSize(FontSize::DOTS_MATRIX_64);
  buff[3] = '6';
  buff[4] = '4';
  disp.displayText(0, 160, buff);
  disp.displayText(0, 450, "ASCII48: Hello, World!");


  disp.refresh();
  delay(5000);
}

void drawImageDemo() {
  disp.clearScreen();
  disp.displayImage(0, 0, "PIC4.BMP");
  disp.refresh();
  delay(5000);

  disp.clearScreen();
  disp.displayImage(0, 100, "PIC2.BMP");
  disp.displayImage(400, 100, "PIC3.BMP");
  disp.refresh();
  delay(5000);

  disp.clearScreen();
  disp.displayImage(0, 0, "PIC7.BMP");
  disp.refresh();
}

void loop() {

  Serial.println("Loop Started");
  baseDraw();

  drawTextDemo();

  drawImageDemo();
  
  disp.enterSleep();

  while (1) {
    delay(500);
  }
}
