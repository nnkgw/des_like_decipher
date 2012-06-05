#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

char K[] = { 1,0,1,0,1,0,1,0,1,0,1,0 };
char E[] = { 1,2,3,4,5,6,7,8,1,2,3,4 };  // 1 origin
char P[] = { 1,2,3,4,5,6,7,8 };          // 1 origin
char S[2][64] = {                        // 0 origin
    { 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
       0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
       4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
      15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13, },
    { 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
       3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
       0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
      13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9, },
};

int s_box(int n, char* preS) {
  int row    = preS[0]*2 + preS[5];
  int column = preS[1]*8 + preS[2]*4 + preS[3]*2 + preS[4];
  return S[n][row*16+column];
}

void s_transform(int n, char* preS, char* postS){
  int value  = s_box(n, preS);
  int bit = 8;
  for (int i = 0; i < 4; i++) {
    postS[i] = (value & bit) ? 1 : 0;
    bit >>= 1;
  }
}

void bin_to_string(char* dst, char* src, int num) {
  char num_str[num];
  for (int i = 0; i < num; i++) {
    dst[i] = src[i] + '0';
  }
}

int feistel_function(char* out_R, char* in_R) {
  char preS[12];
  char postS[8];
  for (int i = 0; i < 12; i++) { preS[i] = in_R[ E[i]-1 ]; }  // extension E
  for (int i = 0; i < 12; i++) { preS[i] ^= K[i];          }  // xor K
  char num_str[8];
  bin_to_string(num_str, preS, 8);
  if (strncmp(&num_str[0],"010011",6) == 0) {
    s_transform(0, &preS[0], &postS[0]);
    bin_to_string(num_str, postS, 8);
    if (strncmp(&num_str[0],"0110",4) == 0) {
      return 0;
    }
  }
  return -1;
}

void dec_to_bin(char* dst, int dec, int num) {
  int bit = 1;
  for (int i = 0; i < num; i++) {
    if (dec & bit) { dst[num-i-1] = 1; }
    else           { dst[num-i-1] = 0; }
    bit <<= 1;
  }
}

void bin_to_dec(int* dec, char* bin, int num) {
  int n = 0;
  for (int i = 0; i < num; i++) {
    switch(bin[num-i-1]) {
    case 1: n += 1;  // fall through
    case 0: n *= 2; break;
    }
  }
  n /= 2;
  *dec = n;
}

void print_result(char* array, int num) {
  for(int i = 0; i < 8; i++){
    printf("%d", array[i]);
  }
}

int main(int argc, char* argv[]) {
  char L[8];
  char R[2][8];
  for(int i = 0; i < 256; i++) {
    dec_to_bin(&R[0][0], i, 8);
    char postF[8];
    int ret = feistel_function(postF, &R[0][0]);
    if (ret == 0){
      printf("R=");
      print_result(&R[0][0], 8);
      printf("\n");
    }
    // 111001 xor 
    // 010011  
    // 101010 = K
  }
  {
    printf("\n");
    std::vector<char> s[16];
    for(int i =0; i < 64; i++){
      char pre[6];
      char post[4];
      dec_to_bin(pre, i, 6);
      int dec = s_box(0, pre);
      s[dec].push_back(i);
    }
    for(int i = 0; i < 16; i++) {
      size_t count = s[i].size();
      printf("%02d | ", i);
      for(int j = 0; j < count; j++) {
        printf( "%02d ", s[i][j] );
      }
      printf("\n");
    }
    printf("\n");
    for(int i = 0; i < 16; i++) {
      size_t count = s[i].size();
      char dec[6];
      char bin[6+1];
      dec_to_bin(dec, i, 4);
      bin_to_string(bin, dec, 4);
      bin[4] = 0;
      printf("%2s | ", bin);
          for(int j = 0; j < count; j++) {
              dec_to_bin(dec, s[i][j], 6);
              bin_to_string(bin, dec, 6);
              bin[6] = 0;
              printf( "%2s ",  bin);
          }
          printf("\n");
      }
  }
}
