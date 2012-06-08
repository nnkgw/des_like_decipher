#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

char K[] = { 1,1,1,1,1,1,0,0,0,0,0,0 };
char E[] = { 1,2,3,4,5,6,1,2,3,4,5,6 };  // 1 origin
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
  char* num_str = (char*)malloc(sizeof(char)*num);
  for (int i = 0; i < num; i++) {
    dst[i] = src[i] + '0';
  }
  free(num_str);
}

void string_to_bin(char* dst, char* src, int num) {
  char* num_str = (char*)malloc(sizeof(char)*num);
  for (int i = 0; i < num; i++) {
    dst[i] = src[i] - '0';
  }
  free(num_str);
}

void bin_xor(char* dst, char* bin_a, char* bin_b, int n) {
  for(int i = 0; i < n; i++) {
    dst[i] = (bin_a[i] ^ bin_b[i]);
  }
}

int feistel_function(int in_S_idx, char* in_R, char* in_PreNum, char* in_PostNum) {
  char preS[12];
  char postS[8];
  for (int i = 0; i < 12; i++) { preS[i] = in_R[ E[i]-1 ]; }  // extension E
  for (int i = 0; i < 12; i++) { preS[i] ^= K[i];          }  // xor K
  char num_str[12];
  int num_str_index = (in_S_idx==0) ? 0 : 6;
  bin_to_string(num_str, preS, 12);
  if (strncmp(&num_str[num_str_index], in_PreNum, 6) == 0) {
    int preS_index = (in_S_idx==0) ? 0 : 6;
    s_transform(in_S_idx, &preS[preS_index], &postS[0]);
    bin_to_string(num_str, postS, 8);
    //num_str_index = (in_S_idx==0) ? 0 : 4;
    if (strncmp(&num_str[0], in_PostNum, 4) == 0) {
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
    case   1: n += 1;  // fall through
    case   0: n *= 2; break;
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
  char R[8];
  struct sHighFreqPair{
    char prev[6+1];
    char post[4+1];
  };
  sHighFreqPair high_freq[2];
  strncpy(high_freq[0].prev, "010011", 6+1);
  strncpy(high_freq[0].post, "0110",   4+1);
  strncpy(high_freq[1].prev, "001110", 6+1);
  strncpy(high_freq[1].post, "0100",   4+1);
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 256; j++) {
      dec_to_bin(R, j, 8);
      int ret = feistel_function(i, R, high_freq[i].prev, high_freq[i].post);
      if (ret == 0){
        printf("R = ", i);
        print_result(R, 8);
        printf("\n");
        char r_str[6+1];
        bin_to_string(r_str, R, 6);
        r_str[6] = 0;
        char h_freq_bin[6+1];
        string_to_bin(h_freq_bin, high_freq[i].prev, 6);
        char k_bin[6];
        bin_xor(k_bin, R, h_freq_bin, 6);
        char k_str[6+1];
        bin_to_string(k_str, k_bin, 6);
        k_str[6] = 0;
        printf("K = %s xor %s = %s\n", r_str, high_freq[i].prev, k_str);
        break;
      }
    }
    printf("\n");
  }
  {
    printf("\n");
    std::vector<char> s[16];
    for(int i =0; i < 64; i++){
      char pre[6];
      //char post[4];
      dec_to_bin(pre, i, 6);
      int dec = s_box(0, pre);
      s[dec].push_back(i);
    }
    for(int i = 0; i < 16; i++) {
      size_t count = s[i].size();
      printf("%2d | ", i);
      for(int j = 0; j < count; j++) {
        printf( "%2d ", s[i][j] );
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
    printf("\n");
    std::vector<char> distribution[64];
    struct sXYPair{
      char x[2];
      char y[2];
    };
    std::vector<sXYPair> less_freq;
    for(int i = 0; i < 64; i++) {
      for(int j = 0; j < 64; j++) {
        int x_xor_x = i;
        x_xor_x ^= j;
        char bin[6];
        dec_to_bin(bin, i, 6);
        int y1 = s_box(1, bin);
        dec_to_bin(bin, j, 6);
        int y2 = s_box(1, bin);
        int y_xor_y = y1;
        y_xor_y ^=y2;
#if 0
        if ((x_xor_x == 52) && (y_xor_y == 4)) {
#else
        if ((x_xor_x == 59) && (y_xor_y == 3)) {
#endif
          sXYPair pair;
          pair.x[0] = i;
          pair.x[1] = j;
          pair.y[0] = y1;
          pair.y[1] = y2;
          less_freq.push_back(pair);
        }
        distribution[x_xor_x].push_back(y_xor_y);
      }
    }
    for(int i = 0; i < 64; i++) {
      printf("%2d | ", i);
      int count = distribution[i].size();
      char frequency[16];
      memset(frequency, 0, sizeof(char)*16);
      for(int j = 0; j < count; j++) {
          for(int k = 0; k < 16; k++) {
            if (distribution[i][j] == k) {
              frequency[k]++;
            }
          }
      }
      for(int j = 0; j < 16; j++) {
        printf("%2d ", frequency[j]);
      }
      printf("\n");
    }
    printf("\n");
    int count = less_freq.size();
    for(int i = 0; i < count; i++) {
      printf("(x,y)=(%2d,%2d), (x',y')=(%2d,%2d)\n", less_freq[i].x[0],
                                                     less_freq[i].y[0],
                                                     less_freq[i].x[1],
                                                     less_freq[i].y[1]);
    }
    printf("\n");
  }
}
