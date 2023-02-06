/*
 * Nish Patel
 * Spring 2022
 * Purpose: This project creates the 2048 game. Based on user input via the arrow keys and keyboard, the game either
 moves the board or quits the program. The game also resets if the user wants to play again. The user can also keep
 playing after the 2048 tile has been achieved
 *Assumptions: I assume that the user knows the valid inputs that can be entered. Entering the wrong inputs will
 cause the program to not make the correct move or action. I also assume that the user knows when it is okay to enter
 date or not.
 */
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

/*
 *"score" is what allows for the score of the game to be calcualted. It is played and used to find the maxscore.
 */
int score(int array[4][4]){
  int maxnum = array[0][0];
  int score = 0;
  for(int i = 0;i < 4;i++){
    for(int j = 0;j < 4;j++){
      if(array[i][j] > maxnum){
        maxnum = array[i][j];
      }
    }
  }
  score += maxnum;
  return score;
}

/*
 *"boardmarker" creates the board, inputs random numbers, and puts the score up. It does this via many loops
 *shown below. The random functions are used here
  */
void boardmaker(int array[4][4], int maxscore){
  int boardnum[10]={2,2,2,2,2,2,2,2,2,4};
  int randarray[4]={0,1,2,3};
  int n;
  int m;

  int counter;
  for(int i = 0;i < 4;i++){
    for(int j = 0;j < 4;j++){
      if(array[i][j] == 0){
        counter = 0;
      }
      else{
        counter++;
        i = 5;
        j = 5;
      }
    }
  }

  if(counter < 1){
    do{
      srand((int)time(NULL));
      m = randarray[rand() % 4];
      n = randarray[rand() % 4];
      if(array[m][n] == 0){
        array[m][n] = 2;
        break;
      }
    }while(1);
    do{
      srand((int)time(NULL));
      m = randarray[rand() % 4];
      n = randarray[rand() % 4];
      if(array[m][n] == 0){
        array[m][n] = 2;
        break;
      }
    }while(1);
  }
  else{

    do{
      srand((int)time(NULL));
      m = randarray[rand() % 4];
      n = randarray[rand() % 4];
      if(array[m][n] == 0){
        array[m][n] = boardnum[rand() % 2];
        break;
      }
    }while(1);
}

  int scoreboard = score(array);
  if(maxscore < scoreboard){
    maxscore = scoreboard;
  }
  printf("**Score = %d, High Score = %d**\n", scoreboard, maxscore);
  for(int i = 0;i < 4;i++){
    printf("-------------------------\n");
    for(int j = 0;j < 4;j++){
        if((m == i) && (n == j)){
          printf("|%5d", array[m][n]);
        }
        else{
          if(array[i][j] > 0){
            printf("|%5d", array[i][j]);
          }
          else{
            printf("|     ");
          }
        }
      }
    printf("|\n");
  }
    printf("-------------------------\n");
}

/*
 *The next 4 functions shift the board in whatever direction is called for. They do this via many for loops
 *which cycle through every spot on the array and move the values within them.
  */
void goup(int array[4][4]){
  int s;
  for(int j = 0;j < 4;j++){
    for(int i = 0;i < 3;i++){
      if(array[i][j] > 0){
        if(array[i][j] == array[i+1][j] && array[i][j] != 0){
          array[i][j] = 2 * array[i][j];
          if(i == 2){
            array[i+1][j] = 0;
          }
          else{
            for(s = i + 1;s < 3;s++)
            {
              array[s][j]=array[s + 1][j];
              array[s + 1][j] = 0;
            }
          }
        }
      }
    }
  }
}

void godown(int array[4][4]){
  int s;
  for(int j = 0;j < 4;j++){
    for(int i = 3;i > 0;i--){
      if(array[i][j] > 0){
        if(array[i][j] == array[i-1][j]){
          array[i][j] = 2 * array[i][j];
          if(i == 1){
            array[i-1][j] = 0;
          }
          else{
            for(s = i - 1;s > 0;s--){
              array[s][j] = array[s - 1][j];
              array[s - 1][j] = 0;
            }
          }
        }
      }
    }
  }
}

