/* Ehud Plaksin 314095605 */
/* Saimon Lankri 209025907 */
#include "Query.h"
#include "TextQuery.h"
#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <regex>
#include <math.h>
using namespace std;
////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<QueryBase> QueryBase::factory(const string& s)
{
	//various ragexes
  regex word_regex("^\\s*([\\w']+)\\s*$");
  regex not_regex("^\\s*NOT\\s+([\\w']+)\\s*$");
  regex and_regex("^\\s*([\\w']+)\\s+AND\\s+([\\w']+)\\s*$");
  regex or_regex("^\\s*([\\w']+)\\s+OR\\s+([\\w']+)\\s*$");
  regex n_regex("^\\s*([\\w']+)\\s+(\\d+)\\s+([\\w']+)\\s*$");

  auto words_begin = std::sregex_iterator(s.begin(), s.end(), word_regex);
  auto words_end = std::sregex_iterator();
  if (std::distance(words_begin, words_end) > 0)
    return std::shared_ptr<QueryBase>(new WordQuery( (*words_begin)[1].str()));

  words_begin =
        std::sregex_iterator(s.begin(), s.end(), not_regex);
  words_end = std::sregex_iterator();

    if (std::distance(words_begin, words_end) > 0)
      return std::shared_ptr<QueryBase>(new NotQuery( (*words_begin)[1].str()));

   words_begin =
          std::sregex_iterator(s.begin(), s.end(), and_regex);
    words_end = std::sregex_iterator();

    if (std::distance(words_begin, words_end) > 0)
      return std::shared_ptr<QueryBase>(new AndQuery((*words_begin)[1].str(),
    (*words_begin)[2].str()));

   words_begin =
        std::sregex_iterator(s.begin(), s.end(), or_regex);
   words_end = std::sregex_iterator();

    if (std::distance(words_begin, words_end) > 0)
      return std::shared_ptr<QueryBase>(new OrQuery((*words_begin)[1].str(),
    (*words_begin)[2].str()));

   words_begin =
        std::sregex_iterator(s.begin(), s.end(), n_regex);
   words_end = std::sregex_iterator();

   if (std::distance(words_begin, words_end) > 0) 
	   return std::shared_ptr<QueryBase>(new NQuery((*words_begin)[1].str(),
		   (*words_begin)[3].str(), stoi((*words_begin)[2].str())));
     
   //wanted to fix it but can only submit .cpp.. :(
   
   throw std::invalid_argument("Unrecognized search");
}
////////////////////////////////////////////////////////////////////////////////
QueryResult NotQuery::eval(const TextQuery &text) const
{
  QueryResult result = text.query(query_word);
  auto ret_lines = std::make_shared<std::set<line_no>>();
  auto beg = result.begin(), end = result.end();
  auto sz = result.get_file()->size();

  for (size_t n = 0; n != sz; ++n)
  {
    if (beg==end || *beg != n)
		ret_lines->insert(n);
    else if (beg != end)
		++beg;
  }
  return QueryResult(rep(), ret_lines, result.get_file());

}

QueryResult AndQuery::eval (const TextQuery& text) const
{
  QueryResult left_result = text.query(left_query);
  QueryResult right_result = text.query(right_query);

  auto ret_lines = std::make_shared<std::set<line_no>>();
  std::set_intersection(left_result.begin(), left_result.end(),
      right_result.begin(), right_result.end(),
      std::inserter(*ret_lines, ret_lines->begin()));

  return QueryResult(rep(), ret_lines, left_result.get_file());
}

QueryResult OrQuery::eval(const TextQuery &text) const
{
  QueryResult left_result = text.query(left_query);
  QueryResult right_result = text.query(right_query);

  auto ret_lines =
      std::make_shared<std::set<line_no>>(left_result.begin(), left_result.end());

  ret_lines->insert(right_result.begin(), right_result.end());

  return QueryResult(rep(), ret_lines, left_result.get_file());
}
/////////////////////////////////////////////////////////
QueryResult NQuery::eval(const TextQuery &text) const
{
	regex words_regex("([\\w']+)");

	QueryResult left_result = text.query(left_query);
	QueryResult right_result = text.query(right_query);
	auto ret_lines = std::make_shared<std::set<line_no>>();

	// we want lines that have both words
	std::set_intersection(left_result.begin(), left_result.end(),
		right_result.begin(), right_result.end(),
		std::inserter(*ret_lines, ret_lines->begin()));

	for (auto set_it = ret_lines->begin(); set_it != ret_lines->end();)
	{
		//need to use copy inorder to not fuck up the iterator
		auto current = set_it++;
		const string curr_line = left_result.get_file()->at(*current);
		
		istringstream tokenStream(curr_line);
		string token;
		char delimiter = ' ';
		size_t counter = 0;
		bool found_left, found_right = false;
		size_t left_counter = 0;
		size_t right_counter = 0;
		bool match = false;

		while (getline(tokenStream, token, delimiter)) {
			//parse curr word
			auto words_begin = std::sregex_iterator(token.begin(), token.end(), words_regex);
			auto words_end = std::sregex_iterator();

			//regex the current word from file.
			if (std::distance(words_begin, words_end) > 0)
				token = (*words_begin)[1].str();

			if (token.compare(left_query) == 0) {
				found_left = true;
				left_counter = counter;
			}else if(token.compare(right_query) == 0) {
				found_right = true;
				right_counter = counter;
			}
			//dist + 1 since its n words between the 2 words!
			if (abs((int) (right_counter - left_counter)) <= dist + 1 && found_left && found_right) {
				match = true;
				break;
			}
			match = false;
			counter++;
		}
		if (!match)
			ret_lines->erase(current);
	}

	return QueryResult(rep(), ret_lines, left_result.get_file());
}
/////////////////////////////////////////////////////////


//UrQuery::UrQuery()
//{
//}

//QueryResult UrQuery::eval(const TextQuery& text) const
//{
//	auto ret_lines = std::make_shared<std::set<line_no>>();
//	auto query = text.query("");
//	return QueryResult(rep(), ret_lines, query.get_file());
//}
//std::string UrQuery::rep() const {
//	return "Unrecognized Search";
//}

