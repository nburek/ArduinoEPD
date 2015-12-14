
#include "epd.h"

namespace EPD {
    const byte Display::FRAME_END[4]                    = {0xCC, 0x33, 0xC3, 0x3C};
    const byte Display::HANDSHAKE_PACKET[9]             = {0xA5, 0x00, 0x09, Command::HANDSHAKE,            0xCC, 0x33, 0xC3, 0x3C, 0xAC};
    const byte Display::GET_BAUD_RATE_PACKET[9]         = {0xA5, 0x00, 0x09, Command::GET_BAUD_RATE,        0xCC, 0x33, 0xC3, 0x3C, 0xAE};
    const byte Display::GET_STORAGE_AREA_PACKET[9]      = {0xA5, 0x00, 0x09, Command::GET_STORAGE_AREA,     0xCC, 0x33, 0xC3, 0x3C, 0xAA};
    const byte Display::ENTER_SLEEP_PACKET[9]           = {0xA5, 0x00, 0x09, Command::ENTER_SLEEP,          0xCC, 0x33, 0xC3, 0x3C, 0xA4};
    const byte Display::REFRESH_PACKET[9]               = {0xA5, 0x00, 0x09, Command::REFRESH,              0xCC, 0x33, 0xC3, 0x3C, 0xA6};
    const byte Display::GET_DISP_DIRECTION_PACKET[9]    = {0xA5, 0x00, 0x09, Command::GET_DISP_DIRECTION,   0xCC, 0x33, 0xC3, 0x3C, 0xA0};
    const byte Display::IMPORT_FONT_LIBRARY_PACKET[9]   = {0xA5, 0x00, 0x09, Command::IMPORT_FONT_LIBRARY,  0xCC, 0x33, 0xC3, 0x3C, 0xA2};
    const byte Display::IMPORT_IMAGE_PACKET[9]          = {0xA5, 0x00, 0x09, Command::IMPORT_IMAGE,         0xCC, 0x33, 0xC3, 0x3C, 0xA3};
    const byte Display::CLEAR_SCREEN_PACKET[9]          = {0xA5, 0x00, 0x09, Command::CLEAR_SCREEN,         0xCC, 0x33, 0xC3, 0x3C, 0x82};
    const byte Display::GET_DRAWING_COLOR_PACKET[9]     = {0xA5, 0x00, 0x09, Command::GET_DRAWING_COLOR,    0xCC, 0x33, 0xC3, 0x3C, 0xBD};
    const byte Display::GET_ENGLISH_FONT_SIZE_PACKET[9] = {0xA5, 0x00, 0x09, Command::GET_ENGLISH_FONT_SIZE,0xCC, 0x33, 0xC3, 0x3C, 0xB0};
    const byte Display::GET_CHINESE_FONT_SIZE_PACKET[9] = {0xA5, 0x00, 0x09, Command::GET_CHINESE_FONT_SIZE,0xCC, 0x33, 0xC3, 0x3C, 0xB1};
    
    Display::Display(HardwareSerial &s, int wakeUpPin, int resetPin):serial(s) {
        s.begin(115200);
        s.setTimeout(100);
        this->wakeUpPin = wakeUpPin;
        pinMode(wakeUpPin, OUTPUT);
        this->resetPin = resetPin;
        pinMode(resetPin, OUTPUT);
    }
    
    
    
    /* Hardware Control Functions */
    
