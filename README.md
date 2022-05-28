# DunGen Dungeon Generator Plugin Sample Project

This is a Project that contains a custom Plugin i made for `Unreal Engine 4 (Ver. 4.27)`.  
With this Plugin you're able to:
* Create your own `Square-Room-based Dungeon`;
* Edit your `Rooms` with your Meshes and other decorations;  

Examples, Meshes given are self-made.  
My first idea was to build Procedural Dungeons with this Isometric style for my Game, but in future i'm going to make it generic for everyone.

Extending this Plugin with new Features and implementations is part of the plan, so stay tuned for new updates :)

`Here's a Preview of this Plugin's GUI:`  

![I](./Docs/Plugin%20GUI%20Overview.PNG)

# Plugin GUI Overview

`Toolbar Commands`  

![I](./Docs/Plugin%20GUI%20Toolbar%20Commands.PNG)  
Divided in two categories:  
*  Dungeon Commands:  
`Generate` Command generates new info for a new Dungeon;  
`Preview` Command gives an on Scene representation of your new Dungeon by using the last generated Dungeon Info with the Generate Command;  
* Rooms Commands:  
`Preview` Command gives an on Scene representation of your Room by using a RoomPreset Asset;  
`Save` Command saves all changes to the Selected RoomPreset;  
`Reset` Command resets all changes to the Selected RoomPresets;  

`Basic Commands` 

![I](./Docs/Plugin%20GUI%20Commands.PNG)  
`Set Dungeon Folder Path`: Here you can locate the DungeonData binary file to a new Folder;  
`Set Dungeon Rooms Count`: Here you can specify how many Rooms should the next Dungeon have;  
`Drop your room Preset Asset here`: Drag one of your RoomPreset Assets and Drop it inside the Border. By doing this you set the Selected Room Preset with your dropped Asset, that will be used for Room Editing and Preview;  

![I](./Docs/Plugin%20Drag%20%26%20Drop%20RoomPreset%20.PNG)
![I](./Docs/Plugin%20After%20Drop%20RoomPreset.PNG)  

`Queries`  

![I](./Docs/Plugin%20GUI%20Queries.PNG)  
Here you can look at some tracked information like:
* Where the Dungeon Data binary file is saved;
* How many Rooms are used to generate Dungeons;
* How many RoomPreset Assets are registered;  


# How it works?
First, create your RoomPreset Asset by the `Asset Menu`

![I](./Docs/RoomPreset%20Assets%20on%20Content%20Browser.PNG)  
and fill them with data.  

![I](./Docs/Room%20Preset%20Asset%20Variables.PNG)  
These are given by default.  
Next, be sure your rooms are how you imagined them with a `Preview`.  

![I](./Docs/Plugin%20On%20Scene%20Room%20Preview.PNG)  
When everything is okay, set a number of Rooms you want for your Dungeon.  
Click the `Generate` Command (Dungeon) and a random Dungeon of N Rooms will be generated.  
Next, see what you got with a `Preview`.

![I](./Docs/Plugin%20On%20Scene%20Dungeon%20Preview.PNG)  

Here are some explanaitions:

# Menu Actions

Importing Plugin will extend the Asset Menu with a couple of Actions like:  
* `Create new RoomPreset Assets` (i'm going to move to a custom Category later);  

![I](./Docs/Plugin%20Asset%20Menu%20New%20RoomPreset.PNG)

* `Register or Unregister an existing RoomPreset Asset` (Changes can be checked by typing the "querypresets" Prompt Command, to be discusse later);  

![I](./Docs/Plugin%20Menu%20Custom%20Actions.PNG)


# Prompt Exec Commands

This Plugin offers two query Prompt Commands:

* `"querydungeon"` Command: This shows all data about the last generated Dungeon Info;  

![I](./Docs/Plugin%20Prompt%20querydungeon%20Command.PNG)

* `"querypresets"` Command: This shows all registered RoomPreset Assets with their relative path;

![I](./Docs/Plugin%20Prompt%20querypresets%20Command.PNG)  


