# 04 — Blackboard and Behavior Tree

This document is the reference for the baseline agent's **belief base** (in Kubík's terms: the internal state set `I` from the formal model in [Kubík 2004, Chapter 1.2]) and its **action-selection function** (`akce : P × I → A`).

## Blackboard: `BB_ISW_Bot`

The blackboard is the agent's belief base. In the formal model from [Kubík 2004, Chapter 1.2], this is `I` — the set of internal states. Each key represents one belief the agent currently holds about the world. The baseline AI uses the following keys:

### Static beliefs (set once per match)

| Key                       | Type   | Set in                                | Meaning                                              |
| ------------------------- | ------ | ------------------------------------- | ---------------------------------------------------- |
| `OwnFlagBaseLocation`     | Vector | `BeginPlay` (after ExperienceReady)   | World-space location of *our* team's flag base pad.  |
| `EnemyFlagBaseLocation`   | Vector | `BeginPlay` (after ExperienceReady)   | World-space location of the *enemy* team's flag base pad. |
| `SelfActor`               | Object | `OnPossess`                           | Reference to this bot's pawn. Convenience for tasks.  |

### Per-life beliefs (reset on each `OnPossess`)

| Key               | Type   | Owner               | Meaning                                                                    |
| ----------------- | ------ | ------------------- | -------------------------------------------------------------------------- |
| `CarryingFlag`    | Bool   | Self                | This bot is currently carrying the enemy flag.                             |
| `WePickedUpFlag`  | Bool   | Self                | This bot picked up the flag (sometimes equivalent to `CarryingFlag` but tracked separately so you can detect the pickup *event* vs. the *state*). |
| `OutOfAmmo`       | Bool   | Self                | Self-explanatory; reserved for ammo-aware behavior students may add.       |
| `TargetEnemy`     | Object | AIPerception        | Currently-perceived enemy pawn.                                            |
| `MoveGoal`        | Vector | BT tasks            | Current movement target. Used as a generic destination key.                |

### Volatile shared beliefs (updated by gameplay messages or services)

| Key                  | Type   | Updater                          | Meaning                                                            |
| -------------------- | ------ | -------------------------------- | ------------------------------------------------------------------ |
| `OurFlagCaptured`    | Bool   | Flag pickup / elimination listeners | An enemy is currently carrying our flag.                        |
| `FlagCarrier`        | Object | Flag pickup / elimination listeners | The pawn currently carrying our flag (only valid when `OurFlagCaptured == true`). |
| `CanSeeFlagCarrier`  | Bool   | `BTS_CheckLOS` (service)         | Line-of-sight test result from `LineOfSightTo(FlagCarrier)`.       |

### Reading the table

A few invariants worth burning in:

- `OwnFlagBaseLocation` / `EnemyFlagBaseLocation` are **set once and never change**. If they're missing it means `BeginPlay` raced ahead of team assignment — guard with `bIsPartOfTeam`.
- `OurFlagCaptured` and `FlagCarrier` are coupled. Always set or clear them together. Setting `OurFlagCaptured` without setting `FlagCarrier` leaves the BT in an inconsistent state.
- `CanSeeFlagCarrier` is **only meaningful while `FlagCarrier != None`**. The service should bail early when the carrier is invalid.

## Behavior Tree: `BT_ISW_CTF_bot`

The baseline tree is a Selector at the root that branches on the agent's current role:

```
Root Selector
├── [OurFlagCaptured == true]  → Defend / Recover our flag
├── [CarryingFlag    == true]  → Return to base with the flag
├── [Default]                  → Steal: navigate to enemy base
```

Each branch is described below.

### Branch 1 — Recover our flag

Active when **our** flag has been stolen and we know the carrier.

```
Sequence [Decorator: OurFlagCaptured == true]
  ├── [BTS_CheckLOS service]
  └── Selector
      ├── Sequence [Decorator: CanSeeFlagCarrier == true, Observer Aborts: Lower Priority]
      │   └── [Lyra Shooting Service attached on this Sequence]
      │       └── Simple Parallel (Finish: Immediate)
      │           ├── Main: MoveTo(FlagCarrier)
      │           └── Background: Wait(9999)
      └── MoveTo(FlagCarrier)          ← chase without shooting
```

Two important patterns are happening here:

**LOS gating of the Lyra Shooting Service.** The shooting service is provided by Lyra and is shared across all stock bot types. You are **strongly discouraged** from editing it. Instead, attach it to the sequence whose decorator (`CanSeeFlagCarrier == true`) gates whether shooting can happen. When the decorator goes false, the subtree aborts and the selector falls through to the chase-only branch. Shooting is therefore reactive to perception without any code changes to Lyra.

This is functionally identical to Brooks's **potlačení a zabránění** mechanism between layers in the subsumpční architektura [Kubík 2004, Chapter 1.3.2]: a higher-priority subtree can inhibit a lower-priority one when its activation condition is satisfied. In Brooks's robots this was wired with explicit signal taps; in a BT it is wired with a decorator and Observer Aborts.

**Why Simple Parallel + Wait(9999)?** This is the Lyra-recommended pattern for "do a movement task while a service continues to tick". `MoveTo` would otherwise terminate the subtree when it succeeds; the parallel keeps the subtree alive (and the shooting service running) until the decorator changes the agent's mind.

**Why Observer Aborts: Lower Priority on the decorator, not inside the parallel?** Empirically, observer aborts placed **inside** a Simple Parallel's background branch are unreliable — the abort signal doesn't always propagate out. Always place the deciding decorator on a Selector *above* the parallel.

### Branch 2 — Return to base

Active when this bot is carrying the enemy flag.

```
Sequence [Decorator: CarryingFlag == true]
  └── MoveTo(OwnFlagBaseLocation)
```

Intentionally minimal. Improving this is one of the obvious assignments for students — currently the carrier walks straight home in a deterministic line.

### Branch 3 — Steal

The fallback branch when neither of the above applies.

```
MoveTo(EnemyFlagBaseLocation)
```

Even more minimal. The carrier doesn't pick paths intelligently; it doesn't engage enemies opportunistically; it doesn't fall back when outnumbered. All of this is intentional headroom.

## Custom services and tasks shipped with the project

- **`BTS_CheckLOS`** — runs the controller's `LineOfSightTo(FlagCarrier)` once per tick interval and writes the result to `CanSeeFlagCarrier`. Uses `GetOwnersBlackboard → SetValueAsBool` with the key name passed as a string. (Inside services and tasks `GetOwnersBlackboard` works; in the controller it does not — use cached `BBComp`.)

## Reading the tree in the editor

Open `BT_ISW_CTF_bot` in the editor and start the game (PIE). Then:

- The currently-running node is highlighted in yellow.
- Right-click any node in the running tree to inspect its blackboard values.
- The **Gameplay Debugger** (`'` key in PIE) shows the live BB and current task on screen for any selected bot.

This live introspection is by far the fastest way to understand the existing tree before you change it.
