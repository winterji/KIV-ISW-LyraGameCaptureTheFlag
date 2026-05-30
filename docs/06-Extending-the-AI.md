# 06 — Extending the AI

This document walks through the most common changes you will make. For each one it gives the safe procedure and lists the pitfalls students have hit in past iterations.

## Adding a new blackboard key

1. Open `Content/Bot/BB_ISW_Bot`.
2. Click **New Key** and choose the type. Bool, Vector, Object, and Float cover almost everything.
3. Name it after **what it means**, not where it comes from. `EnemyIsNearMyBase` is good; `PerceptionEvent42` is bad.
4. Add an entry to the table in [04 — Blackboard and Behavior Tree](04-Blackboard-and-BehaviorTree.md) so future students know what you added.

**Pitfalls:**

- If you read the new key from a BT decorator before anything has written to it, you get the type's default value (false / zero / None). That's usually fine — but if not, write it in `OnPossess`.
- If you intend to read the key from BT services / tasks via `GetOwnersBlackboard → GetValueAsX`, you pass the key **name as a string**. Mis-typing the name silently returns the default. Use a single named string constant where possible.

## Writing a new BT service

Services run while a subtree is active. Use them to maintain a derived belief (e.g. "is the carrier visible") or poll something cheap.

1. In the Content Browser, right-click → **AI → Behavior Tree Service**.
2. Open it. Override `Event Receive Tick` (or `Event Receive Tick AI` if you want the `Controlled Pawn` already wired).
3. From `GetOwnersBlackboard`, write to the key you want to update. **Use the key name as a string** — `SetValueAsBool(KeyName="MyKey", Value=...)`.
4. Drop the service onto a node in the BT. Tune the tick interval (a default of 0.5 s with random deviation is usually fine).

**Pitfalls:**

- `GetOwnersBlackboard` only works inside BT nodes. Don't try to use it from the controller.
- A service ticking 60 times per second is almost never necessary. Use intervals.
- If your service does a line trace, prefer Lyra's `LineOfSightTo` over a raw `LineTraceByChannel` — it already respects perception channel settings.
- Don't use a service to *do* things. Services update beliefs. Tasks do things.

## Writing a new BT task

A task is a concrete action: move somewhere, fire a weapon, equip a different item, drop the flag, etc. Tasks succeed, fail, or stay running (for "in-progress" tasks).

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

- Register in `BeginPlay → AsyncAction_ExperienceReady → OnReady`, not in `OnPossess`.
- Inside the handler, identify "self" via the controller's own `GetPlayerState`, not the pawn's.
- Guard with `IsValid(BBComp)` before any blackboard access.
- Update beliefs, not behavior. Don't drive `MoveTo` from inside a message handler.

## Creating your own controller

Your agent is a **child Blueprint of `B_ISW_AI`** — do not edit `B_ISW_AI` directly. It is the shared baseline that all students build on top of.

### Step 1 — Create the child Blueprint

1. In the Content Browser, create your folder `Content/Bot/Student_<yourname>/`.
2. Right-click `Content/Bot/B_ISW_AI` → **Create Child Blueprint Class**.
3. Name it `B_<yourname>_AI` and save it inside your folder.

What you inherit for free:

- The full `BeginPlay` lifecycle: ExperienceReady gating, flag-pad discovery, and the existing gameplay-message listeners.
- The `OnTargetPerceptionUpdated` handler that writes `TargetEnemy` to the blackboard.
- The three gameplay-message listeners: flag picked up, flag delivered, and elimination (see [05 — Elimination Listener](05-Example-Elimination-Listener.md)).
- The `BBComp` caching pattern and all per-life blackboard resets in `OnPossess`.

### Step 2 — Create your own Behavior Tree

1. Content Browser → right-click → **AI → Behavior Tree**. Name it `BT_<yourname>_bot`.
2. In the BT's Details panel, set **Blackboard Asset** to `BB_ISW_Bot`. You can extend the blackboard later — see "Adding a new blackboard key" above.
3. Build your tree. All existing node types (`BTS_CheckLOS`, `MoveTo`, the Lyra shooting service) are available.

### Step 3 — Override `OnPossess` to run your tree

Override `OnPossess` in your child Blueprint. **Call the parent first** (`Parent: On Possess`) — that runs BB caching and per-life key resets. Then call `RunBehaviorTree` with your tree:

```
Event OnPossess (InPawn)
  └── Parent: On Possess               ← caches BBComp, resets per-life keys
  └── RunBehaviorTree(BT_<yourname>_bot)
```

Do **not** call `UseBlackboard` again — the parent already did it and `BBComp` is now valid.

### Step 4 — Add new gameplay-message listeners

To react to events the baseline does not handle (e.g. a custom death response, an ammo event, team scoring), override `BeginPlay` in your child Blueprint:

1. **Call the parent first** (`Parent: Begin Play`) — this registers the three existing listeners.
2. Add your own `Listen for Gameplay Messages` nodes inside the same `OnReady` continuation, or chain a second `AsyncAction_ExperienceReady`.

The parent's listeners remain active — you are adding new ones, not replacing them. Follow the same rules: register in `BeginPlay`, not `OnPossess`; guard with `IsValid(BBComp)`; update beliefs only.

### Step 5 — Point the experience at your controller

1. Open `Content/System/B_ShooterGame_CaptureTheFlag`.
2. Find the **Bot** entry and change **AI Controller Class** to your `B_<yourname>_AI`.

