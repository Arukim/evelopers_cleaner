#include <stdio.h>

//Test data
#define TESTS_COUNT 5
#define MAX_STRING 255
char test_in[TESTS_COUNT][MAX_STRING] = {"//cpp-style comment \n",
		      "//cpp-style comment A\n//cpp-style comment B",
		      "/*c style comment}*/\n",
		      "No comments at all",
		      "/*c // cpp\n comment*//* very hard * comment */\n"};
char test_out[TESTS_COUNT][MAX_STRING] = {"\n",
		      "\n                     ",
		      "\n",
		      "No comments at all",
		      "\n"};     

typedef enum CommentState_{
  None,
  SlashFound,
  CppStyle,
  CStyle,
  CStyleEnd
}CommentState;

void remove_Comments(char *psz_str){
  CommentState commentState = None;
  do{
    switch(commentState){
    case None:
      if(*psz_str == '/'){
	commentState = SlashFound;
      }
      break;
    case SlashFound:
      if(*psz_str == '/'){
	commentState = CppStyle;
	*psz_str = ' ';
	*(psz_str - 1) = ' ';
      }else if(*psz_str == '*'){
	commentState = CStyle;
	*psz_str = ' ';
	*(psz_str - 1) = ' ';
      }else{
	commentState = None;
      }
      break;
    case CppStyle:
      if(*psz_str == '\n'){
	commentState = None;
      }else{
	*psz_str = ' ';
      }
      break;      
    case CStyle:
      if(*psz_str == '*'){
	commentState = CStyleEnd;
      }
      *psz_str = ' ';
      break;
    case CStyleEnd:
      if(*psz_str == '/'){
	commentState = None;
      }else{
	commentState = CStyle;
      }
      *psz_str = ' ';
      break;
    default:
      break;
    }
  }while(*++psz_str);
}


int main() {
  int i;
  for(i=0;i< TESTS_COUNT; i++){
    remove_Comments(test_in[i]);
    if(strcmp(test_in[i], test_out[i])==0){
      printf("Test %d passed\n", i);
    }else{
      printf("Test %d failed\n", i);
    }
  }
  return 0;
}
