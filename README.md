# Welcome to Lyra Capture the Flag Game Mode with enhanced AI agents!

This plugin was created as course material for semestral work for course KIV/ISW at University of West Bohemia.

![Lyra Start Game Capture the Flag](https://i.imgur.com/5UdE4Ah.png)

Unreal Engine 5 comes with a new feature called [Game Features Plugins](https://dev.epicgames.com/documentation/unreal-engine/game-features-and-modular-gameplay-in-unreal-engine) allowing for modular and standalone game content. The [Lyra Starter game](https://www.unrealengine.com/marketplace/en-US/product/lyra) already uses this feature extensively to create modular game modes such as the Control point and the Team deathmatch.

This plugin is a standalone Capture the Flag game mode that can be added to the Lyra Start Game without any changes to the core content of the project.

Watch the Gameplay Video [here](https://www.youtube.com/watch?v=Dbrez3j3f40) 

# Improvements for course KIV/ISW at University of West Bohemia
This repository builds on existing CTF game mode and enhances AI agents with CTF-specific listeners and actions like **WePickedUpFlag**, or **OurFlagWasCaptured**. This allows to implement custom AI agents with more in-depth decision making for the CTF game mode.

# Installation

- Install Unreal Engine 5.5 and above
- Download Lyra Starter Game Project
- Download and extract this plugin into the following project's directory `LyraStarterGame\Plugins\GameFeatures\ShooterCTF\`
![Directory Structure](https://i.imgur.com/FjoWlo0.png)

- Once the Editor is loaded , Go to the Plugins Window and Activate the `ShooterCTF` plugin under the `GameFeatures` category

![Activate the plugin](https://i.imgur.com/NfcOLTk.png)

- Restart the Editor, and the Game mode should be activated automatically

## Documentation

In the */docs* folder is documentation with main [Readme](./docs/README.md) that offers quick overview of the whole documentation. In the */docs/cs* is the same documentation but in Czech language.

## Possible Future Development

There are some missing features that are not implemented yet.
- Players carrying the flag should drop the flag in place after dying.
- Improve the map so it is bigger and can possibly host 3 teams - update of the AI controller is also necessary in this case.

## Author

Previous student of the KIV/ISW course in 2025/26 - Jiří Winter.