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
void remove_Comments(char *psz_str){
  /*sanity check*/
  if(psz_str == NULL || *psz_str == '\0'){
    return;
  }
  char * insert_pos = NULL;
  CommentState commentState = None;
  do{
    switch(commentState){
    case None:
      if(*psz_str == '/'){
	commentState = SlashFound;
	continue; /**Skip symbol copy */
      }
      break;
    case SlashFound:
      if(*psz_str == '/'){
	commentState = CppStyle;
	if(insert_pos == NULL){
	  insert_pos = psz_str - 1;
	}
	continue; /**Skip*/
      }else if(*psz_str == '*'){
	commentState = CStyle;
	if(insert_pos == NULL){
	  insert_pos = psz_str - 1;
	}
	continue; /**Skip*/
      }else{
	commentState = None;
	/**False '/' found, it's not a comment,
	 write it down*/
	if(insert_pos != NULL){
	  *insert_pos = *(psz_str - 1);
	  insert_pos++;
	}
      }
      break;
    case CppStyle:
      if(*psz_str == '\n'){
	commentState = None;
      }else{
	continue; /**Skip*/
      }
      break;      
    case CStyle:
      if(*psz_str == '*'){
	commentState = CStyleEnd;
      }
      continue; /**Skip*/
      break;
    case CStyleEnd:
      if(*psz_str == '/'){
	commentState = None;
      }else{
	commentState = CStyle;
      }
      continue; /**Skip*/
      break;
    default:
      break;
    }
    
    if(insert_pos != NULL){ 
      *insert_pos = *psz_str;
      insert_pos++;
    }
  }while(*++psz_str);


  if(insert_pos != NULL){
    *insert_pos = '\0';
  }
}


void remove_Comments_File(char *psz_file_in, char *psz_file_out){
  
  FILE * pFile_In = fopen(psz_file_in, "r");
  if(pFile_In == NULL){
    return;
  }

  FILE * pFile_Out = fopen(psz_file_out, "w");
  if(pFile_Out == NULL){
    fclose(pFile_In);
    return;
  }

  char buf_In[FILESYSTEM_BUF_SIZE];

  /**Read first chunk from input */
  int len = fread(buf_In, 1, sizeof(buf_In) - 1, pFile_In);

  while(len > 0){
    buf_In[len] = '\0';
    remove_Comments(buf_In);
    fwrite(buf_In, 1, strlen(buf_In), pFile_Out);
    len = fread(buf_In, 1, sizeof(buf_In) - 1, pFile_In);
  }

  fclose(pFile_In);
  fclose(pFile_Out);

  return;
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

  remove_Comments_File("main.c", "main.c~");
  return 0;
}
