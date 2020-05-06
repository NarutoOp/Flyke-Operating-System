#include "kernel.h"
#include "utils.h"
#include "char.h"

uint32 vga_index;
uint16 cursor_pos = 0, cursor_next_line_index = 1;
static uint32 next_line_index = 1;
uint8 g_fore_color = WHITE, g_back_color = BLACK;

static int Y_INDEX = 1;
UINT16 MEM_SIZE = 0;


#define CALC_SLEEP 4




uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 
{
  uint16 ax = 0;
  uint8 ah = 0, al = 0;

  ah = back_color;
  ah <<= 4;
  ah |= fore_color;
  ax = ah;
  ax <<= 8;
  al = ch;
  ax |= al;

  return ax;
}

void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
  uint32 i;
  for(i = 0; i < BUFSIZE; i++){
    (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
  }
  next_line_index = 1;
  vga_index = 0;
}

void clear_screen()
{
  clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  cursor_pos = 0;
  cursor_next_line_index = 1;
}

void init_vga(uint8 fore_color, uint8 back_color)
{
  vga_buffer = (uint16*)VGA_ADDRESS;
  clear_vga_buffer(&vga_buffer, fore_color, back_color);
  g_fore_color = fore_color;
  g_back_color = back_color;
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

void move_cursor(uint16 pos)
{
  outb(0x3D4, 14);
  outb(0x3D5, ((pos >> 8) & 0x00FF));
  outb(0x3D4, 15);
  outb(0x3D5, pos & 0x00FF);
}

void move_cursor_next_line()
{
  cursor_pos = 80 * cursor_next_line_index;
  cursor_next_line_index++;
  move_cursor(cursor_pos);
}

void gotoxy(uint16 x, uint16 y)
{
  vga_index = 80*y;
  vga_index += x;
  if(y > 0){
    cursor_pos = 80 * cursor_next_line_index * y;
    cursor_next_line_index++;
    move_cursor(cursor_pos);
  }
}

char get_input_keycode()
{
  char ch = 0;
  while((ch = inb(KEYBOARD_PORT)) != 0){
    if(ch > 0)
      return ch;
  }
  return ch;
}


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

void print_new_line()
{
  if(next_line_index >= 55){
    next_line_index = 0;
    clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  }
  vga_index = 80*next_line_index;
  next_line_index++;
  move_cursor_next_line();
}

void print_char(char ch)
{
  vga_buffer[vga_index] = vga_entry(ch, g_fore_color, g_back_color);
  vga_index++;
  move_cursor(++cursor_pos);
}

void print_string(char *str)
{
  uint32 index = 0;
  while(str[index]){
    if(str[index] == '\n'){
      print_new_line();
      index++;
    }else{
      print_char(str[index]);
      index++;
    }
  }
}

void print_int(int num)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num);
  print_string(str_num);
}

