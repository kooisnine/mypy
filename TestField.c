#include <limits.h>
#include <stdlib.h>
#include <stdio.h>								//지워야 한다.
#include <string.h>								//지워야 한다.

typedef unsigned short hpfp;

hpfp int_converter(int input);

int hpfp_to_int_converter(hpfp input);

hpfp float_converter(float input);

float hpfp_to_float_converter(hpfp input);

hpfp addition_function(hpfp a, hpfp b);

hpfp multiply_function(hpfp a, hpfp b);

char* comparison_function(hpfp a, hpfp b);

char* hpfp_to_bits_converter(hpfp result);

char* hpfp_flipper(char* input);

char* hpfp_to_bits_converter_int(hpfp input);


void FloatToBit(float input, unsigned char *bits) {
    unsigned char *pointer = (unsigned char *) & input;
		int k = 0;
    for (int i = 0; i < sizeof(float) ; i++) {
        for (int j = 0; j < 8 ; j++) {
            bits[k++] =(pointer[i] >> j) & 1; // 해당 비트를 2진수로 출력
        }
    }
} //비트를 거꾸로 저장함. 뒤에서부터 읽어야 해



hpfp int_converter(int input){	//exp는 -14~15까지임.
	
	int input_test = input;
    hpfp result = 0;
	
	if( input > 65504 ){
		result |= 0x7c00;	        
		return result;  //hpfp 범위 초과 -> Infinite
	}
	if( input < -65504){
		result |= 0xfc00;
		return result;  //hpfp 범위 초과 -> -Infinite
	}
	if(input == 0){
		result &= 0x0000;
		return result;
	}	//0일 땐 모든 비트를 0으로
		
    if(input > 0){
		int expo_cal = 1;
		int expo = 0;
		for(int i = 0; i < 16 ; i++){
			if (input_test / (expo_cal * 1) == 1){
				expo = i ;
				break;
			}
			expo_cal *= 2;
		}
		int exponent = expo; //exponent는 곱해지는 2의 지수
		expo += 15;
		int binary = 16;
		for(int i = 0 ; i < 5 ; i++){
			result = (result << 1) | (expo / binary); // bits[k]를 result의 최하위 비트에 추가
			expo %= binary;
			binary /=2;
		}//exponent저장
		for(int g = exponent - 1; g >= 0 ; g--){
			result = (result << 1) | ((input_test >> g) & 1);
		}
		result <<= (10-exponent);
	}
	else {
		result = (result << 1)| 1;
		input_test *= -1;
		int expo_cal = 1;
		int expo = 0;
		for(int i = 0; i < 16 ; i++){
			if (input_test / (expo_cal * 1) == 1){
				expo = i ;
				break;
			}
			expo_cal *= 2;
		}
		int exponent = expo; //exponent는 곱해지는 2의 지수
		expo += 15;
		int binary = 16;
		for(int i = 0 ; i < 5 ; i++){
			result = (result << 1) | (expo / binary); // bits[k]를 result의 최하위 비트에 추가
			expo %= binary;
			binary /= 2;
		}	//exponent저장
		for(int g = exponent - 1; g >= 0 ; g--){
			result = (result << 1) | ((input_test >> g) & 1);
		}
		result <<= (10-exponent);
	}
	return result;
}





int hpfp_to_int_converter(hpfp input){
	int result = 0;
	if((input ^ 0x7c00) == 0){
		return 2147483647 ;	//Tmax
	}
	if((input ^ 0xfc00) == 0){
		return -2147483648;	//Tmin
	}
	if(input){
	    char* bits = hpfp_to_bits_converter_int(input);
		int nan_found = 0;
		for(int k = 1; k<6;k++){
			nan_found += bits[k];
		}
        int found = 0; 
        for(int g = 6; g <16 ; g++){
            if (bits[g]==1){
                found = 1;
            }
        }
		if((nan_found == 5) && (found == 1)){
			return -2147483648;	//Tmin. Nan일 때.
		}
		result = (result << 1) & bits[0];	//부호 넣기. 음수면 1, 양수면 0
		result <<= 31;
		int expo = 0;
		int binary = 1;
		for(int i = 5; i >0; i--){
			expo += bits[i] * binary;
			binary *= 2;
		}
		int exponent = expo - 15 ; //곱해지는 2의 지수
		input <<= 5;
		hpfp value = 0;
		value = value | (1<<15);
		input = input | value ;
		input >>= (15-exponent);
		int get_val = 0;
		for(int i = 10; i >=0 ; i--){
			get_val = (get_val << 1) | (input >> i & 1);
		}
		result = result | get_val;
		if(bits[0]==1){
			result *= -1;
		}
	}
	return result;
}



hpfp float_converter(float input){	//denor_ver도 있음. 뒤에
    hpfp result = 0;
	unsigned char bits[sizeof(float)*8];
	FloatToBit(input, bits);
	
    if( input > 65504 ){
        result |= 0x7c00;	
        return result;  //hpfp 범위 초과 -> Infinite
    }
    if( input < -65504){
        result |= 0xfc00;
        return result;  //hpfp 범위 초과 -> -Infinite
    }
	if(input == 0){
		result |= 0x0000;
		return result;
	}							
	if(bits[31]== 1){
		result = (result << 1) | 1;
	} ; // bits[k]를 result의 최하위 비트에 추가

    int expo = 0;
	int binary = 1;
	for(int i = 0 ; i < 8 ; i++){
		expo += bits[23+i] * binary ;
		binary *= 2;
	}//expo값 계산.
	int exponent = expo - 112;
	if(exponent < -10){
		result = result << 15;
		return result;
	}

	if( exponent >= -10 && exponent <= 0){
		result <<= 5 ; //expo에 0 저장
		for(int g = 0; g < -exponent ; g++){
			result = (result << 1) & 0 ; 
		}
		for(int d = 0; d < 10 + exponent ; d++ ){
			result = (result << 1) | 1;
		}
		return result;	//denormalized form
	}
	binary = 16;
	for(int i = 0 ; i < 5 ; i++){
		result = (result << 1) | (exponent/binary); // bits[k]를 result의 최하위 비트에 추가
		exponent %= binary;
		binary /=2;
	}
	for(int k = 22; k > 12 ; k--){
		result = (result << 1) | bits[k]; // bits[k]를 result의 최하위 비트에 추가
		} //sig도 다 땄다.	
	return result;
}



