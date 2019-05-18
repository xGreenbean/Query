#include <iostream>
#include <stdexcept>
#include "Query.h"
#include "TextQuery.h"
#include "QueryResult.h"
using namespace std;

int main(int argc, char **argv)
{
  ifstream infile(argv[1]);
  if (!infile) { cerr << "No input file!" << endl; exit(1); }
  TextQuery tq(infile);
//  tq.display_map();
  while (true)
  {
    cout << "Enter one or two words to look for, or q to quit:" << endl;
    string line;
    getline(cin, line);
    if (line.empty() || line == "q") break;
	shared_ptr<QueryBase> q;
	try {
      q = QueryBase::factory(line);
	}
	catch( const invalid_argument& e ) {
      cout << e.what() << endl;
	  continue;
	}
    print(cout, q->eval(tq)) << endl;
  }
  exit(0);
}
//int main() {
//	regex words_regex("([\\w']+)");
//	string s = "\"selfie\".";
//	auto words_begin = std::sregex_iterator(s.begin(), s.end(), words_regex);
//	auto words_end = std::sregex_iterator();
//	if (std::distance(words_begin, words_end) > 0)
//		cout << (*words_begin)[1].str();
//	system("pause");
//		
//}