This is the only change needed to make every bot in the experience use your class.

**Pitfalls specific to inheritance:**

- **Overriding `OnPossess` without calling the parent.** `BBComp` is never cached; every blackboard write in your override silently no-ops.
- **Overriding `BeginPlay` without calling the parent.** The three existing listeners do not register. The elimination handler will not clear `FlagCarrier`; the BT will chase a dead pawn indefinitely.
- **Calling `UseBlackboard` again in your override.** The parent already ran it. Calling it again is harmless but creates a second reference and is confusing. Use the inherited `BBComp`.

---

## Cooperative behavior

For projects that coordinate between bots (see [Assignments B and G](07-Assignments.md)), the standard approaches map directly onto Kubík's Chapter 4.

### Centralized coordination

A single server-side actor assigns roles to bots at spawn. Each bot stores its role in a new BB key `MyRole` and the BT branches on it.

1. Create a `B_TeamCoordinator` actor (or `WorldSubsystem`) in the experience. It tracks open role slots.
2. In each bot's `BeginPlay → OnReady`, call `TeamCoordinator → RegisterBot(self)`, receive the assigned role, and write it to `BBComp → SetValueAsEnum("MyRole", Role)`.
3. In the BT, add a top-level **Selector** whose children are `[MyRole == Attacker]` and `[MyRole == Defender]` subtrees.
4. On respawn (`OnPossess`), re-register so the coordinator can rebalance if a slot opened.

This is *direct supervision* (*přímý dozor*) [Kubík 2004, Chapter 4.2] — Mintzberg's centralized coordination. Simple and deterministic.

### Decentralized coordination (contract net)

Bots broadcast availability and bid for open roles via gameplay messages or a shared state actor. No coordinator actor. This maps onto Smith's contract-net protocol [Kubík 2004, Chapter 4.3.2]. More implementation work, but a richer theoretical write-up.

### Reading shared world state (stigmergy)

Each bot has its own blackboard — you cannot share a BB key between bots. For shared facts (e.g. "does any teammate currently carry the enemy flag?"), use a **BT service** that polls a shared actor each tick and writes a derived local belief:

1. Read from a shared actor (Game State, `B_TeamCoordinator`, or a flag pad).
2. In a BT service `BTS_CheckTeamCarrying`, poll the shared actor and write a local `bool TeamCarryingFlag` key.
3. Use that key in decorators as normal.

This is the *stigmergy* / *reaktivní komunikace* pattern [Kubík 2004, Chapter 4.2.1]: agents read traces left in the environment rather than exchanging direct messages.

---

## Starting from scratch (advanced)

If your assignment requires removing the BT entirely and running your own decision loop on `Tick`, implementing a GOAP planner, or an InteRRaP-layered controller (assignment G), subclass `LyraPlayerBotController` directly instead of `B_ISW_AI`.

For all other assignments, **inherit from `B_ISW_AI`**.

If you do start from scratch:

- Manually replicate the lifecycle split: ExperienceReady for static setup, `OnPossess` for per-life setup.
- Manually register gameplay-message listeners in `BeginPlay`, not `OnPossess`.
- Wire up `AIPerceptionComponent` yourself if you need sight/hearing.
- Set `Default Pawn Class` in the experience to a Lyra character with the equipment you need.
- Point the experience at your controller class (same as Step 5 above).

## Common pitfalls reference

These are all in the lessons memory, but worth restating in one place:

- **`GetBlackboard()` returns None before `RunBehaviorTree` runs.** Always cache the `UseBlackboard` output pin.
- **`FindTeamFromObject` returns -1 if the pawn isn't team-assigned yet.** Guard with `bIsPartOfTeam`. If false, `Delay 0.1` and retry.
- **`AsyncAction_ExperienceReady` fires once per experience load, not once per spawn.** Use it for static-data setup. Per-life setup goes in `OnPossess`.
- **The Lyra Shooting Service is shared across Lyra bots and fragile.** Never edit it. Gate it by parenting decisions instead.
- **Observer Aborts inside Simple Parallel background branches are unreliable.** Put aborting decorators on a Selector *above* the Simple Parallel.
- **`B_CaptureTheFlagScoring.GetFlagPadByTeam` has a race.** Iterate `GetAllActorsOfClass(B_GrantFlagPad)` directly.
- **`_FlagTeamIndex` inside `B_GrantFlagPad.GrantOrDeliverFlag` is a local var.** Use the class-level `PadTeamIndex` instead.
- **Listener registered in `OnPossess` duplicates per life.** Register once in `BeginPlay`.
- **`GetControlledPawn → GetPlayerState` inside an elimination handler may return None.** Use the controller's own `GetPlayerState`.

## A debugging routine that will save you hours

When your agent does the wrong thing, the question is almost always "what did it believe was true at that moment?". The fastest way to answer that:

1. Set a breakpoint on the BT node that fired (right-click → Add Breakpoint).
2. When PIE pauses, open the **Gameplay Debugger** and snapshot the blackboard.
3. Compare to what you *think* should have been true. Find the key that disagrees with reality.
4. Trace backwards: what was the last thing that wrote to that key? Was the writer ever called? Was it called with the right value?

This is the same loop you would run in any debugger — the only twist is that "state" lives on the blackboard rather than in local variables. Treat the blackboard inspector as your watch window.