/*hpfp float_converter(float input){	//denor_ver도 있음. 뒤에
    hpfp result = 0;
	unsigned char bits[sizeof(float)*8];
	FloatToBit(input, bits);
	
    if( input > 65504 ){
        result |= 0x7c00;	
        return result;  //hpfp 범위 초과 -> Infinite
    }
    if( input < -65504){
        result |= 0xfc00;
        return result;  //hpfp 범위 초과 -> -Infinite
    }
	if(input == 0){
		result |= 0x0000;
		return result;
	}													//if(input){}추가하면 원래대로 돌아감.
	result = (result << 1) | bits[31]; // bits[k]를 result의 최하위 비트에 추가
    int expo = 0;
	int binary = 1;
	for(int i = 0 ; i < 8 ; i++){
		expo += bits[23+i] * binary ;
		binary *= 2;
	}//expo값 계산.
	int exponent = expo - 112;
	if(exponent < -24){
		result = result << 15;
		return result;
	}
	if( exponent >= -24 && exponent <-14){
		for(int k = 0 ;k < 5 ; k++){
			result = (result << 1) & 0;//expo에 0 저장
		}
		for(int g = 0; g < -14-exponent ; g++){
			result = (result << 1) & 0 ; 
		}
		for(int d = 0; d < 24 + exponent ; d++ ){
			result = (result << 1) | 1;
		}
		return result;	//denormalized form
	}
	binary = 16;
	for(int i = 0 ; i < 5 ; i++){
		result = (result << 1) | (exponent/binary); // bits[k]를 result의 최하위 비트에 추가
		exponent %= binary;
		binary /=2;
	}
	for(int k = 22; k > 12 ; k--){
		result = (result << 1) | bits[k]; // bits[k]를 result의 최하위 비트에 추가
		} //sig도 다 땄다.	
	return result;
}*/






float hpfp_to_float_converter(hpfp input){

	if(input == 0x7c00){
		return 0x7f800000;
	}
	if(input == 0xfc00){
		return 0xff800000;
	} 
    union int_float{
        int int_ver;
        float float_ver;
    };
	float error = 0;
    union int_float i_f;
	char* bits = hpfp_to_bits_converter_int(input);
	int expo = 0;
	int binary = 1;
	int exponent;
	for(int i = 5; i >0; i--){
		expo += bits[i] * binary;
		binary *= 2;
	}
	if(expo == 0){
		int result = 0;
		int index;
		if( bits[0] == 1){
			result = (result << 1) | 1;
		}
		result = (result << 1) | bits[0];
		for(int k = 6 ; k <16 ; k++){
			if(bits[k] == 1){
				index = k - 6;
				break;
			}
		}		
		exponent = expo + 113 - index ;
		binary = 128;
		for(int i = 0 ; i < 8 ; i++){
			result = (result<<1) | (exponent/binary); // expo를 result의 왼쪽 비트에 순서대로 추가
			exponent %= binary;
			binary /=2;
		}
		result <<= 23;
		i_f.int_ver = result;
		return i_f.float_ver;
	}
	if(expo !=0){
		int result = 0;
		result = (result << 1) | bits[0];

		exponent = expo + 112 ;
		binary = 128;
		for(int i = 0 ; i < 8 ; i++){
			result = (result<<1) | (exponent/binary); // expo를 result의 왼쪽 비트에 순서대로 추가
			exponent %= binary;
			binary /=2;
		}
		for(int g = 6; g < 16 ; g++) {
			result = (result << 1) | bits[g]; // significand를 result의 왼쪽 비트에 순서대로 추가
		}
		result <<= 13;
		i_f.int_ver = result;
		return i_f.float_ver;

	}
	return error;
}

	/*union int_float{
        int int_ver;
        float float_ver;
    };
	union int_float i_f;
	int result = 0;
	char* bits = hpfp_to_bits_converter_int(input);
	result = (result << 1) | (int)bits[0];
	int expo = 0;
	int binary = 1;
	for(int i = 5; i >0; i--){
		expo += (int)bits[i] * binary;
		binary *= 2;
	}
	int exponent = expo + 112 ;
	binary = 128;
	for(int i = 0 ; i < 8 ; i++){
		result = (result<<1) | (exponent/binary); // expo를 result의 왼쪽 비트에 순서대로 추가
		exponent %= binary;
		binary /=2;
	}
	for(int g = 6; g < 16 ; g++){
		result = (result << 1) | (int)bits[g]; // significand를 result의 왼쪽 비트에 순서대로 추가
	}
	result <<= 13;
    i_f.int_ver = result;
	return i_f.float_ver;
}*/






