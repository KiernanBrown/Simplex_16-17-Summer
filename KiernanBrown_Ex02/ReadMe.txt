Programmer: Kiernan Brown (kwb6721@g.rit.edu)

This program is an implementation of the A* Pathfinding Algorithm, as it would be used in a Strategy RPG
such as Fire Emblem, Devil Survivor, Stella Glow, or Final Fantasy Tactics. Given a map that is split up
into a grid, where each space on the grid is a MapNode, enemies will need to find the shortest path to the
player. MapNodes might not be walkable, or might have a higher terrain cost than other nodes, which will
factor into our pathfinding (nodes that aren't walkable are not used, nodes with lower terrain costs are
favored). Users are able to alter the walkability and terrain cost of nodes in the map and move the location
of the player, which will change our pathfinding accordingly. There is a plane overlay which allows the user
to see which nodes are walkable and what the terrain cost of each MapNode is.

User Controls:
WASD: Camera Movement

Right Click: Allows the camera to be rotated by moving the mouse

O: Toggle the plane overlay. This overlay displays a plane over every MapNode, which shows the MapNode's
position, terrain cost, and walkability. MapNodes displayed in Red are not walkable and will not be used
when finding a path. MapNodes displayed in Green have a terrain cost of 1, Yellow have a terrain cost of 2
and orange have a terrain cost of 3. These terrain costs factor into our pathfinding.

Arrows: Change your selected node. The selected node is displayed by an upside down purple cone floating
above the MapNode. This node can be changed by using the arrow keys to navigate the map. The way that the
current node changes will adjust (to the best of it's ability) based on the direction of the camera.

T: Toggle the walkability of the selected node.

+: Increase the terrain cost of the selected node. The terrain cost in this version has a max of 3.

-: Decrease the terrain cost of the selected node. The terrain cost in this version has a min of 1.

P: Move the player to the selected node. This will instantly reposition the player to be in your selected
node.

R: Reset enemies to their starting nodes. This will instantly reposition all 3 enemies back to their starting
nodes.

Spacebar: Start pathfinding for all enemies. This finds from each enemy to the player, and all of the enemies
will begin following their path to the player. If no path can be found to the player, then the enemy will not
move. Once enemies have started following their paths, the paths cannot be altered. Any changes you make to
the terrain cost of a node or walkability of a node will be ignored while enemies are moving. Enemies will
also not reset to their starting position if they are in the middle of following their path, so any attempts
to do so will be ignored.


References:
Map created by me, using the Sprytile tool: https://chemikhazi.itch.io/sprytile
Tilesets created by Jeiel Aranal (https://chemikhazi.itch.io/sprytile) and
adrix89 (https://opengameart.org/content/lpc-tile-atlas)