void goleft(int array[4][4]){
  int s;
  for(int i = 0;i < 4;i++){
    for(int j = 0;j < 3;j++){
      if(array[i][j] > 0){
        if(array[i][j] == array[i][j + 1]){
          array[i][j] = 2 * array[i][j];
          if(j == 2){
            array[i][j + 1] = 0;
          }
          else{
            for(s = j + 1;s < 3;s++){
              array[i][s] = array[i][s + 1];
              array[i][s + 1]=0;
            }
          }
        }
      }
    }
  }
}

void goright(int array[4][4]){
  int s;
  for(int i = 0;i < 4;i++){
    for(int j = 3;j > 0;j--){
      if(array[i][j] > 0){
        if(array[i][j] == array[i][j-1]){
          array[i][j] = 2 * array[i][j];
          if(j == 1){
            array[i][j - 1] = 0;
          }
          else{
            for(s = j - 1;s > 0;s--)
            {
              array[i][s] = array[i][s - 1];
              array[i][s - 1]=0;
            }
          }
        }
      }
    }
  }
}

/*
 *"positions" takes user input and outputs certain code based on the switch. Each case represents a arrow key. This
 *function calls for the 4 functions above
 */
void positionslider(char b, int array[4][4]){

  int x,m;

  switch(b){

    case 65 :
      for(int j = 0;j < 4;j++){
        for(int i = 1;i <= 3;i ++ ){
          for(x = i,m = i; x > 0;x--, m--){

            if(array[m - 1][j] > 0){
              break;
            }
            else{
              array[m - 1][j] = array[m][j];
              array[m][j] = 0;
            }
          }
        }
      }
      goup(array);
      break;

    case 66 :
      for(int j = 0;j < 4;j++){
        for(int i = 2;i >= 0;i--){
          for(x = 3 - i,m = i;x > 0;x--, m++){
            if(array[m + 1][j] > 0){
              break;
            }
            else{
              array[m + 1][j] =array[m][j];
              array[m][j] = 0;
            }
          }
        }
      }
      godown(array);
      break;

    case 68 :
      for(int i = 0;i < 4;i++){
        for(int j = 1;j <= 3;j++){
          for(x = j,m = j;x > 0;x-- ,m--){
            if(array[i][m-1]>0){
              break;
            }
            else{
              array[i][m - 1] = array[i][m];
              array[i][m] = 0;
            }
          }
        }
      }
      goleft(array);
      break;

    case 67 :
      for(int i = 0;i < 4;i++){
        for(int j = 2;j >= 0;j--){
          for(x = 3 - j, m = j;x > 0;x--, m++){
            if(array[i][m + 1] > 0){
              break;
            }
            else{
              array[i][m + 1] = array[i][m];
              array[i][m] = 0;
            }
          }
        }
      }
      goright(array);
      break;

  }
}


/*
*"main" is what asks for user input and calls the other functions needed. With the user input, the program also
*determines whether the user wants to quit or not. Here, the user can chose to keep playing after getting 2048
*or even losing
*/
int main(){

  int array[4][4]={};
  int maxscore = 0;
  char input = 'o';
  int cont = 1;
  while(input != 'q'){
  cont = 1;
  printf("Welcome to 2048\n");
  printf("After, enter use arrow keys to shift the board\n");
  printf("Enter q to quit\n");

  while(cont == 1){
    input = getchar();
    if(input == '\n'){
      input = getchar();
    }
    //printf("%d", input);
    if(input == 65 || input == 66 ||input == 67 ||input == 68 ||input == 'y'){
      positionslider(input, array);
      boardmaker(array, maxscore);

      //positionslider(input);
      for(int i = 0;i < 4;i++){
        for(int j = 0;j < 4;j++){
          if(array[i][j] == 2048){
            printf("You have won by getting the 2048 tile! Enter y to keep playing or q to quit");
            input = getchar();
            if(input == '\n'){
              input = getchar();
            }
            if(input == 'q'){
              i = 5;
              j = 5;
            }
          }
          if(array[i][j]==0){
            cont = 1;
            i = 5;
            j = 5;
          }
          else{
            cont = 0;;
          }
        }
      }
  }
  if(input == 'q'){
    cont = 0;
  }
}

  printf("Game over!\n"); //game over msg
  for(int i = 0;i < 4;i++){ //this resets the board
    for(int j = 0;j < 4;j++){
      array[i][j] = 0;
    }
  }
}
  return 0;
}
