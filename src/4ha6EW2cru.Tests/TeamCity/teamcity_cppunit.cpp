/* Copyright 2008 JetBrains
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * $Revision: 41624 $
*/

#include <sstream>

#include "cppunit/Test.h"
#include "cppunit/Exception.h"

#include "teamcity_cppunit.h"

using namespace CPPUNIT_NS;
using namespace std;

namespace JetBrains {

TeamcityProgressListener::TeamcityProgressListener()
{}

TeamcityProgressListener::~TeamcityProgressListener()
{}

void TeamcityProgressListener::startTest(Test *test) {
    messages.testStarted(test->getName());
}

static string sourceLine2string(const SourceLine &sline) {
    stringstream ss;
        
    ss <<sline.fileName() <<":" <<sline.lineNumber();
    
    return ss.str();
}

void TeamcityProgressListener::addFailure(const TestFailure &failure) {
    const Exception *e = failure.thrownException();
    
    string details = e->message().details();
    
    if (e->sourceLine().isValid()) {
        details.append(" at ");
        details.append(sourceLine2string(e->sourceLine()));
        details.append("\n");
    }
    
    messages.testFailed(
        failure.failedTest()->getName(),
        e->message().shortDescription(),
        details
   );
}

void TeamcityProgressListener::endTest(Test *test) {
    messages.testFinished(test->getName());
}

void TeamcityProgressListener::startSuite(Test *test) {
    messages.suiteStarted(test->getName());
}

void TeamcityProgressListener::endSuite(Test *test) {
    messages.suiteFinished(test->getName());
}

}
