#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILESYSTEM_BUF_SIZE 4096

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

/*replace C/Cpp style comments with selected symbol */
void remove_Comments(char *psz_file_in, char *psz_file_out){
  FILE * pFile_In = fopen(psz_file_in, "r");
  if(pFile_In == NULL){
    return;
  }

  FILE * pFile_Out = fopen(psz_file_out, "w");
  if(pFile_Out == NULL){
    fclose(pFile_In);
    return;
  }

  char buf[FILESYSTEM_BUF_SIZE];
  int len = fread(buf, 1, FILESYSTEM_BUF_SIZE, pFile_In);
  CommentState commentState = None;

  while(len > 0){

    int i = 0;
    for(i = 0; i < len; i++){
      switch(commentState){
      case None:
	if(buf[i] == '/'){
	  commentState = SlashFound;
	  continue;
	}
	break;
      case SlashFound:
	if(buf[i] == '/'){
	  commentState = CppStyle;
	  continue;
	}else if(buf[i] == '*'){
	  commentState = CStyle;
	  continue;
	}else{
	  commentState = None;
	  fputc(buf[i-1], pFile_Out);
	}
	break;
      case CppStyle:
	if(buf[i] == '\n'){
	  commentState = None;
	}else{
	  continue;
	}
	break;      
      case CStyle:
	if(buf[i] == '*'){
	  commentState = CStyleEnd;
	}
	continue;
	break;
      case CStyleEnd:
	if(buf[i] == '/'){
	  commentState = None;
	}else{
	  commentState = CStyle;
	}
	continue;
	break;
      default:
	break;
      }
    
      fputc(buf[i], pFile_Out);
    }

    if(commentState == SlashFound){
      buf[0] = '/';
      len = 1 + fread(buf + 1, 1, FILESYSTEM_BUF_SIZE - 1, pFile_In);
      if(len == 1){
	fputc('/', pFile_Out);
	break;
      }
    }else{
      len = fread(buf, 1, FILESYSTEM_BUF_SIZE, pFile_In);
    }
  }
  
  fclose(pFile_In);
  fclose(pFile_Out);
}

int main() {
  int i;
  remove_Comments("main.c", "main.c~");
  return 0;
}
