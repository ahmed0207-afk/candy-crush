# candy-crush
PF Game Project

An 8 × 8 match-three game built with C++ and SFML. Swap adjacent candies
to create horizontal or vertical matches of three or more. Reach the
target score before you run out of moves.

Group Members:
Ahmed Mahmood 26L-2524
Muhammad Mohid 26L-2506
Amir Hamza 26L-2630

## Controls

| Input | Action |
| --- | --- |
| Up / Down arrows | Change the highlighted menu option. |
| Enter | Select a menu option or return to the menu from a result screen. |
| Left click | Select a candy, then click an adjacent candy to try a swap. |
| Esc | Save progress and return to the menu. Select **Continue** to resume. |

A swap that makes a match costs one move. A swap that makes no match is
reversed for free. Each cleared candy earns 10 points, including candies
cleared in cascades.

## How to Run on Windows

1. Install Visual Studio with the **Desktop development with C++** workload.
2. Clone this repository.
3. In Visual Studio, choose **File → Open → Folder** and open the folder
   containing `CMakeLists.txt`.
4. Wait for CMake to configure the project. The first configuration needs
   an internet connection to download SFML.
5. Select `CandyCrushStarter.exe` as the startup target and press
   **Ctrl+F5** to build and run.
6. Select **New Game** and press **Enter**.

## Gameplay Screenshot

<img width="597" height="707" alt="image" src="https://github.com/user-attachments/assets/b95acd51-5ba2-47b8-b04f-f2694fa007de" />

<img width="598" height="711" alt="image" src="https://github.com/user-attachments/assets/40d5f0a2-3d64-4848-9c36-ee83ed7fd667" />
