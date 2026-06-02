# 06 — Extending the AI

This document walks through the most common changes you will make. For each one it gives the safe procedure and lists the pitfalls students have hit in past iterations.

## Adding a new blackboard key

1. Open `Content/Bot/BB_ISW_Bot`.
2. Click **New Key** and choose the type. Bool, Vector, Object, and Float cover almost everything.
3. Name it after **what it means**, not where it comes from. `EnemyIsNearMyBase` is good; `PerceptionEvent42` is bad.


## Writing a new BT service

Services run while a subtree is active. Use them to maintain a derived belief (e.g. "is the carrier visible") or poll something cheap.

1. Right-click in the Content Browser → **AI → Blueprint Class → BTService_BlueprintBase** and name it in the format `BTS_<your_service>`.
2. Open it. Override `Event Receive Tick` (or `Event Receive Tick AI` if you want `Controlled Pawn` already wired).
3. From `GetBlackboard`, pull `SetValueAsBool` (or `SetValueAs<OtherDataType>`) and write to the key you want to update. Pull the key and add **`Make literal name`**, then define the key as a string there.
4. Drop the service onto a node in the BT. Tune the tick interval (a default of 0.5 s with random deviation is usually fine).

**Pitfalls:**

- Setting a key in `SetValueAsBool` without using `Make Literal Name`.
- A service ticking 60 times per second is almost never necessary. Use longer intervals.
- If your service does a line trace, prefer Lyra's `LineOfSightTo` over a raw `LineTraceByChannel` — it already respects perception channel settings.
- Don't use a service to *do* things. Services update beliefs. Tasks do things.

## Writing a new BT task - AI Generated, Not Tested

A task is a concrete action: move somewhere, fire a weapon, swap a weapon, drop the flag, etc. Tasks succeed, fail, or stay running (for "in-progress" tasks).

1. Content Browser → right-click → **AI → Behavior Tree Task**.
2. Override `Event Receive Execute AI`.
3. Do your work. At the end, call `Finish Execute` with `Success` or `Fail`. For a long-running task (movement, animation), don't call `Finish Execute` immediately — keep state on the task and call it from a callback (e.g. `OnMoveCompleted`).
4. Drop the task as a leaf in the BT. Configure its exposed parameters in the BT details panel.

**Pitfalls:**

- Forgetting to call `Finish Execute` leaves the BT stuck on your task forever. The gameplay debugger will show your task as the active node and nothing else will run.
- If you call `Finish Execute` synchronously on the same frame the task started, behaviors above it may not see the transition — this is rare but happens. The textbook fix is to delay one frame.
- Don't write *behavior-wide* state to instance variables on the task. Tasks can be re-entered. Use the blackboard for state that must persist.

## Adding a new gameplay-message listener

Use the [worked example](05-Example-Elimination-Listener.md) as a template. Summary of the rules:

- Consider whether to register in `BeginPlay → AsyncAction_ExperienceReady → OnReady` or in `OnPossess`.
- Pay attention to when to use `GetPlayerState`, when `LyraCharacter`, when `Pawn`, and when `AIController`.
- Update beliefs, not behavior. Don't call `MoveTo` from inside a message handler.

## Creating your own controller

Your agent is a **child Blueprint of `B_ISW_AI`** — do not edit `B_ISW_AI` directly. It is the shared baseline that all students build on top of.

### Step 1 — Create the child Blueprint

1. In the Content Browser, create your folder `Content/Bot/Student_<yourname>/`.
2. Right-click `Content/Bot/B_ISW_AI` → **Create Child Blueprint Class**.
3. Name it `B_<yourname>_AI` and save it inside your folder.

What you inherit for free:

- The full `BeginPlay` lifecycle: ExperienceReady gating, flag-pad discovery, and the existing gameplay-message listeners.
- The `OnTargetPerceptionUpdated` handler that writes `TargetEnemy` to the blackboard.
- Three gameplay-message listeners: flag picked up, flag delivered, and elimination (see [05 — Flag pickup listener](05-Example-Elimination-Listener.md)).

### Step 2 — Create your own Blackboard

1. Content Browser → right-click → **AI → Blackboard**. Name it `BB_<yourname>_bot`.
2. Set values according to the baseline template.

### Step 3 — Create your own Behavior Tree

1. Content Browser → right-click → **AI → Behavior Tree**. Name it `BT_<yourname>_bot`.
2. In the BT's Details panel, set **Blackboard Asset** to `BB_ISW_Bot`. You can extend the blackboard — see "Adding a new blackboard key" above.
3. Build your tree. All existing node types (`BTS_CheckLOS`, `MoveTo`, the Lyra shooting service) are available.

### Step 4 — Override `OnPossess` to run your tree

1. In the AI Controller details, set `BTAsset` to your new `BT_<yourname>_bot`.

### Step 5 — Add new gameplay-message listeners

