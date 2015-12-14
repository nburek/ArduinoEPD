/**
 *  Known Bugs:
 *      1. After using the REFRESH command the HANDSHAKE command will return "OK" immediately, but other Get 
 *          commands (such as GET_BAUD_RATE) will not work until the panel finishes refreshing. 
 *      2. In testing on othe Arduino Yun, communicating with the panel at the default 115200 Baud Rate yields
 *          a lot of noise on signal line coming out of the panel. Decreasing the Baud Rate to 57600 solved this
 *          problem in my testing.
 *      3. The command GET_STORAGE_AREA is returning "OK" instead of a '0' or '1' like expected
 *
 */
#ifndef EPD_h
#define EPD_h

#include "Arduino.h"

namespace EPD {

    enum Color : byte {
        BLACK       = 0x00,
        DARK_GREY   = 0x01,
        LIGHT_GREY  = 0x02,
        WHITE       = 0x03
    };
    
    enum StorageArea : byte {
        NAND_FLASH  = 0x00,
        MICRO_SD    = 0x01
    };
    
    enum DisplayDirection : byte {
        NORMAL      = 0x00,
        INVERTED    = 0x01
    };
    
    enum FontSize : byte {
        DOTS_MATRIX_32  = 0x01,
        DOTS_MATRIX_48  = 0x02,
        DOTS_MATRIX_64  = 0x03
    };
    
    enum Command : byte {
        /* System Control Commands */
        HANDSHAKE               = 0x00,
        SET_BAUD_RATE           = 0x01,
        GET_BAUD_RATE           = 0x02,
        GET_STORAGE_AREA        = 0x06,
        SET_STORAGE_AREA        = 0x07,
        ENTER_SLEEP             = 0x08,
        REFRESH                 = 0x0A,
        GET_DISP_DIRECTION      = 0x0C,
        SET_DISP_DIRECTION      = 0x0D,
        IMPORT_FONT_LIBRARY     = 0x0E,
        IMPORT_IMAGE            = 0x0F,
        
        /* Display Parameter Configuration Commands */
        SET_DRAWING_COLOR       = 0x10,
        GET_DRAWING_COLOR       = 0x11,
        GET_ENGLISH_FONT_SIZE   = 0x1C,
        GET_CHINESE_FONT_SIZE   = 0x1D,
        SET_ENGLISH_FONT_SIZE   = 0x1E,
        SET_CHINESE_FONT_SIZE   = 0x1F,
        
        /* Basic Drawing Commands */
        DRAW_POINT              = 0x20,
        DRAW_LINE               = 0x22,
        FILL_RECTANGLE          = 0x24,
        DRAW_RECTANGLE          = 0x25,
        DRAW_CIRCLE             = 0x26,
        FILL_CIRCLE             = 0x27,
        DRAW_TRIANGLE           = 0x28,
        FILL_TRIANGLE           = 0x29,
        CLEAR_SCREEN            = 0x2E,
        DISPLAY_TEXT            = 0x30,
        DISPLAY_IMAGE           = 0x70
    };

    class Display {
        
        public:
            Display(HardwareSerial &s, int wakeUpPin, int resetPin);
            
            /* Hardware Control Functions */
            void reset();
            void wakeUp();
            
            /* System Control Functions */
            bool handshake();
            bool setBaudRate(long baudRate);
            long getBaudRate();
            StorageArea getStorageArea();
            bool setStorageArea(StorageArea storageArea);
            void enterSleep();
            bool refresh();
            DisplayDirection getDisplayDirection();
            bool setDisplayDirection(DisplayDirection displayDirection);
            bool importFontLibrary();
            bool importImage();
            
            /* Display Parameter Configuration Functions */
            bool setDrawingColor(Color color, Color backgroundColor);
            Color getDrawingColor();
            Color getBackgroundColor();
            FontSize getEnglishFontSize();
            FontSize getChineseFontSize();
            bool setEnglishFontSize(FontSize fontSize);
            bool setChineseFontSize(FontSize fontSize);
            
            /* Basic Drawing Functions */
            bool drawPoint(unsigned int x, unsigned int y);
            bool drawLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);
            bool fillRectangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);
            bool drawRectangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);
            bool drawCircle(unsigned int x, unsigned int y, unsigned int radius);
            bool fillCircle(unsigned int x, unsigned int y, unsigned int radius);
            bool drawTriangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
            bool fillTriangle(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
            bool clearScreen();
            
            bool displayText(unsigned int x, unsigned int y, const void *str);
            
            bool displayImage(unsigned int x, unsigned int y, const void *fileName);
            
            
        private:
            static const short WAIT_FOR_RESPONSE_MS = 20;
            static const byte FRAME_HEADER = 0xA5;
            static const byte FRAME_END[4];
            
            /* System Control Command Packets */
            static const byte HANDSHAKE_PACKET[9];
            static const byte GET_BAUD_RATE_PACKET[9];
            static const byte GET_STORAGE_AREA_PACKET[9];
            static const byte ENTER_SLEEP_PACKET[9];
            static const byte REFRESH_PACKET[9];
            static const byte GET_DISP_DIRECTION_PACKET[9];
            static const byte IMPORT_FONT_LIBRARY_PACKET[9];
            static const byte IMPORT_IMAGE_PACKET[9];
            static const byte CLEAR_SCREEN_PACKET[9];
            static const byte GET_DRAWING_COLOR_PACKET[9];
            static const byte GET_ENGLISH_FONT_SIZE_PACKET[9];
            static const byte GET_CHINESE_FONT_SIZE_PACKET[9];
            
            HardwareSerial &serial;
            int wakeUpPin;
            int resetPin;
            byte outputBuffer[1033];
            byte inBuffer[256];
            byte calculateParityByte(const byte *data, int length);
            void sendData(const byte *data, int length);
            void flushInputStream();
            bool checkOkResponse();
            Color charToColor(char inByte);
            FontSize charToFontSize(char inByte);
    };

};

#endif