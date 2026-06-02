# 04 — Blackboard and Behavior Tree

This document is the reference for the baseline agent's **belief base** and its **action-selection function**.

## Blackboard: `BB_ISW_Bot`

The blackboard is the agent's belief base. In the formal model, this is `I` — the set of internal states. Each key represents one belief the agent currently holds about the world. `BB_ISW_Bot` uses the following keys:

### Static beliefs (set once per match)

| Key                       | Type   | Set in                                | Meaning                                              |
| ------------------------- | ------ | ------------------------------------- | ---------------------------------------------------- |
| `OwnFlagBaseLocation`     | Vector | `BeginPlay` (after ExperienceReady)   | World-space location of our team's flag base pad.    |
| `EnemyFlagBaseLocation`   | Vector | `BeginPlay` (after ExperienceReady)   | World-space location of the enemy team's flag base pad. |
| `SelfActor`               | Object | `OnPossess`                           | Reference to this bot's pawn. Convenient for tasks.  |

### Volatile shared beliefs (updated by gameplay messages or services)

| Key                  | Type   | Updater                               | Meaning                                                                     |
| -------------------- | ------ | ------------------------------------- | --------------------------------------------------------------------------- |
| `CarryingFlag`       | Bool   | Pickup / elimination listeners        | This bot is currently carrying the enemy flag.                              |
| `WePickedUpFlag`     | Bool   | Pickup / elimination listeners        | Our team is carrying the enemy flag.                                        |
| `OutOfAmmo`          | Bool   | Self                                  | Indicates whether the agent still has ammo.                                 |
| `TargetEnemy`        | Object | AIPerception                          | Currently-perceived enemy pawn.                                             |
| `MoveGoal`           | Vector | BT tasks                              | Current movement target. Generic destination key.                           |
| `OurFlagCaptured`    | Bool   | Pickup / elimination listeners        | An enemy is currently carrying our flag.                                    |
| `EnemyFlagCarrier`   | Object | Pickup / elimination listeners        | The pawn currently carrying our flag (only valid when `OurFlagCaptured == true`). |
| `OwnFlagCarrier`     | Object | Pickup / elimination listeners        | The pawn currently carrying the enemy flag (only valid when `WePickedUpFlag == true`). |
| `CanSeeFlagCarrier`  | Bool   | `BTS_CheckLOS` (service)              | Line-of-sight test result from `LineOfSightTo(FlagCarrier)`.                |

### Notes on the table

- `OwnFlagBaseLocation` / `EnemyFlagBaseLocation` are **set once and never change**. If they're missing, it means `BeginPlay` raced ahead of team assignment.

## Behavior Tree: `BT_ISW_CTF_bot`

The baseline tree is a Selector at the root that branches on the agent's current beliefs:

```
Root Selector
├── [OutOfAmmo == false]  → has ammo, can shoot
├── [OutOfAmmo == true]   → no ammo, looking for a weapon
```

When `OutOfAmmo` is false, it means the agent can shoot, so the game logic is applied to it.

```
[OutOfAmmo == false] → has ammo, can shoot
├── [CarryingFlag == true]    → carrying the flag, want to return it
├── [OurFlagCaptured == true] → our flag was stolen, want to recover it
├── [WePickedUpFlag == true]  → we're carrying the flag, defending the carrier
├── [Default]                 → going for the flag and fighting
```

Each branch is described below.

### Branch 1 — Return to base

Active when this bot is carrying the enemy flag.

```
Sequence [Decorator: CarryingFlag == true]
  └── MoveTo(OwnFlagBaseLocation)
```

**Intentionally minimal so it can be extended.**

### Branch 2 — Recover our flag

Active when **our** flag has been stolen and we know who is carrying it.

```
Simple Parallel [Decorator: OurFlagCaptured == true]
[BTS_CheckLOS service]
  ├── (Main task) Move To [EnemyFlagCarrier]
  └── (parallel task) Selector
        ├── Sequence [Decorator: CanSeeFlagCarrier == true]
            └── [Lyra Shooting Service attached to this Sequence]
```

Important note:

**Lyra Shooting Service.** The shooting service is provided by Lyra and shared across stock bots. Attach it to the sequence where the target to shoot is set (and the target that can be seen — `BTS_CheckLOS` or `AIPerception`).


### Branch 3 — Steal

The fallback branch when none of the above applies.

```
MoveTo(EnemyFlagBaseLocation)
```

Even more minimal. The carrier doesn't pick paths intelligently; it doesn't engage enemies opportunistically; it doesn't fall back when outnumbered. All of this is intentional headroom.

## Custom services and tasks shipped with the project

- **`BTS_CheckLOS`** — runs `LineOfSightTo(FlagCarrier)` from the controller once per tick interval and writes the result to `CanSeeFlagCarrier`.

<!-- AI Generated, Not tested

## Reading the tree in the editor

Open `BT_ISW_CTF_bot` in the editor and start the game (PIE). Then:

- The currently-running node is highlighted in yellow.
- Right-click any node in the running tree to inspect its blackboard values.
- The **Gameplay Debugger** (`'` key in PIE) shows the live BB and current task on screen for any selected bot.

This live introspection is by far the fastest way to understand the existing tree before you change it. -->
