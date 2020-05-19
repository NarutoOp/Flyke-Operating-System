#include"kernel.h"
#include "utils.h"
#include "char.h"
#define CALC_SLEEP 4

uint16 cursor_pos = 0, cursor_next_line_index = 1;
static int Y_INDEX = 1;
UINT16 MEM_SIZE = 0;


static UINT16 VGA_DefaultEntry(unsigned char to_print) {
	return (UINT16) to_print | (UINT16)VGA_COLOR_WHITE << 8;
}

static UINT16 VGA_ColoredEntry(unsigned char to_print, UINT8 color) {
	return (UINT16) to_print | (UINT16)color << 8;
}


void Clear_VGA_Buffer(UINT16 **buffer)
{
  for(int i=0;i<BUFSIZE;i++){
    (*buffer)[i] = '\0';
  }
  Y_INDEX = 1;
  VGA_INDEX = 0;
}


void clear_screen()
{
  Clear_VGA_Buffer(&TERMINAL_BUFFER);
  cursor_pos = 0;
  cursor_next_line_index = 1;
}

void InitTerminal()
{
  TERMINAL_BUFFER = (UINT16*) VGA_ADDRESS;
  Clear_VGA_Buffer(&TERMINAL_BUFFER);
}




void strcat(char *str_1, char *str_2)
{
  int index_1 = strlen(str_1);
  int index_2 = 0;
  while(str_2[index_2]){
    str_1[index_1] = str_2[index_2];
    index_1++;
    index_2++;
  }
  str_1[index_1] = '\0';
}


void memcpy(char *str_dest, char *str_src)
{
  int index = 0;
  while(str_src[index]){
    str_dest[index] = str_src[index];
    index++;
  }
  str_dest[index] = '\0';
}


int digitCount(int num)
{
  int count = 0;
  if(num == 0)
    return 1;
  while(num > 0){
    count++;
    num = num/10;
  }
  return count;
}



uint8 inb(uint16 port)
{
  uint8 data;
  asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
  return data;
}

void outb(uint16 port, uint8 data)
{
  asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}


char getInputCode() {
  char ch = 0;
  while((ch = inb(KEYBOARD_PORT)) != 0){
    if(ch > 0)
      return ch;
  }
  return ch;
}


void printNewLine()
{
  if(Y_INDEX >= 55){
    Y_INDEX = 0;
    Clear_VGA_Buffer(&TERMINAL_BUFFER);
  }
  VGA_INDEX = 80*Y_INDEX;
  Y_INDEX++;
}

void printN_NewLine(int n)
{
  for(int i=0;i<n;i++)
    printNewLine();
}


void printString(char *str)
{
  int index = 0;
  while(str[index]){
    TERMINAL_BUFFER[VGA_INDEX] = VGA_DefaultEntry(str[index]);
    index++;
    VGA_INDEX++;
  }
}

void printInt(int num)
{
  char str_num[digitCount(num)+1];
  itoa(num, str_num);
  printString(str_num);
}

void printColoredString(char *str, UINT8 color)
{
  int index = 0;
  while(str[index]){
    TERMINAL_BUFFER[VGA_INDEX] = VGA_ColoredEntry(str[index], color);
    index++;
    VGA_INDEX++;
  }
}


void printCharN(char ch, int n)
{
  int i = 0;
  while(i <= n){
    TERMINAL_BUFFER[VGA_INDEX] = VGA_DefaultEntry(ch);
    i++;
    VGA_INDEX++;
  }
}

void printChar(char ch)
{
    TERMINAL_BUFFER[VGA_INDEX] = VGA_DefaultEntry(ch);
    VGA_INDEX++;
}

void print_int(int num)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num);
  printString(str_num);
}

void printColoredCharN(char ch, int n, UINT8 color)
{
  int i = 0;
  while(i <= n){
    TERMINAL_BUFFER[VGA_INDEX] = VGA_ColoredEntry(ch, color);
    i++;
    VGA_INDEX++;
  }
}


void printColored_WCharN(UINT16 ch, int n, UINT8 color)
{
  int i = 0;
  while(i <= n){
    TERMINAL_BUFFER[VGA_INDEX] = VGA_ColoredEntry(ch, color);
    i++;
    VGA_INDEX++;
  }
}



//Operational part

void wait_for_io(uint32 timer_count)
{
  while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
    }
}

void sleep(uint32 timer_count)
{
  wait_for_io(timer_count*0x02FFFFFF);
}

