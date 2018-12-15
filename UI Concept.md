# UI Concept for xqlib

- Separate ui logic from ui layout
- Use component system
  - Widget (generic rectangular UI thingy)
    - has: `rect area` (absolute position on the screen)
    - has: `entity parent`
  - Layouts
    - Stacking (just pack widget nicely together in one direction)
    - Docking (pack widgets to the edges of their container)
    - Overflow (similar to *Stacking*, but will overflow into a new row)
    - Table (has rows and columns for widgets)
    - Canvas (provides relative positioning to the parent)
  - Controls
    - Button
    - CheckBox
    - RadioButton
    - Label
    - TextBox (single line)
    - TextBox (multi line)
    - Picture
  - Styles
    - provide cascading styling for different controls





missing: scrollbox