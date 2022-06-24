# Multris
A modified version of the classic puzzle game, with randomly generated polyominoes of varying sizes.

## Table of Contents
[Showcase](#Showcase)

[How to Play](#How_to_Play)

[Building From Source](#Building_From_Source)

[Pre-Compiled Downloads](#Pre_Compiled_Downloads)

<a name="Showcase"/>

## Showcase
![Showcase](https://img.itch.zone/aW1nLzY4OTU1NjcuZ2lm/original/%2B9z%2FOo.gif)

<a name="How_to_Play"/>

## How to Play

##### DEFAULT CONTROLS
```
LEFT / RIGHT ARROW - Move. Hold to move quicker.
        DOWN ARROW - Soft Drop
          UP ARROW - Hard Drop
             SPACE - Hold current piece / Swap current piece with held piece
                 Z - Rotate CCW
                 X - Rotate CW
                 C - Mirror piece over Y-Axis
             ENTER - Select/Pause
            ESCAPE - Exit the game if pressed in the menu
``` 
##### GAME MODES
```
  MULTRIS - All pieces are a random size. 
            Top Scores are saved.
NUMERICAL - All pieces are the selected size.
            Top Score are NOT saved.
```           
##### GAMEPLAY
```
SPEED - Speed increases as level increases.
LEVEL - Level increases as you complete lines.
        The number of completed lines needed to advance to the next level increases with each level.
Score - You score points by completing lines.
        The more lines you complete at one time, the more points you get.
        Completing lines are also worth more points at higher levels.
```
##### OPTIONS
```
BLOCK SPRITE - Choose what sprite to use for the squares that make up the polyominoes
       GHOST - Choose to have a transparent "ghost" image to show where the current piece would land.
  FULLSCREEN - Enable or disable fullscreen mode
      VOLUME - Set the volume of the game audio. Between 0-9
```

<a name="Building_From_Source"/>

## Building From Source

### Requirements

###### Make sure that you have the development libraries as well as the runtime binaries installed

[SDL2](https://www.libsdl.org/download-2.0.php)

[SDL2_Image](https://www.libsdl.org/projects/SDL_image/)


#### Building the Latest Development Version (Unstable)

##### Linux

```
git clone https://github.com/RustyReich/Multris
cd Multris
make -f Makefile_Linux
```

##### Windows
`Coming Soon`

#### Building a Specific Release Version (Stable)

###### For versions 1.0.4 - 1.0.7

##### Linux

```
git clone https://github.com/RustyReich/Multris --branch v1.x.x
cd Multris
mkdir unix_obj
mkdir unix_dep
mkdir build
wget https://raw.githubusercontent.com/RustyReich/Multris/main/Makefile_Linux
make -f Makefile_Linux
cp -R ./src/AUDIO ./build/AUDIO
cp -R ./src/SPRITES ./build/SPRITES
```

#### Windows
`Coming Soon`

###### Build instructions are not available for versions before 1.0.4

###### The executable will be in `/Multris/build`

<a name="Pre_Compiled_Downloads"/>

## Pre-Compiled Downloads
Since version 1.0.7, Pre-Compiled builds are only available for purchase on [Steam](https://store.steampowered.com/app/1768350/Multris/)

Older versions (1.0.0 - 1.0.6) are available for free on [Itch.io](https://rustymonster.itch.io/multris)
