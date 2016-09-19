
#include <hstmIndexLibrary.h>

#include  <iostream>

using namespace std;

void dummyFunction() {}

int testStringToHstm() {
  const int argc = 4;
  scidb::Value args_[argc];
  const scidb::Value *args = args_;
  scidb::Value*  res  = new scidb::Value;
  void* etc;

  hstm::stringToHstm(&args,res,etc);
  
  return 1;
}

int main(int argc, const char *args[]) {
  cout << endl << endl << "Running tests via <" << args[0] << ">" << endl << flush;
  int notOk = 0;

  notOk = notOk + testStringToHstm();
  
  if(notOk) {
    cout << "tests not okay!" << endl << endl << endl << flush;
    return 1; // Error exit
  } else {
    cout << "tests okay." << endl << endl << endl << flush;
    return 0; // Quiet exit
  }
}
