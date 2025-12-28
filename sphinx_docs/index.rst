MicroMouse Simulator Pro
========================

The MicroMouse Simulator Pro is a high-performance simulation environment for evaluating maze-solving algorithms.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

API Documentation
=================

UI Module
---------
.. doxygenclass:: UI
   :project: MicroMouse
   :members:
   :protected-members:
   :private-members:

Maze Generator
--------------
.. doxygenclass:: MazeGen
   :project: MicroMouse
   :members:

Maze Solver Classes
-------------------
.. doxygenclass:: MazeSolver
   :project: MicroMouse
   :members:

Data Structures
---------------
.. doxygenstruct:: Cell
   :project: MicroMouse
   :members:

.. doxygenstruct:: UILayout
   :project: MicroMouse
   :members:

Algorithms
----------
The simulator supports multiple pathfinding algorithms:

* **BFS**: Breadth-First Search
* **DFS**: Depth-First Search
* **A***: Heuristic-based search
* **Flood Fill**: Distance transform algorithm
* **Wall Follower**: Left-hand rule simulation

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
