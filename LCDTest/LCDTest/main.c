/*
 * LCDTest.c
 *
 * Created: 3/29/2016 12:39:01 AM
 * Author : speku
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <lcd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define LEFT_BUTTON  PINB0
#define RIGHT_BUTTON  PINB1
#define SELECT_BUTTON  PINB2
#define CONFIRM_BUTTON  PINB3
#define BUTTON_PIN PINC

typedef struct QaA {
	char question[30];
	char answer[30];
} QaA;


int main(void)
{
	DDRC = 0x00;
	DDRB = 0x00; // portb handles button inputs - porta is set to output by lcd library
	PORTB = 0xFF; // enable pullup for port b
	// initialize the LCD
	lcd_init(LCD_DISP_ON_CURSOR);

	// questions to be asked
	QaA questions[] = {
		{"66/11 = ", "6"},
		{"77-55 = ", "22"},
		{"4*5 = ", "20"},
		{"9+3 = ", "12"},
		{"55/5+5 = ", "16"},
		{"81/3*5 = ", "135"},
		{"7*7*2-5 =" , "93"},
		{"21/3*2*5/10*3 = ", "21"},
		{"9+9+9-27 = ", "0"},
		{"6*5 = ", "30"},
		{"2*88 = ", "176"},
		{"15*15 = ", "225"},
		{"100/2/5/10 = ", "1"},
	};

	int score[2] = {0, 0}; // the scores of player 1 and 2
	int fails[2] = {0, 0};
	const int WINNING_SCORE = 5; // how many points are needed to win the game
	const int WRONG_ANSWER_THRESHOLD = 3;
	const char NUMBERS[] = "0123456789"; // the numbers, answers can be composed from
	const char TITLE[] = "Math Madness"; // the game's title
	int activePlayer = 0;
	int currentQuestion = 0;
	const int cursorMinX = 0;
	const int cursorMaxX = 9;
	const int cursorY = 0;
	const char digits[] = "0123456789";
	
		void printToLCD(char content[]){
			lcd_puts(content);
		}

			// determines which player has reached the winning threshold
			int overPositiveThreshold(){
				if (score[0] >= WINNING_SCORE) {
					return 0;
					} else if (score[1] >= WINNING_SCORE) {
					return 1;
					} else {
					return -1;
				}
			}

		// determines which player has fallen below the negative threshold
		int underNegativeThreshold(){
			if (fails[0] >= WRONG_ANSWER_THRESHOLD) {
				return 0;
				} else if (fails[1] >= WRONG_ANSWER_THRESHOLD) {
				return 1;
				} else {
				return -1;
			}
		}

		// determines the player with the highest score
		int highestPlayer(){
			if (score[0] > score[1]) {
				return 0;
				} else if (score[0] == score[1]) {
				return -1;
				} else {
				return 1;
			}
		}

	// announces the winner of the game
	void announceWinner(int winner){
		lcd_clrscr();
		if (winner == 0) {
			lcd_puts("Player 1 wins");
		} else {
			lcd_puts("Player 2 wins");
		}
		_delay_ms(2000);
	}

	// announces the loser
	void announceLooser(int looser){
		lcd_clrscr();
		if (looser == 0) {
			lcd_puts("Player 2 wins");
			} else {
			lcd_puts("Player 1 wins");
		}
		_delay_ms(2000);
	}


	// resets the score
	void wipeScoreAndQuestionIndex(){
		score[0] = 0;
		score[1] = 0;
		fails[0] = 0;
		fails[1] = 0;
		currentQuestion = 0;
	}

	// determines the next player
	void nextPlayer(){
		activePlayer = (activePlayer + 1) % 2;
	}

	// randomly chooses a player to start the game
	void choosePlayer(){
		activePlayer = rand() % 2;
	}

	void nextQuestion(){
		currentQuestion++;
	}

	int isGameOver(){
		// check for negative looser
		int underNegativeRes = underNegativeThreshold();
		if (underNegativeRes >= 0) {
			announceLooser(underNegativeRes);
			return 1;
		}

		// check for positive winner
		int overPositiveRes = overPositiveThreshold();
		if (overPositiveRes >= 0) {
			announceWinner(overPositiveRes);
			return 1;
		}

		// check for depleted questions
		if (currentQuestion >= (sizeof(questions) / sizeof(questions[0]))) {
			announceWinner(highestPlayer());
			return 1;
		}

		// the game is not over yet
		return 0;
	}



	// announces the currently active player
	void announceActivePlayer(){
		lcd_clrscr();
		printToLCD(activePlayer == 0 ? "Player 1" : "Player 2");
		_delay_ms(3000);
	}



	void waitForInput(){
		int cursorPosition = 0;
		char input[5] = "";
		int inputCounter = 0;

		int leftWasDown = 0, leftUpAgain = 0;
		int rightWasDown = 0, rightUpAgain = 0;
		int selectWasDown = 0, selectUpAgain = 0;
		int confirmWasDown = 0, confirmUpAgain = 0;

		void isMoveLeft(){
			if (leftWasDown && leftUpAgain){
				cursorPosition = (cursorPosition - 1) >= cursorMinX ? (cursorPosition - 1) : 0;
				leftWasDown = 0;
				leftUpAgain = 0;
				lcd_gotoxy(cursorPosition, cursorY);
			} else if (!leftWasDown && !leftUpAgain) {
				leftWasDown = bit_is_clear(BUTTON_PIN, LEFT_BUTTON);
			} else if (leftWasDown && !leftUpAgain) {
				leftUpAgain = !bit_is_clear(BUTTON_PIN, LEFT_BUTTON);
			}
		}

		void isMoveRight(){
			if (rightWasDown && rightUpAgain){
				cursorPosition = (cursorPosition + 1) <= cursorMaxX ? (cursorPosition + 1) : cursorMaxX;
				rightWasDown = 0;
				rightUpAgain = 0;
				lcd_gotoxy(cursorPosition, cursorY);
				} else if (!rightWasDown && !rightUpAgain) {
					rightWasDown = bit_is_clear(BUTTON_PIN, RIGHT_BUTTON);
				} else if (rightWasDown && !rightUpAgain) {
					rightUpAgain = !bit_is_clear(BUTTON_PIN, RIGHT_BUTTON);
				}
		}

		void isSelect(){
			if (selectWasDown && selectUpAgain){
				input[inputCounter++ % 5] = digits[cursorPosition];
				lcd_gotoxy(strlen(questions[currentQuestion].question), 1);
				lcd_puts(input);
				lcd_gotoxy(cursorPosition, cursorY);
				selectWasDown = 0;
				selectUpAgain = 0;
				} else if (!selectWasDown && !selectUpAgain) {
					selectWasDown = bit_is_clear(BUTTON_PIN, SELECT_BUTTON);
				} else if (selectWasDown && !selectUpAgain) {
					selectUpAgain = !bit_is_clear(BUTTON_PIN, SELECT_BUTTON);
				}
		}

		int isConfirm(){
			if (confirmWasDown && confirmUpAgain){
				confirmWasDown = 0;
				confirmUpAgain = 0;
				return 1;
				} else if (!confirmWasDown && !confirmUpAgain) {
					confirmWasDown = bit_is_clear(BUTTON_PIN, CONFIRM_BUTTON);
				return 0;
				} else if (confirmWasDown && !confirmUpAgain) {
					confirmUpAgain = !bit_is_clear(BUTTON_PIN, CONFIRM_BUTTON);
				return 0;
				}
		}


		void checkAnswer(){
			if (strcmp(questions[currentQuestion].answer, input) == 0) {
				score[activePlayer]++;
				char info[1];
				sprintf(info, "%d", WINNING_SCORE - score[activePlayer]);
				lcd_clrscr();
				lcd_puts("Jolly Good!");
				lcd_gotoxy(0,1);
				lcd_puts("to go: ");
				lcd_puts(info);
			} else {
				fails[activePlayer]++;
				char info[1];
				sprintf(info, "%d", WRONG_ANSWER_THRESHOLD - fails[activePlayer]);
				lcd_clrscr();
				lcd_puts("Wrong Answer!");
				lcd_gotoxy(0,1);
				lcd_puts("lives left: ");
				lcd_puts(info);
			}
			_delay_ms(2000);
		}

		lcd_gotoxy(0,0);
		
		while(1){
			isMoveLeft();
			isMoveRight();
			isSelect();
			if (isConfirm()){
				checkAnswer();
				break;
			}
		}
	}

	void showQuestion(){
		lcd_clrscr();
		lcd_puts("0123456789");
		lcd_gotoxy(0,1);
		printToLCD(questions[currentQuestion].question);
	}


	// initializes a new game state
	void newGame(){
		lcd_clrscr();
		printToLCD(TITLE);
		_delay_ms(3000);
		wipeScoreAndQuestionIndex();
		choosePlayer();
	}


	//
	DDRC = 0x00;
	PORTC = 0xFF;

	newGame();
	// main loop
    while (1) 
    {
		announceActivePlayer();
		showQuestion();
		waitForInput();
		if (isGameOver()){
			newGame();
			continue;
			} else {
			nextPlayer();
			nextQuestion();
		}
    }


	
}




