#include "precompiled.h"
#include "World.h"

#include "../Logging/Logger.h"
using namespace Logging;

#include "../System/ISystem.hpp"

#include "WorldEntity.h"
#include "WorldEntityFactory.h"

#include "Serilaization/XMLSerializer.h"
using namespace Serialization;

#include "../IO/IStream.hpp"
using namespace IO;

namespace State
{
  World::~World()
  {
    for (ISystemScene::SystemSceneMap::reverse_iterator i = m_systemScenes.rbegin(); i != m_systemScenes.rend(); ++i)
    {
      delete (*i).second;
    }

    delete m_serializer;
    delete m_entityFactory;
  }
  
  IWorldEntity* World::CreateEntity(const std::string& name)
  {
    IWorldEntity* entity = m_entityFactory->CreateEntity(name);
    m_entities.insert(std::make_pair(name, entity));
    return entity;
  }

  IWorldEntity* World::CreateEntity(const std::string& name, const std::string& filePath, const std::string& entityType)
  {
    IWorldEntity* entity = this->CreateEntity(name);
    m_serializer->DeSerializeEntity(entity, filePath);
    
    entity->SetAttribute(System::Attributes::EntityType, entityType);
    entity->SetAttribute(System::Attributes::FilePath, filePath);

    entity->Initialize(); 

    return entity;
  }

  void World::DestroyEntity(const std::string& name)
  {
    IWorldEntity::WorldEntityMap::iterator entity = m_entities.find(name);

    if (entity != m_entities.end())
    {
      ISystemComponent::SystemComponentList components = (*entity).second->GetComponents();

      for(ISystemComponent::SystemComponentList::iterator c = components.begin(); c != components.end(); ++c)
      {
        m_systemScenes[ (*c)->GetAttributes()[ System::Attributes::SystemType ].As<System::Types::Type>() ]->DestroyComponent((*c));
      }

      delete (*entity).second;

      m_entities.erase(name);
    }
    else
    {
      //Warn("Attempted to delete a not existing Entity:", name);
    }
  }
  
  void World::Clear()
  {
    IWorldEntity::WorldEntityMap entitiesToErase = m_entities;

    for (IWorldEntity::WorldEntityMap::iterator e = entitiesToErase.begin(); e != entitiesToErase.end(); ++e)
    {
      this->DestroyEntity((*e).first);
    }
  }

  void World::Destroy()
  {
    this->Clear();

    for (ISystemScene::SystemSceneMap::reverse_iterator i = m_systemScenes.rbegin(); i != m_systemScenes.rend(); ++i)
    {
      (*i).second->Destroy();
    }
  }

  void World::Serialize(IO::IStream* stream)
  {
    stream->Write(m_entities.size());

    for (IWorldEntity::WorldEntityMap::iterator e = m_entities.begin(); e != m_entities.end(); ++e)
    {
      (*e).second->Serialize(stream);
    }
  }

  void World::DeSerialize(IO::IStream* stream)
  {
    int entityCount = 0;
    stream->Read(entityCount);

    for(int i = 0; i <entityCount; i++)
    {
      std::string entityName;
      stream->Read(entityName);

      int hasFilePath;
      stream->Read(hasFilePath);

      std::string entityType;
      stream->Read(entityType);

      if (m_entities.find(entityName) != m_entities.end())
      {
        m_entities[ entityName ]->DeSerialize(stream);
      }
      else
      {
        std::stringstream filePath;
        filePath <<"/data/entities/" <<entityType <<".xml";

        IWorldEntity* entity = this->CreateEntity(entityName, filePath.str(), entityType);
        entity->DeSerialize(stream);
      }
    }
  }

  void World::Update(float deltaMilliseconds)
  {
    m_serializer->Update(deltaMilliseconds);
  }

  void World::LoadLevel(const std::string& levelpath)
  {
    m_serializer->DeSerializeLevel(this, levelpath);
  }

  AnyType::AnyTypeMap World::ProcessMessage(const System::MessageType& message, AnyType::AnyTypeMap parameters)
  {
    if(message == System::Messages::Entity::DeSerializeWorld)
    {
      this->DeSerialize(parameters[ System::Parameters::IO::Stream ].As<IStream*>());
    }

    if(message == System::Messages::Entity::SerializeWorld)
    {
      this->Serialize(parameters[ System::Parameters::IO::Stream ].As<IStream*>());
    }

    if (message == System::Messages::Entity::CreateEntity)
    {
      this->CreateEntity(parameters[ System::Attributes::Name ].As<std::string>(), parameters[ System::Attributes::FilePath ].As<std::string>(), parameters[ System::Attributes::EntityType ].As<std::string>());
    }

    if (message == System::Messages::Entity::DestroyEntity)
    {
      this->DestroyEntity(parameters[ System::Attributes::Name ].As<std::string>());
    }

    return AnyType::AnyTypeMap();
  }
}