hpfp addition_function(hpfp a, hpfp b){
	hpfp result = 0 ;
	char* bit_a = hpfp_to_bits_converter_int(a);
	char* bit_b = hpfp_to_bits_converter_int(b);
	int a_exp = 0;
	int b_exp = 0;
	int media = 1;
	for(int k = 5 ; k >0 ; k--){
		a_exp += bit_a[k]*media;
		b_exp += bit_b[k]*media;
		media *= 2;
	}		//exp를 구하자
	a_exp -= 15;
	b_exp -= 15;
	int a_arr[42] = {0};
	int b_arr[42] = {0};
	int arr[42] = {0};
	int a_zero = 0;
	int b_zero = 0;

	for(int k = 6; k <16 ; k++){
		if(bit_a[k]==1){
			a_zero = k;
		}
		if(bit_b[k] == 1){
			b_zero = k;
		}
	}
	if((a_exp == 16 && a_zero!=0) || (b_exp == 16 &&& b_zero!=0)){
		result |= 0xfe00;
		return result;
	}	//하나라도 Nan

	if(( a_exp == b_exp ) && (a_exp == 16)) {	//둘 다 무한대일 때
		if(bit_a[0] != bit_b[0]){
			result |= 0x7e00; //Nan으로 바꿔버리기 음수 무한대 더하기 양수무한대니까
			return result;
		}
		if( bit_a[0] == 0){
			result |= 0x7c00;		//양 무한대
			return result;
		}
		if(bit_a[0] == 1){
			result |= 0xfc00;
			return result;		//음 무한대
		}
	}
	if((a_exp == 16) && (b_exp != 16)){
		if(bit_a[0] == 0){
			result |= 0x7c00;
		}
		else{
			result |= 0xfc00;
		}
		return result;
	}

	if((a_exp != 16) && (b_exp == 16)){
		if(bit_b[0] == 0){
			result |= 0x7c00;
		}
		else{
			result |= 0xfc00;
		}
		return result;
	}
	int min_plus = 0;//현재 플러스인 상태 1이면 마이너스임.

	if(a_exp != -15){
		bit_a[5] = 1;
	}	
	if(b_exp != -15){
		bit_b[5] = 1;
	}
	for(int i = 0; i < 11 ; i++){
		a_arr[15 + a_exp + i] = bit_a[15-i];
		b_arr[15 + b_exp + i] = bit_b[15-i];
	}					//index -25가 실제 2진수의 자리임!
	if(bit_a[0] == bit_b[0]){		//부호동일
		int media;
		if(bit_a[0] == 1){
			min_plus = 1;
		}
		for(int t = 0; t < 42 ; t++){
			arr[t] += a_arr[t] + b_arr[t];
			media = t;
			while(arr[media] >= 2 && media < 41){
				arr[media] -= 2;
				arr[ media + 1 ] += 1;
				media++ ;
			}
			if(arr[41] == 2){
				result = 0;
				if(bit_a[0] == 0){
					result |= 0x7c00;
				}
				else{
					result |= 0xfc00;	//범위초과
				}
				return result;
			}
		}
	}
	if (bit_a[0] != bit_b[0]){		//부호 다름. 차
		int media;
		int bigger = 0 ;	//sig가 더 큰 친구

		if(a_exp == b_exp){
			if(a_zero < b_zero){
				bigger = 1;
			}
			if(a_zero > b_zero){
				bigger = 2;
			}
			if(a_zero == b_zero ){
				for(int h = a_zero ; h < 16; h++){
					if(a_arr[h] == 1 && b_arr[h] != 1){
						bigger = 1;
						break;
					}
					if(b_arr[h] == 1 && a_arr[h] != 1){
						bigger = 2;
						break;
					}
				}	//더 큰 수 구하기
				if(bigger == 0){
					result &= 0x0000;
					return result;	//부호 다른데 절댓값 같을 때.
				}
			}
		}
		if(a_exp > b_exp || bigger  == 1){	//a의 절댓값이 더 커
			if(bit_a[0] == 1){
				min_plus = 1; //a 음수라면 결과도 음수다
			}
			for(int t = 41; t > 0 ; t--){
				arr[t] += a_arr[t] - b_arr[t];
				media = t; 
				while(arr[media] == -1 && media < 41){
					arr[media] = 1 ;
					arr[ media + 1 ] -= 1;
					media++;
				}		//빼는 것
			}
		}
		if(a_exp < b_exp){	//b의 절댓값이 더 커
			if(bit_b[0] == 1){
				min_plus = 1; //b 음수라면 결과도 음수다
			}
			for(int t = 41; t > 0 ; t--){
				arr[t] += b_arr[t] - a_arr[t];
				media = t; 
				while(arr[media] == -1 && media < 41){
					arr[media] = 1 ;
					arr[ media + 1 ] -= 1;
					media++;
				}		//빼는 것
			}
		}
	}
	int one = 0; //뒤에 숫자가 있는지
	for(int k = 41 ; k > 0 ; k--){
		if( arr[k] == 1){
			media = k; //시작하는 index
			break;
		}
	}//여기서부터다. 걱정마라 
	for(int k = media + 11; k < 41; k++){
		if(arr[k] == 1){
			one = 1;
		}
	}
	if( (arr[media + 11] == 1 ) && one == 1){	//바로 뒤에 1이 나왔어. 근데 마지막음 0이면 내려. 1이면 올려!
		arr[media + 10]++;
		int media_p = media + 10;	
		while(arr[media_p] == 2 && media > 0){	//올림하기
			arr[media_p] -= 2;
			arr[media_p - 1] += 1;
			media_p-- ;
		}
	}// 여기까지다. //
		int exponent = media - 10 ;//진짜 곱해지는 수 +15. exp로 표현되는 수
		if(min_plus == 1){
			result |= 1;
			result << 15;	//부호 -
		}
		int binary = 16;
		for(int j = 0; j < 5 ; j++){
			result = (result<<1) | (exponent/binary) ;
			exponent %= binary ;
			binary /= 2 ;
		}	//exp파트
		if(media < 10){
			for(int k = 0 ; k < 10-media ;k++){
				result = (result<<1) | 1;	//1로 채워야하는 곳들
			}
			return result;
		}
		for(int k = media-1 ; k > media - 11; k--){
			result = (result<<1) | arr[k]; 
		}
		
	return result;
}





