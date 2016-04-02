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



typedef struct QaA {
	char question[30];
	char answer[30];
} QaA;


int main(void)
{
	// initialize the LCD
	lcd_init(LCD_DISP_ON);

	// questions to be asked
	QaA questions[] = {
		{"66 / 11", "6"},
		{"77 - 55", "22"},
		{"4*5", "20"},
		{"9+3", "12"},
		{"55/5+5", "16"},
		{"81/3*5", "135"},
		{"7*7*2-5", "93"},
		{"21/3*2*5/10*3", "21"},
		{"9+9+9-27", "0"},
		{"6*5", "30"},
		{"2*88", "176"},
		{"15*15", "225"},
		{"100/2/5/10", "1"},
	};

	int score[2] = {0, 0}; // the scores of player 1 and 2
	int fails[2] = {0, 0};
	const int WINNING_SCORE = 5; // how many points are needed to win the game
	const int WRONG_ANSWER_THRESHOLD = 3;
	const char NUMBERS[] = "0123456789"; // the numbers, answers can be composed from
	const char TITLE[] = "Math Madness"; // the game's title
	int activePlayer = 0;
	int currentQuestion = 0;
	


    
	//lcd_puts("Hello World");

	DDRA = 0xFF;
	PORTA = 0xFF;

    while (1) 
    {
    }


	void printToLCD(char content[]){
		lcd_puts(content);
	}

	// resets the score
	void wipeScore(){
		score[0] = 0;
		score[1] = 0;
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

	// announces the winner of the game
	void announceWinner(int winner){
	char winnerStr[10];
	if (winner == 0) {
		strcpy(winnerStr, "Player 1");
	} else {
		strcpy(winnerStr, "Player 2");
	}
	printToLCD(strcat(winnerStr, " wins!"));

	}

	// announces the loser
	void announceLooser(int looser){
		char winnerStr[10];
		if (looser == 0) {
			strcpy(winnerStr, "Player 1");
		} else {
			strcpy(winnerStr, "Player 2");
		}
		printToLCD(strcat(winnerStr, " looses!"));
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

	void showQuestion(){
		printToLCD(questions[currentQuestion].question);
	}

	void checkAnswer(){

	}



	// initializes a new game state
	void newGame(){
		currentQuestion = 0;
		wipeScore();
		choosePlayer();
		showQuestion();
	}
}




