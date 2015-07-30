#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define GUESSNUM 10
#define WORDSIZE 20
#define ALPHASIZE 26
#define NUMSCORES 5

void chooseword(const char* wordsfile, char* word) {
	char 	c;
	int 	i;
	int 	random;
	int		count = 0;
	FILE* 	words = fopen(wordsfile, "r");

	// Count the number of words in the file
	while ((c = fgetc(words)) != EOF) {
		if (c == '\n') {
			count++;
		}
	}

	// Pick a word randomly
	srand(time(NULL));
	random = rand() % count;
	count = i = 0;
	rewind(words);

	// Load the chosen word into an array
	while ((c = fgetc(words)) != EOF) {
		if (count == random) {
			word[i++] = c;
		}

		if (c == '\n') {
			count++;
		}
	}
	word[i--] = '\0';
}

int dispcheck(char* word, char* usedletters) {
	int wordlen = strlen(word);
	int usedletterslen = strlen(usedletters);
	int count = 0;

	// No guesses have been made yet
	if (usedletterslen == 0) {
		for (int x = 0; x < wordlen - 1; x++) {
			printf("_");
		}
		count = wordlen - 1;
	} else {
		// Loop through the word
		for (int x = 0; x < wordlen - 1; x++) {
			// Check whether any of the used letters match
			for (int y = 0; y < usedletterslen; y++) {
				if (word[x] == usedletters[y]) {
					printf("%c", word[x]);
					break;
				} else if (y == usedletterslen - 1) {
					printf("_");
					count++; // Count the missing letters
				}
			}
		}
	}
	printf("\n");
	return count;
}

bool isused(char guess, char* usedletters) {
	int usedletterslen = strlen(usedletters);

	// Loop through the string of used letters
	for (int x = 0; x < usedletterslen; x++) {
		// If the guess has been used return true
		if (usedletters[x] == guess) {
			return true;
		}
	}
	// The guess has not been used, return false
	return false;
}

void addchar(char add, char* str) {
	*str = add;
	*(str + 1) = '\0';
}

void highscore(const char* scoresfile, char* name, int score) {
	char 	c;
	bool 	swapped;
	int 	tmpnum;
	int 	state = 0;
	int 	recordnum = 0;
	int 	i = 0;
	int 	scores[NUMSCORES];
	char 	scoretemp[NUMSCORES];
	char* 	names[NUMSCORES];
	char*	tmpptr;
	FILE* 	fp;

	// Append the most recent record to the file
	fp = fopen(scoresfile, "a");
	fprintf(fp, "%s,%d\n", name, score);
	fclose(fp);

	names[0] = (char*)malloc(WORDSIZE);

	// Load all of the records from the file
	fp = fopen(scoresfile, "r");
	while ((c = fgetc(fp)) != EOF) {
		if (state == 0) {
			if (c == ',') {
				i = 0;
				state = 1;
			} else if (c != '\n') {
				if (i == 0) {
					names[recordnum] = (char*)malloc(WORDSIZE);
				}
				addchar(c, &names[recordnum][i++]);
			}
		} else if (state == 1) {
			if (c == '\n') {
				scores[recordnum++] = atoi(scoretemp);
				i = 0;
				state = 0;
			} else {
				addchar(c, &scoretemp[i++]);
			}
		}
	}
	fclose(fp);

	// Sort the records using bubble
	do {
		swapped = false;
		for (int x = 1; x < recordnum; x++) {
			if (scores[x - 1] < scores[x]) {
				// Swap the scores
				tmpnum = scores[x];
				scores[x] = scores[x - 1];
				scores[x - 1] = tmpnum;

				// Swap the corresponding names
				tmpptr = names[x];
				names[x] = names[x - 1];
				names[x - 1] = tmpptr;

				// Further sorting is necessary
				swapped = true;
			}
		}
	} while (swapped);

	// Overwrite the old file with the new top 3 and display it
	fp = fopen(scoresfile, "w");
	for (int x = 0; x < recordnum; x++) {
		fprintf(fp, "%s,%d\n", names[x], scores[x]);
		printf("%s, %d\n", names[x], scores[x]);
	}
	fclose(fp);
}

int main() {
	char 	guess;
	char 	name[WORDSIZE];
	char 	word[WORDSIZE] = "";
	char 	usedletters[ALPHASIZE] = "";
	bool 	flag = false;
	int 	lettercount = 0;
	int 	wrongcount = 0;
	int 	userscore;
	int 	remainingcurr;
	int 	remainingprev;

	// Enter player name
	printf("Enter your name: ");
	scanf(" %s", name);

	// Choose a word
	chooseword("words.txt", word);
	remainingprev = dispcheck(word, usedletters);
	printf("%d", remainingprev);

	do {
		printf("\nGuess a letter: ");
		scanf(" %c", &guess);

		// Do not allow repeat guesses
		if (isused(guess, usedletters)) {
			printf("You already guessed that letter!\n");
		} else {
			addchar(guess, &usedletters[lettercount++]);
		}

		// Count wrong guesses
		remainingcurr = dispcheck(word, usedletters);
		if (remainingprev == remainingcurr) {
			printf("Incorrect!\n");
			wrongcount++;
		}
		remainingprev = remainingcurr;
		printf("Used Letters: %s\n", usedletters);
		printf("Remaining Guesses: %d\n", GUESSNUM - wrongcount);

		// Check when the game should finish
		if (remainingcurr == 0) {
			printf("\nCongratulations! You win.\n");
			flag = true;
		} else if (wrongcount == GUESSNUM) {
			printf("\nOh no! You've been hung.\n");
			flag = true;
		}
	} while (!flag);

	// Update the high scores
	printf("\nHigh scores:\n");
	userscore = (strlen(word) * 5) / wrongcount;
	highscore("scores.txt", name, userscore);
}