int read_int()
{
  char ch = 0;
  char keycode = 0;
  char data[32];
  int index = 0;
  do{
    keycode = getInputCode();
    if(keycode == KEY_ENTER){
      data[index] = '\0';
      printNewLine();
      break;
    }else{
      ch = get_ascii_char(keycode);
      printChar(ch);
      data[index] = ch;
      index++;
    }
    sleep(CALC_SLEEP);
  }while(ch > 0);

  return atoi(data);
}

char getchar()
{
  char keycode = 0;
  sleep(CALC_SLEEP);
  keycode = getInputCode();
  sleep(CALC_SLEEP);
  return get_ascii_char(keycode);
}




void DisplayIntro()
{
  printColored_WCharN(2481,79,VGA_COLOR_RED);
  printNewLine();
  printColored_WCharN(2483,79,VGA_COLOR_LIGHT_GREEN);
  printN_NewLine(4);
  VGA_INDEX += 30;
  printColoredString("Flyke Operating System", VGA_COLOR_YELLOW);
  printN_NewLine(6);
  VGA_INDEX += 28;
  printColoredString("Kernel in C - 16/32 Bit", VGA_COLOR_WHITE);
  printN_NewLine(6);
  VGA_INDEX += 25;
  printColoredString("! Press any key to move next !", VGA_COLOR_BROWN);
  printN_NewLine(6);
  printColored_WCharN(2483,79, VGA_COLOR_LIGHT_GREEN);
  printNewLine();
  printColored_WCharN(2481,79, VGA_COLOR_RED);
  getInputCode();
  getInputCode();
  Clear_VGA_Buffer(&TERMINAL_BUFFER);
  
}

void display_menu()
{
  VGA_INDEX += 25;
  printString("! Flyke Operating System !");
  printN_NewLine(2);
  printString("*****  Basic Operations  *****");
  printN_NewLine(2);
  printString("!--- Menu ---!");
  printN_NewLine(2);
  printString("1. Addition");
  printNewLine();
  printString("2. Substraction");
  printNewLine();
  printString("3. Multiplication");
  printNewLine();
  printString("4. Division");
  printNewLine();
  printString("5. Modulus");
  printNewLine();
  printString("6. Logical AND");
  printNewLine();
  printString("7. Logical OR");
  printNewLine();
  printString("8. Exit");
}

void read_two_numbers(int* num1, int *num2)
{
  printString("Enter first number : ");
  sleep(CALC_SLEEP);
  *num1 = read_int();
  printString("Enter second number : ");
  sleep(CALC_SLEEP);
  *num2 = read_int();
}


void Operation()
{
  int choice, num1, num2;
  while(1){
    display_menu();
    printN_NewLine(2);
    printString("Enter your choice : ");
    choice = read_int();
    switch(choice){
      case 1:
        read_two_numbers(&num1, &num2);
        printString("Addition : ");
        print_int(num1 + num2);
        break;
      case 2:
        read_two_numbers(&num1, &num2);
        printString("Substraction : ");
        print_int(num1 - num2);
        break;
      case 3:
        read_two_numbers(&num1, &num2);
        printString("Multiplication : ");
        print_int(num1 * num2);
        break;
      case 4:
        read_two_numbers(&num1, &num2);
        if(num2 == 0){
          printString("Error: Divide by 0");
        }else{
          printString("Division : ");
          print_int(num1 / num2);
        }
        break;
      case 5:
        read_two_numbers(&num1, &num2);
        printString("Modulus : ");
        print_int(num1 % num2);
        break;
      case 6:
        read_two_numbers(&num1, &num2);
        printString("LogicalAND : ");
        print_int(num1 & num2);
        break;
      case 7:
        read_two_numbers(&num1, &num2);
        printString("Logical OR : ");
        print_int(num1 | num2);
        break;
      case 8:
	printNewLine();
        printString("Exiting...");
        sleep(CALC_SLEEP*3);
        clear_screen();
	KERNEL_MAIN();
        printString("Exited...");
        return;
      default:
	printNewLine();
        printString("Invalid choice...!");
        break;
    }
    printN_NewLine(2);
    printString("Press any key to reload screen...");
    getchar();
    clear_screen();
  }
}


void KERNEL_MAIN()
{
  InitTerminal();
  sleep(CALC_SLEEP);
  clear_screen();
  DisplayIntro();
  Operation();

}