hpfp multiply_function(hpfp a, hpfp b){
	hpfp result = 0 ;
	char* a_bit = hpfp_to_bits_converter_int(a);
	char* b_bit = hpfp_to_bits_converter_int(b);
	int a_exp = 0;
	int b_exp = 0;
	int media = 1;
	for(int k = 5 ; k >0 ; k--){
		a_exp += a_bit[k] * media;
		b_exp += b_bit[k] * media;
		media *= 2;
	}								//exp를 구하자
	a_exp -= 15;
	b_exp -= 15;
	int a_arr[11] = {0};
	int b_arr[11] = {0};
	int arr[22] = {0};		//이진수를 나타낼 거야. 곱해서 더해지는 아이들! addition함수 참고하쟈잉. index-20이 2의 지수값.
	int result_exp = a_exp + b_exp ;		//일단 두 exponent의 합을 저장
	int one = 0;
	if(a_exp != -15){
		a_arr[0] = 1;
	}
	if(b_exp != -15){
		b_arr[0] = 1;
	}	
	for(int h = 1 ; h < 11 ; h++){
		a_arr[h] = a_bit[h + 5];	//비트정보 저장
		b_arr[h] = b_bit[h + 5];
	}
	if(a_bit[0] != b_bit[0])	{
		result = (result << 1) | 1 ;			//부호정보 저장
	}	//음수
	for (int i = 0 ; i < 11 ; i++){
		for(int k = 0 ; k < 11 ; k++){	//arr[0]은 2의 1제곱, arr[21]가 2의 -20제곱..거꾸로 계산하는 걸로 하자. 큰 애가 앞으로.
			arr[ i + k + 1] += a_arr[i] * b_arr[k];
			media = i + k + 1;
			while(arr[media] == 2 && media > 0){	//곱의 합이...2를 넘을 수 있나..? 아니 없을텐데...으아ㅏ아아ㅏ앙아ㅏ아아ㅏ아아악
				arr[media] -= 2;
				arr[media - 1] += 1;
				media-- ;
			}
		}
	}
	for(int p = 0 ; p <22 ; p++){
		printf("%d - %d\n", p, arr[p]);
	}
	int starting_p = 0;	//숫자가 시작하는 index. -15exp가 아니라면 얘 뒤부터 읽어야함. 
	for(int y= 0 ; y < 21 ; y++){
		if(arr[y] == 1){
			result_exp -= y - 1;
			starting_p = y;
			break;
		}
	}
	if(result_exp < - 15){	//거의 0으로 나오는 값
		return result;
	}
	for(int k = starting_p + 12 ; k<22; k++){
		if(arr[k] == 1){
			one = 1;
		}
	}
	if( (arr[starting_p + 11] == 1 ) && one == 1){	//바로 뒤에 1이 나왔어. 근데 마지막음 0이면 내려. 1이면 올려!
		arr[starting_p + 10]++;
		media = starting_p + 10;	
		while(arr[media] == 2 && media > 0){	//올림하기
			arr[media] -= 2;
			arr[media - 1] += 1;
			media-- ;
		}
	}


	if(result_exp < - 15){	//거의 0으로 나오는 값
		return result;
	}
	if(result_exp > 16 ){		//무한대
		result <<= 15;
		result |= 0x7c00;
		return result;
	}
	int binary = 16;
	result_exp += 15;
	for(int j = 0; j < 5 ; j++){
		result = (result<<1) | (result_exp / binary) ;
		result_exp %= binary ;
		binary /= 2 ;
	}	//exp파트
	for(int j = starting_p + 1 ; j < starting_p + 11 ; j++){
		result = (result << 1) | arr[j]; //뒤의 10자리수 가져오기.
	}
	return result;
}
	/*hpfp result = 0 ;
	char* a_bit = hpfp_to_bits_converter_int(a);
	char* b_bit = hpfp_to_bits_converter_int(b);
	int a_exp = 0;
	int b_exp = 0;
	int media = 1;
	for(int k = 5 ; k >0 ; k--){
		a_exp += a_bit[k] * media;
		b_exp += b_bit[k] * media;
		media *= 2;
	}								//exp를 구하자
	a_exp -= 15;
	b_exp -= 15;
	int a_arr[11] = {0};
	int b_arr[11] = {0};
	int arr[22] = {0};		//이진수를 나타낼 거야. 곱해서 더해지는 아이들! addition함수 참고하쟈잉. index-20이 2의 지수값.
	int result_exp = a_exp + b_exp ;		//일단 두 exponent의 합을 저장
	if(a_exp != -15){
		a_arr[0] = 1;
	}
	if(b_exp != -15){
		b_arr[0] = 1;
	}	
	for(int h = 1 ; h < 11 ; h++){
		a_arr[h] = a_bit[h + 5];	//비트정보 저장
		b_arr[h] = b_bit[h + 5];
	}
	if(a_bit[0] != b_bit[0])	{
		result = (result << 1) | 1 ;			//부호정보 저장
	}	//음수
	for (int i = 0 ; i < 11 ; i++){
		for(int k = 0 ; k < 11 ; k++){	//arr[0]은 2의 1제곱, arr[21]가 2의 -20제곱..거꾸로 계산하는 걸로 하자. 큰 애가 앞으로.
			arr[ i + k + 1] += a_arr[i] * b_arr[k];
			media = i + k + 1;
			while(arr[media] == 2 && media > 0){	//곱의 합이...2를 넘을 수 있나..? 아니 없을텐데...으아ㅏ아아ㅏ앙아ㅏ아아ㅏ아아악
				arr[media] -= 2;
				arr[media - 1] += 1;
				media-- ;
			}
		}
	}
	for(int p = 0 ; p <22 ; p++){
		printf("%d - %d\n", p, arr[p]);
	}
	int starting_p = 0;	//숫자가 시작하는 index. -15exp가 아니라면 얘 뒤부터 읽어야함.
	for(int i = 0; i < 22 ; i++){
		if(arr[i] == 1){
			starting_p = i;
			break;
		}
	}
	if((arr[starting_p + 11] == 1) && (starting_p + 11 < 22) && (starting_p + 10 != result_exp)){
		arr[starting_p + 10]++;
		media = starting_p + 10;
		while(arr[media] == 2 && media > 0){	//올림하기
			arr[media] -= 2;
			arr[media - 1] += 1;
			media-- ;
		}
	}
	if(arr[0] == 1){
		result_exp++;	//2의 1제곱이 있음. 2가 있다는 뜻. exp 올려라. 
	}
	if(arr[0] != 1){
		for(int y= 1 ; y < 21 ; y++){
			if(arr[y] == 1){
				result_exp -= y - 1;
				starting_p = y;
				break;
			}
		}
	}
	if(result_exp < - 15){	//거의 0으로 나오는 값
		return result;
	}
	if(result_exp > 16 ){		//무한대
		result <<= 15;
		result |= 0x7c00;
		return result;
	}
	int binary = 16;
	result_exp += 15;
	for(int j = 0; j < 5 ; j++){
		result = (result<<1) | (result_exp / binary) ;
		result_exp %= binary ;
		binary /= 2 ;
	}	//exp파트
	for(int j = starting_p + 1 ; j < starting_p + 11 ; j++){
		result = (result << 1) | arr[j]; //뒤의 10자리수 가져오기.
	}
	return result;
}*/
	/*hpfp result = 0 ;
	char* a_bit = hpfp_to_bits_converter_int(a);
	char* b_bit = hpfp_to_bits_converter_int(b);
	int a_exp = 0;
	int b_exp = 0;
	int media = 1;
	for(int k = 5 ; k >0 ; k--){
		a_exp += a_bit[k] * media;
		b_exp += b_bit[k] * media;
		media *= 2;
	}								//exp를 구하자
	a_exp -= 15;
	b_exp -= 15;
	int a_arr[11] = {0};
	int b_arr[11] = {0};
	int arr[22] = {0};		//이진수를 나타낼 거야. 곱해서 더해지는 아이들! addition함수 참고하쟈잉. index-20이 2의 지수값.
	int result_exp = a_exp + b_exp ;		//일단 두 exponent의 합을 저장
	if(a_exp != -15){
		a_arr[0] = 1;
	}
	if(b_exp != -15){
		b_arr[0] = 1;
	}	
	for(int h = 1 ; h < 11 ; h++){
		a_arr[h] = a_bit[h + 5];	//비트정보 저장
		b_arr[h] = b_bit[h + 5];
	}
	if(a_bit[0] != b_bit[0])	{
		result = (result << 1) | 1 ;			//부호정보 저장
	}	//음수
	for (int i = 0 ; i < 11 ; i++){
		for(int k = 0 ; k < 11 ; k++){	//arr[0]은 2의 1제곱, arr[21]가 2의 -20제곱..꺼꾸로 계산하는 걸로 하자. 큰 애가 앞으로.
			arr[ i + k + 1] += a_arr[i] * b_arr[k];
			media = i + k + 1;
			while(arr[media] == 2 && media > 0){	//곱의 합이...2를 넘을 수 있나..? 아니 없을텐데...으아ㅏ아아ㅏ앙아ㅏ아아ㅏ아아악
				arr[media] -= 2;
				arr[media - 1] += 1;
				media-- ;
			}
		}
	}
	int starting_p = 0;	//숫자가 시작하는 index. -15exp가 아니라면 얘 뒤부터 읽어야함.
	if(arr[0] == 1){
		result_exp++;	//2의 1제곱이 있음. 2가 있다는 뜻. exp 올려라. 
	}
	if(arr[0] != 1){
		for(int y= 1 ; y < 21 ; y++){
			if(arr[y] == 1){
				result_exp -= y - 1;
				starting_p = y;
				break;
			}
		}
	}
	if(result_exp < - 15){	//거의 0으로 나오는 값
		return result;
	}
	if(result_exp > 16 ){		//무한대
		result <<= 15;
		result |= 0x7c00;
		return result;
	}
	int binary = 16;
	result_exp += 15;
	for(int j = 0; j < 5 ; j++){
		result = (result<<1) | (result_exp / binary) ;
		result_exp %= binary ;
		binary /= 2 ;
	}	//exp파트
	for(int j = starting_p + 1 ; j < starting_p + 11 ; j++){
		result = (result << 1) | arr[j]; //뒤의 10자리수 가져오기.
	}
	return result;
}*/






