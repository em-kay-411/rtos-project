
void UartInit(unsigned int );	  //setting the baud rate for 115200 baud
int UART_GetChar(void);
int UART_PutChar(unsigned char );

void kbdInit(void);
unsigned char GetKey(void);

void LCD_cmd(unsigned char ch);
void LCD_data(unsigned char ch);
void InitLCD(void);
void LCD_display(int, int, unsigned char *);

void ADCInit(void);
unsigned int ADC_Read(void);

void LEDInit(void);
void LEDset(unsigned char);
void LEDclr(unsigned char);
