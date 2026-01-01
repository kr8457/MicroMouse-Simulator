# Animation Improvements

## Wall Follower Animation
- The Wall Follower algorithm now reports its heading (direction).
- The UI now renders a directional pointer (triangle) for the head of the solver when a heading is available.
- This ensures that when the Wall Follower turns in place (without moving cells), the rotation is visible to the user, mimicking a real mouse's behavior.

## Flood Fill Visualization
- The Flood Fill algorithm now passes its distance map to the UI.
- The UI renders the distance value (digit) in each cell as it is visited/updated.
- This allows visualizing the flood spread and the distance values propagating through the maze.
