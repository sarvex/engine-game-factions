/*!
	@file
	@author		Albert Semenov
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
#ifndef __BASE_PANEL_VIEW_ITEM_H__
#define __BASE_PANEL_VIEW_ITEM_H__

#include <MyGUI.h>
#include "BaseLayout/BaseLayout.h"
#include "PanelView/BasePanelViewCell.h"

namespace wraps
{

	class BasePanelViewItem : public wraps::BaseLayout
	{
	public:
		BasePanelViewItem(const std::string& _layout) :
			BaseLayout("", nullptr),
			mLayout(_layout)
		{
		}

		void _initialise(BasePanelViewCell * _cell)
		{
			mPanelCell = _cell;
			mWidgetClient = mPanelCell->getClient();

			if ( ! mLayout.empty()) {
				BaseLayout::initialise(mLayout, mWidgetClient);
				mMainWidget->setCoord(0, 0, mWidgetClient->getWidth(), mMainWidget->getHeight());
				mPanelCell->setClientHeight(mMainWidget->getHeight(), false);
			}

			initialise();
		}

		void _shutdown()
		{
			shutdown();

			if ( ! mLayout.empty()) {
				BaseLayout::shutdown();
			}

			mPanelCell = 0;
			mWidgetClient = nullptr;
		}

		// ������� ���������� ������ ������
		virtual void notifyChangeWidth(int _width)
		{
		}

		virtual void setVisible(bool _visible)
		{
			mPanelCell->setVisible(_visible);
			mPanelCell->eventUpdatePanel(mPanelCell);
		}

		BasePanelViewCell * getPanelCell() { return mPanelCell; }

	protected:
		virtual void initialise() { }
		virtual void shutdown() { }

	protected:
		BasePanelViewCell * mPanelCell;
		MyGUI::WidgetPtr mWidgetClient;
		std::string mLayout;
	};

} // namespace wraps

#endif // __BASE_PANEL_VIEW_ITEM_H__
