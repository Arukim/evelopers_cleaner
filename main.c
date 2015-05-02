#include <stdio.h>

//Test data
char * test_in[] = {"//c-style comment \\n",
		      "//c-style comment A\\n//c-style comment B",
		      "/*c++ style comment}*/\\n",
		      "No comments at all",
		      "/*C++ // c comment*/\n"};
char * test_out[] = {"                 \\n",
		      "                   \\n                   ",
		      "                      \\n",
		      "No comments at all",
		      "                    \n"};     

void remove_Comments(char *psz_str){
  
}


int main() {
  int i;
  for(i=0; i<sizeof(test_in); i++){
    remove_Comments(test_in[i]);
    if(strcmp(test_in[i], test_out[i])==0){
      printf("Test %d passed\n", i);
    }else{
      printf("Test %d failed\n", i);
    }
  }
  return 0;
}
