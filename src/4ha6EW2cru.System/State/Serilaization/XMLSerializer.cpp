#include "precompiled.h"
#include "XMLSerializer.h"

using namespace State;

#include "../../Logging/Logger.h"
using namespace Logging;

#include "../../IO/IResource.hpp"
using namespace Resources;

#include "../../System/SystemTypeMapper.hpp"

using namespace Events;

#include "ComponentSerializerFactory.h"
#include "AIComponentSerializer.h"
#include "GeometryComponentSerializer.h"
#include "GraphicsComponentSerializer.h"
#include "InputComponentSerializer.h"
#include "PhysicsComponentSerializer.h"
#include "ScriptComponentSerializer.h"
//#include "..\IWorldEntity.hpp"

using namespace ticpp;

#include "../../IO/IStream.hpp"
using namespace IO;

#include "../../Utility/StringUtils.h"
using namespace Utility;

#include "../../Events/EventData.hpp"
#include "../../Events/Event.h"
using namespace Events;

namespace Serialization
{
  XMLSerializer::~XMLSerializer()
  {
    while(m_loadQueueEl.size()> 0)
    {
      delete m_loadQueueEl.front();
      m_loadQueueEl.pop();
    }
  }
  
  void XMLSerializer::DeSerializeLevel(IWorld* world, const std::string& levelPath)
  {
    if (!m_resourceCache->ResourceExists(levelPath))
    {
      std::stringstream logMessage;
      logMessage <<"Unable to locate level file at path: " <<levelPath;
      //Warn(logMessage.str());
      return;
    }

    m_world = world;
    m_loadProgress = 0;
    m_loadTotal = 0;

    IEventData* eventData = new UIEventData("WORLD_LOADING_STARTED");
    m_eventManager->QueueEvent(new Event(EventTypes::WORLD_LOADING_STARTED, eventData));
  
    IResource* resource = m_resourceCache->GetResource(levelPath);
    Document levelFile(resource->GetFileBuffer()->fileBytes);

    this->LoadElement(levelFile.FirstChildElement());
  }

  void XMLSerializer::LoadElement(ticpp::Element* element)
  {
    for(Iterator<Element> child = element->FirstChildElement(false); child != child.end(); child++)
    {
      std::string elementName;
      (*child).GetValue(&elementName);

      if (elementName == "color" || elementName == "entity")
      {
        m_loadQueueEl.push((*child).ClonePtr());
        m_loadTotal++;
      }

      this->LoadElement(&(*child));
    }
  }

  void XMLSerializer::DeserializeElement(ticpp::Element* element)
  {
    std::string elementName;
    element->GetValue(&elementName);

    if (elementName == "entity")
    {
      this->LoadEntity(element);
    }
    else if (elementName == "color")
    {
      this->LoadColor(element);
    }
  }
  
  void XMLSerializer::LoadColor(ticpp::Element* element)
  {
    if (m_systemManager->HasSystem(System::Types::RENDER))
    {
      std::string key;

      float red = 0;
      float green = 0;
      float blue = 0;

      element->GetAttribute("type", &key);
      element->GetAttribute("r", &red);
      element->GetAttribute("g", &green);
      element->GetAttribute("b", &blue);

      AnyType::AnyTypeMap parameters;
      parameters[ "r" ] = red;
      parameters[ "g" ] = green;
      parameters[ "b" ] = blue;

      ISystem* graphicsSystem = m_systemManager->GetSystem(System::Types::RENDER);

      graphicsSystem->SetAttribute(key, parameters);
    }
  }

  void XMLSerializer::ImportEntity(const std::string& src, NodePtrMap& components)
  {
    IResource* resource = m_resourceCache->GetResource(src);
    Document externalFile(resource->GetFileBuffer()->fileBytes);

    std::string externalSource;
    externalFile.FirstChildElement(false)->GetAttribute("src", &externalSource, false);

    if (!externalSource.empty())
    {
      this->ImportEntity(externalSource, components);
    }

    for(Iterator<Element> child = externalFile.FirstChildElement(false); child != child.end(); child++)
    {
      this->LoadEntityComponents(&*child, components);
    }
  }

