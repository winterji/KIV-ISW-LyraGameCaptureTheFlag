# Lyra Capture the Flag — Unreal Engine 5 GameFeature Plugin for KIV/ISW

This plugin provides a Capture the Flag (CTF) game mode for the Lyra Starter Game with enhanced AI agents. It was created as course material for the KIV/ISW course at the University of West Bohemia.

![Lyra Starter Game — Capture the Flag](https://i.imgur.com/5UdE4Ah.png)

Unreal Engine 5 supports modular content via Game Features Plugins. The Lyra Starter Game uses this system to provide modular game modes (Control Point, Team Deathmatch, etc.). This plugin is a standalone CTF mode that can be added to a Lyra project without modifying core content.

Watch the gameplay video: https://www.youtube.com/watch?v=Dbrez3j3f40

## Improvements for KIV/ISW
Enhances the base CTF mode by adding CTF-specific AI listeners and actions (for example: WePickedUpFlag, OurFlagWasCaptured) to enable richer AI decision-making.

## Installation

1. Install Unreal Engine 5.5 or newer.
2. Download the Lyra Starter Game project.
3. Extract this plugin into the project directory:

	`LyraStarterGame/Plugins/GameFeatures/ShooterCTF/`

	![Directory Structure](https://i.imgur.com/FjoWlo0.png)

4. Open the Editor, go to the Plugins window and enable the `ShooterCTF` plugin (category: GameFeatures).

	![Activate the plugin](https://i.imgur.com/NfcOLTk.png)

5. Restart the Editor. The game mode should be activated automatically.

## Documentation

See the documentation in the docs folder. The main entry is `docs/README.md`. Czech translations are available in `docs/cs/`.

## Possible future work

- Drop the flag on player death when carrying it.
- Expand the map to support more players and potentially 3 teams (AI controller updates required).

## Author

**Jiří Winter**

- Previous student of the KIV/ISW course (2025/26)
- Email: jjiiwwii@gmail.com