    void Display::reset() {
        digitalWrite(resetPin, LOW);
        delayMicroseconds(10);
        digitalWrite(resetPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(resetPin, LOW);
        delay(3000);
    }
    
    void Display::wakeUp() {
        digitalWrite(wakeUpPin, LOW);
        delayMicroseconds(10);
        digitalWrite(wakeUpPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(wakeUpPin, LOW);
        delay(10);
    }
    
    
    
    /* System Control Functions */
    
    bool Display::handshake() {
        flushInputStream();
        sendData(HANDSHAKE_PACKET, 9);
        
        return checkOkResponse();
    }
    
    bool Display::setBaudRate(long baudRate) {
        flushInputStream();
        
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0D;	
        
        outputBuffer[3] = Command::SET_BAUD_RATE;	
        
        outputBuffer[4] = (baudRate >> 24) & 0xFF;
        outputBuffer[5] = (baudRate >> 16) & 0xFF;
        outputBuffer[6] = (baudRate >> 8) & 0xFF;
        outputBuffer[7] = baudRate & 0xFF;
        
        
        memcpy(outputBuffer + 8, FRAME_END, 4);	
        outputBuffer[12] = calculateParityByte(outputBuffer, 12);
        
        sendData(outputBuffer, 13);	
        
        delay(125);	
        serial.begin(baudRate);
        
        return handshake();
    }
    
    long Display::getBaudRate() {
        flushInputStream();
        sendData(GET_BAUD_RATE_PACKET, 9);
        delay(WAIT_FOR_RESPONSE_MS);
        int size = serial.readBytes(inBuffer, 20);
        inBuffer[size] = 0x00;
        return atol((char *)inBuffer);
    }
    
    StorageArea Display::getStorageArea() {
        flushInputStream();
        sendData(GET_STORAGE_AREA_PACKET, 9);
        delay(WAIT_FOR_RESPONSE_MS);
        
        serial.readBytes(inBuffer, 1);
        if (inBuffer[0] == '1')
            return StorageArea::MICRO_SD;
        
        //else assume it's '0'
        return StorageArea::NAND_FLASH;
    }
    
    
    bool Display::setStorageArea(StorageArea storageArea) {
        flushInputStream();
        
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0A;
        
        outputBuffer[3] = Command::SET_STORAGE_AREA;
        
        outputBuffer[4] = storageArea;
        
        memcpy(outputBuffer + 5, FRAME_END, 4);

        outputBuffer[9] = calculateParityByte(outputBuffer, 9);
        
        sendData(outputBuffer, 10);
        
        return checkOkResponse();
    }
    
    void Display::enterSleep() {
        sendData(ENTER_SLEEP_PACKET, 9);
    }
    
    bool Display::refresh() {
        flushInputStream();
        sendData(REFRESH_PACKET, 9);
        return checkOkResponse();
    }
    
    DisplayDirection Display::getDisplayDirection() {
        flushInputStream();
        
        sendData(GET_DISP_DIRECTION_PACKET, 9);
        delay(WAIT_FOR_RESPONSE_MS);
        
        serial.readBytes(inBuffer, 1);
        if (inBuffer[0] == '1')
            return DisplayDirection::INVERTED;

        //assume the output is '0'
        return DisplayDirection::NORMAL;
    }

    bool Display::setDisplayDirection(DisplayDirection displayDirection) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0A;	
        
        outputBuffer[3] = Command::SET_DISP_DIRECTION;
        
        outputBuffer[4] = displayDirection;
        
        memcpy(outputBuffer + 5, FRAME_END, 4);
        
        outputBuffer[9] = calculateParityByte(outputBuffer, 9);
        
        sendData(outputBuffer, 10);
        
        return checkOkResponse();
    }
    
    bool Display::importFontLibrary() {
        flushInputStream();
        sendData(IMPORT_FONT_LIBRARY_PACKET, 10);
        return checkOkResponse();
    }
    
    bool Display::importImage() {
        flushInputStream();
        sendData(IMPORT_IMAGE_PACKET, 10);
        return checkOkResponse();
    }
    
    
    /* Display Parameter Configuration Functions */
    
    bool Display::setDrawingColor(Color color, Color backgroundColor) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0B;
        
        outputBuffer[3] = Command::SET_DRAWING_COLOR;
        
        outputBuffer[4] = color;
        outputBuffer[5] = backgroundColor;
        
        memcpy(outputBuffer + 6, FRAME_END, 4);
        outputBuffer[10] = calculateParityByte(outputBuffer, 10);
        
        sendData(outputBuffer, 11);
        
        return checkOkResponse();
    }
    
    Color Display::getDrawingColor() {
        flushInputStream();
        
        sendData(GET_DRAWING_COLOR_PACKET, 9);
        delay(WAIT_FOR_RESPONSE_MS);
        
        serial.readBytes(inBuffer, 2);

        return charToColor(inBuffer[0]);
    }
    
    Color Display::getBackgroundColor() {
        flushInputStream();
        
        sendData(GET_DRAWING_COLOR_PACKET, 9);
        delay(WAIT_FOR_RESPONSE_MS);
        
        serial.readBytes(inBuffer, 2);

        return charToColor(inBuffer[1]);
    }
    
