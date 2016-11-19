//
//  main.cpp
//  bugmatic_tests
//
//  Created by Uli Kusterer on 19/11/2016.
//  Copyright © 2016 Uli Kusterer. All rights reserved.
//

#include <iostream>
#include <vector>
#include <sstream>
#include "bugmatic.hpp"
#include "fake_filesystem.hpp"


using namespace std;
using namespace bugmatic;
using namespace fake;


#define TEST_STR_EQUAL(a,b)		test_str_equal( (a), (b), numTests, numFailures, __FILE__, __LINE__ )
#define TEST_TRUE(expr)			test_true( (expr), #expr, numTests, numFailures, __FILE__, __LINE__ )


void	test_str_equal( const string a, const string b, size_t& numTests, size_t &numFailures, const char* filePath, size_t lineNo )
{
	++numTests;
	
	if( a.compare(b) != 0 )
	{
		++numFailures;
		cout << "error:" << filePath << ":" << lineNo << ": failed!" << endl << a << endl;
	}
	else
		cout << "note: Passed." << endl;
}


void	test_true( bool inSuccess, const char* exprStr, size_t& numTests, size_t &numFailures, const char* filePath, size_t lineNo )
{
	++numTests;
	
	if( !inSuccess )
	{
		++numFailures;
		cout << "error:" << filePath << ":" << lineNo << ": failed!" << endl << exprStr << endl;
	}
	else
		cout << "note: Passed." << endl;
}


int main(int argc, const char * argv[])
{
	size_t				numTests = 0, numFailures = 0;
	filesystem::path	test1path( filesystem::path(argv[0]).parent_path() / "testdata" / "testdata1" );
	working_copy		wc( test1path.string() );
	
	cout << "note: ===== Find all uliwitness' bugs =====" << endl;
	stringstream	output1;
	wc.list( (std::vector<std::string>){ "assignee.login=uliwitness" }, [&output1]( issue_info currIssue )
	{
		output1 << currIssue.issue_number() << "," << currIssue.title();
		vector<user_info>	assignees = currIssue.assignees();
		for( const user_info& currAssignee : assignees )
			 output1 << "," << currAssignee.user_login();
		output1 << "\n";
	} );
	TEST_STR_EQUAL( output1.str(), "4,Add support for OAuth,uliwitness\n5,Should report rate limit exceeded,uliwitness\n" );

	cout << "note: ===== Find unclosed bugs in range 1...3 =====" << endl;
	stringstream	output2;
	wc.list( (std::vector<std::string>){ "closed_at=null", "number<4" }, [&output2]( issue_info currIssue )
	{
		output2 << currIssue.issue_number() << "," << currIssue.title() << "," << currIssue.closed_at() << "\n";
	} );
	TEST_STR_EQUAL( output2.str(), "1,test,\n2,Found a bug,\n3,Found a bug,\n" );
	
	cout << "note: ===== Create a new local bug =====" << endl;
	int	bugNumber = wc.next_bug_number();
	cout << "note: next_bug_number = " << bugNumber << endl;
	string issuePath = wc.new_issue( "This is local.", "It has issues." );
	stringstream filter;
	filter << "number=" << bugNumber;
	bool	foundOne = false;
	wc.list( (std::vector<std::string>){ filter.str() }, [&numTests,&numFailures,issuePath,&foundOne]( issue_info currIssue )
	{
		TEST_TRUE( currIssue.filepath() == issuePath );
		foundOne = true;
	} );
	TEST_TRUE( foundOne );
	
	if( numFailures > 0 )
		cout << "error: "<< numFailures << " tests of " << numTests << " failed." << endl;
	else
		cout << "note: ===== All tests passed. =====" << endl;
	
    return (int)numFailures;
}
