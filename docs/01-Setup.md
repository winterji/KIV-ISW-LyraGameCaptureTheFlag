# 01 — Setup

This document gets you from "I just cloned the repo" to "I'm running the CTF game mode and can see the baseline bots play".

## 1. Install Unreal Engine 5.7

Get UE 5.7 through the Epic Games Launcher. Newer 5.x versions may work, but Lyra was authored against 5.7 and using a different engine version is the single most common reason setup fails.

## 2. Get Lyra Starter Game

Lyra is a free Epic-published reference project. Open the **Epic Games Launcher → Unreal Engine → Library → Marketplace**, search for *Lyra Starter Game*, and **Create Project**. Pick a location with at least 30 GB free.

## 3. Place the ShooterCTF plugin

The plugin should live at:

```
<YourProject>/Plugins/GameFeatures/ShooterCTF/
```

If the `Plugins/GameFeatures/` folder doesn't exist, create it. If you cloned this whole repository, the path is already correct.

## 4. Enable the plugin

1. Open the project in the Unreal Editor.
2. **Edit → Plugins**.
3. Find **ShooterCTF** under the **Game Features** category and enable it.
4. Restart the editor when prompted.

After restart, the editor should report that the `ShooterCTF` Game Feature is **Registered** and its experience is loaded.

## 5. Launch a CTF match

The fastest path:

1. In the Content Browser, navigate to `Plugins/ShooterCTF Content/Maps/`.
2. Open `L_ShooterCFT_FiringRange` (the smaller training map) or `L_Limitation` (the full CTF arena).
3. Click **Play** in the toolbar.

You should see your character spawn and bots (`B_ISW_AI` and Lyra's stock bots) populate both teams. The flag bases are the glowing pads at the team-colored ends of the map.

## 6. Verify the AI is alive

While Play is running:

1. Open **Window → Developer Tools → Output Log**. Filter by `LogBehaviorTree` to see BT activity, or `LogAIModule` for controller events.
2. Press **`** (backtick) in PIE to drop a console, then type `showdebug AI` to overlay AI debug info on the screen.
3. Press **Apostrophe** (`'`) to open the **Gameplay Debugger**. Number keys toggle categories — `2` is AI, `3` is Behavior Tree, `5` is Perception. Click on a bot to inspect its blackboard live.

If the gameplay debugger shows a populated blackboard and the bots move around chasing/scoring, you're good.

## 7. Find the files you'll be editing

| Asset                        | Path                                                  | Purpose                                |
| ---------------------------- | ----------------------------------------------------- | -------------------------------------- |
| `B_ISW_AI`                   | `Content/Bot/B_ISW_AI`                                | The Blueprint AI controller            |
| `BB_ISW_Bot`                 | `Content/Bot/BB_ISW_Bot`                              | The blackboard                         |
| `BT_ISW_CTF_bot`             | `Content/Bot/BT_ISW_CTF_bot`                          | The main behavior tree                 |
| `BTS_CheckLOS`               | `Content/Bot/BTS_CheckLOS`                            | Service that updates `CanSeeFlagCarrier` |
| `B_GrantFlagPad`             | `Content/Blueprint/B_GrantFlagPad`                    | The flag base actor                    |
| `B_FlagActor`                | `Content/Items/Flag/`                                 | The flag itself                        |
| `B_CaptureTheFlagScoring`    | `Content/Blueprint/B_CaptureTheFlagScoring`           | Scoring component                      |
| Experience asset             | `Content/Experiences/B_ShooterGame_CaptureTheFlag`    | The Lyra Experience that boots CTF mode |

## 8. Troubleshooting

- **The plugin doesn't appear in the Plugins window.** Check that the `.uplugin` file is directly under `Plugins/GameFeatures/ShooterCTF/`, not one folder deeper.
- **Editor crashes on load after enabling.** You're almost certainly on the wrong engine version. Look for a "Module not compatible" dialog in the log.
- **Match starts but no bots spawn.** Open the experience asset and verify it points at a valid spawner; on the firing range map there are explicit spawn volumes.
- **Bots spawn but don't do anything.** The controller may have failed to start the BT. Open the gameplay debugger (`'`) and look at the bot's BB — if it's empty, see the lifecycle discussion in [03 — AI Architecture](03-AI-Architecture.md).