    FontSize Display::getEnglishFontSize() {
        flushInputStream();
        
        sendData(GET_ENGLISH_FONT_SIZE_PACKET, 9);
        delay(WAIT_FOR_RESPONSE_MS);
        
        serial.readBytes(inBuffer, 1);

        return charToFontSize(inBuffer[0]);
    }
    
    FontSize Display::getChineseFontSize() {
        flushInputStream();
        
        sendData(GET_CHINESE_FONT_SIZE_PACKET, 9);
        delay(WAIT_FOR_RESPONSE_MS);
        
        serial.readBytes(inBuffer, 1);

        return charToFontSize(inBuffer[0]);
    }
    
    bool Display::setEnglishFontSize(FontSize fontSize) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0A;	
        
        outputBuffer[3] = Command::SET_ENGLISH_FONT_SIZE;
        
        outputBuffer[4] = fontSize;
        
        
        memcpy(outputBuffer + 5, FRAME_END, 4);	
        outputBuffer[9] = calculateParityByte(outputBuffer, 9);
        
        sendData(outputBuffer, 10);
        return checkOkResponse();
    }
    
    bool Display::setChineseFontSize(FontSize fontSize) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0A;	
        
        outputBuffer[3] = Command::SET_CHINESE_FONT_SIZE;
        
        outputBuffer[4] = fontSize;
        
        
        memcpy(outputBuffer + 5, FRAME_END, 4);	
        outputBuffer[9] = calculateParityByte(outputBuffer, 9);
        
        sendData(outputBuffer, 10);
        return checkOkResponse();
    }
    
    
    
    /* Basic Drawing Functions */
    
    bool Display::drawPoint(unsigned int x, unsigned int y) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0D;
        
        outputBuffer[3] = Command::DRAW_POINT;
        
        outputBuffer[4] = (x >> 8) & 0xFF;
        outputBuffer[5] = x & 0xFF;
        outputBuffer[6] = (y >> 8) & 0xFF;
        outputBuffer[7] = y & 0xFF;
        
        memcpy(outputBuffer + 8, FRAME_END, 4);

        outputBuffer[12] = calculateParityByte(outputBuffer, 12);
        
        sendData(outputBuffer, 13);
        
        return checkOkResponse();
    }
    
    bool Display::drawLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x11;	
        
        outputBuffer[3] = Command::DRAW_LINE;
        
        outputBuffer[4] = (x0 >> 8) & 0xFF;
        outputBuffer[5] = x0 & 0xFF;
        outputBuffer[6] = (y0 >> 8) & 0xFF;
        outputBuffer[7] = y0 & 0xFF;
        outputBuffer[8] = (x1 >> 8) & 0xFF;
        outputBuffer[9] = x1 & 0xFF;
        outputBuffer[10] = (y1 >> 8) & 0xFF;
        outputBuffer[11] = y1 & 0xFF;	
        
        memcpy(outputBuffer + 12, FRAME_END, 4);

        outputBuffer[16] = calculateParityByte(outputBuffer, 16);
        
        sendData(outputBuffer, 17);
        
        return checkOkResponse();
    }
    
    bool Display::fillRectangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x11;	
        
        outputBuffer[3] = Command::FILL_RECTANGLE;	
        
        outputBuffer[4] = (x0 >> 8) & 0xFF;
        outputBuffer[5] = x0 & 0xFF;
        outputBuffer[6] = (y0 >> 8) & 0xFF;
        outputBuffer[7] = y0 & 0xFF;
        outputBuffer[8] = (x1 >> 8) & 0xFF;
        outputBuffer[9] = x1 & 0xFF;
        outputBuffer[10] = (y1 >> 8) & 0xFF;
        outputBuffer[11] = y1 & 0xFF;	
        
        
        memcpy(outputBuffer + 12, FRAME_END, 4);	
        outputBuffer[16] = calculateParityByte(outputBuffer, 16);
        
        sendData(outputBuffer, 17);
        return checkOkResponse();
    }
    
    bool Display::drawRectangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x11;	
        
        outputBuffer[3] = Command::DRAW_RECTANGLE;	
        
        outputBuffer[4] = (x0 >> 8) & 0xFF;
        outputBuffer[5] = x0 & 0xFF;
        outputBuffer[6] = (y0 >> 8) & 0xFF;
        outputBuffer[7] = y0 & 0xFF;
        outputBuffer[8] = (x1 >> 8) & 0xFF;
        outputBuffer[9] = x1 & 0xFF;
        outputBuffer[10] = (y1 >> 8) & 0xFF;
        outputBuffer[11] = y1 & 0xFF;	
        
        
        memcpy(outputBuffer + 12, FRAME_END, 4);	
        outputBuffer[16] = calculateParityByte(outputBuffer, 16);
        
        sendData(outputBuffer, 17);
        return checkOkResponse();
    }
    
    bool Display::drawCircle(unsigned int x, unsigned int y, unsigned int radius) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0F;	
        
        outputBuffer[3] = Command::DRAW_CIRCLE;	
        
        outputBuffer[4] = (x >> 8) & 0xFF;
        outputBuffer[5] = x & 0xFF;
        outputBuffer[6] = (y >> 8) & 0xFF;
        outputBuffer[7] = y & 0xFF;
        outputBuffer[8] = (radius >> 8) & 0xFF;
        outputBuffer[9] = radius & 0xFF;
        
        
        memcpy(outputBuffer + 10, FRAME_END, 4);	
        outputBuffer[14] = calculateParityByte(outputBuffer, 14);
        
        sendData(outputBuffer, 15);
        return checkOkResponse();
    }
    
    bool Display::fillCircle(unsigned int x, unsigned int y, unsigned int radius) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x0F;	
        
        outputBuffer[3] = Command::FILL_CIRCLE;	
        
        outputBuffer[4] = (x >> 8) & 0xFF;
        outputBuffer[5] = x & 0xFF;
        outputBuffer[6] = (y >> 8) & 0xFF;
        outputBuffer[7] = y & 0xFF;
        outputBuffer[8] = (radius >> 8) & 0xFF;
        outputBuffer[9] = radius & 0xFF;
        
        
        memcpy(outputBuffer + 10, FRAME_END, 4);	
        outputBuffer[14] = calculateParityByte(outputBuffer, 14);
        
        sendData(outputBuffer, 15);
        return checkOkResponse();
    }
    
    bool Display::drawTriangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x15;	
        
        outputBuffer[3] = Command::DRAW_TRIANGLE;	
        
        outputBuffer[4] = (x0 >> 8) & 0xFF;
        outputBuffer[5] = x0 & 0xFF;
        outputBuffer[6] = (y0 >> 8) & 0xFF;
        outputBuffer[7] = y0 & 0xFF;
        outputBuffer[8] = (x1 >> 8) & 0xFF;
        outputBuffer[9] = x1 & 0xFF;
        outputBuffer[10] = (y1 >> 8) & 0xFF;
        outputBuffer[11] = y1 & 0xFF;	
        outputBuffer[12] = (x2 >> 8) & 0xFF;
        outputBuffer[13] = x2 & 0xFF;
        outputBuffer[14] = (y2 >> 8) & 0xFF;
        outputBuffer[15] = y2 & 0xFF;	
        
        
        memcpy(outputBuffer + 16, FRAME_END, 4);	
        outputBuffer[20] = calculateParityByte(outputBuffer, 20);
        
        sendData(outputBuffer, 21);
        return checkOkResponse();
    }
    
    bool Display::fillTriangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
        flushInputStream();
        outputBuffer[0] = FRAME_HEADER;
	
        outputBuffer[1] = 0x00;
        outputBuffer[2] = 0x15;	
        
        outputBuffer[3] = Command::FILL_TRIANGLE;	
        
        outputBuffer[4] = (x0 >> 8) & 0xFF;
        outputBuffer[5] = x0 & 0xFF;
        outputBuffer[6] = (y0 >> 8) & 0xFF;
        outputBuffer[7] = y0 & 0xFF;
        outputBuffer[8] = (x1 >> 8) & 0xFF;
        outputBuffer[9] = x1 & 0xFF;
        outputBuffer[10] = (y1 >> 8) & 0xFF;
        outputBuffer[11] = y1 & 0xFF;	
        outputBuffer[12] = (x2 >> 8) & 0xFF;
        outputBuffer[13] = x2 & 0xFF;
        outputBuffer[14] = (y2 >> 8) & 0xFF;
        outputBuffer[15] = y2 & 0xFF;	
        
        
        memcpy(outputBuffer + 16, FRAME_END, 4);	
        outputBuffer[20] = calculateParityByte(outputBuffer, 20);
        
        sendData(outputBuffer, 21);
        return checkOkResponse();
    }
    
    bool Display::clearScreen() {
        flushInputStream();
        sendData(CLEAR_SCREEN_PACKET, 10);
        bool clearScreenSuccess = checkOkResponse();
        //NOTE: There appears to be a bug that causes the next command after CLEAR_SCREEN to return "OK". This
        //      causes problems if you try to use a command like GET_DRAWING_COLOR. Calling HANDSHAKE right after
        //      is a hack to fix this.
        bool handshakeSuccess = handshake();
        return clearScreenSuccess && handshakeSuccess;
    }
    
    bool Display::displayText(unsigned int x, unsigned int y, const void *str) {
        int stringSize;
        char * ptr = (char *)str;
        
        stringSize = strlen(ptr) + 1; //Add one for the null character
        int packetSize = stringSize + 13;
        
        outputBuffer[0] = FRAME_HEADER;
        
        outputBuffer[1] = (packetSize >> 8) & 0xFF;
        outputBuffer[2] = packetSize & 0xFF;
        
        outputBuffer[3] = Command::DISPLAY_TEXT;
        
        outputBuffer[4] = (x >> 8) & 0xFF;
        outputBuffer[5] = x & 0xFF;
        outputBuffer[6] = (y >> 8) & 0xFF;
        outputBuffer[7] = y & 0xFF;
        
        strcpy((char *)(outputBuffer + 8), ptr);
                
        memcpy(outputBuffer + (packetSize - 5), FRAME_END, 4);
        outputBuffer[packetSize - 1] = calculateParityByte(outputBuffer, packetSize - 1);
        
        sendData(outputBuffer, packetSize);
        
        return checkOkResponse();
    }
    
    bool Display::displayImage(unsigned int x, unsigned int y, const void *fileName) {
        int stringSize;
        char * ptr = (char *)fileName;
        
        stringSize = strlen(ptr) + 1; //Add one for the null character
        int packetSize = stringSize + 13;
        
        outputBuffer[0] = FRAME_HEADER;
        
        outputBuffer[1] = (packetSize >> 8) & 0xFF;
        outputBuffer[2] = packetSize & 0xFF;
        
        outputBuffer[3] = Command::DISPLAY_IMAGE;
        
        outputBuffer[4] = (x >> 8) & 0xFF;
        outputBuffer[5] = x & 0xFF;
        outputBuffer[6] = (y >> 8) & 0xFF;
        outputBuffer[7] = y & 0xFF;
        
        strcpy((char *)(outputBuffer + 8), ptr);
                
        memcpy(outputBuffer + (packetSize - 5), FRAME_END, 4);
        outputBuffer[packetSize - 1] = calculateParityByte(outputBuffer, packetSize - 1);
        
        sendData(outputBuffer, packetSize);
        
        return checkOkResponse();
    }
    

    /* Private functions */
    
    byte Display::calculateParityByte(const byte *data, int length) {
        byte parityByte = 0x00;
        for (int i = 0; i<length; ++i) {
            parityByte ^= data[i];
        }
        return parityByte;
    }
    
    void Display::sendData(const byte *data, int length) {
        for(int i = 0; i < length; i++)
        {
            serial.write(data[i]);
        }
    }
    
    void Display::flushInputStream() {
        while (serial.available()) {
            serial.read();
        }
    }
    
    
    bool Display::checkOkResponse() {
        delay(WAIT_FOR_RESPONSE_MS); //give the display a short ammount of time to process and respond
        int size = serial.readBytes(inBuffer, 2);
        if (inBuffer[0] == 'O' && inBuffer[1] == 'K')
            return true;
        
        return false;
    }
    
    Color Display::charToColor(char inChar) {
        if (inChar == '3')
            return Color::WHITE;
        else if (inChar == '2')
            return Color::LIGHT_GREY;
        else if (inChar == '1')
            return Color::DARK_GREY;
        else //assume it's 0
            return Color::BLACK;
    }
    
    FontSize Display::charToFontSize(char inChar) {
        if (inChar == '3')
            return FontSize::DOTS_MATRIX_64;
        else if (inChar == '2')
            return FontSize::DOTS_MATRIX_48;
        else //assume it's 1
            return FontSize::DOTS_MATRIX_32;
    }
};