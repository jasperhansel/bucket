#include "frontend/source_file.hxx"
#include <iostream>
using namespace std;


int main(int argc, char* argv[])
{
  try {
    if (argc == 2) {
      frontend::SourceFile sf{argv[1]};
      while (sf.current() != frontend::SourceFile::eof) {
        cout << static_cast<char>(sf.current());
        sf.next();
      }
    }
    return 0;
  } catch (exception& e) {
    cerr << "bucket: \033[31merror:\033[0m " << e.what() << '\n';
    return 1;
  }
}
