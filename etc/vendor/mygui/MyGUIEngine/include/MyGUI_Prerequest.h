/*!
	@file
	@author		Denis Koronchik
	@author		Georgiy Evmenov
	@author		Ну и я чуть чуть =)
	@date		09/2007
*//*
	This file is part of MyGUI.
	
	MyGUI is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	MyGUI is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with MyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MyGUI_Platform.h"

#ifndef __MYGUI_PREREQUEST_H__
#define __MYGUI_PREREQUEST_H__

#if MYGUI_COMPILER == MYGUI_COMPILER_MSVC
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS
#	endif
#endif

#define MYGUI_DEFINE_VERSION(major, minor, patch) ((major << 16) | (minor << 8) | patch)

#ifndef MYGUI_DONT_REPLACE_NULLPTR
	#if MYGUI_COMPILER == MYGUI_COMPILER_MSVC
		#ifndef _MANAGED
			#define nullptr 0
		#endif
	#else
		#define nullptr 0
	#endif
#endif

#include <OgrePrerequisites.h> // for OGRE_VERSION

// для полной информации о выделении памяти
#if OGRE_VERSION < MYGUI_DEFINE_VERSION(1, 6, 0)

   #include <OgreMemoryManager.h>

   #if OGRE_DEBUG_MEMORY_MANAGER && OGRE_DEBUG_MODE

      #define MYGUI_VALIDATE_PTR(ptr) assert(ptr == 0 || Ogre::MemoryManager::instance().validateAddr(ptr))

   #else

      #define OGRE_MALLOC(bytes, category) new unsigned char[bytes]
      #define OGRE_ALLOC_T(T, count, category) new T[count]
      #define OGRE_FREE(ptr, category) { delete[] ptr; ptr=0; }

      #define OGRE_NEW_T(T, category) new T
      #define OGRE_NEW_ARRAY_T(T, count, category) new T[count]
      #define OGRE_DELETE_T(ptr, T, category) { delete ptr; ptr=0; }
      #define OGRE_DELETE_ARRAY_T(ptr, T, count, category) { delete [] ptr; ptr=0; }

      #define MYGUI_VALIDATE_PTR(ptr)

   #endif

#else

	#define MYGUI_VALIDATE_PTR(ptr)

#endif

#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <deque>
#include "MyGUI_Utility.h"
#include "MyGUI_Delegate.h"

#include "MyGUI_LastHeader.h"

namespace MyGUI
{
	class Gui;

	using MyGUI::delegates::newDelegate;

	class WidgetSkinInfo;
	class MaskPickInfo;
	class IWidgetCreator;

	// managers
	class InputManager;
	class SubWidgetManager;
	class LayerManager;
	class SkinManager;
	class WidgetManager;
	class FontManager;
	class ControllerManager;
	class PointerManager;
	class ClipboardManager;
	class LayoutManager;
	class PluginManager;
	class DynLibManager;
	class DelegateManager;
	class LanguageManager;
	class ResourceManager;

	class IWidgetFactory;

	class DynLib;

	namespace factory
	{
		template <typename T>
		class BaseWidgetFactory;

		class WidgetFactory;
		class ButtonFactory;
		class WindowFactory;
		class ListFactory;
		class HScrollFactory;
		class VScrollFactory;
		class EditFactory;
		class ComboBoxFactory;
		class StaticTextFactory;
		class TabFactory;
		class TabItemFactory;
		class ProgressFactory;
		class ItemBoxFactory;
		class MultiListFactory;
		class StaticImageFactory;
		class MessageFactory;
		class RenderBoxFactory;
		class PopupMenuFactory;
		class MenuItemFactory;
		class MenuBarFactory;
		class ScrollViewFactory;
		class DDContainerFactory;
		class CanvasFactory;
	}

	class Widget;
	class Button;
	class Window;
	class List;
	class HScroll;
	class VScroll;
	class Edit;
	class ComboBox;
	class StaticText;
	class Tab;
	class TabItem;
	class Progress;
	class ItemBox;
	class MultiList;
	class StaticImage;
	class Message;
	class RenderBox;
	class MenuCtrl;
	class MenuItem;
	class PopupMenu;
	class MenuBar;
	class ScrollView;
	class DDContainer;
	class Canvas;

	typedef Widget * WidgetPtr;
	typedef Button * ButtonPtr;
	typedef Window * WindowPtr;
	typedef List * ListPtr;
	typedef HScroll * HScrollPtr;
	typedef VScroll * VScrollPtr;
	typedef Edit * EditPtr;
	typedef ComboBox * ComboBoxPtr;
	typedef StaticText * StaticTextPtr;
	typedef Tab * TabPtr;
	typedef TabItem * TabItemPtr;
	typedef Progress * ProgressPtr;
	typedef ItemBox * ItemBoxPtr;
	typedef MultiList * MultiListPtr;
	typedef StaticImage * StaticImagePtr;
	typedef Message * MessagePtr;
	typedef RenderBox * RenderBoxPtr;
	typedef MenuCtrl * MenuCtrlPtr;
	typedef MenuItem * MenuItemPtr;
	typedef PopupMenu * PopupMenuPtr;
	typedef MenuBar * MenuBarPtr;
	typedef ScrollView * ScrollViewPtr;
	typedef DDContainer * DDContainerPtr;
	typedef Canvas * CanvasPtr;

	typedef TabItem Sheet; // OBSOLETE
	typedef TabItem * SheetPtr; // OBSOLETE

	// Define version
    #define MYGUI_VERSION_MAJOR 2
    #define MYGUI_VERSION_MINOR 3
    #define MYGUI_VERSION_PATCH 0

    #define MYGUI_VERSION    MYGUI_DEFINE_VERSION(MYGUI_VERSION_MAJOR, MYGUI_VERSION_MINOR, MYGUI_VERSION_PATCH)

	// Disable warnings for MSVC compiler
#if MYGUI_COMPILER == MYGUI_COMPILER_MSVC

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4786)

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4503)

// disable: "conversion from 'double' to 'float', possible loss of data
#   pragma warning (disable : 4244)

// disable: "truncation from 'double' to 'float'
#   pragma warning (disable : 4305)

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable : 4251)

// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

// disable: "C++ Exception Specification ignored"
// This is because MSVC 6 did not implement all the C++ exception
// specifications in the ANSI C++ draft.
#   pragma warning( disable : 4290 )

// disable: "no suitable definition provided for explicit template
// instantiation request" Occurs in VC7 for no justifiable reason on all
// #includes of Singleton
#   pragma warning( disable: 4661)

#endif

} // namespace MyGUI

#endif // __MYGUI_PREREQUEST_H__

