#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
using namespace std;
FILE *op;
ofstream fout;
int dir_traverse(char *dir_name)
{
    DIR *dirp = opendir(dir_name);
    if(!dirp) {
        perror("opendir");
        return -1;
    }

    string s = "";
    for (int j = 0;dir_name[ j] != '\0'; j ++){
        s += dir_name[j];      
    }
    struct stat st;
    struct dirent *dir;
    char fullpath[1000];
    while((dir = readdir(dirp)) != NULL) {
        if(!strcmp(dir->d_name, ".") || // 考虑当前目录和上级目录，否则会死循环
           !strcmp(dir->d_name, "..")) {
            continue;
        } 

        sprintf(fullpath, "%s/%s", dir_name, dir->d_name); //获取全局路径
	int i;
	for (i = 0; fullpath[i] != '\0'; i++){
	  
	}
	if (fullpath[i-2] == '.' && fullpath[i-1] == 'c'){
	  //printf("%s\n", fullpath); // 打印路径
	  fprintf(op, "%s\n", fullpath); // 打印路径
	}
	if (fullpath[i-2] == '.' && fullpath[i-1] == 'h'){
	  //string s = "";
	  //for (int j = 0; j < i; j ++){
	  //  if (fullpath[j] == '/')
	  //    fout << s << endl;
	  //  s += fullpath[j];
	  //}
	}
        if(lstat(fullpath, &st) < 0) {
            perror("lstat");
            continue;
        }
        if(S_ISDIR(st.st_mode)) {
            dir_traverse(fullpath); // 递归遍历子目录
        }

    }

    closedir(dirp);

    return 0;
}

int main(int argc, char* argv[])
{
    op = fopen("filelist.txt","w");
    fout.open("config.txt",ios::out);
    char *topdir, pwd[2]=".";
    if (argc != 2)
        topdir=pwd;
    else
        topdir=argv[1];
    string s = "";
   
    for (int i = 0; topdir[i] != '\0'; i++){
      s += topdir[i];
    }
    int pos = s.find_last_of('/');
    string main_project = s.substr(pos+1, s.length()-1-pos);
    string all_project_path = s.substr(0, pos);
    
    //write config.txt:
    fout << main_project << endl;
    fout << all_project_path << endl;
    
    //write filelist.txt:
    dir_traverse(topdir);
    fclose(op);
    fout.close();
    return 0;
}