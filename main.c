#include <stdio.h>

#define REPLACE_SYM 0x11

/*Test data*/
#define TESTS_COUNT 5
#define MAX_STRING 255
char test_in[TESTS_COUNT][MAX_STRING] = {"//cpp-style comment \n",
		      "//cpp-style comment A\n//cpp-style comment B",
		      "/*c style comment}*/\n",
		      "No comments at all",
		      "/*c // cpp\n comment*/\n/* very hard * comment */\n"};
char test_out[TESTS_COUNT][MAX_STRING] = {"\n",
		      "\n",
		      "\n",
		      "No comments at all",
		      "\n\n"};     

typedef enum CommentState_{
  None,
  SlashFound,
  CppStyle,
  CStyle,
  CStyleEnd
}CommentState;

/* Remove selected symbol from string and shrink it
   remove "ABCCAA", A => "BCC" */
void remove_Symbol(char *psz_str, char symbol){
  /* sanity check */
  if(psz_str == NULL || symbol == '\0' || *psz_str == '\0'){
    return;
  }
  char * insert_pos = NULL;
  do{
    if(*psz_str == symbol){
      if(insert_pos == NULL){
	insert_pos = psz_str;
      }
    }else{
      if(insert_pos != NULL){
	*insert_pos = *psz_str;
	insert_pos++;
      }
    }
  }while(*psz_str++);
  
  if(insert_pos != NULL){
    *insert_pos = '\0';
  }
}

/*replace C/Cpp style comments with selected symbol */
void replace_Comments(char *psz_str, char symbol){
  /*sanity check*/
  if(psz_str == NULL || symbol == '\0' || *psz_str == '\0'){
    return;
  }
  
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
	*psz_str = symbol;
	*(psz_str - 1) = symbol;
      }else if(*psz_str == '*'){
	commentState = CStyle;
	*psz_str = symbol;
	*(psz_str - 1) = symbol;
      }else{
	commentState = None;
      }
      break;
    case CppStyle:
      if(*psz_str == '\n'){
	commentState = None;
      }else{
	*psz_str = symbol;
      }
      break;      
    case CStyle:
      if(*psz_str == '*'){
	commentState = CStyleEnd;
      }
      *psz_str = symbol;
      break;
    case CStyleEnd:
      if(*psz_str == '/'){
	commentState = None;
      }else{
	commentState = CStyle;
      }
      *psz_str = symbol;
      break;
    default:
      break;
    }
  }while(*++psz_str);
}

void remove_Comments(char *psz_str){
  replace_Comments(psz_str, REPLACE_SYM);
  remove_Symbol(psz_str, REPLACE_SYM);
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
