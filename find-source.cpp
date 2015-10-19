#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include <fstream>
#include <stdlib.h>
#include <string>
#include <iostream>
using namespace std;
using namespace clang;
using namespace clang::tooling;
using namespace llvm;
ofstream outfile;
ifstream infile;
static cl::OptionCategory MyToolCategory("My tool options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\nMore help text...");

string project_name;
string file_name;
string full_file_name;
string header_file;
string qualified_name;
string short_name;
unsigned int start_line;
unsigned int end_line;
int ranks;

class ExampleVisitor : public RecursiveASTVisitor<ExampleVisitor> {
private:
  ASTContext *Context;
  SourceManager *sm;
  FunctionDecl* CurrentFunc;
public:
  explicit ExampleVisitor(ASTContext *Context)
    : Context(Context) {
      sm = &Context->getSourceManager();
      CurrentFunc = NULL;
    }

  bool VisitDecl(Decl *decl) {
     if (decl->isFunctionOrFunctionTemplate())
        CurrentFunc = decl->getAsFunction();
     return true;
  }
    
  bool VisitCallExpr(CallExpr* call){
    //outs() << "do found" << "\n";
    NamedDecl* calleedecl = (NamedDecl*)call->getCalleeDecl();
    if (calleedecl != NULL){
      //ignore macro expansion
      SourceLocation loc = call->getLocStart();
      if (!sm->isInMainFile(loc))
      return true;
      
      if (call->getCalleeDecl()->isFunctionOrFunctionTemplate()){
	SourceRange sr = call->getCalleeDecl()->getAsFunction()->getSourceRange();
	SourceLocation start_pos = sr.getBegin();
	if (!start_pos.isValid()){
	  //outs() << "error: invalid location." << "\n";
	  //outfile << "error: invalid location." << "\n";
	  return true;
	}
	else{
	  StringRef headerFilename = sm->getFilename(start_pos);
	  string fullheaderfile = headerFilename.str();
	  if (fullheaderfile == "")
	    return true;
	  if (fullheaderfile.find(project_name) < fullheaderfile.length())
	    return true;
	  if (fullheaderfile[0] != '/')
	    return true;
	  /*
	  int i;
	  for (i = fullheaderfile.length() - 1; i >=0; i--)
	    if (fullheaderfile[i] == '/') 
	      break;
	  string headerfile;
	  if (fullheaderfile.substr(fullheaderfile.length()-2, 2) == ".h")
	    headerfile = fullheaderfile.substr(i+1, fullheaderfile.length() - i - 3);
	  else
	    headerfile = fullheaderfile.substr(i+1, fullheaderfile.length() - i - 1);
	  bool flag = false;
	  for (int j = 0; j < API_NUM; j++)
	  {
	    bool flag2 = true;
	    for (int k = 0; k < headerfile.length(); k++){
	      if (headerfile[k] != APIList[j][k])
		flag2 = false;
	    }
	    if (flag2 == true){
	      flag = true;
	    }
	  }
	  if (!flag)
	    return true;
	  */
	}
      }
      else {
	//return true;
      }
      
      outs() << "Found " << calleedecl->getQualifiedNameAsString() <<" at: "; 
      //outfile << "Found " << calleedecl->getQualifiedNameAsString() <<" at: "; 
      string s = calleedecl->getQualifiedNameAsString();
      qualified_name = "";
      short_name = "";
      unsigned int pos = s.length()-1;
      while (pos > 0){
	if (s[pos] == ':' && s[pos-1] == ':')
	  break;
	pos--;
      }
      if (pos == 0)
	pos = -1;
      short_name = s.substr(pos+1, s.length()-pos-1);
      int mark = 0;
      for (unsigned int i = 0; i < s.length(); i++){
	if (s[i] == ',')
	  s[i] = ';';
	if (s[i] == '<'){
	  mark ++;
	}
	if (i > 0){
	  if (s[i-1] == '>'){
	    mark --;
	  }
	}
	if (mark == 0 || i >= pos+1)
	  qualified_name = qualified_name + s[i];
      }
      
      StringRef filename = sm->getFilename(loc);
      //unsigned int col = sm->getSpellingColumnNumber(loc);
      //unsigned int line = sm->getSpellingLineNumber(loc);
      outs() << filename << "\n";
      //file_name = filename.str();
      //outfile << filename.str() << "\n";
      //outfile <<filename.str();
      //outs() << ", line: " <<line<< " column: " << col << "\n";
      //outfile << ", line: " <<line<< " column: " << col << "\n";
      
      //get parent function
      if (CurrentFunc != NULL){
	SourceRange sr = CurrentFunc->getSourceRange();
	SourceLocation start_pos = sr.getBegin();
        SourceLocation end_pos = sr.getEnd();
        start_line = sm->getSpellingLineNumber(start_pos);
        end_line = sm->getSpellingLineNumber(end_pos);
        outs() << "Function:" << CurrentFunc->getNameInfo().getAsString() <<", StartLine: " <<start_line<< " EndLine: " << end_line << "\n";
	//outfile << "Function:" << CurrentFunc->getNameInfo().getAsString() <<", StartLine: " <<start_line<< " EndLine: " << end_line << "\n";
      }
      else{
	outs() << "error: no parent func" << "\n";
	//outfile << "error: no parent func" << "\n";
      }
      
      //get header file
      if (call->getCalleeDecl()->isFunctionOrFunctionTemplate()){
	SourceRange sr = call->getCalleeDecl()->getAsFunction()->getSourceRange();
	SourceLocation start_pos = sr.getBegin();
	if (!start_pos.isValid()){
	  outs() << "error: invalid location." << "\n";
	  return true;
	  //outfile << "error: invalid location." << "\n";
	}
	else{
	  StringRef headerFilename = sm->getFilename(start_pos);
	  outs() << "From declaration file:" << headerFilename<< ", line " << sm->getSpellingLineNumber(start_pos) <<"\n";
	  //outfile << "From declaration file:" << headerFilename.str()<< ", line " << sm->getSpellingLineNumber(start_pos) <<"\n";
	  header_file = headerFilename.str();
	}
      }
      else {
	outs() << "error: header file not found(callee is not a function)" << "\n";
	return true;
	//outfile << "error: header file not found(callee is not a function)" << "\n";
      }
      outs() << "\n";
      //outfile << "\n";
      full_file_name = "D:/Code/Repos/r10k_cpp"+file_name;
      if (end_line < start_line)
	return true;
      if (end_line-start_line< 8)
	ranks = -(end_line-start_line-8);
      else
	ranks = (end_line-start_line-8);
      ranks = ranks / 3;
      //if (header_file != ""){
        outfile<<short_name<<","<<qualified_name<<","<<full_file_name<<","<<start_line<<","<<end_line<<","<<ranks<<","<<project_name<<endl;
      
	//outfile<<project_name<<","<<file_name<<","<<qualified_name<<","<<short_name<<","<<header_file<<","<<start_line<<","<<end_line<<endl;
      //}
      return true;
    }else{
      //outs() << "null" << "\n";
      return true;
    }
  }
  
};

