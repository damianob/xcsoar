/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_SCREEN_CONTAINER_WINDOW_HXX
#define XCSOAR_SCREEN_CONTAINER_WINDOW_HXX

#include "Screen/PaintWindow.hpp"

#ifndef USE_GDI
#include <list>
#endif

class WindowReference;

/**
 * A container for more #Window objects.  It is also derived from
 * #PaintWindow, because you might want to paint a border between the
 * child windows.
 */
class ContainerWindow : public PaintWindow {
protected:
#ifndef USE_GDI
  std::list<Window*> children;

  /**
   * The active child window is used to find the focused window.  If
   * this attribute is NULL, then the focused window is not an
   * (indirect) child window of this one.
   */
  Window *active_child;

  /**
   * The child window which captures the mouse.
   */
  Window *capture_child;

public:
  ContainerWindow();
  virtual ~ContainerWindow();
#endif /* !USE_GDI */

protected:
  virtual const Brush *on_color(Window &window, Canvas &canvas);

#ifndef USE_GDI
  virtual void OnDestroy();
  virtual bool OnCancelMode();
  virtual bool OnMouseMove(PixelScalar x, PixelScalar y, unsigned keys);
  virtual bool OnMouseDown(PixelScalar x, PixelScalar y);
  virtual bool OnMouseUp(PixelScalar x, PixelScalar y);
  virtual bool OnMouseDouble(PixelScalar x, PixelScalar y);
  virtual bool OnMouseWheel(PixelScalar x, PixelScalar y, int delta);

#ifdef HAVE_MULTI_TOUCH
  virtual bool OnMultiTouchDown();
  virtual bool OnMultiTouchUp();
#endif

  virtual void OnPaint(Canvas &canvas);
#else /* USE_GDI */
  virtual LRESULT OnMessage(HWND hWnd, UINT message,
                             WPARAM wParam, LPARAM lParam);
#endif

public:
#ifndef USE_GDI
  void AddChild(Window &child);
  void Removehild(Window &child);

  gcc_pure
  bool HasChild(const Window &child) const;

  void BringChildToTop(Window &child);

  void BringChildToBottom(Window &child) {
    children.remove(&child);
    children.push_back(&child);
    Invalidate();
  }

  /**
   * Locate a child window by its relative coordinates.
   */
  gcc_pure
  Window *ChildAt(PixelScalar x, PixelScalar y);

  /**
   * Locates the child which should get a mouse event.  Prefers the
   * captured child.
   */
  gcc_pure
  Window *EventChildAt(PixelScalar x, PixelScalar y);

  void SetActiveChild(Window &child);
  virtual void SetFocus();
  virtual void ClearFocus();

  /**
   * Override the Window::GetFocusedWindow() method, and search in
   * the active child window.
   */
  gcc_pure
  virtual Window *GetFocusedWindow();

  gcc_pure
  WindowReference GetFocusedWindowReference();

  void SetChildCapture(Window *window);
  void ReleaseChildCapture(Window *window);
  virtual void ClearCapture();

protected:
  gcc_pure
  static Window *FindControl(std::list<Window*>::const_iterator i,
                             std::list<Window*>::const_iterator end);

  gcc_pure
  static Window *FindControl(std::list<Window*>::const_reverse_iterator i,
                             std::list<Window*>::const_reverse_iterator end);

  gcc_pure
  Window *FindFirstControl();

  gcc_pure
  Window *FindLastControl();

  gcc_pure
  Window *FindNextChildControl(Window *reference);

  gcc_pure
  Window *FindPreviousChildControl(Window *reference);

  gcc_pure
  Window *FindNextControl(Window *reference);

  gcc_pure
  Window *FindPreviousControl(Window *reference);

public:
#endif /* !USE_GDI */

  /**
   * Sets the keyboard focus on the first descendant window which has
   * the WindowStyle::tab_stop() attribute.
   *
   * @return true if the focus has been moved
   */
  bool FocusFirstControl();

  /**
   * Sets the keyboard focus on the next descendant window which has
   * the WindowStyle::tab_stop() attribute.
   *
   * @return true if the focus has been moved
   */
  bool FocusNextControl();

  /**
   * Sets the keyboard focus on the previous descendant window which
   * has the WindowStyle::tab_stop() attribute.
   *
   * @return true if the focus has been moved
   */
  bool FocusPreviousControl();
};

#endif
