#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILESYSTEM_BUF_SIZE 4096

typedef struct OutputBuffer_{
  char buf[FILESYSTEM_BUF_SIZE];
  int pos;
  FILE * file;
}OutputBuffer;

typedef enum CommentState_{
  None,
  SlashFound,
  CppStyle,
  CStyle,
  CStyleEnd
}CommentState;

void init_Output(FILE * file, OutputBuffer * buf){
  buf->file = file;
  buf->pos = 0;
}

void write_Output(char ch, OutputBuffer * buf){
  /** TODO: add input check*/
  if(!(buf->pos < sizeof(buf->buf))){
    fwrite(buf->buf, 1, sizeof(buf->buf), buf->file);
	buf->pos = 0;
  }
  buf->buf[buf->pos] = ch;
  buf->pos++;
}

void flush_Output(OutputBuffer * buf){
  fwrite(buf->buf, 1, buf->pos, buf->file);
  buf->pos = 0;
}

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

  char buf_In[FILESYSTEM_BUF_SIZE];
  OutputBuffer output_Buffer;
  init_Output(pFile_Out, &output_Buffer);

  /**Read first chunk from input */
  int len = fread(buf_In, 1, sizeof(buf_In), pFile_In);
  CommentState commentState = None;

  while(len > 0){

    int i = 0;
    for(i = 0; i < len; i++){
      
      switch(commentState){
      case None:
	if(buf_In[i] == '/'){
	  commentState = SlashFound;
	  continue;/**move to next character without write operation*/
	}
	break;
      case SlashFound:
	if(buf_In[i] == '/'){
	  commentState = CppStyle;
	  continue; /** move on*/
	}else if(buf_In[i] == '*'){
	  commentState = CStyle;
	  continue; /** move on*/
	}else{
	  commentState = None;
	  /** False comment start was found, we should write prev '/' */
	  write_Output(buf_In[i-1], &output_Buffer);
	}
	break;
      case CppStyle:
	if(buf_In[i] == '\n'){
	  commentState = None;
	}else{
	  continue; /** move on */
	}
	break;      
      case CStyle:
	if(buf_In[i] == '*'){
	  commentState = CStyleEnd;
	}
	continue; /** move on */
	break;
      case CStyleEnd:
	if(buf_In[i] == '/'){
	  commentState = None;
	}else{
	  commentState = CStyle;
	}
	continue; /** move on */
	break;
      default:
	break;
      }

      /**At this point symbol is sent to output*/
      write_Output(buf_In[i], &output_Buffer);
    }

    /**In_Buf is empty, need to pull next chunk*/
    if(commentState != SlashFound){
      /**Usually just get next chunk from file */
       len = fread(buf_In, 1, sizeof(buf_In), pFile_In);
    }else{
      /** If we are in SlashFound state, we need to last symbol
       * of prev buffer*/
      buf_In[0] = '/';
      len = 1 + fread(buf_In + 1, 1, sizeof(buf_In) - 1, pFile_In);
      if(len == 1){
	/**file suddenly ended, write down last symbol */
	write_Output('/', &output_Buffer);
	break;
      }
    }
  }

  /** Flush temp output buffer to file */
  flush_Output(&output_Buffer);
  
  fclose(pFile_In);
  fclose(pFile_Out);
}

int main() {
  int i;
  remove_Comments("main.c", "main.c~");
  return 0;
}
