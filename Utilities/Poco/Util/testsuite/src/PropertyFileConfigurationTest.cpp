//
// PropertyFileConfigurationTest.cpp
//
// $Id$
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "PropertyFileConfigurationTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Util/PropertyFileConfiguration.h"
#include "Poco/AutoPtr.h"
#include "Poco/Exception.h"
#include <sstream>
#include <algorithm>


using Poco::Util::PropertyFileConfiguration;
using Poco::Util::AbstractConfiguration;
using Poco::AutoPtr;
using Poco::NotFoundException;


PropertyFileConfigurationTest::PropertyFileConfigurationTest(const std::string& name): CppUnit::TestCase(name)
{
}


PropertyFileConfigurationTest::~PropertyFileConfigurationTest()
{
}


void PropertyFileConfigurationTest::testLoad()
{
	static const std::string propFile = 
		"! comment\n"
		"! comment\n"
		"prop1=value1\n"
		"prop2 = value2  \n"
		"prop3.prop31: value3\n"
		"# comment\n"
		"  prop3.prop32: value 4\n"
		"prop3.prop33: value5, value6, \\\n"
		"value7, value8, \\\r\n"
		"value9\n"
		"prop4 = escaped[\\t\\r\\n\\f]\n"
		"#prop4 = foo\n"
		"prop5:foo";
		
	std::istringstream istr(propFile);
	AutoPtr<PropertyFileConfiguration> pConf = new PropertyFileConfiguration(istr);
	
	assert (pConf->getString("prop1") == "value1");
	assert (pConf->getString("prop2") == "value2");
	assert (pConf->getString("prop3.prop31") == "value3");
	assert (pConf->getString("prop3.prop32") == "value 4");
	assert (pConf->getString("prop3.prop33") == "value5, value6, value7, value8, value9");
	assert (pConf->getString("prop4") == "escaped[\t\r\n\f]");
	assert (pConf->getString("prop5") == "foo");
	
	AbstractConfiguration::Keys keys;
	pConf->keys(keys);
	assert (keys.size() == 5);
	assert (std::find(keys.begin(), keys.end(), "prop1") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop2") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop3") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop4") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop5") != keys.end());
	
	pConf->keys("prop3", keys);
	assert (keys.size() == 3);
	assert (std::find(keys.begin(), keys.end(), "prop31") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop32") != keys.end());
	assert (std::find(keys.begin(), keys.end(), "prop33") != keys.end());
	
	try
	{
		std::string s = pConf->getString("foo");
		fail("No property - must throw");
	}
	catch (NotFoundException&)
	{
	}
}


void PropertyFileConfigurationTest::testSave()
{
	AutoPtr<PropertyFileConfiguration> pConf = new PropertyFileConfiguration;
	
	pConf->setString("prop1", "value1");
	pConf->setInt("prop2", 42);
	
	std::ostringstream ostr;
	pConf->save(ostr);
	std::string propFile = ostr.str();
	assert (propFile == "prop1: value1\n"
	                    "prop2: 42\n");
}


void PropertyFileConfigurationTest::setUp()
{
}


void PropertyFileConfigurationTest::tearDown()
{
}


CppUnit::Test* PropertyFileConfigurationTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("PropertyFileConfigurationTest");

	CppUnit_addTest(pSuite, PropertyFileConfigurationTest, testLoad);
	CppUnit_addTest(pSuite, PropertyFileConfigurationTest, testSave);

	return pSuite;
}
