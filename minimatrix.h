#ifndef MINIMATRIX_H
#define MINIMATRIX_H

unsigned char reverse(unsigned char b);

#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

static byte font[30] = {
                        0xF8,0x88,0xF8,  // 0
                        0x48,0xF8,0x08,  // 1
                        0xB8,0xA8,0xE8,  // 2
                        0x88,0xA8,0xF8,  // 3
                        0xE0,0x20,0xF8,  // 4
                        0xE8,0xA8,0xB8,  // 5
                        0x78,0xA8,0xB8,  // 6
                        0x98,0xA0,0xC0,  // 7
                        0xF8,0xA8,0xF8,  // 8
                        0xE8,0xA8,0xF0   // 9
                      };

class MiniMatrix {
  private:
        /* The array for shifting the data to the devices */
        byte spidata[16];
        byte *font;
        /* Send out a single command to the device */
        void spiTransfer(int addr, byte opcode, byte data);

        /* Framebuffer for all 8 devices in this array */
        byte status[32];
        /* Data is shifted out of this pin*/
        int SPI_MOSI;
        /* The clock is signaled on this pin */
        int SPI_CLK;
        /* This one is driven LOW for chip selectzion */
        int SPI_CS;
        /* The maximum number of devices we use */
        int maxDevices;
  public:
    MiniMatrix(int dataPin, int clkPin, int csPin, int numDevices=1);
    void shutdown(int addr, bool b);
    void shutdown(bool b);
    void clearDisplay();
    void printChar(int x, byte ch);
    void setFont(byte *newfont);
    void setDigit(int x, byte digit);
    void showBuffer();
    void setIntensity(int intensity);
    void setLed(int addr, int row, int column, boolean state);
};

#endif