int read_int()
{
  char ch = 0;
  char keycode = 0;
  char data[32];
  int index = 0;
  do{
    keycode = get_input_keycode();
    if(keycode == KEY_ENTER){
      data[index] = '\0';
      print_new_line();
      break;
    }else{
      ch = get_ascii_char(keycode);
      print_char(ch);
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
  keycode = get_input_keycode();
  sleep(CALC_SLEEP);
  return get_ascii_char(keycode);
}

void display_menu()
{
  gotoxy(25, 0);
  print_string("! Flyke Operating System !");
  print_string("\n\n*****  Functionality  *****");
  print_string("\n\n!--- Menu ---!");
  print_string("\n\n1. Addition");
  print_string("\n2. Substraction");
  print_string("\n3. Multiplication");
  print_string("\n4. Division");
  print_string("\n5. Modulus");
  print_string("\n6. Logical AND");
  print_string("\n7. Logical OR");
  print_string("\n8. Exit");
}

void read_two_numbers(int* num1, int *num2)
{
  print_string("Enter first number : ");
  sleep(CALC_SLEEP);
  *num1 = read_int();
  print_string("Enter second number : ");
  sleep(CALC_SLEEP);
  *num2 = read_int();
}


static UINT16 VGA_ColoredEntry(unsigned char to_print, UINT8 color) {
	return (UINT16) to_print | (UINT16)color << 8;
}



UINT8 IN_B(UINT16 port)
{
  UINT8 ret;
  asm volatile("inb %1, %0" :"=a"(ret) :"Nd"(port) );
  return ret;
}

void Clear_VGA_Buffer(UINT16 **buffer)
{
  for(int i=0;i<BUFSIZE;i++){
    (*buffer)[i] = '\0';
  }
  Y_INDEX = 1;
  VGA_INDEX = 0;
}

void InitTerminal()
{
  TERMINAL_BUFFER = (UINT16*) VGA_ADDRESS;
  Clear_VGA_Buffer(&TERMINAL_BUFFER);
}

char getInputCode() {
  char ch = 0;
  do{
    if(IN_B(0x60) != ch) {
      ch = IN_B(0x60);
      if(ch > 0)
        return ch;
    }
  }while(1);
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

void printColoredString(char *str, UINT8 color)
{
  int index = 0;
  while(str[index]){
    TERMINAL_BUFFER[VGA_INDEX] = VGA_ColoredEntry(str[index], color);
    index++;
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

void DisplayIntro()
{
  printColored_WCharN(65,79,RED);
  printNewLine();
  printColored_WCharN(2483,79,BRIGHT_GREEN);
  printN_NewLine(4);
  VGA_INDEX += 30;
  printColoredString("Flyke Operating System", YELLOW);
  printN_NewLine(6);
  VGA_INDEX += 28;
  printColoredString("Kernel in C - 16/32 Bit", WHITE);
  printN_NewLine(6);
  VGA_INDEX += 25;
  printColoredString("! Press any key to move next !", BROWN);
  printN_NewLine(6);
  printColored_WCharN(2483,79, BRIGHT_GREEN);
  printNewLine();
  printColored_WCharN(2481,79, RED);
  getInputCode();
  Clear_VGA_Buffer(&TERMINAL_BUFFER);
  
}





void calculator()
{
  int choice, num1, num2;
  while(1){
    display_menu();
    print_string("\n\nEnter your choice : ");
    choice = read_int();
    switch(choice){
      case 1:
        read_two_numbers(&num1, &num2);
        print_string("Addition : ");
        print_int(num1 + num2);
        break;
      case 2:
        read_two_numbers(&num1, &num2);
        print_string("Substraction : ");
        print_int(num1 - num2);
        break;
      case 3:
        read_two_numbers(&num1, &num2);
        print_string("Multiplication : ");
        print_int(num1 * num2);
        break;
      case 4:
        read_two_numbers(&num1, &num2);
        if(num2 == 0){
          print_string("Error: Divide by 0");
        }else{
          print_string("Division : ");
          print_int(num1 / num2);
        }
        break;
      case 5:
        read_two_numbers(&num1, &num2);
        print_string("Modulus : ");
        print_int(num1 % num2);
        break;
      case 6:
        read_two_numbers(&num1, &num2);
        print_string("LogicalAND : ");
        print_int(num1 & num2);
        break;
      case 7:
        read_two_numbers(&num1, &num2);
        print_string("Logical OR : ");
        print_int(num1 | num2);
        break;
      case 8:
        print_string("\nExiting...");
        sleep(CALC_SLEEP*3);
        clear_screen();
        print_string("Exited...");
        return;
      default:
        print_string("\nInvalid choice...!");
        break;
    }
    print_string("\n\nPress any key to reload screen...");
    getchar();
    clear_screen();
  }
}

void kernel_entry()
{
  
  InitTerminal();
  init_vga(WHITE, BLACK);
  DisplayIntro();

  calculator();
}




