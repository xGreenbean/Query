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
using namespace std;
////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<QueryBase> QueryBase::factory(const string& s)
{
  regex word_regex(("^\\s*(\\w+)\\s*$"));
  regex not_regex("^\\s*NOT\\s+(\\w+)\\s*$");
  regex and_regex("^\\s+(\\w+)\\s+AND\\s+(\\w+)\\s*$");
  regex or_regex("^\\s+(\\w+)\\s+OR\\s+(\\w+)\\s*$");
  regex n_regex("^\\s+(\\w+)\\s+\(d+)\\s+(\\w+)\\s*$");

  auto words_begin =
      std::sregex_iterator(s.begin(), s.end(), word_regex);
  auto words_end = std::sregex_iterator();
  if (std::distance(words_begin, words_end) > 0)
    return std::shared_ptr<QueryBase>(new WordQuery( (*words_begin)[1].str()));

    auto words_begin =
        std::sregex_iterator(s.begin(), s.end(), not_regex);
    auto words_end = std::sregex_iterator();

    if (std::distance(words_begin, words_end) > 0)
      return std::shared_ptr<QueryBase>(new NotQuery( (*words_begin)[1].str()));

      auto words_begin =
          std::sregex_iterator(s.begin(), s.end(), and_regex);
      auto words_end = std::sregex_iterator();

    if (std::distance(words_begin, words_end) > 0)
      return std::shared_ptr<QueryBase>(new AndQuery((*words_begin)[1].str(),
    (*words_begin)[2].str()));

    auto words_begin =
        std::sregex_iterator(s.begin(), s.end(), or_regex);
    auto words_end = std::sregex_iterator();

    if (std::distance(words_begin, words_end) > 0)
      return std::shared_ptr<QueryBase>(new OrQuery((*words_begin)[1].str(),
    (*words_begin)[2].str()));

    auto words_begin =
        std::sregex_iterator(s.begin(), s.end(), n_regex);
    auto words_end = std::sregex_iterator();

    if (std::distance(words_begin, words_end) > 0)
      return std::shared_ptr<QueryBase>(new NQuery((*words_begin)[1].str(),
    (*words_begin)[2].str(), (*words_begin)[3].str()));

    std::cout <<"‫‪Unrecognized‬‬ ‫‪search‬‬" << std::endl;
    return null;

  // if(s == "smart") return std::shared_ptr<QueryBase>(new WordQuery("smart"));
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
  
}
/////////////////////////////////////////////////////////