char* comparison_function(hpfp a, hpfp b){
	char* a_bit = hpfp_to_bits_converter_int(a);
	char* b_bit = hpfp_to_bits_converter_int(b);
	int a_exp = 0;
	int b_exp = 0;
	int a_sig = 0;
	int b_sig = 0;
	int media = 1;
    char* result = (char*)malloc(sizeof(char) + 1); // 메모리 동적 할당
    result[1] = '\0'; // 문자열의 끝을 나타냄

	for(int k = 15 ; k > 5 ; k--){
		a_sig += a_bit[k] * media;
		b_sig += b_bit[k] * media;
		media*=2;
	}
	media = 1;
	for(int t = 5; t > 0 ; t--){
		a_exp += a_bit[t] * media;
		b_exp += b_bit[t] * media;
		media *= 2;
	}
	if(a_bit[0] == 1){
		a_sig *= -1;
	}
	if(b_bit[0] == 1){
		b_sig *= -1;
	}
	if( ((a_exp == 31) && (a_sig !=0)) || ((b_exp ==31) &&(b_sig != 0)) ){
		result[0] = '=';
        return result;
	}   //Nan 결과값
	if((a_exp == b_exp) && (b_sig == a_sig)){
		result[0] = '=';
        return result;
	}
	if((a_exp == 31) && (b_exp !=31)){
		if(a_bit[0] == 0){
			result[0] = '>';
            return result;
		}
		else{
			result[0] = '<';
            return result;
		}
	}
	if((b_exp == 31)&& (a_exp !=31)){
		if(b_bit[0] == 0){
			result[0] = '<';
            return result;
		}
		else{
			result[0] = '>';
            return result;
		}
	}
	if(a_bit[0] > b_bit[0]){
		result[0] = '<';
        return result;
	}
	if(a_bit[0] < b_bit[0]){
		result[0] = '>';
        return result;
	}
	if(a_exp > b_exp) {
		if(a_sig < 0){
			result[0] = '<';
		}
		else{
			result[0] = '>';
        	return result;
		}
	}
	if(b_exp > a_exp) {
		if(b_sig < 0){
			result[0] = '>';
		}
		else{
			result[0] = '<';
        	return result;
		}
	}
    if(a_exp == b_exp){
        if( a_sig> b_sig){
            result[0] = '>';
            return result;
        }
        if(a_sig == b_sig){
            result[0]='=';
            return result;
        }
        if(a_sig < b_sig ){
            result[0] = '<';
            return result;
        }
    }
    return result;
} 






