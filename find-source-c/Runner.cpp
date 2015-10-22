#include <stdlib.h>
#include <fstream>
#include <string>
using namespace std;

int main()
{
  ifstream fin;
  fin.open("filelist.txt", ios::in);
  string s = "";
  getline(fin, s);
  while (s != ""){
    string cmd = "find-source-c "+s;
    system(cmd.c_str());
    getline(fin,s);
  }
  
  fin.close();
  return 0;
}
