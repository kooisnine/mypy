#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 100000


int OddNum(char* food, int i, int longest){

	int alphabet_num[26] = {0};
	int count = 0;

	for(int k = 0; k < longest ; k++){
		alphabet_num[food[k + i]-65] = (alphabet_num[food[k + i]-65] + 1) % 2;
	}

	for(int h = 0 ; h < 26 ; h++ ){
		count += alphabet_num[h];
	}

	if(count < 2){
		return 1;
	}
	else{
		return 0;
	}
}

int findSavedStrength(char* food) ;

int main() {
    char *food = (char *)malloc(MAX_LENGTH * sizeof(char));

    if (food == NULL) {
        printf("Failed to allocate memory.\n");
        return 1;
    }    

    printf("Enter Food: ");
    scanf("%s", food);
    
    printf("Saved Strength is: %d\n", findSavedStrength(food));
    
    free(food);

    return 0;
}


int findSavedStrength(char* food) {

	int length = strlen(food);
	int longest = length;
	int found = 0;


	while(!found){
		for(int i = 0; i <= length - longest ; i++){
			if(OddNum(food, i, longest)){
				found = longest;
				break;
			}
		}
		longest--;
	}

	return (++longest) * 50;
		
}