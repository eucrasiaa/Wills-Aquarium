basic concept:

double buffer system:


cell struct:
    - char ch; (character to display)
    - fgColor, bgColor; (foreground and background colors, ansi)
    - isDirty; (flag to indicate if the cell needs to be redrawn)


terminal context:
  - width, height
  - cell *nextBuffer (state to be drawn next)
  - cell *liveBuffer (state currently displayed)
      - so double buffer will be checking next vs live and marking and updating dirty cells
      - so on update we modify nextBuffer, iterate thru it all on print and if they differ from liveBuffer we print the nextBuffer at that cells
      - isRunning (just a flag to indicate if the terminal is active)
  - Aquarium (struct)   

aquarium struct:
   - Fish inhabitants\[MAX_FISH]; (all active fish)
   - numFish; (current number of fish)
   - aqElems otherFriends[MAX_AQ_ELEMS]; (other aquarium elements like plants, rocks, etc)
   - numOtherFriends; (current number of other elements)


fish: (mutable state)
  - position (x, y) (of the top-left corner of the fish)
  - velocity (dx, dy)
  - direction (facing left or right)
  - animation frame (current frame index for animation
  - color (defined as consts so use the names)
  - fishTemplate * (pointer to readonly data defining the fish appearance and such)
  - bounceCount (if it bounces, how many times to. -1 if infinite, 0 if none)
  - wrapCount (if it wraps, how many times to. -1 if infinite, 0 if none)

fishTemplate: (read only)
  - char **frames; (2D array of characters representing different animation frames)
  - numFrames; (number of animation frames)
  - bool hasAnim (for quick checking if we need to calculate or check animation)
  - width, height;
  - speciesName (for identification)


a frame render walk thru:

initial state:
 - termianal currently displaying liveBuffer
 - liveBuffer has data, nextBuffer has  cleared to default background state
 - aquarium has some fish and elements

update cycle:
    1. Update aquarium state:
        - For each fish in aquarium:
            - Update position based on velocity
            - if bounce, check for collision and flip direction and decrement bounceCount
            - if wrap, check for out of bounds and wrap around (blehhh) and decrement wrapCount 
            - Update animation frame if hasAnim
    2. Render aquarium to nextBuffer:
        - Clear nextBuffer to default state
        - For each fish in aquarium:
            - Calculate which cells in nextBuffer the fish occupies based on position and template
            - For each occupied cell:
                - Set character from current animation frame
                - Set fgColor and bgColor
                - Mark cell as dirty if it differs from liveBuffer
        - For each other element in aquarium:
            - same as fish. just might be completely static and if so, we can skip over clearing these elements?
    3. Draw dirty cells to terminal:
        - For each cell in nextBuffer:
            - If isDirty:
                - Move cursor to cell position
                - Set fgColor and bgColor
                - Print characters
                - Update liveBuffer cell to match nextBuffer cells
                - Clear isDirty flag
    4. Swap buffers:
        - Swap pointers of nextBuffer and liveBuffer
    5. Repeat for next frame