char* hpfp_to_bits_converter(hpfp result){
	
    char* bits = (char*)malloc(sizeof(hpfp) * 8 + 1) ;
	for (int i = sizeof(hpfp) * 8 - 1; i >= 0; i--) {
		bits[15-i] = ((result >> i) & 1) ? '1' : '0';;
	}   
	bits[sizeof(hpfp)*8] = '\0';
	return bits;
}




char* hpfp_to_bits_converter_int(hpfp input){
	char* bits = (char*)malloc(sizeof(hpfp) * 8) ;
	for (int i = sizeof(hpfp) * 8 - 1; i >= 0; i--) {
		bits[15-i] = ((input >> i) & 1) ;
	}  
	return bits;
}





char* hpfp_flipper(char* input){
	hpfp result_h = 0;
	for(int i = 0; i <16 ; i++){
		result_h = (result_h << 1 ) | ((int)input[i]-48);
	}
	int int_ver = hpfp_to_int_converter(result_h);
	float float_ver = hpfp_to_float_converter(result_h);

	if(float_ver - int_ver != 0){
		float result_f = 0;
		int result_int = 0;
		result_f = hpfp_to_float_converter(result_h);
		int integer_part = result_f;
		float float_part = result_f - integer_part ; //정수부분 소수부분 분리하기.
		float result = 0;	//플립된 float값
		float parameter = 1; //매개해주는 아이
		float parameter_1 = 0; //매개해주는 아이 2
		while ( integer_part != 0 ){
			parameter *= 10;
			parameter_1 = integer_part % 10 ;
			result += parameter_1 / parameter; 
			integer_part /= 10;
		}
		parameter = 1;
		while (float_part != 0){
			float_part *= 10;
			integer_part = float_part; //일의자리수만 빼내기
			result += integer_part * (parameter);	//더하세요!
			parameter *= 10;
			float_part -= integer_part;
		}	//result가 현재 flipeed된 float값임.
		hpfp return_value = float_converter(result);
		char* bits = hpfp_to_bits_converter(return_value);
		return bits;
	}
	else{
		int result = 0;	//플립된 int값
		int parameter = 1; //매개해주는 아이
		int parameter_1 = 0; //매개해주는 아이 2
		int parameter_2 = int_ver;
		int expo = 0;
		while(parameter_2 > 0){
			parameter_2 /= 10;
			expo++;
		}
		for(int k = 0; k < expo-1 ; k++){
			parameter *= 10;
		}
		while ( int_ver != 0 ){
			parameter_1 = int_ver % 10 ;
			result += parameter_1 * parameter; 
			int_ver /= 10;
			parameter /=10;
		}
		hpfp return_value = int_converter(result);
		char* bits = hpfp_to_bits_converter(return_value);
		return bits;
	}

}
/*char* hpfp_flipper(char* input){
	hpfp result_h = 0;
	for(int i = 0; i <16 ; i++){
		result_h = (result_h << 1 ) | ((int)input[i]-48);
	}
	int min_plus = 0;
	if(input[0] == 49){
		min_plus = 1;
	}
	int int_ver = hpfp_to_int_converter(result_h);
	float float_ver = hpfp_to_float_converter(result_h);
	if( min_plus == 1){
		int_ver *= -1;
		float_ver *= -1;
	}

	if(float_ver - int_ver != 0){
		float result_f = 0;
		result_f = hpfp_to_float_converter(result_h);
		int integer_part = result_f;
		float float_part = result_f - integer_part ; //정수부분 소수부분 분리하기.
		float result = 0;	//플립된 float값
		float parameter = 1; //매개해주는 아이
		float parameter_1 = 0; //매개해주는 아이 2
		
		while ( integer_part != 0 ){
			parameter *= 10;
			parameter_1 = integer_part % 10 ;
			result += parameter_1 / parameter; 
			integer_part /= 10;
		}
		parameter = 1;
		while (float_part != 0){
			float_part *= 10;
			integer_part = float_part; //일의자리수만 빼내기
			result += integer_part * (parameter);	//더하세요!
			parameter *= 10;
			float_part -= integer_part;
		}	//result가 현재 flipeed된 float값임.

		hpfp return_value = float_converter(result);
		char* bits = hpfp_to_bits_converter(return_value);
		return bits;
	}
	else{
		int result = 0;	//플립된 int값
		int parameter = 1; //매개해주는 아이
		int parameter_1 = 0; //매개해주는 아이 2
		int parameter_2 = int_ver;
		int expo = 0;
		while(parameter_2 > 0){
			parameter_2 /= 10;
			expo++;
		}
		for(int k = 0; k < expo-1 ; k++){
			parameter *= 10;
		}
		while ( int_ver > 0 ){
			parameter_1 = int_ver % 10 ;
			result += parameter_1 * parameter; 
			int_ver /= 10;
			parameter /=10;
		}
		if(min_plus == 1){
			result *= -1;
		}
		hpfp return_value = int_converter(result);
		char* bits = hpfp_to_bits_converter(return_value);
		return bits;
	}

}
*/
int main(){
	
	float  p ;
	int a;
	printf("값을 입력하시오 : ");
	scanf("%d",&a);
	scanf("%f",&p);
	

	hpfp result = int_converter(a);
	hpfp resulty = float_converter(p);
	printf("변환된 값: ");
	/*char *bit = (char*) malloc(sizeof(hpfp)*8);
	bit = hpfp_to_bits_converter_int(result);
	
	for(int i= 0 ; i<16 ;i++){
		printf("%d ", bit[i]);
	}
	printf("\n");*/
	char* result_1 = comparison_function(result,resulty);
	printf("%s ",result_1);



	/*float result2= hpfp_to_float_converter(result);
	char* bits = hpfp_to_bits_converter_int(result_1);
	for(int i= 0 ; i<16 ;i++){
		printf("%d ", bits[i]);
	}
	printf("float 값은 바로 : %f\n", result2);*/


	return 0;
}




