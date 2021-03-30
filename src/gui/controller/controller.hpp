#pragma once

/**
 * Singleton class for storing and rendering all GUI elements.
 */
class GUI
{
private:
  std::vector<GUIElement *> gui_elements;

protected:
  GUI()
  {
  }

  static GUI *singleton_;

public:
  /**
   * Singletons should not be cloneable.
   */
  GUI(GUI &other) = delete;

  /**
   * Singletons should not be assignable.
   */
  void operator=(const GUI &) = delete;

  /**
   * Returns singleton instance of the GUI object, creates a new
   * object if one does not already exist.
   * @returns pointer to shared GUI object instance
   */
  static GUI *GetInstance();

  /**
   * Add element to stored vector of elements.
   */
  void AddGUIElement(GUIElement *element);

  /**
   * Find which GUI element the mouse (x, y) position falls within.
   * @returns pointer to GUI element that was clicked
   */
  GUIElement *GetClickedElement(float x_pos, float y_pos);

  /**
   * Render all stored GUI elements.
   */
  void RenderGUI();

  /**
   * Free all GUI elements and erase all GUI* from the stored vector.
   */
  void DestroyGUI();
};
