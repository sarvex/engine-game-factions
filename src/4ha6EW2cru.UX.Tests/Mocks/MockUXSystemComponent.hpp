/*!
*  @company Black Art Studios
*  @author Nicholas Kostelnik
*  @file   MockUXSystemComponent.hpp
*  @date   2009/09/02
*/
#pragma once
#ifndef MOCKUXSYSTEMCOMPONENT_HPP
#define MOCKUXSYSTEMCOMPONENT_HPP

#include <gmock/gmock.h>

#include "IUXSystemComponent.hpp"

namespace
{
  class MockUXSystemComponent : public IUXSystemComponent
  {

  public:

    MOCK_METHOD1(Serialize, void(IO::IStream*));
    MOCK_METHOD1(DeSerialize, void(IO::IStream*));
    MOCK_METHOD0(Initialize, void());
    MOCK_METHOD1(Update, void(float));
    MOCK_METHOD0(Destroy, void());
    MOCK_CONST_METHOD0(GetAttributes, AnyType::AnyTypeMap());
    MOCK_METHOD2(SetAttribute, void(const System::Attribute&, const AnyType&));
    MOCK_METHOD3(Observe, AnyType(const ISubject*, const System::MessageType&, AnyType::AnyTypeMap));
    MOCK_METHOD1(AddObserver, void(IObserver*));
    MOCK_METHOD2(PushMessage, AnyType(const System::MessageType&, AnyType::AnyTypeMap));

    MOCK_METHOD2(RegisterEventHandler, void(const std::string&, const luabind::object&));
    MOCK_METHOD2(RegisterEventHandler, void(const std::string&, Script::IScriptFunctionHandler*));
    MOCK_METHOD2(UnregisterEventHandler, void(const std::string&, const luabind::object&));
    MOCK_METHOD2(UnregisterEventHandler, void(const std::string&, Script::IScriptFunctionHandler*));
    MOCK_CONST_METHOD0(GetName, std::string());
    
  };
};

#endif