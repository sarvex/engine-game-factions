/*!
	@file
	@author		Evmenov Georgiy
	@author		Alexander Ptakhin
	@date		01/2009
	@module
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
#ifndef __MYGUI_RENDER_BOX_H__
#define __MYGUI_RENDER_BOX_H__

#include "MyGUI_Prerequest.h"
#include "MyGUI_Canvas.h"

namespace MyGUI
{

	typedef delegates::CDelegate1<RenderBoxPtr> EventInfo_RenderBoxPtr;

	/** @brief Widget that show one entity or anything from viewport.

		This widget can show autorotaded and rotatable by mouse mesh.
		Also you can set your own Ogre::Camera and yo'll see anything from your viewport.
	*/
	class MYGUI_EXPORT RenderBox : public Canvas
	{
		// для вызова закрытого конструктора
		friend class factory::BaseWidgetFactory<RenderBox>;

		MYGUI_RTTI_CHILD_HEADER( RenderBox, Canvas );

	public:
		/** Set any user created Camera instead of showing one mesh*/
		void setCamera(Ogre::Camera * _camera);

		/** Removes camera. Renders nothing */
		void removeCamera();

		//! @copydoc Widget::setPosition(const IntPoint & _point)
		virtual void setPosition(const IntPoint & _point);
		//! @copydoc Widget::setSize(const IntSize& _size)
		virtual void setSize(const IntSize & _size);
		//! @copydoc Widget::setCoord(const IntCoord & _coord)
		virtual void setCoord(const IntCoord & _coord);

		/** @copydoc Widget::setPosition(int _left, int _top) */
		void setPosition(int _left, int _top) { setPosition(IntPoint(_left, _top)); }
		/** @copydoc Widget::setSize(int _width, int _height) */
		void setSize(int _width, int _height) { setSize(IntSize(_width, _height)); }
		/** @copydoc Widget::setCoord(int _left, int _top, int _width, int _height) */
		void setCoord(int _left, int _top, int _width, int _height) { setCoord(IntCoord(_left, _top, _width, _height)); }


		/** Set colour behind entity.*/
		void setBackgroungColour(const Ogre::ColourValue& _backgroundColour);
		/** Get colour behind entity.*/
		const Ogre::ColourValue& getBackgroungColour() { return mBackgroungColour; }


	/*event:*/
		/** Event : Viewport size was changed\n
			signature : void method(MyGUI::RenderBoxPtr _sender)\n
			@param _sender
		 */
		EventInfo_RenderBoxPtr eventUpdateViewport;


	protected:
		RenderBox(WidgetStyle _style, const IntCoord& _coord, Align _align, WidgetSkinInfo* _info, Widget* _parent, ICroppedRectangle * _croppedParent, IWidgetCreator * _creator, const std::string & _name);
		virtual ~RenderBox();

		void baseChangeWidgetSkin(WidgetSkinInfoPtr _info);

		void preTextureChanges( MyGUI::CanvasPtr _canvas );

		void requestUpdateCanvas( MyGUI::CanvasPtr _canvas, MyGUI::Canvas::Event _canvasEvent );

	private:
		void initialiseWidgetSkin(WidgetSkinInfoPtr _info);
		void shutdownWidgetSkin();

	private:
		Ogre::RenderTexture* mRenderTexture;
		Ogre::Camera* mCamera;
		Ogre::Viewport* mViewport;
		Ogre::ColourValue mBackgroungColour;

	};

} // namespace MyGUI

#endif // __MYGUI_RENDER_BOX_H__
