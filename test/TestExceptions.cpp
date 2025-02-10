/***********************************************************************************************************************
** The KirHut Application Development Library
** TestExceptions.cpp
** Copyright © KirHut Software Company
**
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
***********************************************************************************************************************/

#include "kh/exception.hpp"
#include "kh/exceptions.hpp"

#include "kh/base.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace KirHut;

struct TestException : public Exception
{
	using Exception::Exception;
};

TEST_CASE("Exception String View Constructor", "[exception][constructor]")
{
	STATIC_REQUIRE(noexcept(TestException{"Test"sv}));
	TestException ex{ "Test Exception."sv };
	TestException ex2{ "Test Exception 2." };
	
	REQUIRE(ex.info() == "Test Exception."sv);
	REQUIRE("Test Exception."sv == ex.what());
	REQUIRE(ex2.info() == "Test Exception 2."sv);
	REQUIRE("Test Exception 2."sv == ex2.what());
}

TEST_CASE("Exception Copying String View Constructor", "[exception][constructor]")
{
	string startStr = "Test Exception.";
    TestException ex(make, startStr);
    startStr.clear();
	REQUIRE(ex.info() == "Test Exception."sv);
	REQUIRE("Test Exception."sv == ex.what());
}

TEST_CASE("Exception String Owning Constructor", "[exception][constructor]")
{
	STATIC_REQUIRE(noexcept(TestException{std::declval<string>()}));
	string test = "Test Exception.";
	TestException ex{ std::move(test) };
	REQUIRE(ex.info() == "Test Exception."sv);
	REQUIRE("Test Exception."sv == ex.what());
}

TEST_CASE("Exception Copy Constructor", "[exception][constructor]")
{
    STATIC_REQUIRE(std::is_nothrow_copy_constructible_v<TestException>);
    TestException ex(string{"Test Exception."});
	UPtr<TestException> ex2 = make_unique<TestException>(ex);
	UPtr<TestException> ex3 = make_unique<TestException>(*ex2);
	UPtr<TestException> ex4 = make_unique<TestException>(*ex3);
	TestException ex5(*ex4);
	
	REQUIRE(ex.info() == "Test Exception."sv);
	REQUIRE(ex2->info() == "Test Exception."sv);
	REQUIRE(ex3->info() == "Test Exception."sv);
	REQUIRE(ex4->info() == "Test Exception."sv);
	REQUIRE(ex5.info() == "Test Exception."sv);
	
	ex4.reset();
	
	REQUIRE(ex.info() == "Test Exception."sv);
	REQUIRE(ex2->info() == "Test Exception."sv);
	REQUIRE(ex3->info() == "Test Exception."sv);
	REQUIRE(ex5.info() == "Test Exception."sv);
	
	ex2.reset();
	
	REQUIRE(ex.info() == "Test Exception."sv);
	REQUIRE(ex3->info() == "Test Exception."sv);
	REQUIRE(ex5.info() == "Test Exception."sv);
	
	ex3.reset();
	
	REQUIRE(ex.info() == "Test Exception."sv);
	REQUIRE(ex5.info() == "Test Exception."sv);
}

TEST_CASE("Exception Move Constructor", "[exception][constructor]")
{
	STATIC_REQUIRE(noexcept(TestException{std::declval<TestException>()}));
	
	// TODO: Finish this.
}

void throwMe(auto const &ex)
{
	throw ex;
}

template <typename T>
bool checkException()
{
	try
	{
		// The C++ standard says that exceptions thrown in a throw expression allocate a buffer of memory and copy the
		// exception thrown to it, which is then deallocated in an unspecified way. This should be supported then.
		throwMe(T{ "Test Exception." });
	}
	catch (T &ex)
	{
		return ex.info() == "Test Exception."sv;
	}
	
	return false;
}

TEST_CASE("Throw KirHutSucksAtProgramming", "[exception][throw]")
{
	REQUIRE(checkException<KirHutSucksAtProgramming>());
	REQUIRE_THROWS_AS(throwMe(KirHutSucksAtProgramming{"Test"}), KirHutSucksAtProgramming);
}

TEST_CASE("Throw BullshitEnvironment", "[exception][throw]")
{
	REQUIRE(checkException<BullshitEnvironment>());
	REQUIRE_THROWS_AS(throwMe(BullshitEnvironment{"Test"}), BullshitEnvironment);
}

TEST_CASE("Throw IllegalArgument", "[exception][throw]")
{
	REQUIRE(checkException<IllegalArgument>());
	REQUIRE_THROWS_AS(throwMe(IllegalArgument{"Test"}), IllegalArgument);
}

TEST_CASE("Throw NotInitializedYet", "[exception][throw]")
{
	REQUIRE(checkException<NotInitializedYet>());
	REQUIRE_THROWS_AS(throwMe(NotInitializedYet{"Test"}), NotInitializedYet);
}

TEST_CASE("Throw Exception subclass", "[exception][throw]")
{
	REQUIRE(checkException<TestException>());
	REQUIRE_THROWS_AS(throwMe(TestException{"Test"}), TestException);
}
