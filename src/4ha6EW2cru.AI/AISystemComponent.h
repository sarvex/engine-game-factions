/*!
*  @company Black Art Studios
*  @author Nicholas Kostelnik
*  @file   AISystemComponent.h
*  @date   2009/04/25
*/
#pragma once
#ifndef AISYSTEMCOMPONENT_H
#define AISYSTEMCOMPONENT_H

#include <string>
#include "System/SystemType.hpp"

#include "Maths/MathVector3.hpp"
#include "Maths/MathQuaternion.hpp"

#include "IBehavior.hpp"
#include "IAISystemComponent.hpp"

#include "Service/IServiceManager.h"

namespace AI
{
  /*! 
   *  An Artificial Intelligence System Scene Component
   */
  class AISystemComponent : public IAISystemComponent
  {

  public:

    /*! Default Destructor
     *
     *  @return ()
     */
    ~AISystemComponent() { };


    /*! Default Constructor
     *
     *  @param[in] const std::string & name
     *  @param[in] IScriptComponent * scriptComponent
     *  @return ()
     */
    AISystemComponent(const std::string& name, Services::IServiceManager* serviceManager)
      : m_name(name)
      , m_observer(0)
      , m_playerDistance(0)
      , m_serviceManager(serviceManager)
    {

    }

    /* Inherited from ISystemComponent */

    /*! Initializes the Component
     *
     *  @param[in] AnyType::AnyValueMap attributes
     *  @return (void)
     */
    virtual void Initialize() { };


    /*! Steps the internal data of the Component
     *
     *  @param[in] float deltaMilliseconds
     *  @return (void)
     */
    virtual void Update(float deltaMilliseconds) { };


    /*! Destroys the Component
     *
     *  @return (void)
     */
    virtual void Destroy() { };


    /*! Adds an Observer to the Component
     *
     *  @param[in] IObserver * observer
     *  @return (void)
     */
    virtual void AddObserver(IObserver* observer) { m_observer = observer; };


    /*! Posts a message to observers
    *
    *  @param[in] const std::string & message
    *  @param[in] AnyType::AnyValueMap parameters
    *  @return (AnyType)
    */
    virtual AnyType PushMessage(const System::MessageType& message, AnyType::AnyTypeMap parameters);


    /*! Messages the Component to influence its internal state
    *
    *  @param[in] const std::string & message
    *  @return (AnyType)
    */
    virtual AnyType Observe(const ISubject* subject, const System::MessageType& message, AnyType::AnyTypeMap parameters);


    /*! Gets the attributes of the Component
     *
     *  @return (AnyValueMap)
     */
    inline AnyType::AnyTypeMap GetAttributes() const { return m_attributes; };


    /*! Sets an Attribute on the Component *
    *
    *  @param[in] const unsigned int attributeId
    *  @param[in] const AnyType & value
    */
    inline void SetAttribute(const System::Attribute& attributeId, const AnyType& value) { m_attributes[ attributeId ] = value; };


    /*! Writes the contents of the object to the given stream
    *
    * @param[in] IStream * stream
    * @return (void)
    */
    void Serialize(IO::IStream* stream) { };


    /*! Reads the contents of the object from the stream
    *
    * @param[in] IStream * stream
    * @return (void)
    */
    void DeSerialize(IO::IStream* stream) { };


    /*! Returns the Name of the Component
    *
    * @return (std::string)
    */
    inline std::string GetName() const { return (*m_attributes.find(System::Attributes::Name)).second.As<std::string>(); };

  protected:

    AISystemComponent() { };
    AISystemComponent(const AISystemComponent & copy) { };
    AISystemComponent & operator = (const AISystemComponent & copy) { return *this; };

    std::string m_name;
    Services::IServiceManager* m_serviceManager;

    IObserver* m_observer;

    float m_playerDistance;

    AnyType::AnyTypeMap m_attributes;

    Maths::MathVector3::MathVector3List m_activeWaypoints;

  };
};

#endif
