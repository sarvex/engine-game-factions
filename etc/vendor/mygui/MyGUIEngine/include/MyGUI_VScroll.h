/*!
	@file
	@author		Albert Semenov
	@date		11/2007
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
#ifndef __MYGUI_VSCROLL_H__
#define __MYGUI_VSCROLL_H__

#include "MyGUI_Prerequest.h"
#include "MyGUI_Widget.h"
#include "MyGUI_EventPair.h"

namespace MyGUI
{

	typedef delegates::CDelegate2<VScrollPtr, size_t> EventHandle_VScrollPtrSizeT;

	class MYGUI_EXPORT VScroll : public Widget
	{
		// для вызова закрытого конструктора
		friend class factory::BaseWidgetFactory<VScroll>;

		MYGUI_RTTI_CHILD_HEADER( VScroll, Widget );

	public:
		/** Set scroll range */
		void setScrollRange(size_t _range);
		/** Get scroll range */
		size_t getScrollRange() {return mScrollRange;}

		/** Set scroll position (value from 0 to range - 1) */
		void setScrollPosition(size_t _position);
		/** Get scroll position (value from 0 to range - 1) */
		size_t getScrollPosition() {return mScrollPosition;}

		/** Set scroll page
			@param _page Tracker step when buttons pressed
		*/
		void setScrollPage(size_t _page = 1) {mScrollPage = _page;}
		/** Get scroll page */
		size_t getScrollPage() {return mScrollPage;}

		/** Set scroll view page
			@param _viewPage Tracker step when pressed on scroll line
		*/
		void setScrollViewPage(size_t _viewPage = 1) {mScrollViewPage = _viewPage;}
		/** Get scroll view page */
		size_t getScrollViewPage() {return mScrollViewPage;}

		/** Get size in pixels of area where scroll moves */
		virtual int getLineSize();

		/** Set size of track in pixels
			@param _size in pixels, if less than MinTrackSize, MinTrackSize used
		*/
		virtual void setTrackSize(int _size);
		/** Get size of track in pixels */
		virtual int getTrackSize();

		/** Set minimal track size (used for setTrackSize)*/
		void setMinTrackSize(int _size) {mMinTrackSize = _size;}
		/** Get minimal track size */
		int getMinTrackSize() {return mMinTrackSize;}

		/** Enable or disable move to click mode.
			Move to click mode: Tracker moves to cursor when pressed on scroll line.\n
			Disabled (false) by default.
		*/
		void setMoveToClick(bool _begin) { mMoveToClick = _begin; }
		/** Get move to click mode flag */
		bool getMoveToClick() { return mMoveToClick; }

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


	/*event:*/
		/** Event : scroll tracker position changed.\n
			signature : void method(MyGUI::VScrollPtr _sender, size_t _position)\n
			@param _sender widget that called this event
			@param _position - new tracker position
		*/
		EventPair<EventHandle_WidgetSizeT, EventHandle_VScrollPtrSizeT> eventScrollChangePosition;


	/*obsolete:*/
#ifndef MYGUI_DONT_USE_OBSOLETE

		MYGUI_OBSOLETE("use : void Widget::setCoord(const IntCoord& _coord)")
		void setPosition(const IntCoord & _coord) { setCoord(_coord); }
		MYGUI_OBSOLETE("use : void Widget::setCoord(int _left, int _top, int _width, int _height)")
		void setPosition(int _left, int _top, int _width, int _height) { setCoord(_left, _top, _width, _height); }

#endif // MYGUI_DONT_USE_OBSOLETE

		virtual ~VScroll();

		VScroll(WidgetStyle _style, const IntCoord& _coord, Align _align, WidgetSkinInfo* _info, Widget* _parent, ICroppedRectangle * _croppedParent, IWidgetCreator * _creator, const std::string & _name);

	protected:
		

		void baseChangeWidgetSkin(WidgetSkinInfoPtr _info);

		virtual void updateTrack();
		virtual void TrackMove(int _left, int _top);

		virtual void onMouseWheel(int _rel);

		void notifyMousePressed(WidgetPtr _sender, int _left, int _top, MouseButton _id);
		void notifyMouseReleased(WidgetPtr _sender, int _left, int _top, MouseButton _id);
		void notifyMouseDrag(WidgetPtr _sender, int _left, int _top);
		void notifyMouseWheel(WidgetPtr _sender, int _rel);

	private:
		void initialiseWidgetSkin(WidgetSkinInfoPtr _info);
		void shutdownWidgetSkin();

	protected:
		// наши кнопки
		ButtonPtr mWidgetStart;
		ButtonPtr mWidgetEnd;
		ButtonPtr mWidgetTrack;
		// куски между кнопками
		ButtonPtr mWidgetFirstPart;
		ButtonPtr mWidgetSecondPart;

		// смещение внутри окна
		IntPoint mPreActionOffset;

		// диапазон на который трек может двигаться
		size_t mSkinRangeStart;
		size_t mSkinRangeEnd;

		size_t mScrollRange;
		size_t mScrollPosition;
		size_t mScrollPage; // на сколько перещелкивать, при щелчке на кнопке
		size_t mScrollViewPage; // на сколько перещелкивать, при щелчке по полосе

		int mMinTrackSize;
		bool mMoveToClick;

	}; // class VScroll : public Widget

} // namespace MyGUI

#endif // __MYGUI_VSCROLL_H__
