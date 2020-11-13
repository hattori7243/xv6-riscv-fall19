#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char* dirpath, char* filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(dirpath, 0)) < 0){               //打开文件、文件夹路径
    fprintf(2, "ls: cannot open %s\n", dirpath);
    return;
  }

  if(fstat(fd, &st) < 0){    //读取路径信息
    fprintf(2, "ls: cannot stat %s\n", dirpath);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    exit();
    break;

  case T_DIR:
    if(strlen(dirpath) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, dirpath);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name,".") == 0)
        continue;
      if(strcmp(de.name,"..") == 0)
        continue;    
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      switch (st.type)
      {
        case T_FILE:
          if(strcmp(filename,de.name) == 0)
          {
            printf("%s\n",buf);
            continue;
          }
          break;
        case T_DIR:
          find(buf,filename);
          break;
      }
      
    }
    break;
  }
  close(fd);

}

int
main(int argc, char *argv[])
{

  if(argc != 3){
    printf("Usage:find [dirpath] [filename].\n");
    exit();
  }


  find(argv[1],argv[2]);

  exit();
}