class ExampleASTConsumer : public ASTConsumer{
private:
  ExampleVisitor Visitor;
public:
  explicit ExampleASTConsumer(ASTContext *Context)
    : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class ExampleFrontendAction : public ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new ExampleASTConsumer(&Compiler.getASTContext()));
  }
};

int main(int argc, const char **argv) {
  string s = "";
  
  infile.open("config.txt", ios::in);
  getline(infile,project_name);
  infile.close();
  
  s = "";
  int i;
  for (i = 0; argv[1][i] != '\0'; i++){
    if (argv[1][i] == '/'||argv[1][i]=='.')
      s += '_';
    else
      s += argv[1][i];
  }
  file_name = argv[1];
  string filepath = "result.csv";
  outfile.open(filepath, ios::app);
  //outfile<<"project_name"<<","<<"file_name"<<","<<"qualified_name"<<","<<"short_name"<<","<<"header_file"<<","<<"start_line"<<","<<"end_line"<<endl;
  //runToolOnCode(new ExampleFrontendAction, "#include <iostream> using namespace std; int main(){ int a; cin >> a; cout << a << endl;}");
  CommonOptionsParser op(argc, argv, MyToolCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  
  Tool.run(newFrontendActionFactory<ExampleFrontendAction>().get());
  outfile.close();
  return 0;
}