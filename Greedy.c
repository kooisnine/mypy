#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 100000

int findSavedStrength(char* food);


int main() {
	char *food = (char *)malloc(MAX_LENGTH * sizeof(char));
	FILE *fp = fopen("./hard_testcase2_input.txt","r");
	
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


int findSavedStrength(char* food){

	int length_food = strlen(food);	//문자열 길이 구하기. 입력받은 알파벳 개수 
	int length = length_food ;	//비교 크기 
	char alphabet[26] = {0}; 
	int found = 0;	// 먹이가 되는 string의 길이 
	int odd_num = 0;	// 홀수의 숫자 
	int k = 0; // 홀짝 개수 구하기 위한 index 
	while(length >= 2 && found == 0){	// 1보다 큰 개수만큼 묶어서 계산 
		for(int i = 0 ; i <= length_food - length ; i++){ 
			for(int j = i ; j < i + length ; j++){ 
				alphabet[food[j]-65] ++; 
			} 
			while(odd_num <= 1){
				if(alphabet[k++] %2 != 0){
					odd_num ++;
				};
				if(k == 26){
					found = 1;		//찾음!
					break;
				};
			};			
			odd_num = 0;
			k=0;
			if(found == 1){
				break;
			}
			for(int p = 0; p < 26; p++){
				alphabet[p] = 0;
			};
		};
		length--; 
};

	if(length == 1 ){
		return 50;
	}	;

	int savedStrength = 50 * (++length);	//비축하는 힘 계산
	
	return savedStrength;

}

