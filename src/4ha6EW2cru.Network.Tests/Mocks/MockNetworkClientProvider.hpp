/*!
*  @company Black Art Studios
*  @author Nicholas Kostelnik
*  @file   MockNetworkClientProvider.hpp
*  @date   2009/08/18
*/
#pragma once
#ifndef MOCKNETWORKCLIENTPROVIDER_HPP
#define MOCKNETWORKCLIENTPROVIDER_HPP

#include "INetworkClientProvider.hpp"
using namespace Network;

#include <gmock/gmock.h>

namespace
{
  class MockNetworkClientProvider : public INetworkClientProvider
  {

  public:
    
    MOCK_METHOD1(Connect, void(const std::string&));
    MOCK_METHOD0(Disconnect, void());
    MOCK_METHOD1(Initialize, void (int));
    MOCK_METHOD1(Update, void(float));
    MOCK_METHOD3(Message, void(ISystemComponent*, const System::MessageType&, AnyType::AnyTypeMap));
    MOCK_METHOD0(Destroy, void());
    MOCK_METHOD1(SelectCharacter, void(const std::string&));
    MOCK_METHOD0(FindServers, void());
    MOCK_METHOD1(GetServerAdvertisement, IServerAdvertisement*(int));
    MOCK_METHOD1(SetPassive, void(bool));
    MOCK_METHOD0(LevelLoaded, void ());

  };
};

#endif