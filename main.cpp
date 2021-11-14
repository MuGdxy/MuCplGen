#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <MuCplGen/EasyScanner.h>
#include <MuCplGen/FileLoader.h>
#include <MuCplGen/Highlight.h>
#include <MuCplGen/Parser.h>
#include <MuCplGen/SyntaxDirected.h>
#include <Examples/ILGenerator/ILGenerator.h>
#include <any>
//#include <MuCplGen/Scanner.h>

using namespace MuCplGen;

struct MyTestCompiler : SyntaxDirected<SLRParser<size_t>>
{
	MyTestCompiler(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
		Initialize();
	}
};



int main()
{
	auto input_text = FileLoader("assignment_test.txt");
	EasyScanner easyScanner;
	auto token_set = easyScanner.Scann(input_text);
	Highlight(input_text, token_set);
	ILGenerator ILGen("complete_syntax.syn");
	////MyTestCompiler test("complete_syntax.syn");
	////test.Parse(input_text, token_set);
	ILGen.Parse(input_text, token_set);
	ILGen.HighlightIfHasError();
	ILGen.ShowILCode();
}


//#include <iostream>
//#include <iterator>
//#include <string>
//#include <regex>
//
//int main()
//{
//    //std::string s = "Some people, when confronted with a problem, think "
//    //    "\"I know, I'll use regular expressions.\" "
//    //    "Now they have two problems.";
//
//    //std::regex self_regex("REGULAR EXPRESSIONS",
//    //    std::regex_constants::ECMAScript | std::regex_constants::icase);
//    //if (std::regex_search(s, self_regex)) {
//    //    std::cout << "Text contains the phrase 'regular expressions'\n";
//    //}
//
//    //std::regex word_regex("(\\w+)");
//    //auto words_begin =
//    //    std::sregex_iterator(s.begin(), s.end(), word_regex);
//    //auto words_end = std::sregex_iterator();
//
//    //std::cout << "Found "
//    //    << std::distance(words_begin, words_end)
//    //    << " words\n";
//
//    //const int N = 6;
//    //std::cout << "Words longer than " << N << " characters:\n";
//    //for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
//    //    std::smatch match = *i;
//    //    std::string match_str = match.str();
//    //    if (match_str.size() > N) {
//    //        std::cout << "  " << match_str << '\n';
//    //    }
//    //}
//
//    //std::regex long_word_regex("(\\w{7,})");
//    //std::string new_s = std::regex_replace(s, long_word_regex, "[$&]");
//    //std::cout << new_s << '\n';
//
//    std::string s = "float i = 0.067 xx";
//    std::regex regex("(\\-|\\+)?\\d+(\\.\\d+)?");
//    std::smatch m;
//    auto res = std::regex_search(s.cbegin(),s.cend(), m, regex);
//    auto next = m.suffix().first;
//    auto l = std::distance(s.cbegin(), next);
//    auto str = m.str();
//    auto& pre = m.prefix().second;
//    auto& suf = m.suffix().first;
//
//    auto input_text = FileLoader("test_scanner.txt");
//    Scanner scanner;
//    Scanner::ScannRule num_rule;
//    num_rule.tokenType = "number";
//    num_rule.expression = "^(\\-|\\+)?\\d+(\\.\\d+)?";
//
//    Scanner::ScannRule id;
//    id.tokenType = "identifier";
//    id.expression = "^\\w+";
//
//    Scanner::ScannRule blank;
//    blank.tokenType = "blank";
//    blank.expression = "^( |\n|\t)+";
//
//
//    Scanner::ScannRule op;
//    op.tokenType = "operator";
//    op.expression = "^(\\+|\\-|\\*|/)";
//
//    Scanner::ScannRule assign;
//    assign.tokenType = "assignment";
//    assign.expression = "^=";
//
//    Scanner::ScannRule sep;
//    sep.tokenType = "separator";
//    //sep.expression = "^(\\.|->|::|\\{|\\}|\\(|\\)|\\[|\\]|\\,)";
//
//
//    scanner.rules.push_back(num_rule);
//    scanner.rules.push_back(id);
//    scanner.rules.push_back(op);
//    scanner.rules.push_back(blank);
//    scanner.rules.push_back(assign);
//    scanner.rules.push_back(sep);
//    scanner.defaultAction = [](std::smatch match, BaseToken& token)
//    {
//        std::cout << "token:" << token.Type() << "=" << token.name << "|" << token.start << "-" << token.end << std::endl;
//        return ScannActionResult::None;
//    };
//    scanner.Scann(input_text);
//}
