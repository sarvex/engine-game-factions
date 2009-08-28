#include "LuaState.h"

using namespace luabind;
using namespace luabind::adl;

extern "C" 
{
#	include <lua.h>
#	include <lualib.h>
}

#include "Exceptions/OutOfRangeException.hpp"

#include "Logging/Logger.h"
using namespace Logging;

namespace Script
{
	void LuaState::Initialize( )
	{
		m_state = lua_open( );

		luaL_openlibs( m_state );
		luabind::open( m_state );
	}

	void LuaState::Destroy( )
	{
		lua_close( m_state );
		m_state = 0;
	}

	ILuaState* LuaState::CreateChild( )
	{
		LuaState* childState = new LuaState( );

		int result = lua_checkstack( m_state, LUA_MINSTACK );

		if( !result )
		{
			OutOfRangeException e( "Unable to grow the LUA stack to the required size" );
			Fatal( e.what( ) );
			throw e;
			std::terminate( );
		}

		int top = lua_gettop( m_state ); 
		lua_getfield( m_state, LUA_REGISTRYINDEX, "Scripts" ); // top + 1 

		childState->m_state = lua_newthread( m_state ); // top + 2 

		lua_newtable( m_state );  // a global table for this script 
		lua_newtable( m_state );  // meta table 

		lua_getfenv( m_state,top + 2 ); // that returns the global table (we are going to protect) 
		lua_setfield( m_state, -2, "__index" ); // set global table as __index of the thread 
		lua_setmetatable( m_state, -2 );
		lua_setfenv( m_state, top + 2 );  // set environment of the new thread

		return childState;
	}

	void LuaState::RegisterTypes( const luabind::scope& classScope )
	{
		module( m_state )
		[
			classScope
		];
	}

	void LuaState::LoadScript( const std::string& scriptPath )
	{

	}

	void LuaState::Execute()
	{
		try
		{
			resume< void >( m_state );
		}
		catch( error& e )
		{
			object error_msg( from_stack( e.state( ) , -1) );
			std::stringstream logMessage;
			logMessage << error_msg;
			Warn( logMessage.str( ) );
		}
	}

	void LuaState::ExecuteString( const std::string& scriptString )
	{
		if ( luaL_dostring( m_state, scriptString.c_str( ) ) )
		{
			std::stringstream errorMessage;
			errorMessage << lua_tostring( m_state, -1 );
			Warn( errorMessage.str( ) );
			lua_pop( m_state, 1 );
		}
	}
}