/*/////////////////////////////<inttohpfp -Demp_typecasting>///////////////////////////////////////////////////////////////////////////////////////////

hpfp int_converter(int input){	//exp는 -14~15까지임.
	
	int input_test = input;
  hpfp result = 0;
	
  if( input > 65504 ){
    result |= 0x7c00;	        
		return result;  //hpfp 범위 초과 -> Infinite
	}
  if( input < -65504){
    result |= 0xfc00;
		return result;  //hpfp 범위 초과 -> -Infinite
  }
	if(input == 0){
		result |= 0x0000;
		return result;
	}	//0일 땐 모든 비트를 0으로
	
  if(input < 0){
		result = (result << 1)| 1;
		input_test *= -1;
	}
		int expo_cal = 1;
		int expo = 0;
		for(int i = 0; i < 16 ; i++){
			if (input_test / (expo_cal * 1) == 1){
				expo = i ;
				break;
			}
			expo_cal *= 2;
		}
		int exponent = expo; //exponent는 곱해지는 2의 지수
		expo += 15;
		int binary = 16;
		for(int i = 0 ; i < 5 ; i++){
			result = (result << 1) | (expo / binary); // bits[k]를 result의 최하위 비트에 추가
			expo %= binary;
			binary /=2;
		}//exponent저장
		for(int g = exponent - 1; g >= 0 ; g--){
			result = (result << 1) | ((input_test >> g) & 1);
		}
		result <<= (10-exponent);



	return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/






/*/////////////////////////////<hpfptobits -Demo_if statement>///////////////////////////////////////////////////////////////////////////////////////////

char* hpfp_to_bits_converter(hpfp result){
  char* bits = malloc(sizeof(hpfp)*8);
	for (int i = sizeof(hpfp) * 8 - 1; i >= 0; i--) {
		bits[15-i] = (result >> i) & 1;
	}
	printf("\n");
    
  return bits;
}   //순서대로 bit안에 비트 정보 저장

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/






