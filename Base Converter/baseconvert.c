/*
 * Nish Patel
 * Spring 2022
 * Purpose: The purpose of this code is for the user to input a numerical value in an Quaternary, Octal, or Hexadecimal base.
 * Based on which base the number is entered in, the program outputs the same value in the unused bases. After the values are
 * displayed, the user can quit the program or enter in another value in a chosen base.
 * Assumptions: I assume that the user knows a basic understanding of how to use and enter numbers in the bases
 * which values can be entered in. If not, the program should be able to catch most errors and allow the user to input
 * new values.
 */
#include <stdio.h>
#include <string.h>

/*
 * "charReader" is responsible for intaking letter values and converting them to numbers if needed. This fuction will run many times
 * but will only really affect inputted hexidecimal values.
 */
int charReader(char letter){
  if(letter >= '0' && letter <= '9'){
    return (int)letter-'0';
  }
  else{
    return (int)letter- 'A' + 10;
  }
}

/*"charReplacer" does the opposite of the function above and actually replaces numbers with letters if needed. Once again, This will really
 * be used with hexidecimal values only.
 */
char charReplacer(int replacedNum){
  if(replacedNum >= 0 && replacedNum <= 9){
    return (char)(replacedNum + '0');
  }
  else{
    return (char)(replacedNum - 10 + 'A');
  }
}

/*"reverse" simply takes a given char and reverses it. It is used for converting the decimal values back to different bases
 * in the function specifically.
 */
void reverse(char *stri){
  for(int i = 0; i < (strlen(stri)/2); i++){
    char holder = stri[i];
    stri[i] = stri[strlen(stri)-1-i];
    stri[strlen(stri)-1-i] = holder;
  }
}

/*"InputtoDecimal" is one of the bigger functions responsible for taking the inputted value from the user along with the base
 * which is found via the first letter in the user input. Using many loops, it returns the decimal value of the inputted value.
 */
int InputtoDecimal(char *UserInput, int base){
  int pow = 1;
  int decimalVal = 0;
  int bitval = 0;
  //used for bitwise operations
  if(base == 4){
    bitval = 2;
  }
  else if(base == 8){
    bitval = 3;
  }
  else if(base == 16){
    bitval = 4;
  }
  //uses bitwise operations to change the base to decimal
  for(int i = strlen(UserInput) - 1; i >= 0; i--){
    decimalVal += charReader(UserInput[i]) * pow;
    pow = pow << bitval;
  }
  //returns decimal value
  return decimalVal;
}

/*"OutputfromDecimal" is responsible for converting decimal numbers to a base inputted in the function. The main references this function
 * several times as there are 3 different bases which the function converts decimal numbers to by the end of the program.
 */
char* OutputfromDecimal(char convert[], int base, int decimalVal){
  int tracker = 0;

  int bitval = 0;
  //used for bitwise operations
  if(base == 4){
    bitval = 2;
  }
  else if(base == 8){
    bitval = 3;
  }
  else if(base == 16){
    bitval = 4;
  }

  //uses bitwise operations to change the base
  while(decimalVal > 0){
    convert[tracker++] = charReplacer(decimalVal % base);
    decimalVal = decimalVal >> bitval;
  }
  convert[tracker] = '\0';
  reverse(convert);
  //returns value in whatever base was inputted
  return convert;
}

/*"main" is responsible for getting user input for the initial value inputted. This function also figures out which base the input was written
 * in and passes it onto the functions which need it. It references the two functions above several times. Finally, it outputs the different user inputted value
 * in the bases not chosen by the user. This function will basically repeat itself until the user chooses to quit due to the while funtion at the top.
*/
int main(){

  char str[32];
  //initial user input
  printf("Enter a number in any of the following bases:\nQuaternary staring with Q\nOctal starting with O\nHexidecimal staring with H\nYou can also enter 'quit' to end the program\n");
  scanf("%s", str);

  while(strcmp("quit", str) != 0){

  int base = 0;
  int inputbase;

  //determines the base inputted
  if(strstr(str, "Q")){
    base = 4;
  }
  if(strstr(str, "O")){
    base = 8;
  }
  if(strstr(str, "H")){
    base = 16;
  }

  inputbase = str[0];
  char *stri = str+1;


  //start of output lines
  char convertedVal[32];
  if(inputbase == 'Q'){
    OutputfromDecimal(convertedVal, 8, InputtoDecimal(stri, base));
    printf("The Inputted Number in Octal is O%s\n", convertedVal);
    OutputfromDecimal(convertedVal, 16, InputtoDecimal(stri, base));
    printf("The Inputted Number in Hexadecimal is H%s\n\n", convertedVal);
  }
  else if(inputbase == 'O'){
    OutputfromDecimal(convertedVal, 4, InputtoDecimal(stri, base));
    printf("The Inputted Number in Quaternary is Q%s\n", convertedVal);
    OutputfromDecimal(convertedVal, 16, InputtoDecimal(stri, base));
    printf("The Inputted Number in Hexadecimal is H%s\n\n", convertedVal);
  }
  else if(inputbase == 'H'){
    OutputfromDecimal(convertedVal, 4, InputtoDecimal(stri, base));
    printf("The Inputted Number in Quaternary is Q%s\n", convertedVal);
    OutputfromDecimal(convertedVal, 8, InputtoDecimal(stri, base));
    printf("The Inputted Number in Octal is O%s\n\n", convertedVal);
  }
  else{
    printf("Invalid Input. Try Again.\n\n");
  }

  printf("Enter a number in any of the following bases:\nQuaternary staring with Q\nOctal starting with O\nHexidecimal staring with H\nYou can also enter 'quit' to end the program\n");
  scanf("%s", str);
  //end of while loop
}

  return 0;
}