  IWorldEntity* XMLSerializer::CreateEntity(const std::string& name, NodePtrMap& components)
  {
    IWorldEntity* entity = m_world->CreateEntity(name);

    this->PopulateEntity(entity, components);

    return entity;
  }

  void XMLSerializer::PopulateEntity(IWorldEntity* entity, NodePtrMap& components)
  {
    for(NodePtrMap::iterator i = components.begin(); i != components.end(); ++i)
    {
      if (m_systemManager->HasSystem((*i).first)) 
      {
        IComponentSerializer* serializer = ComponentSerializerFactory::Create((*i).first);
        ISystemComponent* component = serializer->DeSerialize(entity->GetName(), (*i).second->ToElement(), m_world->GetSystemScenes());
        entity->AddComponent(component);

        delete serializer;
      }

      delete (*i).second;
    }
  }
  
  void XMLSerializer::LoadEntity(ticpp::Element* element)
  {   
    std::string src;
    element->GetAttribute("src", &src, false);

    NodePtrMap components;

    if (!src.empty())
    {
      this->ImportEntity(src, components);
    }

    this->LoadEntityComponents(element, components);

    std::string name;
    element->GetAttribute("name", &name);

    IWorldEntity* entity = this->CreateEntity(name, components);
    entity->Initialize();
  }

  void XMLSerializer::LoadEntity(const std::string& name, const std::string& entityFilePath)
  {
    NodePtrMap components;

    this->ImportEntity(entityFilePath, components);
    IWorldEntity* entity = this->CreateEntity(name, components);

    entity->SetAttribute(System::Attributes::FilePath, entityFilePath);
    entity->Initialize();
  }

  void XMLSerializer::LoadEntityComponents(ticpp::Element* element, NodePtrMap& components)
  {
    for(Iterator<Element> child = element->FirstChildElement(false); child != child.end(); child++)
    {
      std::string elementName;
      child->GetValue(&elementName);

      if (elementName == "components")
      {
        for(Iterator<Element> component = (*child).FirstChildElement(false); component != component.end(); component++)
        {
          std::string system;
          (*component).GetAttribute("system", &system);

          System::Types::Type systemType = System::SystemTypeMapper::StringToType(system);

          if (components.find(systemType) != components.end())
          {
            delete components[ systemType ];
          }

          components[ systemType ] = (*component).ClonePtr();
        }
      }
    }
  }
  
  void XMLSerializer::Update(float deltaMilliseconds)
  {
    if (!m_loadQueueEl.empty())
    {
      this->DeserializeElement(m_loadQueueEl.front()->ToElement());

      delete m_loadQueueEl.front();

      m_loadQueueEl.pop();

      float progressPercent = ((float) ++m_loadProgress / (float) m_loadTotal) * 100.0f;

      std::stringstream progress;
      progress <<static_cast<int>(progressPercent);

      IEventData* eventData = new UIEventData(progress.str());
      m_eventManager->QueueEvent(new Event(EventTypes::WORLD_LOADING_PROGRESS, eventData));

      if (m_loadQueueEl.empty())
      {
        m_loadProgress = m_loadTotal;

        m_eventManager->QueueEvent(new Event(EventTypes::WORLD_LOADING_FINISHED));

        m_serviceManager->MessageAll(System::Messages::Network::Client::LevelLoaded, AnyType::AnyTypeMap());
      }
    }
  }

  void XMLSerializer::DeSerializeEntity(State::IWorldEntity* entity, const std::string& filepath)
  {
    NodePtrMap components;

    this->ImportEntity(filepath, components);
    this->PopulateEntity(entity, components);
  }
}