/*/////////////////////////////<floattohpfp -Demo1_variables>///////////////////////////////////////////////////////////////////////////////////////////

void FloatToBit(float input, unsigned char *bits) {
    unsigned char *pointer = (unsigned char *) & input;
		int k = 0;
    for (int i = 0; i < sizeof(float) ; i++) {
        for (int j = 0; j < 8 ; j++) {
            bits[k++] =(pointer[i] >> j) & 1; // 해당 비트를 2진수로 출력
        }
    }
} //비트를 거꾸로 저장함. 뒤에서부터 읽어야 해


hpfp float_converter(float input){
	
    hpfp result = 0;
		unsigned char bits[sizeof(float)*8];
		FloatToBit(input, bits);
	
    if( input > 65504 ){
        result |= 0x7c00;	
        return result;  //hpfp 범위 초과 -> Infinite
    }
    if( input < -65504){
        result |= 0xfc00;
        return result;  //hpfp 범위 초과 -> -Infinite
    }
	if(input == 0){
		result |= 0x0000;
		return result;
	}
	if(abs (input) < 0.0006104){
		result = (result << 1) | bits[31]; // bits[k]를 result의 최하위 비트에 추가
    	int expo = 0;
		int binary = 1;
		for(int i = 0 ; i < 8 ; i++){
			expo += bits[23+i] * binary ;
			binary *= 2;
		}//expo값 계산.
		int exponent = expo - 112;
		if(exponent < - 24 ){
			result |= 0x0000;
		}
		binary = 16;
		for(int y = 1 ; y < 6 ; y++){
			result = (result << y) & 0;
		}//expo를 0으로 만들어. 가장 작은 ,denormalized를 표현할 거니까.
		int gap = -14 - exponent;
		for(int d = 6; d < 6 + gap ; d++){
			result = (result <<d ) & 0;
		}
		for(int a = 6+ gap ; a <16 ; a++){
			result = (result<<a)|1;
		}
	return result;
	}

    if(abs(input) >= 0.00006104){ 
		result = (result << 1) | bits[31]; // bits[k]를 result의 최하위 비트에 추가
    	int expo = 0;
		int binary = 1;
		for(int i = 0 ; i < 8 ; i++){
			expo += bits[23+i] * binary ;
			binary *= 2;
		}//expo값 계산.
		int exponent = expo - 112;
		if(exponent <= 0){
			result = result << 15;
			return result;
		}
		binary = 16;
		for(int i = 0 ; i < 5 ; i++){
			result = (result << 1) | (exponent/binary); // bits[k]를 result의 최하위 비트에 추가
			exponent %= binary;
			binary /=2;
		}
	
		for(int k = 22; k > 12 ; k--){
			result = (result << 1) | bits[k]; // bits[k]를 result의 최하위 비트에 추가
			} //sig도 다 땄다.	
    }
	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/








/*/////////////////////////////<hpfptoint - Demo_incr,decr>///////////////////////////////////////////////////////////////////////////////////////////
int hpfp_to_int_converter(hpfp input){
	int result = 0;
	
	if((input ^ 0x7c00) == 0){
		return 2147483647 ;	//Tmax
	}
	if((input ^ 0xfc00) == 0){
		return -2147483648;	//Tmin
	}
	if(input){
		unsigned char* bits = malloc(sizeof(hpfp)*8);
		bits = hpfp_to_bits_converter(input);
		int nan_found = 0;
		for(int k = 1; k<6;k++){
			nan_found += bits[k];
		}
		if(nan_found == 5){
			return -2147483648;	//Tmin
		}
		result = (result << 1) & bits[0];	//부호 넣기. 음수면 1, 양수면 0
		result <<= 31;
		int expo = 0;
		int binary = 1;
		for(int i = 5; i >0; i--){
			expo += bits[i] * binary;
			binary *= 2;
		}
		int exponent = expo - 15 ; //곱해지는 2의 지수
		input <<= 5;
		hpfp value = 0;
		value = value | (1<<15);
		input = input | value ;
		input >>= (15-exponent);
		int get_val = 0;
		for(int i = 10; i >=0 ; i--){
			get_val = (get_val << 1) | (input >> i & 1);
		}
		result = result | get_val;
		if(bits[0]==1){
			result *= -1;
		}
	}
	
	return result;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/









/*/////////////////////////////<hpfptofloat ->///////////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdint.h>
typedef unsigned short hpfp;


void hpfpToBit(hpfp input, unsigned char *bits) {
    unsigned char *pointer = (unsigned char *) & input;
		int k = 0;
    for (int i = 0; i < sizeof(hpfp) ; i++) {
        for (int j = 0; j < 8 ; j++) {
            bits[k++] =(pointer[i] >> j) & 1; // 해당 비트를 2진수로 출력
        }
		}
} //비트를 거꾸로 저장함. 뒤에서부터 읽어야 해



float hpfp_to_float_converter(hpfp input){
	int result = 0;
	
	unsigned char* bits = malloc(sizeof(hpfp)*8);
	bits = hpfp_to_bits_converter(input);
	result = (result << 1) | bits[0];
	int expo = 0;
	int binary = 1;
	for(int i = 5; i >0; i--){
		expo += bits[i] * binary;
		binary *= 2;
	}
	int exponent = expo + 112 ;
	binary = 128;
	for(int i = 0 ; i < 8 ; i++){
		result = (result<<1) | (exponent/binary); // expo를 result의 왼쪽 비트에 순서대로 추가
		exponent %= binary;
		binary /=2;
	}
	for(int g = 6; g < 16 ; g++){
		result = (result << 1) | bits[g]; // significand를 result의 왼쪽 비트에 순서대로 추가
	}
	result <<= 13;
	for (int i = sizeof(int) * 8 - 1; i >= 0; i--) {
			printf("%d", (result >> i) & 1);
	}
	printf("\n");
	float *result_2 = &result;	//int의 비트 정보를 그대로 가져가서 float에 저장하기
	return *result_2;
}




//result_2가 바로 float. 그리고 float to hpfp랑 같이 있을 때 돌아감. 왜냐면 hpfp가 내가 정의한 변수라 입력이 불가,,,

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/



/*///////////////////hpfp flipper - Demo 5주차에 있는 show divisors///////////////////////////////////
char* hpfp_flipper(char* input){
	
	hpfp result_h = 0;
	for(int i = 0; i <16 ; i++){
		result_h = (result_h << 1 )| input[i];
	}
	int int_ver = hpfp_to_int_converter(result_h);
	float float_ver = hpfp_to_float_converter(result_h);
	if(float_ver - int_ver != 0){
		float result_f = 0;
		int result_int = 0;
		result_f = hpfp_to_float_converter(result_h);
		int integer_part = result_f;
		float float_part = result_f - integer_part ; //정수부분 소수부분 분리하기.
		float result = 0;	//플립된 float값
		float parameter = 1; //매개해주는 아이
		float parameter_1 = 0; //매개해주는 아이 2
		while ( integer_part > 0 ){
			parameter *= 10;
			parameter_1 = integer_part % 10 ;
			result += parameter_1 / parameter; 
			integer_part /= 10;
		}
		parameter = 1;
		while (float_part != 0){
			float_part *= 10;
			integer_part = float_part; //일의자리수만 빼내기
			result += integer_part * (parameter);	//더하세요!
			parameter *= 10;
			float_part -= integer_part;
		}	//result가 현재 flipeed된 float값임.
		hpfp return_value = float_converter(result);
		char*	bits = hpfp_to_bits_converter(return_value);
		return bits;
	}
	else{
		int result = 0;	//플립된 int값
		int parameter = 1; //매개해주는 아이
		int parameter_1 = 0; //매개해주는 아이 2
		int parameter_2 = int_ver;
		int expo = 0;
		while(parameter_2 > 0){
			parameter_2 /= 10;
			expo++;
		}
		for(int k = 0; k < expo-1 ; k++){
			parameter *= 10;
		}
		while ( int_ver > 0 ){
			parameter_1 = int_ver % 10 ;
			result += parameter_1 * parameter; 
			int_ver /= 10;
			parameter /=10;
		}
		hpfp return_value = int_converter(result);
		char*	bits = hpfp_to_bits_converter(return_value);
		return bits;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////*/