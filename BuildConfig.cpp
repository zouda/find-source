#include <iostream> 
#include <fstream>
#include <string>

using namespace std;

string findsource_path;
//string include_path;
string all_project_path;
string project_name;
string dirs[100000];
int dirs_count = 0;
int main(){
  string s = "";
  ofstream fout;
  ifstream fin;
  ifstream fdir;
  ifstream infile;
  infile.open("config.txt", ios::in);
  fdir.open("dirs.txt", ios::in);
  string dir = "";
  dirs_count = 0;
  getline(fdir, dir);
  while (dir != ""){
    dirs[dirs_count] = dir;
    dirs_count++;
    //fout << " -I" << dir;
    getline(fdir,dir);
  }
  getline(infile, project_name);
  getline(infile, all_project_path);
  fin.open("filelist.txt", ios::in);
  string json_path = all_project_path+"/compile_commands.json";
  fout.open(json_path.c_str(), ios::out);
  fout << "[" << endl;
  
  getline(fin, s);
  bool toggle = false;
  while (s != ""){
    if (!toggle){
      fout << "{" << endl;
      toggle = true;
    }
    else{
      fout << "," << endl;
      fout << "{" << endl;
    }
    int pos = s.find_last_of('/');
    string file_dir = s.substr(0, pos);
    int s_pos = s.find(project_name);
    int count = 0;
    int i;
    for (i = s_pos; i < pos; i++)
      if (s[i] == '/')
	count++;
    count++;
    fout << "  \"directory\": \"" << file_dir <<"\","<< endl;
    fout << "  \"command\": \"g++   -std=gnu++11";
    for (i = 0; i < count; i++){
      fout << " -I..";
      int j;
      for (j = 0; j < i; j++)
	fout << "/..";
    }
    for (i = 0; i < count; i++){
      fout << " -I..";
      int j;
      for (j = 0; j < i; j++)
	fout << "/..";
      fout << "/include";
    }
    //fout << " -I.. -I../.. -I ../../.. -I ../../../.."
    fout <<"  -W -std=c++11 -g"
	   //<<"  CMakeFiles/find-source.dir"
	   //<< s 
	   //<<".o"
	   <<" -c "
	   <<s<<"\","<<endl;
    fout << "  \"file\": \""<<s<<"\""<<endl;
    fout << "}";
    getline(fin,s);
  }
  fout << endl;
  fout << "]" << endl;
  infile.close();
  fin.close();
  fdir.close();
  fout.close();
}