To react to events the baseline does not handle (e.g. a custom death response, an ammo event, team scoring), override `BeginPlay` in your child Blueprint:

1. **Call the parent first** (`Parent: Begin Play`) — this registers the three existing listeners.
2. Add your own `Listen for Gameplay Messages` nodes inside the same `OnReady` continuation, or chain a second `AsyncAction_ExperienceReady`.

The parent's listeners remain active — you are adding new ones, not replacing them. Follow the same rules: register in `BeginPlay`, not `OnPossess`; guard with `IsValid(BBComp)`; update beliefs only.

### Step 6 — Point the experience at your controller

1. Open `Content/System/B_ShooterGame_CaptureTheFlag`.
2. Find the **Bot** entry and change **AI Controller Class** to your `B_<yourname>_AI`.

This is the only change needed to make every bot in the experience use your class.

**Pitfalls specific to inheritance:**

- **Overriding `OnPossess` without calling the parent.**
- **Overriding `BeginPlay` without calling the parent.** The three existing listeners will not register. The elimination handler will not clear `EnemyFlagCarrier`; the BT will chase a dead pawn indefinitely.
- **Calling `UseBlackboard` again in your override.** The parent already ran it. Calling it again does not break correctness but creates a second reference and is confusing.
- **I can't see the BT, BB, or Service:** They are probably in the wrong directory. Check the location. If it still doesn't work, try putting them in the `Bot` directory.

---

## Cooperative behavior - AI Generated, Not tested

For projects that coordinate between bots (see [Assignments B and D](07-Assignments.md)), the standard approaches map directly onto Kubík's Chapter 4.

### Centralized coordination

A single server-side actor assigns roles to bots at spawn. Each bot stores its role in a new BB key `MyRole` and the BT branches on it.

1. Create a `B_TeamCoordinator` actor (or `WorldSubsystem`) in the experience. It tracks occupied role slots.
2. In each bot's `BeginPlay → OnReady`, call `TeamCoordinator → RegisterBot(self)`, receive the assigned role, and write it to `BBComp → SetValueAsEnum("MyRole", Role)`.
3. In the BT, add a top-level **Selector** whose children are `[MyRole == Attacker]` and `[MyRole == Defender]` subtrees.
4. On respawn (`OnPossess`), re-register so the coordinator can rebalance if a slot opened.

This is *direct supervision* (*přímý dozor*) [Kubík 2004, Chapter 4.2] — Mintzberg's centralized coordination. Simple and deterministic.

### Decentralized coordination (contract net)

Bots broadcast availability and bid for open roles via gameplay messages or a shared state actor. No coordinator. This maps onto Smith's contract-net protocol [Kubík 2004, Chapter 4.3.2]. More implementation work, but a richer theoretical writeup.

### Reading shared world state (stigmergy)

Each bot has its own blackboard — sharing a BB key between bots is not possible. For shared facts (e.g. "does any teammate currently carry the enemy flag?"), use a **BT service** that polls a shared actor each tick and writes a derived local belief:

1. Read from a shared actor (Game State, `B_TeamCoordinator`, or a flag pad).
2. In a BT service `BTS_CheckTeamCarrying`, poll the shared actor and write a local `bool TeamCarryingFlag` key.
3. Use that key in decorators as normal.

This is the *stigmergy* / *reaktivní komunikace* pattern [Kubík 2004, Chapter 4.2.1]: agents read traces left in the environment rather than exchanging direct messages.

---

## Common pitfalls reference

- **`GetBlackboard()` doesn't work**, use `GetBlackboard()` instead and optionally set the target.
- **`GetBlackboard()` returns None before `RunBehaviorTree`.** Use `UseBlackboard` and manually override the Blackboard being used.
- **`FindTeamFromObject` returns -1 if the pawn isn't team-assigned yet.** Guard with `bIsPartOfTeam`. If false, `Delay 0.1` and retry.
- **`AsyncAction_ExperienceReady` fires once per experience load, not once per spawn.** Use it for static data. Per-life setup goes in `OnPossess`.
- **`B_CaptureTheFlagScoring.GetFlagPadByTeam` has a race.** Iterate `GetAllActorsOfClass(B_GrantFlagPad)` directly.
- **`_FlagTeamIndex` inside `B_GrantFlagPad.GrantOrDeliverFlag` is a local variable.** Use the class-level `PadTeamIndex`.
- **Listener registered in `OnPossess` duplicates per life.** Register once in `BeginPlay`.
- **`GetControlledPawn → GetPlayerState` inside an elimination handler may return None.** Use the controller's own `GetPlayerState`.

## A debugging routine that will save you hours

When your agent does the wrong thing, the question is almost always "what did it believe was true at that moment?". Use the Visual Logger: **Tools → Debug → Visual Logger**, start the game and start the logger. You can then see the current state of the AI Controller over time, including its BB values.
