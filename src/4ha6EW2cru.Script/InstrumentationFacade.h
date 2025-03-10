/*!
*  @company Black Art Studios
*  @author Nicholas Kostelnik
*  @file   InstrumentationFacade.h
*  @date   2009/07/08
*/
#pragma once
#ifndef INSTRUMENTATIONFACADE_H
#define INSTRUMENTATIONFACADE_H

#include <luabind/luabind.hpp>
#include "System/SystemType.hpp"
#include "IScriptFacade.hpp"
#include "System/IInstrumentation.hpp"

namespace Script
{
  /*! 
   *  A Facade interface to the Instrumentation Provider
   */
  class GAMEAPI InstrumentationFacade : public IScriptFacade
  {

  public:

    /*! Default Destructor
     *
     *  @return ()
     */
    ~InstrumentationFacade() { };


    /*! Default Constructor
    *
    * @return ()
    */
    InstrumentationFacade(System::IInstrumentation* instrumentation)
      : m_instrumentation(instrumentation)
    {

    }


    /*! Registers the Script functions with the given state
    *
    * @param[in] lua_State * state
    * @return (void)
    */
    static luabind::scope  RegisterFunctions();


    /*! Returns the Name that the Facade will use in script
    *
    * @return (std::string)
    */
    inline std::string GetName() { return "instrumentation"; };


    /*! Gets the FPS Statistic for the Game
    *
    * @return (int)
    */
    inline int GetFPS() { return m_instrumentation->GetFPS(); };


    /*! Returns the Round Time for the given Queue
    *
    * @param[in] const System::Queues::Queue & queue
    * @return (float)
    */
    inline float GetRoundTime(const System::Queues::Queue& queue) { return m_instrumentation->GetRoundTime(queue); };


    /*! Initializes the Facade with the given ScriptComponent
    *
    * @return (void)
    */
    void Initialize() { };

  private:

    InstrumentationFacade(const InstrumentationFacade & copy) { };
    InstrumentationFacade & operator = (const InstrumentationFacade & copy) { return *this; };

    System::IInstrumentation* m_instrumentation;
    
  };
};

#endif
